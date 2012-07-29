/* 
 * linux/drivers/char/tc317x/tc317x_dxb.c
 *
 * Author:  <linux@telechips.com>
 * Created: 10th Jun, 2010
 * Description: Telechips Linux tc317x driver
 *
 * Copyright (c) Telechips, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/io.h>
#include <linux/poll.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/spi/spi.h>
#include <linux/kfifo.h>
#include <mach/bsp.h>
#include <asm/uaccess.h>
#include <asm/mach-types.h>
#include "tc317x_dxb.h"
#include "TCC317X_BOOT_TDMB.h"
//#define CMD_IFACE_I2C
#define CMD_IFACE_CSPI

#define SUBCH_SELECT  1
#define SUBCH_RELEASE 0

#if defined(CMD_IFACE_I2C)
//#define LEGACY_DRIVER
#define NEW_STYLE_DRIVER
#define I2C_BUS               1 
#define I2C_ADDR       (0xA0>>1)
#endif

#ifdef TC_AP
volatile PGPIO  RGPIO;
volatile PPIC   RPIC;
#endif

struct tc317x_dxb_drv_data_t {
  int32_t open_cnt;
  int32_t cmd_if;
  int32_t data_if;
  int32_t size_to_read;
  wait_queue_head_t wait_q;
  struct kfifo fifo;
  struct mutex fifo_lock;
};

static struct tc317x_dxb_drv_data_t tc317x_drv_data;

static void tc317x_test(void)
{
  int32_t data;
  data = TC317X_API_REG_Read(0, 0, 0x0C);
  TCDBG("chip id : 0x%X\n", data);
}

/* put fic, msc data to kfifo */
uint32_t tc317x_dxb_fifo_put(
    void *buffer, uint32_t size, uint32_t crc, int32_t type)
{
  int32_t qdsize;

  //mutex_lock(&tc317x_drv_data.fifo_lock);
  if( kfifo_avail(&(tc317x_drv_data.fifo)) < 
      (size+sizeof(uint32_t)*3) ){
    TCDBG("fifo overflow! put data to beginning of fifo!\n");
    kfifo_reset(&(tc317x_drv_data.fifo));
  }
  qdsize = size + sizeof(int)*2;

  kfifo_in(&(tc317x_drv_data.fifo), (void*)&qdsize, sizeof(qdsize));
  kfifo_in(&(tc317x_drv_data.fifo), (void*)&type, sizeof(type));
  kfifo_in(&(tc317x_drv_data.fifo), (void*)&crc, sizeof(crc));
  kfifo_in(&(tc317x_drv_data.fifo), buffer, size);

  //mutex_unlock(&tc317x_drv_data.fifo_lock);

  wake_up(&(tc317x_drv_data.wait_q));
  return size;
}

static int32_t tc317x_dxb_get_ant_level(uint32_t pcber, uint32_t snr)
{
  int32_t ant_level = 3;

  //PRINTF_LV_0("scaled_pcber:%u, scaled_snr:%u\n", pcber, snr);
  if(pcber <= 50) ant_level = 0;
  else if(pcber <= 30000) ant_level = 5;
  else if(pcber <= 50000) ant_level = 4;
  else if(pcber <= 70000) ant_level = 3;
  else if(pcber <= 90000) ant_level = 2;
  else if(pcber <= 120000) ant_level = 1;
  else if(pcber >  120000) ant_level = 0;

  //TCDBG("antena level : %d\n", ant_level);

  return ant_level;
}

