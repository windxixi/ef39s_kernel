//=============================================================================
// File       : Tdmb_dev.c
//
// Description: 
//
// Revision History:
//
// Version         Date           Author        Description of Changes
//-----------------------------------------------------------------------------
//  1.0.0       2009/05/06       yschoi         Create
//=============================================================================

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/errno.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <mach/board.h>
#include <mach/gpio.h>
#include <linux/kthread.h>

#include "tdmb_comdef.h"
#include "tdmb_bb.h"
#include "tdmb_dev.h"
#include "tdmb_gpio.h"
#include "tdmb_test.h"

#ifdef FEATURE_QTDMB_I2C_CMD
#include "tdmb_i2c.h"
#endif

#ifdef FEATURE_QTDMB_TSIF_IF
#include "tdmb_tsif.h"
#endif

#ifdef FEATURE_QTDMB_SPI_IF
#include "tdmb_spi.h"
#endif

#ifdef FEATURE_QTDMB_USE_INC_T3900
#include "t3900/t3900_bb.h"
#include "t3900/t3900_includes.h"
#endif

#ifdef FEATURE_QTDMB_USE_INC_T3700
#include "t3700/t3700_bb.h"
#include "t3700/t3700_includes.h"
#endif

#ifdef FEATURE_QTDMB_USE_FCI_FC8050
#include "fc8050/fc8050_regs.h"
#ifdef FEATURE_QTDMB_TSIF_IF
#include "fc8050/fc8050_demux.h"
#endif
#endif

#ifdef FEATURE_QTDMB_USE_RTV_MTV350
#include "mtv350/mtv350_bb.h"
#endif /* FEATURE_QTDMB_USE_RTV */

#ifdef FEATURE_QTDMB_USE_TCC_TCC3170
#include "tcc3170/tcc3170_bb.h"
#endif


/*================================================================== */
/*================      TDMB Module Definition     ================= */
/*================================================================== */

#if (defined(FEATURE_TEST_ON_BOOT) || defined(FEATURE_NETBER_TEST_ON_BOOT))
#define FEATURE_TEST_INT
#endif

//#define FEATURE_TDMB_IGNORE_1ST_INT

struct tdmb_dev {
  struct cdev cdev;
  struct device *dev;
  struct fasync_struct *fasync; // async notification
  wait_queue_head_t wq;
  int irq;
};

static struct tdmb_dev *tdmb_device;
static dev_t tdmb_dev_num;
static struct class *tdmb_dev_class;

static int tdmb_dvevice_major;
static int DEVICE_OPEN;
static bool power_on_flag = FALSE;


ts_data_type ts_data;

fic_data_type fic_data;

#ifdef FEATURE_QTDMB_USE_FCI
extern tdmb_bb_int_type fci_int_type;
#endif /* FEATURE_QTDMB_USE_FCI */


extern tdmb_mode_type dmb_mode;
extern tdmb_autoscan_state autoscan_state;
extern uint8 gFrequencyBand;
extern uint8 gFreqTableNum;

extern tSignalQuality g_tSigQual;


#ifdef CONFIG_SKY_TDMB_MICRO_USB_DETECT
int pm8058_is_dmb_ant(void);
#endif
static int uUSB_ant_detect = 0;

#ifndef FEATURE_TS_PKT_MSG
int g_tdmb_interrup_cnt = 0;
#endif /* FEATURE_TS_PKT_MSG */

static int play_start = 0;
static int first_tdmb_int_flag = 0;



/*================================================================== */
/*================      TDMB Module Functions      ================= */
/*================================================================== */

static int tdmb_open(struct inode *inode, struct file *file);
static int tdmb_release(struct inode *inode, struct file *file);
static ssize_t tdmb_read(struct file *filp, char *buffer, size_t length, loff_t *);
static ssize_t tdmb_write(struct file *filp, const char *buffer, size_t length, loff_t *offset);
static int tdmb_ioctl(struct inode *inodep, struct file *filp, unsigned int cmd, unsigned long arg);
static int tdmb_fasync(int fd, struct file *file, int on);

#ifdef FEATURE_QTDMB_TSIF_IF
irqreturn_t tdmb_interrupt(int irq, void *dev_id);
#else
static irqreturn_t tdmb_interrupt(int irq, void *dev_id);
#endif

#if (defined(FEATURE_TEST_INT) && !defined(FEATURE_TDMB_THREAD))
static void tdmb_test_interrupt(void);
#endif /* FEATURE_TEST_INT */

static void tdmb_send_sig(void);
static void tdmb_read_data(void);

#ifdef FEATURE_TDMB_THREAD
static DECLARE_WAIT_QUEUE_HEAD(tdmb_isr_wait);
static u8 tdmb_isr_sig = 0;
static struct task_struct *tdmb_kthread = NULL;
#endif

