//=============================================================================
// File       : TCC3170_bb.c
//
// Description: 
//
// Revision History:
//
// Version         Date           Author        Description of Changes
//-----------------------------------------------------------------------------

//=============================================================================

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <mach/gpio.h>
#ifdef FEATURE_TDMB_PMIC_POWER
#include <mach/vreg.h>
#include <mach/mpp.h>
#endif /* FEATURE_TDMB_PMIC_POWER */

#include "../tdmb_comdef.h"
#include "../tdmb_dev.h"
#include "../tdmb_gpio.h"
#ifdef FEATURE_QTDMB_I2C_CMD
#include "../tdmb_i2c.h"
#endif /* FEATURE_QTDMB_I2C_CMD */
#include "../tdmb_test.h"

#include <linux/regulator/machine.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/pmic8058-regulator.h>

#ifdef FEATURE_QTDMB_SPI_CMD
#include "../tdmb_spi.h"
#endif

#include "tcc3170_bb.h"
#include "TC317X_API.h"
#include "TC317X_Sub_API.h"
#include "CORE/TC317X_device_Interrupt_Task.h"
#include "TCC317X_BOOT_TDMB.h"
#include <asm/irq.h>

/*================================================================== */
/*=================       TCC3170 BB Function       ================== */
/*================================================================== */

#define TMDB_SPI_CTRL_ID TCC3170_CID
#define TC3170_MSC_BUF_SIZE 16 // temp value
#define TC3170_FIC_SIZE 384
#define TC3170_DATA_SIZE ((MAX_FIFO_SIZE>>1)+(188*4)) //SDK 0617 TC317x_core.h
//#define TC3170_DATA_SIZE TDMB_CSPI_READ_SIZE_MSC //SDK 0609 separate FIC, MSC size
//#define TC3170_DATA_SIZE (TDMB_CSPI_READ_SIZE ) //from GetTDMBStream() size SDK 0418
//#define TC3170_DATA_SIZE (TDMB_CSPI_READ_SIZE << 2) //from GetTDMBStream() size SDK 0404

typedef struct 
{
  uint8 tcc_fic_buf[TC3170_FIC_SIZE];
  uint16 tcc_fic_size;
  uint8 tcc_msc_buf[TC3170_DATA_SIZE*TC3170_MSC_BUF_SIZE];
  uint16 tcc_msc_size;
} tcc_data_type;

tcc_data_type tcc_data;

#ifdef USE_IF_CSPI
TC3X_MainIOSeries gCmdInterface = TC3X_IF_CSPI;
TC3X_StreamIOSeries gDataInterface = TC3X_STREAM_IO_MAINIO;	
#else
TC3X_MainIOSeries gCmdInterface = TC3X_IF_I2C;
TC3X_StreamIOSeries gDataInterface = TC3X_STREAM_IO_STS;	
#endif

TDMBService_t tTDMBService;

//#define MSC_BUFFER_FOR_TCC_SDK
#ifdef MSC_BUFFER_FOR_TCC_SDK
int msc_put_cnt;
#else
int msc_put_count = 0;
int msc_get_count = 0;
#endif

uint8 sync_lock;

extern tdmb_mode_type dmb_mode;
extern ts_data_type ts_data;

extern irqreturn_t tdmb_interrupt_tcc(void);

static boolean tcc3170_function_register(tdmb_bb_function_table_type *);

