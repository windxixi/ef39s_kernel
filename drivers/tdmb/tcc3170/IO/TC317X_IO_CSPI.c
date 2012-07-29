//----------------------------------------------------------------------------
//   FileName    : TC317X_IO_CSPI.c
//    Description :
//----------------------------------------------------------------------------
//
//   TCC Version : 1.0.0
//   Copyright (c) Telechips, Inc.
//   ALL RIGHTS RESERVED
//
//--------------------------------------------------------------------------
#include <linux/spi/spi.h>
#include <linux/crc32.h>
#include "TC317X_IO_CSPI.h"
#include "TC317X_IO_UTIL.h"
#include "TC317X_IO.h"

#define SPICMD_VALID_BITS 36
#define SPICMD_BUFF_LEN    8
#define SPICMD_ACK      0x47

#define SPI_SPEED_HZ      10800000
#define SPI_BITS_PER_WORD 8

#define DMA_MAX_SIZE    (2048)

struct tc317x_tcspi_data_t {
  spinlock_t          spin_lock;
  unsigned char       dummy_buff[DMA_MAX_SIZE+SPICMD_BUFF_LEN*2];
  unsigned char       rw_buff[DMA_MAX_SIZE+SPICMD_BUFF_LEN*2];
  unsigned char       init_cmd_buf[SPICMD_BUFF_LEN]; /*Set all bit to 1*/
  struct spi_device*  spi_dev;
};

//static struct spi_device* find_spi_device(void);
static struct tc317x_tcspi_data_t tc317x_tcspi_data;

//static DEFINE_MUTEX(lock);

static int tc317x_spi_write_and_read(
    int moduleidx,
    unsigned char * pBufIn,
    unsigned char * pBufOut,
    unsigned int Length)
{

  struct spi_transfer xfer;
  struct spi_message msg;
  int ret = 0;
  struct tc317x_tcspi_data_t *spi_data = &tc317x_tcspi_data;

  if(!spi_data->spi_dev || !Length)
  {
    PRINTF_LV_0 ("  spi_dev NULL [%d]\n",Length);
    return -EFAULT;
  }
  if(!pBufIn && !pBufOut) 
  {
    PRINTF_LV_0 ("  In Out buf NULL\n"); 
    return -EFAULT;
  }

#if 1  //SDK
  xfer.tx_buf = pBufIn;
  xfer.rx_buf = pBufOut;
  xfer.len = Length;
  xfer.speed_hz = SPI_SPEED_HZ;
  xfer.delay_usecs = 0; //must set.
  xfer.bits_per_word = SPI_BITS_PER_WORD;

  spi_message_init(&msg);
  spi_message_add_tail(&xfer, &msg);
#else
  spi_message_init(&msg);  
  memset(&xfer, 0, sizeof xfer); // must set.
  spi_message_add_tail(&xfer, &msg);
  
  xfer.tx_buf = pBufIn;
  xfer.rx_buf = pBufOut;
  xfer.len = Length;
#endif

  ret = spi_sync(spi_data->spi_dev, &msg);
  if(!ret) ret = ((unsigned char*)xfer.rx_buf)[0];
  return ret;
}

static int TC317X_IO_CSPI_SingleRW (
    int moduleidx,
    int chipAddr,
    unsigned char WriteFlag,
    unsigned short RegAddr,
    unsigned char *pData)
{
  unsigned char buffer[SPICMD_BUFF_LEN];
  unsigned char buffout[SPICMD_BUFF_LEN];
  unsigned char crc;

  //mutex_lock(&lock);

  tc317x_err = TC3X_IO_FAIL;

  buffer[0] =  chipAddr;                    /* start bit(1) + chip_id(7) */
  /* mode(1) + rw(1) + fix(1) + addr(5) */
  buffer[1] = 0 << 7 | WriteFlag << 6 | 1 << 5 | ((RegAddr & 0x7c0) >> 6);
  buffer[2] = (RegAddr & 0x03f) << 2 | 0x0; /* addr(6bit) + NULL(2bit) */

  if (WriteFlag)  buffer[3] = pData[0];     /* write */
  else buffer[3] = 0x0;                     /* null(8) */

  buffer[4] = 0x00;

  crc = TC317X_IO_UTIL_CRC7 (buffer, 36);
  buffer[4] = 0x00 | ((crc & 0x7f) >> 3);   /* null(4) + crc(4) */
  buffer[5] = ((crc & 0x07) << 5) | 0x0f;   /* crc(3) + end bit(5) */
  buffer[6] = 0xff;
  buffer[7] = 0xff;

  tc317x_spi_write_and_read(moduleidx, buffer, buffout, SPICMD_BUFF_LEN);
  if (buffout[7] != SPICMD_ACK) {                 /* ack */
    PRINTF_LV_0 ("# Single %s ACK error [%x][%x]", WriteFlag ? "Write" : "Read",buffout[6],buffout[7]);
    PRINTF_LV_0 ("# [%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x]//[%02x]\n",
        buffer[0], buffer[1], buffer[2], buffer[3],
        buffer[4], buffer[5], buffer[6], buffer[7], crc);
    //mutex_unlock(&lock);
    return (-1);
  }
  if (WriteFlag == 0)
    *pData = buffout[6];

  //mutex_unlock(&lock);

  tc317x_err = TC3X_IO_SUCCESS;
  return 0;
}