static struct file_operations tdmb_fops = {
  .owner    = THIS_MODULE,
  .ioctl    = tdmb_ioctl,
  .open     = tdmb_open,
  .release  = tdmb_release,
  .read     = tdmb_read,
  .write    = tdmb_write,
  .fasync   = tdmb_fasync,
};

/*====================================================================
FUNCTION       tdmb_ioctl  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
static int tdmb_ioctl(struct inode *inodep, struct file *filp, unsigned int cmd, unsigned long arg)
{
  unsigned long flags;
  //unsigned int data_buffer_length = 0;
  void __user *argp = (void __user *)arg;

  get_frequency_type freq;
  ch_scan_type ch_scan;
  chan_info ch_info;
  g_var_type gVar;

  //TDMB_MSG_DEV("[%s] ioctl cmd [%x] enum[%d]\n", __func__, cmd, _IOC_NR(cmd));
  //TDMB_MSG_DEV("[%s] ioctl cmd_enum[%d]\n", __func__, _IOC_NR(cmd));

#if 0 // not used
  /* First copy down the buffer length */
  if (copy_from_user(&data_buffer_length, argp, sizeof(unsigned int)))
    return -EFAULT;
#endif // 0

  if(_IOC_TYPE(cmd) != IOCTL_TDMB_MAGIC)
  {
    TDMB_MSG_DEV("[%s] invalid Magic Char [%c]\n", __func__, _IOC_TYPE(cmd));
    return -EINVAL;
  }
  if(_IOC_NR(cmd) >= IOCTL_TDMB_MAXNR)
  {
    return -EINVAL;
  }

#if 0 // not used
  size = _IOC_SIZE(cmd);

  if(size)
  {
    err = 0;

    if(_IOC_DIR(cmd) & _IOC_READ)
      err = verify_area(VERIFY_WRITE, (void *) arg, size);
    else if(_IOC_DIR(cmd) & _IOC_WRITE)
      err = verify_area(VERIFY_READ, (void *) arg, size);

    if (err)
      return err;
  }