/*====================================================================
FUNCTION       tdmb_bb_tcc3170_init  
DESCRIPTION    matching tcc3170 Function at TDMB BB Function
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
boolean tdmb_bb_tcc3170_init(tdmb_bb_function_table_type *function_table_ptr)
{
  boolean bb_initialized;

  bb_initialized = tcc3170_function_register(function_table_ptr);

  return bb_initialized;
}


static boolean tcc3170_function_register(tdmb_bb_function_table_type *ftable_ptr)
{
  ftable_ptr->tdmb_bb_drv_init          = tcc3170_init;
  ftable_ptr->tdmb_bb_power_on          = tcc3170_power_on;
  ftable_ptr->tdmb_bb_power_off         = tcc3170_power_off;
  ftable_ptr->tdmb_bb_set_antenna_path  = tcc3170_set_antenna_path;
  ftable_ptr->tdmb_bb_ch_scan_start     = tcc3170_ch_scan_start;
  ftable_ptr->tdmb_bb_resync            = tcc3170_bb_resync;
  ftable_ptr->tdmb_bb_subch_start       = tcc3170_subch_start;
  ftable_ptr->tdmb_bb_read_int          = tcc3170_read_int;
  ftable_ptr->tdmb_bb_get_sync_status   = tcc3170_get_sync_status;
  ftable_ptr->tdmb_bb_read_fib          = tcc3170_read_fib;
  ftable_ptr->tdmb_bb_set_subchannel_info = tcc3170_set_subchannel_info;
  ftable_ptr->tdmb_bb_read_msc          = tcc3170_read_msc;
  ftable_ptr->tdmb_bb_get_ber           = tcc3170_get_ber;
  ftable_ptr->tdmb_bb_ch_stop           = tcc3170_stop;
  ftable_ptr->tdmb_bb_powersave_mode    = tcc3170_set_powersave_mode;
  ftable_ptr->tdmb_bb_ch_test           = tcc3170_test;

  return TRUE;
}

/*====================================================================
FUNCTION       tcc3170_i2c_write
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
uint8 tcc3170_i2c_write(uint8 chipid, uint16 reg, uint16 data)
{
  uint8 ret = 0;

#ifdef FEATURE_QTDMB_I2C_CMD
  ret = tdmb_i2c_write(chipid, reg, sizeof(uint8), data, sizeof(uint8));
#endif /* FEATURE_QTDMB_I2C_CMD */

  return ret;
}

/*====================================================================
FUNCTION       tcc3170_i2c_write_word
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
uint8 tcc3170_i2c_read(uint8 chipid, uint16 reg, uint16 *data)
{
#ifdef FEATURE_QTDMB_I2C_CMD
  tdmb_i2c_read(chipid, reg, sizeof(uint8), data, sizeof(uint8));
#endif /* FEATURE_QTDMB_I2C_CMD */

  return TRUE;
}

void tcc3170_i2c_read_len(uint8 chipid, unsigned int uiAddr, uint8 ucRecvBuf[], uint16 ucCount)
{
#ifdef FEATURE_QTDMB_I2C_CMD
  tdmb_i2c_read_len(chipid, uiAddr, ucRecvBuf, ucCount, sizeof(uint8));
#endif /* FEATURE_QTDMB_I2C_CMD */

  return;
}