static int32_t tc317x_dxb_get_mon_stat(unsigned long arg)
{
  int32_t ret;
  uint32_t pcber, pcber_moving_avg;
  uint32_t snr, snrdb, snr_moving_avg;
  uint32_t rssi, rssi_moving_avg;
  uint32_t vber, vber_moving_avg;
  uint32_t per, per_moving_avg;
  int32_t ant_level;
  struct tc317x_signal_quality signalquality;

  if(!arg) return -EFAULT;

  ret = TC317X_API_GetTDMB_PCBER(0, 0, &pcber, &pcber_moving_avg);
  if(ret < 0) { TCDBG("error\n"); return -EFAULT; }
  ret = TC317X_API_GetTDMB_SNR(0, 0, &snr, &snrdb, &snr_moving_avg);
  if(ret < 0) { TCDBG("error\n"); return -EFAULT; }
  ret = TC317X_API_GetTDMB_RSSI(0, 0, &rssi, &rssi_moving_avg);
  if(ret < 0) { TCDBG("error\n"); return -EFAULT; }
  ret = TC317X_API_GetTDMB_VITERBIBER(0, 0, &vber, &vber_moving_avg);
  if(ret < 0) { TCDBG("error\n"); return -EFAULT; }
  ret = TC317X_API_GetTDMB_TSPER(0, 0, &per, &per_moving_avg);
  if(ret < 0) { TCDBG("error\n"); return -EFAULT; }

  if(per_moving_avg > 900000) {
    ret = TC317X_API_GetTDMB_Lock_Wait(0, 0);
    TCDBG("lock status : %d\n", ret);
  }

  signalquality.SNR = snr_moving_avg;
  signalquality.PCBER = pcber_moving_avg/10;
  signalquality.RSBER = vber_moving_avg;
  signalquality.RSSI = rssi;

  PRINTF_LV_0("rssi:%d, pcber:%d, snr:%d, vber:%d, per:%d\n", 
    (int32_t)rssi, pcber_moving_avg, snr_moving_avg, 
    vber_moving_avg, per_moving_avg);


  if( !access_ok(VERIFY_WRITE, 
        (u8 __user*)(uintptr_t)arg, sizeof(struct tc317x_signal_quality)) )
    return -EFAULT;

  ant_level = tc317x_dxb_get_ant_level(pcber_moving_avg, snr_moving_avg);
  //TCDBG("ant_level:%d\n", ant_level);

  if(copy_to_user((void*)arg, (void*)&signalquality, 
    sizeof(struct tc317x_signal_quality)))
    return -EFAULT;

  return 0;
}

static int32_t tc317x_dxb_set_frequency(unsigned long arg)
{
  int32_t ret;
  uint32_t freq;

  if(copy_from_user(&freq, (void*)arg, sizeof(int))){
    TCDBG("cannot copy from user parameter "
        "in IOCTL_TC317X_SET_FREQ");
    return -EFAULT; 
  }
  ret = TC317X_API_SetFreq(0, 1500, freq, NULL);
  if(ret != 1) return -EFAULT;

  ret = TC317X_API_GetTDMB_Lock_Wait(0, 0);
  TCDBG("lock status : %d\n", ret);
  if(ret != 1) return -EFAULT;

  return 0;
}

/* select sub channel */
static int32_t tc317x_dxb_sevice_ctrl(int32_t flag, unsigned long arg)
{
  int32_t ret;
  TDMBService_t srv;

  if(copy_from_user(&srv, (void*)arg, sizeof(srv))){
    TCDBG("cannot copy from user parameter "
        "in IOCTL_TC317X_SELECT_SUBCH");
    return  -EFAULT;
  }

  ret = TC317X_API_SET_TDMB_Service(0, flag, &srv);
  if(ret != 1) return -EFAULT;

  return 0;
}

/* set interrupt threshold size of tcc3170 */
static int32_t tc317x_dxb_set_inttrupt_threshold_size(unsigned long arg)
{
  int32_t fifo_size;
  if(copy_from_user(&fifo_size, (void*)arg, sizeof(int))){
    TCDBG("cannot copy from user parameter ");
    return  -EFAULT;
  }

  /* interrupt threshold should be at most half of fifo size */
  if(fifo_size > MAX_FIFO_SIZE>>1 || fifo_size <= 0){
    return -EINVAL;
  }

  TC317X_API_SetFifoSize(0, fifo_size>>2);
  tc317x_drv_data.size_to_read = fifo_size;

  return 0;
}

static int32_t tc317x_dxb_rw_reg(unsigned long arg)
{
  struct tc317x_dxb_ioctl_param_t param;

  if(copy_from_user((void*)&param, (void*)arg, sizeof(param))){
    return -EFAULT;
  }

  switch(param.cmd){
  case 1:
    param.reg_data = TC317X_API_REG_Read(0, 0, param.addr);
    break;
  case 2:
    TC317X_API_REG_Write(0, 0, param.addr, param.reg_data);
    break;
  default:
    return -EINVAL;
    break;
  }

  if(!arg || !access_ok(VERIFY_WRITE, 
        (u8 __user*)(uintptr_t)arg, sizeof(param)) )
    return -EFAULT;

  if(copy_to_user((void*)arg, (void*)&param, sizeof(param))){
    return -EFAULT;
  }
  return 0;
}