#endif // 0

  switch(cmd)
  {
    case IOCTL_TDMB_BB_DRV_INIT:
#ifdef FEATURE_APP_CALL_TEST_FUNC
      TDMB_MSG_DEV("[%s] blocked for test func\n", __func__);
#else
      tdmb_bb_drv_init();
      power_on_flag = TRUE;
#endif

      play_start = 0;
      break;

    case IOCTL_TDMB_BB_INIT:
      tdmb_bb_init();
      break;

    case IOCTL_TDMB_BB_POWER_ON:
      tdmb_bb_power_on();
      break;

    case IOCTL_TDMB_BB_POWER_OFF:
      if(power_on_flag)
      {
        tdmb_bb_power_off();
        power_on_flag = FALSE;
      }

      play_start = 0;
      break;

    case IOCTL_TDMB_BB_SET_ANTENNA_PATH:
#ifdef FEATURE_APP_CALL_TEST_FUNC
      TDMB_MSG_DEV("[%s] blocked for test func\n", __func__);
#else
      if(copy_from_user(&flags, argp, sizeof(flags)))
        return -EFAULT;

      tdmb_bb_set_antenna_path(flags);
      //tdmb_bb_set_antenna_path(TDMB_ANT_EXTERNAL);
#endif
      break;

    case IOCTL_TDMB_BB_CH_SCAN_START:
      if(copy_from_user(&ch_scan, argp, sizeof(ch_scan_type)))
        return -EFAULT;

      tdmb_bb_ch_scan_start(ch_scan.freq, ch_scan.band, ch_scan.freq_offset);
      break;

    case IOCTL_TDMB_BB_GET_FREQUENCY:
      if(copy_from_user(&freq, argp, sizeof(get_frequency_type)))
        return -EFAULT;

      tdmb_bb_get_frequency(&(freq.freq), freq.band, freq.index);
      //TDMB_MSG_DEV("[%s] IOCTL_TDMB_BB_GET_FREQUENCY [%d]\n", __func__, (unsigned int)freq.freq);

      if(copy_to_user(argp, &freq, sizeof(get_frequency_type)))
        return -EFAULT;
      break;

    case IOCTL_TDMB_BB_FIC_PROCESS:
      tdmb_bb_fic_process();
      break;

    case IOCTL_TDMB_BB_SET_FIC_ISR:
      if(copy_from_user(&flags, argp, sizeof(flags)))
        return -EFAULT;

      flags = tdmb_bb_set_fic_isr(flags);

      if(copy_to_user(argp, &flags, sizeof(flags)))
        return -EFAULT;
      break;

    case IOCTL_TDMB_BB_CHW_INTHANDLER2:
      tdmb_bb_chw_IntHandler2();
      break;

    case IOCTL_TDMB_BB_EBI2_CHW_INTHANDLER:
      if(copy_from_user(&flags, argp, sizeof(flags)))
        return -EFAULT;

      flags = tdmb_bb_ebi2_chw_IntHandler((uint8 *)flags);

      if(copy_to_user(argp, &flags, sizeof(flags)))
        return -EFAULT;
      break;

    case IOCTL_TDMB_BB_RESYNC:
      if(copy_from_user(&flags, argp, sizeof(flags)))
        return -EFAULT;

      tdmb_bb_resync(flags);
      break;

    case IOCTL_TDMB_BB_SUBCH_START:
#ifdef FEATURE_APP_CALL_TEST_FUNC
      TDMB_MSG_DEV("[%s] blocked for test func\n", __func__);
      tdmb_bb_ch_test(TDMB_TEST_CH);
#else
      flags = tdmb_bb_subch_start(0, 0);

      if(dmb_mode == TDMB_MODE_NETBER)
      {
        netber_init();
      }
      play_start = 1;

      if(copy_to_user(argp, &flags, sizeof(flags)))
        return -EFAULT;
#endif
      break;

    case IOCTL_TDMB_BB_DRV_START:
      tdmb_bb_drv_start();
      break;

    case IOCTL_TDMB_BB_DRV_STOP:
      tdmb_bb_drv_stop();
      break;

    case IOCTL_TDMB_BB_REPORT_DEBUG_INFO:
      tdmb_bb_report_debug_info();
      break;

    case IOCTL_TDMB_BB_GET_TUNING_STATUS:
      flags = tdmb_bb_get_tuning_status();

      if(copy_to_user(argp, &flags, sizeof(flags)))
        return -EFAULT;
      break;

    case IOCTL_TDMB_BB_SET_FIC_CH_RESULT:
      if(copy_from_user(&flags, argp, sizeof(flags)))
        return -EFAULT;

      tdmb_bb_set_fic_ch_result(flags);
      break;

    case IOCTL_TDMB_BB_GET_FIC_CH_RESULT:
      flags = tdmb_bb_get_fic_ch_result();

      if(copy_to_user(argp, &flags, sizeof(flags)))
        return -EFAULT;
      break;

    case IOCTL_TDMB_BB_READ_INT:
      flags = tdmb_bb_read_int();

      if(copy_to_user(argp, &flags, sizeof(flags)))
        return -EFAULT;
      break;

    case IOCTL_TDMB_BB_GET_SYNC_STATUS:
      flags = tdmb_bb_get_sync_status();

      if(copy_to_user(argp, &flags, sizeof(flags)))
        return -EFAULT;
      break;

    case IOCTL_TDMB_BB_READ_FIB:
#if (defined(FEATURE_QTDMB_USE_FCI) && defined(FEATURE_QTDMB_SPI_IF) && !defined(FEATURE_TDMB_THREAD))
      fci_int_type = tdmb_bb_read_int();
#endif
      fic_data.fib_num = tdmb_bb_read_fib(fic_data.fic_buf);

      if(copy_to_user(argp, &fic_data, sizeof(fic_data)))
        return -EFAULT;
      break;

    case IOCTL_TDMB_BB_SET_SUBCHANNEL_INFO:
#ifdef FEATURE_APP_CALL_TEST_FUNC
      TDMB_MSG_DEV("[%s] blocked for test func\n", __func__);
#else

#ifdef FEAUTRE_USE_FIXED_FIC_DATA
      tdmb_get_fixed_chan_info(TDMB_TEST_CH, &ch_info);
#else
      if(copy_from_user(&ch_info, argp, sizeof(chan_info)))
        return -EFAULT;
#endif /* FEAUTRE_USE_FIXED_FIC_DATA */

#ifdef FEATURE_TDMB_VISUAL_RADIO_SERVICE
      if (ch_info.uiServiceType == TDMB_BB_SVC_VISUALRADIO)
      {
        ch_info.uiServiceType = TDMB_BB_SVC_DMB;
      }
#endif

      TDMB_MSG_DEV("[%s] ch_info.ulRFNum [%d]\n", __func__, (unsigned int)ch_info.ulRFNum);
      TDMB_MSG_DEV("[%s] ch_info.uiEnsumbleID [0x%x]\n", __func__, ch_info.uiEnsumbleID);
      TDMB_MSG_DEV("[%s] ch_info.uiSubChID [0x%x]\n", __func__, ch_info.uiSubChID);
      TDMB_MSG_DEV("[%s] ch_info.uiServiceType [0x%x]\n", __func__, ch_info.uiServiceType);
      //TDMB_MSG_DEV("[%s] ch_info.uiStarAddr [0x%x]\n", __func__, ch_info.uiStarAddr);
      TDMB_MSG_DEV("[%s] ch_info.uiBitRate [%d]\n", __func__, ch_info.uiBitRate);
      //TDMB_MSG_DEV("[%s] ch_info.uiTmID [%d]\n", __func__, ch_info.uiTmID);

      //TDMB_MSG_DEV("[%s] ch_info.uiSlFlag [%d]\n", __func__, ch_info.uiSlFlag);
      //TDMB_MSG_DEV("[%s] ch_info.ucTableIndex [%d]\n", __func__, ch_info.ucTableIndex);
      //TDMB_MSG_DEV("[%s] ch_info.ucOption [%d]\n", __func__, ch_info.ucOption);
      //TDMB_MSG_DEV("[%s] ch_info.uiProtectionLevel [%d]\n", __func__, ch_info.uiProtectionLevel);
      //TDMB_MSG_DEV("[%s] ch_info.uiDifferentRate [0x%x]\n", __func__, ch_info.uiDifferentRate);
      //TDMB_MSG_DEV("[%s] ch_info.uiSchSize [0x%x]\n", __func__, ch_info.uiSchSize);

      tdmb_bb_set_subchannel_info(&ch_info);
#endif /* FEATURE_APP_CALL_TEST_FUNC */
      break;

    case IOCTL_TDMB_BB_READ_MSC: // tdmb_read 로 대체
      TDMB_MSG_DEV("[%s] IOCTL_TDMB_BB_READ_MSC\n", __func__);
      break;

    case IOCTL_TDMB_BB_I2C_TEST:
      //tdmb_bb_i2c_test(arg);
      break;

    case IOCTL_TDMB_BB_GET_BER:
      if (dmb_mode == TDMB_MODE_AIR_PLAY)
      {
        tdmb_bb_get_ber();
      }

#if 0//(defined(FEATURE_QTDMB_SPI_IF) && defined(FEATURE_QTDMB_USE_FCI))
      tdmb_spi_clear_int();
#endif

#ifndef FEATURE_TS_PKT_MSG
      g_tdmb_interrup_cnt = 0;
#endif

      if(copy_to_user(argp, &g_tSigQual, sizeof(tSignalQuality)))
        return -EFAULT;
      break;

    case IOCTL_TDMB_BB_CH_STOP:
      tdmb_bb_ch_stop();
      play_start = 0;
      ts_data.type = TYPE_NONE;

#if (defined(FEATURE_QTDMB_USE_FCI) && defined(FEATURE_QTDMB_SPI_IF) && !defined(FEATURE_TDMB_THREAD))
      fci_int_type = tdmb_bb_read_int();
#endif
      break;

    case IOCTL_TDMB_BB_CH_TEST:
      if(copy_from_user(&flags, argp, sizeof(flags)))
        return -EFAULT;

      tdmb_bb_ch_test(flags);
      break;

    case IOCTL_TDMB_GVAR_RW:
      if(copy_from_user(&gVar, argp, sizeof(g_var_type)))
        return -EFAULT;

      if(gVar.type == G_READ)
      {
        switch(gVar.name)
        {
        case G_DMB_MODE:
          gVar.data = dmb_mode;
          break;
        case G_GFREQUENCYBAND:
          gVar.data = gFrequencyBand;
          break;
        case G_GFREQTABLENUM:
          gVar.data = gFreqTableNum;
          break;
        case G_AUTOSCAN_STATE:
          gVar.data = autoscan_state; // not used
          break;
        case G_MICRO_USB_ANT:
          uUSB_ant_detect = 0;
#ifdef CONFIG_SKY_TDMB_MICRO_USB_DETECT
          uUSB_ant_detect = pm8058_is_dmb_ant();
#endif
          gVar.data = uUSB_ant_detect;
          break;
        default:
          break;
        }

        if(copy_to_user(argp, &gVar, sizeof(g_var_type)))
          return -EFAULT;
      }
      else if(gVar.type == G_WRITE)
      {
        switch(gVar.name)
        {
        case G_DMB_MODE:
          dmb_mode = gVar.data;
          break;
        case G_GFREQUENCYBAND:
          gFrequencyBand = gVar.data;
          break;
        case G_GFREQTABLENUM:
          gFreqTableNum = gVar.data;
          break;
        case G_AUTOSCAN_STATE:
          autoscan_state = gVar.data; // not used
          break;
        default:
          break;
        }
      }

      break;

    default:
      TDMB_MSG_DEV("[%s] unknown command!!!\n", __func__);
      break;
  }

  //TDMB_MSG_DEV("[%s] end!!!\n", __func__);
  return 0;
}