#if 0
/*====================================================================
FUNCTION       tcc3170_spi_write
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
int tcc3170_spi_write(uint8 chipid, uint16 reg, uint8 data)
{
  int ret = 0;

  ret = tcc3170_IO_CSPI_Reg_Write(0, chipid, reg, data);

  return ret;
}

/*====================================================================
FUNCTION       tcc3170_spi_write_len
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
int tcc3170_spi_write_len(uint8 chipid, uint16 reg, uint8 *data, uint16 length)
{
  int  ret = 0;
  
  ret = tcc3170_IO_CSPI_Reg_WriteEx(0, chipid, reg, data, length);

  return ret;
}

/*====================================================================
FUNCTION       tcc3170_i2c_write_word
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
uint8 tcc3170_spi_read(uint8 chipid, uint16 reg, uint8 data)
{
  data = tcc3170_IO_CSPI_Reg_Read(0, chipid, reg, 0);

  return data;
}

/*====================================================================
FUNCTION       tcc3170_spi_read_len
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
uint8 tcc3170_spi_read_len(uint8 chipid, uint16 reg, uint8 *data, uint16 length)
{
  int ret=0;

  ret = tcc3170_IO_CSPI_Reg_ReadEx(0, chipid, reg, data, length);
  
  return ret;
}
#endif

#ifdef FEATURE_TDMB_SET_ANT_PATH
/*====================================================================
FUNCTION       t3700_pmic_ant_switch_power
DESCRIPTION  MSM8x60 VREG L11 ON/OFF
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
static boolean tcc3170_pmic_ant_switch_power(int on)
{
  static struct regulator *vreg; //If verg value was changed, can not disable regulator
  int ret = 0;
  
  TDMB_MSG_TCC_BB("[%s] on[%d]!!!\n", __func__, on);

  if(on)
  {
    vreg = regulator_get(NULL, "8058_l11");

    ret = regulator_set_voltage(vreg, 2600000, 2600000);
    
    if(!ret) ret = regulator_enable(vreg);
    if(ret)
    {
      TDMB_MSG_TCC_BB("TDMB regulator_enable Fail [%d] !!!\n",ret);
    }
  }
  else
  {
    ret = regulator_disable(vreg);
    if(ret)
    {
      TDMB_MSG_TCC_BB("TDMB regulator_disable Fail [%d] !!!\n",ret);
    }
  }

  return TRUE;
}
#endif

/*====================================================================
FUNCTION       tcc3170_set_powersave_mode
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
void tcc3170_set_powersave_mode(void)
{
  // if necessary

  //TDMB_MSG_TCC_BB("[%s] \n", __func__);
}


/*====================================================================
FUNCTION       tcc3170_power_on
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
void tcc3170_power_on(void)
{
  //power on sequency
  
  TDMB_MSG_TCC_BB("[%s] start!!!\n", __func__);

#ifdef FEATURE_TDMB_SET_ANT_PATH
  tcc3170_pmic_ant_switch_power(TRUE);
#endif

#ifdef FEATURE_TDMB_PMIC_TCXO_192M
  tdmb_set_gpio(DMB_XO_SEL, 1);
#endif

 //Set in Telechips SDK
#if 0
  tdmb_set_gpio(DMB_RESET, 0);
  msleep(1);

#ifdef FEATURE_TDMB_PMIC_POWER
  fc8050_pmic_power_onoff(1);
#endif

  tdmb_set_gpio(DMB_PWR_EN, 1);
  msleep(10);

  tdmb_set_gpio(DMB_RESET, 1);
#endif
}


/*====================================================================
FUNCTION       tcc3170_power_off
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
void tcc3170_power_off(void)
{
  //power off sequency
 
  TDMB_MSG_TCC_BB("[%s] start!!!\n", __func__);

  TC317X_API_Close(0);

//Set in Telechips SDK
#if 0
  tdmb_set_gpio(DMB_RESET, 0);
  msleep(1);
  
  tdmb_set_gpio(DMB_PWR_EN, 0);
#endif

#ifdef FEATURE_TDMB_SET_ANT_PATH
  tcc3170_pmic_ant_switch_power(FALSE);
#endif

#ifdef FEATURE_TDMB_PMIC_TCXO_192M
  tdmb_set_gpio(DMB_XO_SEL, 0);
#endif
  
#ifdef FEATURE_TDMB_PMIC_POWER
  fc8050_pmic_power_onoff(0);
#endif
}


/*====================================================================
FUNCTION       tcc3170_set_antenna
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
void tcc3170_set_antenna_path(int ant_type)
{
#ifdef FEATURE_TDMB_SET_ANT_PATH
  TDMB_MSG_BB("[%s] ant_type[%d]\n", __func__, ant_type);
  
  if (ant_type == TDMB_ANT_EARJACK)
  {
    tdmb_set_gpio(DMB_ANT_SEL, DMB_ANT_EAR_ACT);
  }
  else
  {
    tdmb_set_gpio(DMB_ANT_SEL, (DMB_ANT_EAR_ACT)? 0 : 1);
  }
#else
  TDMB_MSG_BB("[%s]  Do nothing, No ANT. switch\n", __func__);
#endif /* FEATURE_TDMB_SET_ANT_PATH */
}

/*====================================================================
FUNCTION       tcc3170_ch_scan_start
DESCRIPTION  
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
void tcc3170_ch_scan_start(int freq, int band, unsigned char for_air)
{
  int ret;
  // freq. setting use for start autoscan &  air play

  TDMB_MSG_TCC_BB("[%s] Channel Frequency[%d]\n", __func__, freq);

  ret = TC317X_API_SetFreq(0, 1500/*bandwidth khz*/, freq /*khz*/, NULL);
  TDMB_MSG_TCC_BB("[%s] SetFreq  ret[%d]\n", __func__,ret);

  ret = TC317X_API_GetTDMB_Lock_Wait(0, 0);
  sync_lock = ret;
  TDMB_MSG_TCC_BB("[%s] sync lock %s  sync_lock[%d] %d \n", __func__, ret==1?"OK~":"Fail!!", sync_lock, ret);
}