static int32_t tc317x_dxb_ioctl(struct inode *inode, struct file *filp, 
              uint32_t cmd, unsigned long arg)
{
  int32_t ret = 0;

  switch (cmd) {
  case IOCTL_TC317X_SET_FREQ:
    ret = tc317x_dxb_set_frequency(arg);
    break;

  case IOCTL_TC317X_SELECT_SUBCH:
    ret = tc317x_dxb_sevice_ctrl(SUBCH_SELECT,  arg);
    break;

  case IOCTL_TC317X_RELEASE_SUBCH:
    ret = tc317x_dxb_sevice_ctrl(SUBCH_RELEASE,  arg);
    break;

  case IOCTL_TC317X_GET_MON_STAT:
    ret = tc317x_dxb_get_mon_stat(arg);
    break;

  case IOCTL_TC317X_SET_INTR_THRESHOLD_SIZE:
    ret = tc317x_dxb_set_inttrupt_threshold_size(arg);
    break;

  case IOCTL_TC317X_RW_REG:
    ret = tc317x_dxb_rw_reg(arg);
    break;

  case IOCTL_TC317X_TEST:
    tc317x_test();
    break;
  default:
    TCDBG("bl: unrecognized ioctl (0x%x)\n", cmd); 
    ret = -EINVAL;
  }

  return ret;
}

static int32_t tc317x_dxb_release(struct inode *inode, struct file *filp)
{
  TC317X_API_Close(0);
  tc317x_drv_data.open_cnt--;
  TCDBG("\n");
  return 0;
}

static int32_t tc317x_dxb_open(struct inode *inode, struct file *filp)
{
  int32_t ret;

  TCDBG("\n");
  if(tc317x_drv_data.open_cnt){
    TCDBG("driver aready opened!\n");
    return -EIO;
  }

  TC317X_API_Init(0, 1);
  TC317X_API_IOOpen(0, tc317x_drv_data.cmd_if, tc317x_drv_data.data_if);
  if(tc317x_err != TC3X_IO_SUCCESS){
    TCDBG("io open err\n");
    return -EIO;
  }

  TC317X_API_BBOpen(0);
  TC317X_API_BBPreInit(0);
  TC317X_API_PreSet_PLLData(0, 0x60,0x0,0xf,0x03,19200);
  ret = TC317X_API_ColdBootTable(0, TCC317X_BOOT_DATA_TDMB, 
      TCC317X_BOOT_SIZE_TDMB, CMD_BB_COLDBOOT_NORMAL);

  if(ret != TC3X_IO_FAIL)
    TC317X_API_BBInit(0);
  else {
    TCDBG("param download failed!\n");
    return -EIO;
  }
  tc317x_drv_data.open_cnt++;
  return 0;
}

#if !defined(CMD_IFACE_I2C)
static ssize_t tc317x_dxb_write(
  struct file *flip, const char *buf, size_t len, loff_t *ppos)
{
  return 0;
}

static ssize_t tc317x_dxb_read(
  struct file *flip, char *buf, size_t len, loff_t *ppos)
{
  int32_t ret;
  uint32_t qdsize, copied_size;

  //mutex_lock(&tc317x_drv_data.fifo_lock);
  if(kfifo_is_empty(&(tc317x_drv_data.fifo))){
    //mutex_unlock(&tc317x_drv_data.fifo_lock);
    TCDBG("fifo empty! \n");
    return 0;
  }

  kfifo_out(&(tc317x_drv_data.fifo), (void*)&qdsize, sizeof(uint32_t));
  if(qdsize > MAX_FIFO_SIZE){
    kfifo_in(&(tc317x_drv_data.fifo), (void*)&qdsize, sizeof(uint32_t));
    //mutex_unlock(&tc317x_drv_data.fifo_lock);
    TCDBG("worng size! %u\n", qdsize);
    return -EFAULT;
  }

  if(!access_ok(VERIFY_WRITE, 
        (u8 __user*)(uintptr_t)buf, qdsize) ){
    //mutex_unlock(&tc317x_drv_data.fifo_lock);
    return -EFAULT;
  }