/*====================================================================
FUNCTION       tdmb_write  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
static ssize_t tdmb_write(struct file *filp, const char *buffer, size_t length, loff_t *offset)
{
  TDMB_MSG_DEV("[%s] write\n", __func__);
  return 0;
}


/*====================================================================
FUNCTION       tdmb_read  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
static ssize_t tdmb_read(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
  //TDMB_MSG_DEV("[%s] read\n", __func__);

#if (defined(FEATURE_QTDMB_SPI_IF) && !defined(FEATURE_TDMB_THREAD))
#ifdef FEATURE_QTDMB_USE_FCI
  fci_int_type = tdmb_bb_read_int();
#endif

  ts_data.type = TYPE_NONE;
  ts_data.fic_size = 0;
  ts_data.msc_size = 0;

  ts_data.msc_size = tdmb_bb_read_msc(ts_data.msc_buf);
#endif /* FEATURE_QTDMB_SPI_IF */

  //TDMB_MSG_DEV("[%s] copy_to_user\n", __func__);

  if (copy_to_user((void __user *)buffer, &ts_data, length))
    return -EFAULT;

  //TDMB_MSG_DEV("[%s] tdmb_read end\n", __func__);

  if (dmb_mode == TDMB_MODE_NETBER)
  {
    netber_GetError(ts_data.msc_size, ts_data.msc_buf);
  }

  //TDMB_MSG_DEV("[%s] read end\n", __func__);

  return 0;
}