static int TC317X_IO_CSPI_MultiRW (
    int moduleidx,
    int chipAddr,
    unsigned char WriteFlag,
    unsigned short RegAddr,
    unsigned char *pData,
    unsigned int Size,
    int FixedModeFlag)
{
  //unsigned int crc32_calc, crc32_src;
  struct tc317x_tcspi_data_t *spi_data = &tc317x_tcspi_data;
  unsigned char crc;
#ifdef TC317X_REDUCED_IO
  unsigned char *buffer;//[SPICMD_BUFF_LEN];
  unsigned char *buffout;//[SPICMD_BUFF_LEN];

  if(WriteFlag == 0){
    buffer = spi_data->dummy_buff;
    buffout = spi_data->rw_buff;
  } else {
    memcpy(spi_data->rw_buff+SPICMD_BUFF_LEN, pData, Size);
    buffer = spi_data->rw_buff;
    buffout = spi_data->dummy_buff;
  }
  memset(buffer+SPICMD_BUFF_LEN+Size, 0xFF, SPICMD_BUFF_LEN);
#else
  unsigned char buffer[SPICMD_BUFF_LEN];
  unsigned char buffout[SPICMD_BUFF_LEN];
#endif

  if (Size > DMA_MAX_SIZE)
    return (-1);

  //mutex_lock(&lock);

  tc317x_err = TC3X_IO_FAIL;

  Size--; /* MAX 16KB (Output buffer max size 7KB) (LENGTH + 1 Byte) */

  /* start bit(1) + chip_id(7) */
  buffer[0] = chipAddr;
  /* mode(1) + rw(1) + fix(1) + addr(5) */
  buffer[1] = 1 << 7 | WriteFlag << 6 | FixedModeFlag << 5 | ((RegAddr & 0x7c0) >> 6);
  /* addr(6bit) + length(2bit) */
  buffer[2] = (RegAddr & 0x03f) << 2 | ((Size & 0x3000) >> 12);
  /* length(8bit) */
  buffer[3] = (Size & 0xff0) >> 4;

  buffer[4] = (Size & 0xf) << 4;
  crc = TC317X_IO_UTIL_CRC7 (buffer, 36);
  /* length(4) + crc(4) */
  buffer[4] = ((Size & 0xf) << 4) | ((crc & 0x7f) >> 3);
  /* crc(3) + end bit(5) */
  buffer[5] = ((crc & 0x07) << 5) | 0x0f;
  buffer[6] = 0xff;
  buffer[7] = 0xff;

  Size++;

#ifndef TC317X_REDUCED_IO 
  tc317x_spi_write_and_read(moduleidx, buffer, buffout, SPICMD_BUFF_LEN);
  if (buffout[7] != SPICMD_ACK)	{    /* ack */
    PRINTF_LV_0 ("# Burst %s ACK error [%d][%d]\n", WriteFlag ? "Write" : "Read",buffout[6],buffout[7]);
    PRINTF_LV_0 ("# [%x][%x][%x][%x][%x][%x][%x][%x]//[%x]\n",
        buffout[0], buffout[1], buffout[2], buffout[3],
        buffout[4], buffout[5], buffout[6], buffout[7], crc);
    //mutex_unlock(&lock);
    return (-1);
  }

  if (WriteFlag == 0) {
    /* Receive Data */
    tc317x_spi_write_and_read(moduleidx, spi_data->dummy_buff, pData, Size);

    /* initialize tcc3170 spi cmd interface and read crc for pData*/
    tc317x_spi_write_and_read(moduleidx,
        spi_data->init_cmd_buf, buffout, SPICMD_BUFF_LEN);
    /*TODO check crc*/
  } else {
    /* Send Data */
    tc317x_spi_write_and_read(moduleidx, pData, spi_data->dummy_buff, Size);

    /* initialize tcc3170 spi cmd interface */
    tc317x_spi_write_and_read(moduleidx, spi_data->init_cmd_buf,
        buffout, SPICMD_BUFF_LEN);
  }
#else //TC317X_REDUCE_TRANSACTION 
  tc317x_spi_write_and_read(moduleidx, buffer, buffout, Size+SPICMD_BUFF_LEN*2);

  if (buffout[7] != SPICMD_ACK)	{    /* ack */
    PRINTF_LV_0 ("# Burst %s ACK error (%d)\n", 
      WriteFlag ? "Write" : "Read", Size);
    PRINTF_LV_0 ("# [%x][%x][%x][%x][%x][%x][%x][%x]//[%x]\n",
        buffer[0], buffer[1], buffer[2], buffer[3],
        buffer[4], buffer[5], buffer[6], buffer[7], crc);
    //mutex_unlock(&lock);
    return (-1);
  }

  if (WriteFlag == 0) {
    memcpy(pData, buffout + SPICMD_BUFF_LEN, Size);
  } 
#endif

  //mutex_unlock(&lock);

  tc317x_err = TC3X_IO_SUCCESS;
  return 0;
}