  ret = kfifo_to_user(&(tc317x_drv_data.fifo), buf, qdsize, &copied_size); 
  if(ret) {
    TCDBG("could not copy data to user buffer %d\n", ret);
    //mutex_unlock(&tc317x_drv_data.fifo_lock);
    return -EFAULT;
  }
  if(qdsize != copied_size){
    TCDBG("queued size and copied size miss match"
        "size:%u, copied_size:%u\n", qdsize, copied_size);
  }
  //mutex_unlock(&tc317x_drv_data.fifo_lock);

  return copied_size;
}

static uint32_t tc317x_dxb_poll(
  struct file *flip, struct poll_table_struct *wait)
{
  if(!kfifo_is_empty(&(tc317x_drv_data.fifo))){
    return (POLLIN | POLLRDNORM);
  }

  poll_wait(flip, &(tc317x_drv_data.wait_q), wait);

  if(!kfifo_is_empty(&(tc317x_drv_data.fifo))){
    return (POLLIN | POLLRDNORM);
  }
  return 0;
}
#endif

struct file_operations tc317x_dxb_fops =
{
  .owner    = THIS_MODULE,
  .open     = tc317x_dxb_open,
  .release  = tc317x_dxb_release,
  .ioctl    = tc317x_dxb_ioctl,
#if !defined(CMD_IFACE_I2C)
  .read     = tc317x_dxb_read,
  .write    = tc317x_dxb_write,
  .poll     = tc317x_dxb_poll,
#endif
};

#if defined(CMD_IFACE_I2C)
#ifdef NEW_STYLE_DRIVER
static struct i2c_board_info i2c_device[] = {
  {I2C_BOARD_INFO("tc317x", I2C_ADDR ), NULL}
};
#endif
#endif
static struct class *tc317x_dxb_class;

static int32_t __init tc317x_dxb_init(void)
{
  int32_t res;
#if defined(CMD_IFACE_I2C)
  tc317x_drv_data.cmd_if = TC3X_IF_I2C;
  tc317x_drv_data.data_if = TC3X_STREAM_IO_STS;
#ifdef NEW_STYLE_DRIVER
  i2c_register_board_info(I2C_BUS, i2c_device, ARRAY_SIZE(i2c_device));
#endif
#elif defined(CMD_IFACE_CSPI)
  tc317x_drv_data.cmd_if = TC3X_IF_CSPI;
  tc317x_drv_data.data_if = TC3X_STREAM_IO_MAINIO;
#endif

  res = register_chrdev(TC317X_DEV_MAJOR, TC317X_DEV_NAME, &tc317x_dxb_fops);
  if (res < 0) return res;

  tc317x_dxb_class = class_create(THIS_MODULE, TC317X_DEV_NAME);
  if(NULL == device_create(tc317x_dxb_class, NULL, 
              MKDEV(TC317X_DEV_MAJOR, TC317X_DEV_MINOR), NULL, TC317X_DEV_NAME))
    TCDBG("device_create failed\n");      
#ifdef TC_AP
  RGPIO = (volatile PGPIO)tcc_p2v(HwGPIO_BASE);
  RPIC = (volatile PPIC)tcc_p2v(HwPIC_BASE);
#endif

  init_waitqueue_head(&(tc317x_drv_data.wait_q));
  mutex_init(&tc317x_drv_data.fifo_lock);

  if(kfifo_alloc(&(tc317x_drv_data.fifo), (MAX_FIFO_SIZE), GFP_KERNEL)){
    TCDBG("kfifo alloc failed!\n");
  }
  return 0;
}

static void __exit tc317x_dxb_exit(void)
{
  device_destroy(tc317x_dxb_class, MKDEV(TC317X_DEV_MAJOR, TC317X_DEV_MINOR));
  class_destroy(tc317x_dxb_class);
  unregister_chrdev(TC317X_DEV_MAJOR, TC317X_DEV_NAME);
  printk(KERN_INFO "%s\n", __FUNCTION__);
}

module_init(tc317x_dxb_init);
module_exit(tc317x_dxb_exit);

MODULE_AUTHOR("Telechips Inc.");
MODULE_DESCRIPTION("TCC317X control");
MODULE_LICENSE("GPL");