/*====================================================================
FUNCTION       tdmb_open  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
static int tdmb_open(struct inode *inode, struct file *file)
{
  TDMB_MSG_DEV("[%s] open\n", __func__);

  //file->private_data = tdmb_device;

  if(DEVICE_OPEN){
    TDMB_MSG_DEV("[%s] already opened\n", __func__);
    return -EBUSY;
  }
  DEVICE_OPEN++;

  return 0;
}


/*====================================================================
FUNCTION       tdmb_release  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
static int tdmb_release(struct inode *inode, struct file *file)
{
  TDMB_MSG_DEV("[%s] release\n", __func__);

  DEVICE_OPEN--;

  return 0;
}


/*====================================================================
FUNCTION       tdmb_fasync  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
static int tdmb_fasync(int fd, struct file *file, int on)
{
  int err;

  TDMB_MSG_DEV("[%s]\n", __func__);

  err = fasync_helper(fd, file, on, &tdmb_device->fasync);
  if (err < 0)
    return err;

  //TDMB_MSG_DEV("[%s] tdmb_fasync [0x%8x]\n", __func__, tdmb_device->fasync);

  return 0;
}



/*================================================================== */
/*============      TDMB handler interrupt setting     ============= */
/*================================================================== */

/*====================================================================
FUNCTION       tdmb_set_isr  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
boolean tdmb_set_isr(int on_off)
{
#ifdef FEATURE_QTDMB_USE_RTV_MTV350 // 20101110 cys MTV350 TSIF에서 인터럽트 발생
  return TRUE;
#endif

  int irq;

  if(on_off)
  {
    first_tdmb_int_flag = 1;

    irq = gpio_to_irq(DMB_INT);
    if (request_irq(irq, tdmb_interrupt, IRQF_TRIGGER_FALLING, TDMB_DEV_NAME, tdmb_device))
    {
      TDMB_MSG_DEV ("[%s] unable to get IRQ %d.\n", __func__, DMB_INT);
      return FALSE;
    }
  }
  else
  {
    first_tdmb_int_flag = 0;

    free_irq(gpio_to_irq(DMB_INT), tdmb_device);
    TDMB_MSG_DEV ("[%s] free irq\n", __func__);

 #if (defined(FEATURE_QTDMB_USE_FCI) && !defined(FEATURE_TDMB_THREAD))
    fci_int_type = tdmb_bb_read_int();
 #endif
  }

  return TRUE;
}


/*====================================================================
FUNCTION       tdmb_interrupt  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
#ifdef FEATURE_QTDMB_TSIF_IF
irqreturn_t tdmb_interrupt(int irq, void *dev_id)
#else
static irqreturn_t tdmb_interrupt(int irq, void *dev_id)
#endif
{
#ifndef FEATURE_TS_PKT_MSG
  if(g_tdmb_interrup_cnt >= 100) // get_ber 을 하지 않는 경우를 위해, 정해진 간격으로 인터럽트가 뜨는지 메시지로 보여줌.
  {
    TDMB_MSG_DEV ("[%s] irq[%d] count[%d]\n", __func__, irq, g_tdmb_interrup_cnt);
    g_tdmb_interrup_cnt = 0;
  }
  else
  {
    g_tdmb_interrup_cnt ++;
  }
#else
  TDMB_MSG_DEV ("[%s] irq[%d]\n", __func__, irq);
#endif /* FEATURE_TS_PKT_MSG */

#ifdef FEATURE_TDMB_IGNORE_1ST_INT
  if(first_tdmb_int_flag)
  {
    TDMB_MSG_DEV ("[%s] ignore 1st interrupt after tdmb_set_isr(on)\n", __func__);
    first_tdmb_int_flag = 0;

    return IRQ_HANDLED;
  }
#endif

#if (defined(FEATURE_TEST_INT) && !defined(FEATURE_TDMB_THREAD))
  tdmb_test_interrupt();

  return IRQ_HANDLED;
#endif /* FEATURE_TEST_INT */

#ifdef FEATURE_TDMB_THREAD
  tdmb_isr_sig = 1;

  wake_up_interruptible(&tdmb_isr_wait);
#else
  tdmb_read_data();
#endif

  return IRQ_HANDLED;
}