TC3X_IO_Err TC317X_IO_CSPI_Close (int moduleidx)
{
  struct tc317x_tcspi_data_t *spi_data = &tc317x_tcspi_data;
  tc317x_err = TC3X_IO_FAIL;
#ifdef TC_AP
  spi_tcc_close(spi_data->spi_dev);
  spi_dev_put(spi_data->spi_dev);
#endif
  spi_data->spi_dev = NULL;

  TCDBG("\n");
  return TC3X_IO_SUCCESS;
}

TC3X_IO_Err TC317X_IO_CSPI_Init (int moduleidx)
{
#if 0
  int ret = 0;
#endif
  struct tc317x_tcspi_data_t *spi_data = &tc317x_tcspi_data;

  tc317x_err = TC3X_IO_FAIL;
  TC317X_IO_memset(&tc317x_tcspi_data, 0, sizeof(tc317x_tcspi_data));
  TC317X_IO_memset(spi_data->init_cmd_buf, 0xFF, SPICMD_BUFF_LEN);
#ifndef TC317X_REDUCED_IO
  TC317X_IO_memset( spi_data->dummy_buff + DMA_MAX_SIZE+SPICMD_BUFF_LEN, 
    0xFF, sizeof(SPICMD_BUFF_LEN));
  TC317X_IO_memset( spi_data->rw_buff + DMA_MAX_SIZE+SPICMD_BUFF_LEN, 
    0xFF, sizeof(SPICMD_BUFF_LEN));
#endif

#if 1
  spi_data->spi_dev = tdmb_spi_setup();
  if(spi_data->spi_dev == NULL)
  {
    TCDBG("Setup Fail !! \n");
    return TC3X_IO_FAIL;
  }
  else
  {
    TCDBG("Setup OK ~ mode[%d] Hz[%d]\n",spi_data->spi_dev->mode, spi_data->spi_dev->max_speed_hz);
    tc317x_err = TC3X_IO_SUCCESS;
    return TC3X_IO_SUCCESS;
  }

#else
  spi_data->spi_dev = find_spi_device();

  if(spi_data->spi_dev) {
#ifdef TC_AP
    ret = spi_tcc_open(spi_data->spi_dev);
    if(ret < 0) goto cspi_init_fail;
#endif
    spi_data->spi_dev->mode = SPI_MODE_0;
    spi_data->spi_dev->bits_per_word = 8;
    spi_data->spi_dev->max_speed_hz = 2000000;
    ret = spi_setup(spi_data->spi_dev);
    if(ret < 0) {
      TCDBG("spi_setup failed :%d\n", ret);
      goto cspi_init_fail;
    }
  }
  tc317x_err = TC3X_IO_SUCCESS;
  return TC3X_IO_SUCCESS;

cspi_init_fail:
  return TC3X_IO_FAIL;
#endif
}

//==============================================================================
//
//  Read / Write API
//
//==============================================================================
unsigned int TC317X_IO_CSPI_Reg_Read ( int moduleidx, int ChipAddr,
    int RegAddr, TC3X_IO_Err * pError) // use 52cmd
{
  unsigned int retv = 0;

  TC317X_IO_IF_LOCK (moduleidx);

  TC317X_IO_CSPI_SingleRW(moduleidx, ChipAddr, 0, RegAddr, (unsigned char *) &retv);
  retv = (retv & 0xff);

  TC317X_IO_IF_UnLOCK (moduleidx);
  return retv;
}