/*====================================================================
FUNCTION       tcc3170_init
DESCRIPTION            
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
uint8 tcc3170_init(void)
{
  //tcc3170 initialization
  int ret;

  TDMB_MSG_TCC_BB("[%s] start  Cmd[%d] Data[%d] size[%d]\n", __func__, gCmdInterface, gDataInterface,TC3170_DATA_SIZE);

  TC317X_API_Init(0, 1);
  TC317X_API_IOOpen(0, gCmdInterface, gDataInterface);
  TC317X_API_BBOpen(0);
  TC317X_API_BBPreInit(0); // function pointer connect tc317x_deviceApi.c  TC317X_Entry
  TC317X_API_PreSet_PLLData(0, 0x60,0x0,0xf,0x03,19200);

  ret = TC317X_API_ColdBootTable(0, &TCC317X_BOOT_DATA_TDMB[0],TCC317X_BOOT_SIZE_TDMB, CMD_BB_COLDBOOT_NORMAL);

  if(ret != TC3X_IO_FAIL)
  {
    TC317X_API_BBInit(0);
    TDMB_MSG_TCC_BB("[%s] OK ~~\n", __func__);
  }
  else
  {
    TDMB_MSG_TCC_BB("[%s] ColdBoot Fail[%d]\n", __func__, ret);
  }
#ifdef MSC_BUFFER_FOR_TCC_SDK
  msc_put_cnt = 0;
#else
  msc_put_count = 0;
  msc_get_count = 0;
#endif
  memset((void*)&tcc_data, 0x00, sizeof(tcc_data_type));
  return 0;
}


/*====================================================================
FUNCTION       tcc3170_bb_resync
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
void tcc3170_bb_resync(unsigned char imr)
{
  // if necessary

  //TDMB_MSG_TCC_BB("[%s] \n", __func__);
}


/*====================================================================
FUNCTION       tcc3170_stop
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
uint8 tcc3170_stop(void)
{
  // if necessary
  int ret;
  
  TDMB_MSG_TCC_BB("[%s] \n", __func__);

  // TODO: check..  stop channel
  ret = TC317X_API_SET_TDMB_Service(0, 0/*release*/, &tTDMBService);
  TDMB_MSG_TCC_BB("[%s] end!\n", __func__);
  return 0;
}


/*====================================================================
FUNCTION       tcc3170_subch_start
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
int tcc3170_subch_start(uint8 *regs, uint32 data_rate)
{
  // subch setting  code
  int ret, icontrol = 1;

  TDMB_MSG_TCC_BB("[%s] type[%x] subchId[%x] bitrate[%d]\n", __func__,tTDMBService.ServType, tTDMBService.SubChID,tTDMBService.BitRate);

  ret = TC317X_API_SET_TDMB_Service(0, icontrol, &tTDMBService);
  TDMB_MSG_TCC_BB("[%s]  end !\n", __func__);

#ifdef MSC_BUFFER_FOR_TCC_SDK
    msc_put_cnt = 0;
#else
    msc_put_count = 0;
    msc_get_count = 0;
#endif

  return TRUE;
}


/*====================================================================
FUNCTION       tcc3170_read_int
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
tdmb_bb_int_type tcc3170_read_int(void)
{
  // clear INTR, read data
  //TDMB_MSG_TCC_BB("[%s] \n", __func__);

  TC317XTask_Interrupt(0);

  return TDMB_BB_INT_MSC;
}


/*====================================================================
FUNCTION       tcc3170_get_sync_status
DESCRIPTION
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
tBBStatus tcc3170_get_sync_status(void)
{
  // check sync status
  TDMB_MSG_TCC_BB("[%s] %d\n", __func__, sync_lock);

  //return BB_SUCCESS;
  return sync_lock;
}


/*====================================================================
FUNCTION       tcc3170_read_fib
DESCRIPTION use for read FIC data only autoscan
DEPENDENCIES
RETURN VALUE number of FIB
SIDE EFFECTS
======================================================================*/
int tcc3170_read_fib(uint8 *fibs)
{
  // Read FIC data only autoscan

  uint16 wFicLen=0;

  TDMB_MSG_TCC_BB("[%s] size[%d]\n", __func__, tcc_data.tcc_fic_size);

  wFicLen = tcc_data.tcc_fic_size;
  tcc_data.tcc_fic_size = 0;
  if(wFicLen != 0)
  {
    memcpy(fibs, tcc_data.tcc_fic_buf, wFicLen);
  }
  return wFicLen;
}