#ifdef FEATURE_QTDMB_TSIF_IF
EXPORT_SYMBOL(tdmb_interrupt);
#endif /* FEATURE_QTDMB_TSIF_IF */


#if (defined(FEATURE_TEST_INT) && !defined(FEATURE_TDMB_THREAD))
/*====================================================================
FUNCTION       tdmb_test_interrupt  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
static void tdmb_test_interrupt(void)
{
#ifdef FEATURE_QTDMB_TSIF_IF
  TDMB_MSG_DEV ("[%s] TSIF test\n", __func__);
#else // EBI2, SPI
  static boolean first_int = FALSE;

  TDMB_MSG_DEV ("[%s] EBI2, SPI test\n", __func__);

  if(first_int)
  {
    tdmb_read_data();
  }

  first_int = TRUE;
#endif /* FEATURE_QTDMB_TSIF_IF */
}
#endif /* FEATURE_TEST_INT */


/*====================================================================
FUNCTION       tdmb_send_sig  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
static void tdmb_send_sig(void)
{
  // send signal to framework
  kill_fasync(&tdmb_device->fasync, SIGIO, POLL_IN);

  //TDMB_MSG_DEV ("[%s] kill_fasync[0x%8x]\n", __func__, &tdmb_device->fasync);
  //TDMB_MSG_DEV ("[%s] kill_fasync\n", __func__);
}


#ifdef FEATURE_TDMB_THREAD
/*====================================================================
FUNCTION       tdmb_thread  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
static int tdmb_thread(void *x)
{
  TDMB_MSG_DEV ("[%s] \n", __func__);

#if (defined(CONFIG_SKY_EF39S_BOARD) || defined(CONFIG_SKY_EF40S_BOARD) || defined(CONFIG_SKY_EF40K_BOARD))
  set_user_nice(current, 0);
#else
  set_user_nice(current, -20);
#endif

  while(1)
  {
    wait_event_interruptible(tdmb_isr_wait, tdmb_isr_sig || kthread_should_stop());

    tdmb_isr_sig = 0;

#if (defined(FEATURE_QTDMB_SPI_IF) && defined(FEATURE_QTDMB_USE_FCI))
    if (dmb_mode == TDMB_MODE_AIR_PLAY)
    {
      tdmb_spi_clear_int();
    }
#endif

    tdmb_read_data();

    if(kthread_should_stop())
      break;
  }

  TDMB_MSG_DEV ("[%s] end\n", __func__);
  
  return 0;
}

#endif


/*====================================================================
FUNCTION       tdmb_tsif_data_parser  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
static void tdmb_read_data(void)
{
  bool read_msc = FALSE;

  //TDMB_MSG_DEV ("[%s] \n", __func__);

  ts_data.type = TYPE_NONE;
  ts_data.fic_size = 0;
  ts_data.msc_size = 0;

#ifdef FEATURE_QTDMB_USE_FCI
 #if (defined(FEATURE_QTDMB_EBI_IF) || defined(FEATURE_QTDMB_SPI_IF))
  fci_int_type = tdmb_bb_read_int();

  if(play_start == 1)
  {
    if(fci_int_type == TDMB_BB_INT_MSC)
    {
      read_msc = TRUE;
    }
    else // TDMB_BB_INT_FIC 인 경우 리턴
    {
      return;
      //read_msc = FALSE;
    }
  }
 #else // TSIF
  if(play_start == 0)
  {
    fci_int_type = tdmb_bb_read_int();
  }
 #endif
#elif defined(FEATURE_QTDMB_USE_TCC)
 #if (defined(FEATURE_QTDMB_EBI_IF) || defined(FEATURE_QTDMB_SPI_IF))
    tdmb_bb_read_int();
 #endif 
   if(play_start == 1)
   {
     read_msc = TRUE;
   }
#else
  read_msc = TRUE;
#endif

#if (defined(FEATURE_QTDMB_EBI_IF) || defined(FEATURE_QTDMB_SPI_IF))
  if(read_msc)
  {
    ts_data.msc_size = tdmb_bb_read_msc(ts_data.msc_buf);
  }
#endif

  tdmb_send_sig();

  //TDMB_MSG_DEV ("[%s] end\n", __func__);

  return;
}


#ifdef FEATURE_QTDMB_TSIF_IF
/*====================================================================
FUNCTION       tdmb_tsif_data_parser  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
void tdmb_tsif_data_parser(char* user_buf, void * data_buffer, int size)
{
  int i, packet_size;

  //TDMB_MSG_DEV ("[%s]\n", __func__);

  if(data_buffer == NULL || size <= 0)
  {
    TDMB_MSG_DEV("[%s] TSIF data buffer [%x] Or Size [%d]",__func__, (int)data_buffer, size);
    ts_data.type = TYPE_NONE;
    ts_data.fic_size = 0;
    ts_data.msc_size = 0;

    if (copy_to_user((void __user *)user_buf, &ts_data, sizeof(ts_data)))
    {
      return;
    }

    return;
  }

  packet_size = (size / TSIF_DATA_SIZE) * TDMB_TS_PKT_SIZE;

  if (packet_size > BB_MAX_DATA_SIZE)
  {
    TDMB_MSG_DEV("[%s] overflow! size[%d], packet_size[%d].cnt[%d]\n", __func__, size, packet_size, packet_size/TDMB_TS_PKT_SIZE);
    TDMB_MSG_DEV("[%s] Max data buf[%d], Max TSIF buf[%d]\n", __func__, BB_MAX_DATA_SIZE, TSIF_DATA_SIZE*TSIF_CHUNK_SIZE);
    packet_size = BB_MAX_DATA_SIZE;
  }

  for(i=0; i < (packet_size/TDMB_TS_PKT_SIZE); i++)
  {
    memcpy((void*)&ts_data.msc_buf[i*TDMB_TS_PKT_SIZE], (data_buffer+i*TSIF_DATA_SIZE), TDMB_TS_PKT_SIZE);
  }

#ifdef FEATURE_QTDMB_MULTI_CHANNEL_ENABLE
  ts_data.type = TYPE_NONE;
  ts_data.fic_size = 0;
#if defined(FEATURE_QTDMB_USE_INC_T3900)
  ts_data.msc_size = t3700_header_parsing_tsif_data(ts_data.msc_buf, packet_size);
#elif defined(FEATURE_QTDMB_USE_FCI_FC8050)
  fc8050_demux(ts_data.msc_buf, packet_size);
#elif defined(FEATURE_QTDMB_USE_RTV_MTV350)

#elif defined(FEATURE_QTDMB_USE_TCC_TCC3170)

#else
  ##error
#endif /* FEATURE_QTDMB_USE_INC_T3900 */
#else // Single Channel
  ts_data.type = MSC_DATA;
  ts_data.fic_size = 0;
  ts_data.msc_size = packet_size;