TC3X_IO_Err TC317X_IO_CSPI_Reg_Write ( int moduleidx, int ChipAddr,
    int RegAddr, unsigned int data)  // use 52cmd
{
  unsigned char inputdata;

  TC317X_IO_IF_LOCK (moduleidx);

  inputdata = (data & 0xff);
  TC317X_IO_CSPI_SingleRW(moduleidx, ChipAddr, 1, RegAddr, &inputdata);

  TC317X_IO_IF_UnLOCK (moduleidx);
  return TC3X_IO_SUCCESS;
}

TC3X_IO_Err TC317X_IO_CSPI_Reg_ReadEx (
    int moduleidx, int ChipAddr, int RegAddr, unsigned char *data, int iSize)
{
  unsigned int cmax;
  unsigned int cremain;
  unsigned int i;
  int result;
  int mode;

  cmax = (iSize / DMA_MAX_SIZE);
  cremain = (iSize % DMA_MAX_SIZE);

  mode = (ChipAddr>>BURST_MODE_POS) & BURST_MODE_MSK;
  ChipAddr = ChipAddr & (~(BURST_MODE_MSK<<BURST_MODE_POS));

  if(mode == BURST_CONTINUOUS_MODE) mode = 0;
  else /*(mode == BURST_FIXED_MODE)*/ mode = 1;

  TC317X_IO_IF_LOCK (moduleidx);

  for (i = 0; i < cmax; i++) {
    result = TC317X_IO_CSPI_MultiRW(moduleidx, ChipAddr, 0,
        RegAddr, (unsigned char *) &data[i * DMA_MAX_SIZE], DMA_MAX_SIZE, mode);
    if(result < 0) return TC3X_IO_FAIL;
  }

  if (cremain != 0) {
    result = TC317X_IO_CSPI_MultiRW(moduleidx, ChipAddr, 0,
        RegAddr, (unsigned char *) &data[i * DMA_MAX_SIZE], cremain, mode);
    if(result < 0) return TC3X_IO_FAIL;
  }

  TC317X_IO_IF_UnLOCK (moduleidx);
  return TC3X_IO_SUCCESS;
}

TC3X_IO_Err TC317X_IO_CSPI_Reg_WriteEx (
    int moduleidx, int ChipAddr, int RegAddr, unsigned char *pCh, int iSize)
{
  unsigned int cmax;
  unsigned int cremain;
  unsigned int i;
  int result;
  int mode;

  cmax = (iSize / DMA_MAX_SIZE);
  cremain = (iSize % DMA_MAX_SIZE);

  mode = (ChipAddr>>BURST_MODE_POS) & BURST_MODE_MSK;
  ChipAddr = ChipAddr & (~(BURST_MODE_MSK<<BURST_MODE_POS));

  if(mode == BURST_CONTINUOUS_MODE) mode = 0;
  else /*(mode == BURST_FIXED_MODE)*/ mode = 1;

  TC317X_IO_IF_LOCK (moduleidx);

  for (i = 0; i < cmax; i++) {
    result = TC317X_IO_CSPI_MultiRW(moduleidx, ChipAddr, 1,
        RegAddr, (unsigned char *) &pCh[i * DMA_MAX_SIZE], DMA_MAX_SIZE, mode);
    if(result < 0) return TC3X_IO_FAIL;
  }

  if (cremain) {
    result = TC317X_IO_CSPI_MultiRW(moduleidx, ChipAddr, 1,
        RegAddr, (unsigned char *) &pCh[i * DMA_MAX_SIZE], cremain, mode);
    if(result < 0) return TC3X_IO_FAIL;
  }

  TC317X_IO_IF_UnLOCK (moduleidx);
  return TC3X_IO_SUCCESS;
}

unsigned int TC317X_IO_CSPI_Reg_Read_InnerSem (
    int moduleidx, int ChipAddr, int RegAddr, TC3X_IO_Err * pError)
{
  unsigned int retv;

  TC317X_IO_CSPI_SingleRW(moduleidx, ChipAddr, 0, RegAddr, (unsigned char *) &retv);
  retv = (retv & 0xff);

  return retv;
}