/*====================================================================
FUNCTION       tcc3170_get_bitrate
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
uint16 tcc3170_get_bitrate(uint8 type, uint16 bitrate, uint16 subch_size, uint16 pro_level, uint16 option)
{
  uint8 i;
  uint16 tcc_bitrate = 0;
  uint16 UEP_bitrate_table[14] = {32,48,56,64,80,96,112,128,160,192,224,256,320,384};

  if(type == TDMB_BB_SVC_DMB)// EEP
  {
    if(option == 0) // ucOption 0
    {
      switch(pro_level)
      {
        case 0: tcc_bitrate = (subch_size/12); break;
        case 1: tcc_bitrate = (subch_size/8);  break;
        case 2: tcc_bitrate = (subch_size/6);  break;
        case 3: tcc_bitrate = (subch_size/4);  break;
      }
    }
    else if(option == 1)
    {
      switch(pro_level)
      {
        case 0: tcc_bitrate = (subch_size/27); break;
        case 1: tcc_bitrate = (subch_size/21);  break;
        case 2: tcc_bitrate = (subch_size/18);  break;
        case 3: tcc_bitrate = (subch_size/15);  break;
      }
    }
  }
  else if(type == TDMB_BB_SVC_DAB) // UEP (shrot form)
  {
    for (i = 0; i < 14; i++)
    {
      if (bitrate == UEP_bitrate_table[i])
      {
        break;
      }
    }
    if(i > 13)
    {
      TDMB_MSG_TCC_BB("%s  Error DAB  bitrate[%d]\n",__func__, i);
      return FALSE;
    }
    tcc_bitrate = i;
  }
  TDMB_MSG_TCC_BB("%s  type[%d] bitrate[%x]\n",__func__, type, tcc_bitrate);
  return tcc_bitrate;
}

/*====================================================================
FUNCTION       tcc3170_set_subchannel_info
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
void tcc3170_set_subchannel_info(void * sub_ch_info)
{
  static chan_info *tcc_ch_info;

  tcc_ch_info = (chan_info *)sub_ch_info;

  tTDMBService.ServType  = (tcc_ch_info->uiServiceType == TDMB_BB_SVC_DAB) ? 0x01 : 0x04;
  tTDMBService.PType     = tcc_ch_info->uiSlFlag;
  tTDMBService.SubChID   = tcc_ch_info->uiSubChID;
  tTDMBService.CUSize    = tcc_ch_info->uiSchSize;
  tTDMBService.StartCU   = tcc_ch_info->uiStarAddr;
  tTDMBService.ReConfig  = 0x02;
  tTDMBService.RS_ON     = (tcc_ch_info->uiServiceType == TDMB_BB_SVC_DAB) ? 0x00 : 0x01;

  // MUSICAM(DAB), UEP : must -1 PLevel
  tTDMBService.PLevel  = (tcc_ch_info->uiServiceType == TDMB_BB_SVC_DAB) ? (tcc_ch_info->uiProtectionLevel-1) : tcc_ch_info->uiProtectionLevel;

  //NETBER(DTV) : DAB but EEP(?) : change Bitrate, PLevel
  if((dmb_mode == TDMB_MODE_NETBER) && (tcc_ch_info->uiServiceType == TDMB_BB_SVC_DAB))
  {
    tTDMBService.PLevel  = tcc_ch_info->uiProtectionLevel;
    tTDMBService.BitRate = tcc3170_get_bitrate(TDMB_BB_SVC_DMB, tcc_ch_info->uiBitRate, tcc_ch_info->uiSchSize, tcc_ch_info->uiProtectionLevel, tcc_ch_info->ucOption);
  }
  else
  {
    tTDMBService.BitRate = tcc3170_get_bitrate(tcc_ch_info->uiServiceType, tcc_ch_info->uiBitRate, tcc_ch_info->uiSchSize, tcc_ch_info->uiProtectionLevel, tcc_ch_info->ucOption);
  }

  TDMB_MSG_TCC_BB("TCC SType[%x] PType[%x] subchID[%x] CUsize[%x] StartCU[%x]\n",
    tTDMBService.ServType,tTDMBService.PType,tTDMBService.SubChID,tTDMBService.CUSize,tTDMBService.StartCU);
  TDMB_MSG_TCC_BB("TCC Recon[%x] RS[%x] Plevel[%x] Bitrate[%x][%d] %d\n",
    tTDMBService.ReConfig,tTDMBService.RS_ON,tTDMBService.PLevel,tTDMBService.BitRate,tcc_ch_info->uiBitRate, tcc_ch_info->uiProtectionLevel);
}


/*====================================================================
FUNCTION       tcc3170_read_msc
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
int tcc3170_read_msc(uint8 *msc_buffer)
{
  uint16 msc_size=0, fic_size=0;

  //TDMB_MSG_TCC_BB("[%s] start\n", __func__);

  fic_size = tcc_data.tcc_fic_size;
  if(fic_size != 0)
  {
    memcpy(ts_data.fic_buf, tcc_data.tcc_fic_buf, fic_size);
    ts_data.type = FIC_DATA;
    ts_data.fic_size = fic_size;
    //TDMB_MSG_TCC_BB("  FIC in Read MSC size[%d] \n", fic_size);
  }
  tcc_data.tcc_fic_size = 0;

#ifndef MSC_BUFFER_FOR_TCC_SDK
  if(msc_get_count == msc_put_count)
  {    
    TDMB_MSG_TCC_BB("  MSC buffer alloc Error !! get[%d] put[%d]\n", msc_get_count,msc_put_count);
    return 0;
  }
#endif

  msc_size = tcc_data.tcc_msc_size;
  if(msc_size != 0)
  {
    if(ts_data.type == FIC_DATA)
      ts_data.type = FIC_MSC_DATA;
    else
      ts_data.type = MSC_DATA;

    //TDMB_MSG_TCC_BB("  MSC read size[%d]  type[%d]  get cnt[%d]\n", msc_size, ts_data.type,msc_get_count);
#ifdef MSC_BUFFER_FOR_TCC_SDK
    memcpy(msc_buffer, (void*)&tcc_data.tcc_msc_buf, msc_size);
#else
    memcpy(msc_buffer, (void*)&tcc_data.tcc_msc_buf[msc_get_count*TC3170_MSC_BUF_SIZE], msc_size);
    msc_get_count = (msc_get_count +1) % TC3170_MSC_BUF_SIZE;
#endif
  }
  else
  {
    TDMB_MSG_TCC_BB("MSC NULL !! [%d]\n", msc_size);
  }
  tcc_data.tcc_msc_size = 0;
  memset((void*)&tcc_data, 0x00, sizeof(tcc_data_type));

#ifdef MSC_BUFFER_FOR_TCC_SDK
  msc_put_cnt = 0;
#endif

  return msc_size;
}


/*====================================================================
FUNCTION       tcc3170_reconfig_n_ber
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
void tcc3170_get_ber(tdmb_bb_sig_type *psigs)
{
  uint32_t pcber, pcber_moving_avg;
  uint32_t snr, snrdb, snr_moving_avg;
  uint32_t rsber, rsber_moving_avg;
  uint32_t rssi, rssi_moving_avg;
  int ret;

  ret = TC317X_API_GetTDMB_PCBER(0, 0, &pcber, &pcber_moving_avg);
  if(ret < 0) 
  { 
    TDMB_MSG_TCC_BB("PCBER error\n");
  }

  ret = TC317X_API_GetTDMB_SNR(0, 0, &snr, &snrdb, &snr_moving_avg);
  if(ret < 0) 
  { 
    TDMB_MSG_TCC_BB("SNR error\n"); 
  }

  ret = TC317X_API_GetTDMB_VITERBIBER(0, 0, &rsber, &rsber_moving_avg);
  if(ret < 0) 
  { 
    TDMB_MSG_TCC_BB("TSPER error\n"); 
  }

  ret = TC317X_API_GetTDMB_RSSI(0, 0, &rssi, &rssi_moving_avg);
  if(ret < 0) 
  { 
    TDMB_MSG_TCC_BB("RSSI error\n"); 
  }

  psigs->PCBER = pcber_moving_avg;
  psigs->RSBER = rsber_moving_avg;
  psigs->SNR = rssi;//snr_moving_avg;
  psigs->RSSI = tcc3170_Ant_Level(pcber_moving_avg, snr_moving_avg);

  //TDMB_MSG_TCC_BB("Ant[%d] pcber[%d] rssi[%d] rsber[%d] snr[%d]\n", psigs->RSSI, psigs->PCBER, psigs->SNR, psigs->RSBER, snr_moving_avg);
}


/*====================================================================
FUNCTION       tcc3170_Ant_Level
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
int tcc3170_Ant_Level(uint32 pcber, int snr)
{
  // set ant level (0 ~ 6 level)
#if 0
  //from TCC SDK
  if(pcber <= 50) ant_level = 0;
  else if(pcber <= 30000) ant_level = 5;
  else if(pcber <= 50000) ant_level = 4;
  else if(pcber <= 70000) ant_level = 3;
  else if(pcber <= 90000) ant_level = 2;
  else if(pcber <= 120000) ant_level = 1;
  else if(pcber > 120000) ant_level = 0;
#endif

  uint8 level = 0;
  static uint8 save_level = 0;
  uint32 ant_tbl[] = {120000, // 0 <-> 1
                       90000,   // 1 <-> 2
                       70000,   // 2 <-> 3
                       50000,   // 3 <-> 4
                       30000};  // 4 <-> 5

  uint16 hystery_value[]= {5000,5000};
  
  if(pcber >= ant_tbl[0] || pcber <= 50) level = 0;
  else if(pcber >= ant_tbl[1] && pcber < ant_tbl[0]) level = 1;
  else if(pcber >= ant_tbl[2] && pcber < ant_tbl[1]) level = 2;
  else if(pcber >= ant_tbl[3] && pcber < ant_tbl[2]) level = 3;
  else if(pcber >= ant_tbl[4] && pcber < ant_tbl[3]) level = 4;
  else if(pcber < ant_tbl[4]) level = 5;
   
  if((level > 0) && (abs(level - save_level) == 1))
  {
    if (pcber < (ant_tbl[level - 1] - (level < 3 ? hystery_value[0] : hystery_value[1])))
    {
      save_level = level;
    }
  }
  else 
  {
    save_level = level;
  }

  return save_level;

}


/*====================================================================
FUNCTION       tcc3170_header_parsing_tsif_data
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
uint16 tcc3170_header_parsing_tsif_data(uint8* p_data_buf, uint32 size)
{
  // TSIF data parsing MSC, FIC, BER

  uint16 fic_Size=0, ts_size=0;

  /* ATTENTION!! 
   * callback to TC317X_TDMB_OccurEvent */
  TC317X_API_ParseData(0, 0, p_data_buf, size);  

  fic_Size = tcc_data.tcc_fic_size;
  ts_size = tcc_data.tcc_msc_size;

  return ts_size;
}