#endif /* FEATURE_QTDMB_MULTI_CHANNEL_ENABLE */

  //TDMB_MSG_DEV ("[%s] fic_size[%d], msc_size[%d]\n", __func__, ts_data.fic_size, ts_data.msc_size);

#ifdef FEATURE_QTDMB_TSIF_READ_ONCE
  if (copy_to_user((void __user *)user_buf, &ts_data, sizeof(ts_data)))
  {
    return;
  }
#endif /* FEATURE_DMB_TSIF_READ_ONCE */

  if (dmb_mode == TDMB_MODE_NETBER)
  {
    netber_GetError(ts_data.msc_size, ts_data.msc_buf);
  }

  //TDMB_MSG_DEV ("[%s] end\n", __func__);
}

EXPORT_SYMBOL(tdmb_tsif_data_parser);
#endif /* FEATURE_QTDMB_TSIF_IF */



/*================================================================== */
/*=================      TDMB Module setting      ================== */
/*================================================================== */

/*====================================================================
FUNCTION       tdmb_probe  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
static int __devinit tdmb_probe(struct platform_device *pdev)
{
  TDMB_MSG_DEV ("[%s] pdev[0x%8x]\n", __func__, (unsigned int)pdev);

  //tdmb_device->dev = pdev;

#ifdef FEATURE_QTDMB_EBI_IF
  if (pdev->id == 0)
  {
    ebi2_tdmb_base = ioremap(pdev->resource[0].start,
    pdev->resource[0].end -
    pdev->resource[0].start + 1);
    if (!ebi2_tdmb_base)
    {
      TDMB_MSG_DEV("[%s] ebi2_tdmb_base ioremap failed!\n", __func__);
      return -ENOMEM;
    }
    else
    {
      TDMB_MSG_DEV("[%s] ebi2_tdmb_base[0x%x], start[0x%x], end[0x%x]\n", __func__, ebi2_tdmb_base, pdev->resource[0].start, pdev->resource[0].end);
    }
  }
  else
  {
      TDMB_MSG_DEV("[%s] pdev->id is not zero!\n", __func__);
  }
#endif /* FEATURE_QTDMB_EBI_IF */
  
  return 0;
}


/*====================================================================
FUNCTION       tdmb_remove  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
static int __devexit tdmb_remove(struct platform_device *pdev)
{
  TDMB_MSG_DEV ("[%s]\n", __func__);
  //struct tdmb_dev *tdmb_dev = platform_get_drvdata(pdev);

  free_irq(gpio_to_irq(DMB_INT), tdmb_device);
  //unregister device
  return 0;
}


/*====================================================================

======================================================================*/
static struct platform_driver tdmb_driver = {
  .probe    = tdmb_probe,
  .remove   = tdmb_remove,
//  .remove   = __devexit_p(tdmb_remove),
  .driver   = {
    .name   = TDMB_PLATFORM_DEV_NAME,
    .owner  = THIS_MODULE,
  },
};