TC3X_IO_Err TC317X_IO_CSPI_Reg_Write_InnerSem (
    int moduleidx, int ChipAddr, int RegAddr, unsigned int data)
{
  int ret;
  unsigned char inputdata;

  inputdata = (data & 0xff);
  ret = TC317X_IO_CSPI_SingleRW(moduleidx, ChipAddr, 1, RegAddr, &inputdata);

  if(ret < 0)
    return TC3X_IO_FAIL;
  else
    return TC3X_IO_SUCCESS;
}

TC3X_IO_Err TC317X_IO_CSPI_Reg_ReadEx_InnerSem (
    int moduleidx, int ChipAddr, int RegAddr, unsigned char *data, int iSize)
{
  unsigned int cmax;
  unsigned int cremain;
  unsigned int i;
  int result;
  int mode;

  cmax = (iSize / DMA_MAX_SIZE);
  cremain = (iSize % DMA_MAX_SIZE);

  mode = (ChipAddr>>BURST_MODE_POS) & BURST_MODE_MSK;
  ChipAddr = ChipAddr & (~(BURST_MODE_MSK<<BURST_MODE_POS));

  if(mode == BURST_CONTINUOUS_MODE) mode = 0;
  else /*(mode == BURST_FIXED_MODE)*/ mode = 1;

  for (i = 0; i < cmax; i++) {
    result = TC317X_IO_CSPI_MultiRW(moduleidx, ChipAddr, 0,
        RegAddr, (unsigned char *) &data[i * DMA_MAX_SIZE], DMA_MAX_SIZE, mode);
    if(result == -1) return TC3X_IO_FAIL;
  }

  if (cremain != 0) {
    result = TC317X_IO_CSPI_MultiRW(moduleidx, ChipAddr, 0,
        RegAddr, (unsigned char *) &data[i * DMA_MAX_SIZE], cremain, mode);
    if(result == -1) return TC3X_IO_FAIL;
  }
  return TC3X_IO_SUCCESS;
}

TC3X_IO_Err TC317X_IO_CSPI_Reg_WriteEx_InnerSem (
    int moduleidx, int ChipAddr, int RegAddr, unsigned char *pCh, int iSize)
{
  unsigned int cmax;
  unsigned int cremain;
  unsigned int i;
  int result;
  int mode;

  cmax = (iSize / DMA_MAX_SIZE);
  cremain = (iSize % DMA_MAX_SIZE);

  mode = (ChipAddr>>BURST_MODE_POS) & BURST_MODE_MSK;
  ChipAddr = ChipAddr & (~(BURST_MODE_MSK<<BURST_MODE_POS));

  if(mode == BURST_CONTINUOUS_MODE) mode = 0;
  else /*(mode == BURST_FIXED_MODE)*/ mode = 1;

  for (i = 0; i < cmax; i++) {
    result = TC317X_IO_CSPI_MultiRW(moduleidx, ChipAddr, 1,
        RegAddr, (unsigned char *) &pCh[i * DMA_MAX_SIZE], DMA_MAX_SIZE, mode);
    if(result < 0)  return TC3X_IO_FAIL;
  }

  if (cremain != 0) {
    result = TC317X_IO_CSPI_MultiRW(moduleidx, ChipAddr, 1,
        RegAddr, (unsigned char *) &pCh[i * DMA_MAX_SIZE], cremain, mode);
    if(result < 0) return TC3X_IO_FAIL;
  }

  return TC3X_IO_SUCCESS;
}

#if 0
static struct spi_device* find_spi_device(void)
{
  struct spi_master *spi_master;
  struct spi_device *spi_device;
  struct device *pdev;
  char buff[64];

  spi_master = spi_busnum_to_master(5);
  if (!spi_master) {
    TCDBG( "spi_busnum_to_master(%d) returned NULL\n", 0);
    return NULL;
  }

  spi_device = spi_alloc_device(spi_master);
  if (!spi_device) {
    put_device(&spi_master->dev);
    TCDBG( "spi_alloc_device() failed\n");
    return NULL;
  }

  /* specify a chip select line */
  spi_device->chip_select = 0;

  snprintf(buff, sizeof(buff), "%s.%u",
      dev_name(&spi_device->master->dev),
      spi_device->chip_select);
  TCDBG("%s.%u",dev_name(&spi_device->master->dev),spi_device->chip_select);

  pdev = bus_find_device_by_name(spi_device->dev.bus, NULL, buff);

  if (pdev) {
    TCDBG("spi_device :0x%X\n", (unsigned int)spi_device);
  }

  put_device(&spi_master->dev);
  return spi_device;
}
#endif