/*====================================================================
FUNCTION       tcc3170_put_fic_data
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
int tcc3170_put_fic_data(uint8* data_buf, uint32 size, uint16 crc, uint8 type)
{
  //TDMB_MSG_TCC_BB("[%s]\n", __func__);

  switch(type)
  {
    case SRVTYPE_FIC:
    case SRVTYPE_FIC_WITH_ERR:
      //TDMB_MSG_TCC_BB("Put  FIC type[%d] size[%d]\n", type,(int)size);
      tcc_data.tcc_fic_size = size;
      memcpy(tcc_data.tcc_fic_buf, data_buf, size);
    break;
    
    case EWS_ANNOUNCE_FLAG:
    case RECONFIGURATION_FLAG:
    case EWS_ANNOUNCE_RECONFIGURATION_FLAG:
      TDMB_MSG_TCC_BB("FIC do nothing  type[%d] size[%d]\n", type,(int)size);
    break;

    default:
      TDMB_MSG_TCC_BB("[%s] Not support stream type[%d]\n", __func__, type);
    break;
  }

  return TRUE;
}


/*====================================================================
FUNCTION       tcc3170_put_msc_data
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
int tcc3170_put_msc_data(uint8* data_buf, uint32 size, uint16 crc, uint8 type)
{
  //TDMB_MSG_TCC_BB("[%s] crc[%d] type[%d]\n", __func__, crc, type);

  switch(type)
  {    
    case SRVTYPE_DMB:
    case SRVTYPE_DAB:
#ifdef MSC_BUFFER_FOR_TCC_SDK
      memcpy((void*)&tcc_data.tcc_msc_buf[tcc_data.tcc_msc_size], data_buf, size);
      tcc_data.tcc_msc_size += size;
      msc_put_cnt++;
      TDMB_MSG_TCC_BB("Put  MSC  -- now size[%d] x [%d] = total[%d] \n", (int)size, msc_put_cnt, tcc_data.tcc_msc_size);
#else
#ifdef FEATURE_TS_PKT_MSG
      TDMB_MSG_TCC_BB("--> [%d] Put MSC size[%d]  [%x] [%x]\n",type,(int)size,data_buf[0],data_buf[1]);
#endif
      tcc_data.tcc_msc_size = size;
      memcpy((void*)&tcc_data.tcc_msc_buf[msc_put_count*TC3170_MSC_BUF_SIZE], data_buf, size);
      msc_put_count = (msc_put_count +1) % TC3170_MSC_BUF_SIZE;
#endif
#if 0//def FEATURE_TS_PKT_MSG
      if(type == SRVTYPE_DMB)
      {
        uint16 cnt;
        for(cnt=0; cnt<size;cnt+=188)
        {
          if((data_buf[cnt+0] != 0x47) )//||(data_buf[cnt+1] == 0x80))
          {
            TDMB_MSG_TCC_BB("%s [%x] [%x] [%x] [%x]\n", __func__, data_buf[cnt+0],data_buf[cnt+1], data_buf[cnt+2], data_buf[cnt+3]);
          }
        }
      }
#endif
    break;

    case SRVTYPE_DATA:
      TDMB_MSG_TCC_BB("[%s] Data Do nothing.. type[%d]\n", __func__, type);
    break;

    default:
      TDMB_MSG_TCC_BB("[%s] Not support stream type[%d]\n", __func__, type);
    break;
  }

  return TRUE;
}

/*================================================================== */
/*=================    TCC3170 BB Test Function     ================== */
/*================================================================== */