/*====================================================================
FUNCTION       tdmb_init  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
int __init tdmb_init(void)
{
  int rc = 0;
  
  TDMB_MSG_DEV("[%s]\n", __func__);

#if 1
  // PLATFORM Driver
  platform_driver_register(&tdmb_driver);
  TDMB_MSG_DEV("[%s] platform_driver_register tdmb_driver\n", __func__);
#endif

  tdmb_device = kzalloc(sizeof(struct tdmb_dev), GFP_KERNEL);
  if (!tdmb_device)
  {
    TDMB_MSG_DEV("[%s] Unable to allocate memory for tdmb_dev\n", __func__);
    return -ENOMEM;
  }
  else
  {
    //TDMB_MSG_DEV("[%s] tdmb_device [0x%8x]\n", __func__, (unsigned int)tdmb_device);
  }

  // allocation device number
  rc = alloc_chrdev_region(&tdmb_dev_num, 0, 1, TDMB_DEV_NAME);
  if(rc < 0)
  {
    TDMB_MSG_DEV("[%s] alloc_chrdev_region Failed rc = %d\n", __func__, rc);
    return 0;
  }
  else
  {
    tdmb_dvevice_major = MAJOR(tdmb_dev_num);
    TDMB_MSG_DEV("[%s] registered with DeviceNum [0x%8x] Major [%d], Minor [%d]\n"
      , __func__, tdmb_dev_num, MAJOR(tdmb_dev_num), MINOR(tdmb_dev_num));
  }

  // create class
  tdmb_dev_class = class_create(THIS_MODULE, TDMB_DEV_NAME);
  if (!tdmb_dev_class)
  {
    rc = PTR_ERR(tdmb_dev_class);
    TDMB_MSG_DEV("[%s] couldn't create tdmb_dev_class rc = %d\n", __func__, rc);
    return 0;
  }
  else
  {
    //TDMB_MSG_DEV("[%s] class_create [0x%8x]\n", __func__, (unsigned int)tdmb_dev_class);
  }

  // create device
  tdmb_device->dev = device_create(tdmb_dev_class, NULL, tdmb_dev_num, NULL, TDMB_DEV_NAME);
  if (!tdmb_device->dev)
  {
    rc = PTR_ERR(tdmb_device->dev);
    TDMB_MSG_DEV("[%s] class_device_create failed %d\n", __func__, rc);
    return 0;
  }
  else
  {
    //TDMB_MSG_DEV("[%s] device_create, tdmb_device->dev[0x%8x]\n", __func__, (unsigned int)tdmb_device->dev);
  }

  // add character device
  cdev_init(&tdmb_device->cdev, &tdmb_fops);
  tdmb_device->cdev.owner = THIS_MODULE;

  rc = cdev_add(&(tdmb_device->cdev), tdmb_dev_num, 1);
  if (rc < 0) {
    TDMB_MSG_DEV("[%s] cdev_add failed\n", __func__);
  }
  else
  {
    //TDMB_MSG_DEV("[%s] cdev_add\n", __func__);
  }

  init_waitqueue_head(&tdmb_device->wq);

  // init tdmb bb function table
  tdmb_bb_func_tbl_init();

  // 20101102 cys
#ifdef FEATURE_TDMB_GPIO_INIT
  tdmb_gpio_init();
#endif /* FEATURE_TDMB_GPIO_INIT */

#ifdef FEATURE_QTDMB_I2C_CMD
  tdmb_i2c_api_Init();
#endif /* FEATURE_QTDMB_I2C_CMD */

#ifdef FEATURE_QTDMB_SPI_CMD
  tdmb_spi_init();
#endif

// TEST_ON_BOOT
#if (defined(FEATURE_TEST_ON_BOOT) || defined(FEATURE_NETBER_TEST_ON_BOOT))// 부팅중에 테스트
  tdmb_test_on_boot();
#endif /* TEST_ON_BOOT */

#ifdef FEATURE_TDMB_THREAD
  if(!tdmb_kthread)
  {
    tdmb_kthread = kthread_run(tdmb_thread, NULL, "tdmb_thread");

  }
#endif

  TDMB_MSG_DEV("[%s] tdmb_init end!!!\n", __func__);

  return 0;
}


/*====================================================================
FUNCTION       tdmb_exit  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
void __exit tdmb_exit(void)
{
  TDMB_MSG_DEV("[%s] exit!!!\n", __func__);

  cdev_del(&(tdmb_device->cdev));
  device_destroy(tdmb_dev_class, tdmb_dev_num);  
  class_destroy(tdmb_dev_class);
  kfree(tdmb_device);

  platform_driver_unregister(&tdmb_driver);
  unregister_chrdev_region(tdmb_dvevice_major, 0);

#ifdef FEATURE_TDMB_THREAD
  kthread_stop(tdmb_kthread);
#endif

  TDMB_MSG_DEV("[%s] unregister tdmb_driver\n", __func__);
}


module_init(tdmb_init);
module_exit(tdmb_exit);


MODULE_LICENSE("GPL");