/*====================================================================
FUNCTION       tcc3170_test
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
void tcc3170_test(int ch)
{
  // Just start TCC3170 & check MSC data
  chan_info tcc_ch_Info;
  uint8 result;

  TDMB_MSG_TCC_BB("[%s] Start\n", __func__);

  tcc3170_init();

  //while(1)
  { 
    tdmb_get_fixed_chan_info((service_t)ch, &tcc_ch_Info);
 
    TDMB_MSG_TCC_BB("[%s]  freq[%d] type[%d] subchid[0x%x]\n", __func__, (int)tcc_ch_Info.ulRFNum, tcc_ch_Info.uiServiceType, tcc_ch_Info.uiSubChID);

    tcc3170_set_subchannel_info(&tcc_ch_Info);
  
    tcc3170_ch_scan_start((int)tcc_ch_Info.ulRFNum, 0,0);
  
    result = tcc3170_subch_start((uint8*)&tcc_ch_Info, (uint32)tcc_ch_Info.uiBitRate);
    
    TDMB_MSG_TCC_BB("[%s] end !![%d]\n", __func__, result);
  
    msleep(500);
    msleep(500);
  }
}


/*====================================================================
FUNCTION       tcc3170_rw_test
DESCRIPTION  
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
uint8 tcc3170_rw_test(void) 
{
  uint16  data;

  // chip version read -> default 0x37
  data = TC317X_API_REG_Read(0, 0, 0x0C);
  TDMB_MSG_TCC_BB("[%s] Chip ID : [0x%x]\n",__func__, data);
  
  TDMB_MSG_TCC_BB("[%s] tcc3170 RW Test end\n", __func__);

  return 0;
}

