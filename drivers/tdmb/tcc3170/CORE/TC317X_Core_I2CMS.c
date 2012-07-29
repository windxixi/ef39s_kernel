//=============================================================================
// Telechips Confidential Proprietary
// Copyright (C) 2005-2009 Telechips Corporation
// All rights are reserved by Telechips Corporation
// File       : TC317X_Core_I2CMS.c
// Description:
//
// Revision History:
// Version    Date             Author             Description of Changes
//-----------------------------------------------------------------------------
// 1.0.0       2009/04/16   stkim              Draft
//=============================================================================
#include "../IO/TC317X_IO.h"
#if defined (USE_IF_I2C)
#include "../IO/TC317X_IO_I2C.h"
#endif // USE_IF_I2C
#include "TC317X_Core.h"
#include "TC317X_Core_RegCtrl.h"
#include "TC317X_Core_I2CMS.h"
#include "TC317X_Core_Util.h"

//---------------------------------------------------------------------
//
// I2C Master OneShot Mode - Not Interrupt but Polling
//
void TC317X_InitI2CMS_Polling (t_TC3X_DEMODULE * pTC317X, int32_t in_khz)
{
  // Use OneShot Mode
  //[27]  RW  I2C Master Enable
  //[26]  RW  I2C Master CMD mode
  //[25]  RW  I2C Master Interrput Enable
  //[15:0]    RW  I2C Master PRER

  uint32_t TC317XMainClk;
  uint32_t I2CClk;
  uint32_t PRER;

  TC317XMainClk = pTC317X->MainClk_Khz;

  I2CClk = in_khz;        // 400KHz or 100KHz

  PRER = (((TC317XMainClk / (5 * I2CClk)) - 1) & 0xffff);
  //I2C Mater Enable, CMD enable, PRER Setting
  TC317XREGSET_I2CMST_CTRL (pTC317X, TC3XREG_I2CMST_EN | TC3XREG_I2CMST_AUTOMODE, 0);
  TC317XREGSET_I2CMST_PRER (pTC317X, PRER, 0);
  //TC3XREGSET_I2CMST_FILTER (pTC3X, 0x07, 0);
}

uint32_t TC317X_I2CMS_Polling_Write8 (t_TC3X_DEMODULE * pTC317X, uint8_t deviceaddr, uint8_t targetaddr, uint8_t input)
{
  uint32_t Data;
  uint32_t wait;
  uint32_t cnt;
#if defined (USE_ANDROID)
  long long CurrTime;
#else
  uint32_t CurrTime;
#endif // USE_ANDROID
  uint32_t needs;
  int32_t       semret;

  TC317X_IO_IF_LOCK (pTC317X->moduleidx);

  // auto cmd start, device addr, target addr, input
  TC317XREGSET_I2CMST_AutoConfig (pTC317X, TC3XREG_I2CMST_AUTOCONFIG_CMDQUE_INIT, 1);
  TC317XREGSET_I2CMST_DevAddr (pTC317X, deviceaddr | TC3XREG_I2CMST_DEV_ADDR_WRITE, 1);
  TC317XREGSET_I2CMST_AutoTargetAddr (pTC317X, targetaddr, 1);
  TC317XREGSET_I2CMST_TXRRXR (pTC317X, input, 1);
  TC317XREGSET_I2CMST_AutoConfig (pTC317X, TC3XREG_I2CMST_AUTOCONFIG_CMDQUE_ADD, 1);
  semret = TC317XREGGET_I2CMST_AutoConfig (pTC317X, 1);
  TC317XREGSET_I2CMST_Start (pTC317X, TC3XREG_I2CMST_AUTOSTART, 1);
  cnt = 30;
  wait = 1;

  needs = (TC3XREG_I2CMST_STAT1_AUTODONE | TC3XREG_I2CMST_STAT1_AUTOERR);

  CurrTime = TC317X_GetTickCnt ();
  do
  {
    Data = TC317XREGGET_I2CMST_Stat1 (pTC317X, 1);
    if (TC317X_GetInterval (CurrTime) > 2000000)
    {
      PRINTF_LV_0 ("# I2C WriteTimeERR[0x%x]\n", targetaddr);
      TC317X_IO_IF_UnLOCK (pTC317X->moduleidx);
      return 1;
    }
  }
  while (!(needs & Data));

  TC317X_IO_IF_UnLOCK (pTC317X->moduleidx);

  if (Data & TC3XREG_I2CMST_STAT1_AUTOERR)
  {
    PRINTF_LV_0 ("# I2C WriteERR[0x%02x]\n", targetaddr);
    //return 1; // ERR
    return 0;   // ?ӽ?
  }
  else
  {
    return 0;   // OK
  }

  return 0;
}

uint32_t TC317X_I2CMS_Polling_Read8 (t_TC3X_DEMODULE * pTC317X, uint8_t deviceaddr, uint8_t targetaddr, uint8_t *pData)
{
  uint32_t Data;
  uint32_t wait;
  uint32_t cnt;
#if defined (USE_ANDROID)
  long long CurrTime;
#else
  uint32_t CurrTime;
#endif // USE_ANDROID
  uint32_t needs;


  TC317X_IO_IF_LOCK (pTC317X->moduleidx);

  // auto cmd start, read deviceaddr, target addr
  TC317XREGSET_I2CMST_AutoConfig (pTC317X, TC3XREG_I2CMST_AUTOCONFIG_CMDQUE_INIT, 1);
  TC317XREGSET_I2CMST_DevAddr (pTC317X, deviceaddr | TC3XREG_I2CMST_DEV_ADDR_READ, 1);
  TC317XREGSET_I2CMST_AutoTargetAddr (pTC317X, targetaddr, 1);
  TC317XREGSET_I2CMST_AutoConfig (pTC317X, TC3XREG_I2CMST_AUTOCONFIG_CMDQUE_ADD, 1);
  TC317XREGSET_I2CMST_Start (pTC317X, TC3XREG_I2CMST_AUTOSTART, 1);

  cnt = 30;
  wait = 1;

  needs = (TC3XREG_I2CMST_STAT1_AUTODONE | TC3XREG_I2CMST_STAT1_AUTOERR);

  CurrTime = TC317X_GetTickCnt ();
  do
  {
    Data = TC317XREGGET_I2CMST_Stat1 (pTC317X, 1);
    if (TC317X_GetInterval (CurrTime) > 2000000)
    {
      PRINTF_LV_0 ("# I2C ReadTimeERR\n");
      pData[0] = 0;
      TC317X_IO_IF_UnLOCK (pTC317X->moduleidx);
      return 1;
    }
  }
  while (!(needs & Data));

  if (Data & TC3XREG_I2CMST_STAT1_AUTOERR)
  {
    pData[0] = 0;
    PRINTF_LV_0 ("# I2C ReadERR[0x%02x]\n", targetaddr);
    TC317X_IO_IF_UnLOCK (pTC317X->moduleidx);
    return 0;   // ?ӽ?
  }
  else
  {
    Data = TC317XREGGET_I2CMST_TXRRXR (pTC317X, 1);
    pData[0] = (Data & 0xff);
    TC317X_IO_IF_UnLOCK (pTC317X->moduleidx);
    return 0;   // OK
  }
  return 0;
}

// ------------------------------------------------------------------------------------------------
//
// multi write - use admtv803 and etc...
//
uint32_t TC317X_I2CMS_Polling_MultiWrite8 (t_TC3X_DEMODULE * pTC317X, uint8_t deviceaddr, uint8_t *ptargetaddr,
    uint8_t *pinput, int32_t length)
{
#if defined (USE_IF_I2C)
  //uint32_t roop;
#endif

  uint32_t Data;
#if defined (USE_ANDROID)
  long long CurrTime;
#else
  uint32_t CurrTime;
#endif // USE_ANDROID
  uint32_t needs;
  int32_t       i;

  if (length > 16)
  {
    PRINTF_LV_0 ("# MaxRegAccessExeed[Max:16]\n");
    length = 16;
  }

  TC317X_IO_IF_LOCK (pTC317X->moduleidx);

  // auto cmd start, device addr, target addr, input
  TC317XREGSET_I2CMST_AutoConfig (pTC317X, TC3XREG_I2CMST_AUTOCONFIG_CMDQUE_INIT, 1);
  TC317XREGSET_I2CMST_DevAddr (pTC317X, deviceaddr | TC3XREG_I2CMST_DEV_ADDR_WRITE, 1);
  for (i = 0; i < length; i++)
  {
    TC317XREGSET_I2CMST_AutoTargetAddr (pTC317X, ptargetaddr[i], 1);
    TC317XREGSET_I2CMST_TXRRXR (pTC317X, pinput[i], 1);
    TC317XREGSET_I2CMST_AutoConfig (pTC317X, TC3XREG_I2CMST_AUTOCONFIG_CMDQUE_ADD, 1);
  }
  TC317XREGSET_I2CMST_Start (pTC317X, TC3XREG_I2CMST_AUTOSTART, 1);
  needs = (TC3XREG_I2CMST_STAT1_AUTODONE | TC3XREG_I2CMST_STAT1_AUTOERR);

  CurrTime = TC317X_GetTickCnt ();
  do
  {
    Data = TC317XREGGET_I2CMST_Stat1 (pTC317X, 1);
    if (TC317X_GetInterval (CurrTime) > 2000000)
    {
      PRINTF_LV_0 ("# I2C WriteTimeERR[multiwrite]\n");
      TC317X_IO_IF_UnLOCK (pTC317X->moduleidx);
      return 1;
    }
  }
  while (!(needs & Data));

  TC317X_IO_IF_UnLOCK (pTC317X->moduleidx);

  if (Data & TC3XREG_I2CMST_STAT1_AUTOERR)
  {
    PRINTF_LV_0 ("# I2C WriteERR\n");
    return 0;   // ?ӽ?
  }
  else
  {
    return 0;   // OK
  }

  return 0;
}

uint32_t TC317X_I2CMS_Polling_MultiRead8 (t_TC3X_DEMODULE * pTC317X, uint8_t deviceaddr, uint8_t *ptargetaddr,
    uint8_t *pData, int32_t length)
{
#if defined (USE_IF_I2C)
  //uint32_t roop;
#endif

  uint32_t Data;
#if defined (USE_ANDROID)
  long long CurrTime;
#else
  uint32_t CurrTime;
#endif // USE_ANDROID
  uint32_t needs;
  uint32_t i;

  if (length > 16)
  {
    PRINTF_LV_0 ("# MaxRegAccessExeed[Max:16]\n");
    length = 16;
  }

  TC317X_IO_IF_LOCK (pTC317X->moduleidx);

  TC317X_IO_memset (pData, 0, length);

  // auto cmd start, read deviceaddr, target addr

  TC317XREGSET_I2CMST_AutoConfig (pTC317X, TC3XREG_I2CMST_AUTOCONFIG_CMDQUE_INIT, 1);
  TC317XREGSET_I2CMST_DevAddr (pTC317X, deviceaddr | TC3XREG_I2CMST_DEV_ADDR_READ, 1);
  for (i = 0; i < (uint32_t) (length); i++)
  {
    TC317XREGSET_I2CMST_AutoTargetAddr (pTC317X, ptargetaddr[i], 1);
    TC317XREGSET_I2CMST_AutoConfig (pTC317X, TC3XREG_I2CMST_AUTOCONFIG_CMDQUE_ADD, 1);
  }
  TC317XREGSET_I2CMST_Start (pTC317X, TC3XREG_I2CMST_AUTOSTART, 1);
  needs = (TC3XREG_I2CMST_STAT1_AUTODONE | TC3XREG_I2CMST_STAT1_AUTOERR);

  CurrTime = TC317X_GetTickCnt ();
  do
  {
    Data = TC317XREGGET_I2CMST_Stat1 (pTC317X, 1);
    if (TC317X_GetInterval (CurrTime) > 2000000)
    {
      PRINTF_LV_0 ("# I2C ReadTimeERR\n");
      pData[0] = 0;
      TC317X_IO_IF_UnLOCK (pTC317X->moduleidx);
      return 1;
    }
  }
  while (!(needs & Data));

  if (Data & TC3XREG_I2CMST_STAT1_AUTOERR)
  {
    PRINTF_LV_0 ("# I2C ReadERR\n");
    TC317X_IO_IF_UnLOCK (pTC317X->moduleidx);
    return 0;   // ?ӽ?
  }
  else
  {
    for (i = 0; i < (uint32_t) (length); i++)
    {
      pData[i] = (TC317XREGGET_I2CMST_TXRRXR (pTC317X, 1) & 0xFF);
    }
    TC317X_IO_IF_UnLOCK (pTC317X->moduleidx);
    return 0;   // OK
  }

  TC317X_IO_IF_UnLOCK (pTC317X->moduleidx);
  return 0;
}

//-------------------------------------------------------------------------------------------
// For Diversity SDIO, CSPI IF
//
void TC317X_SlaveIF_I2CMS_Init (t_TC3X_DEMODULE * pTC317X, int32_t in_khz)
{
  TC317X_InitI2CMS_Polling (pTC317X, in_khz);
}

int32_t TC317X_SlaveIF_I2CMS_Waiting (int32_t moduleidx, uint32_t Mode, uint32_t Needs, uint32_t ErrorCondition, uint32_t *pData)
{
#if defined (USE_ANDROID)
  long long CurrTime;
#else
  uint32_t CurrTime;
#endif // USE_ANDROID
  uint32_t Data;

  CurrTime = TC317X_GetTickCnt ();
  do
  {
    if (Mode == TC3XREG_I2CMST_AUTOMODE)
      Data = TC317XREGGET_I2CMST_Stat1 (&g_pTC3X[moduleidx][0], 1);
    else
      Data = TC317XREGGET_I2CMST_Stat0 (&g_pTC3X[moduleidx][0], 1);

    *pData = Data;
    if (TC317X_GetInterval (CurrTime) > 2000000)
    {
      PRINTF_LV_0 ("# [SlaveIF] I2C Time ERR\n");
      return -1;  // Timeout
    }
  }
  while (!(Needs & Data));

  if (Data & ErrorCondition)
  {
    PRINTF_LV_0 ("# [SlaveIF] I2C ERR [0x%02x] \n", Data);
    return -2;      // Data Error
  }

  return 0;
}

void TC317X_SlaveIF_I2CMS_Ending (int32_t moduleidx, int32_t IFunLock, int32_t Reserved1)
{
  if (IFunLock)
    TC317X_IO_IF_UnLOCK (moduleidx);
}

uint32_t TC317X_SlaveIF_I2CMS_Reg_Read (int32_t moduleidx, int32_t ChipAddr, int32_t RegAddr, TC3X_IO_Err * pError)       // use 52cmd
{
  uint32_t retv;
  uint32_t Data;
  int32_t       ret;

  retv = 0;
  TC317X_IO_IF_LOCK (moduleidx);

  // auto cmd start, read deviceaddr, target addr
  TC317XREGSET_I2CMST_AutoConfig (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_AUTOCONFIG_CMDQUE_INIT, 1);
  TC317XREGSET_I2CMST_DevAddr (&g_pTC3X[moduleidx][0], ChipAddr | TC3XREG_I2CMST_DEV_ADDR_READ, 1);
  TC317XREGSET_I2CMST_AutoTargetAddr (&g_pTC3X[moduleidx][0], RegAddr, 1);
  TC317XREGSET_I2CMST_AutoConfig (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_AUTOCONFIG_CMDQUE_ADD, 1);
  TC317XREGSET_I2CMST_Start (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_AUTOSTART, 1);

  ret = TC317X_SlaveIF_I2CMS_Waiting (moduleidx, TC3XREG_I2CMST_AUTOMODE, TC3XREG_I2CMST_STAT1_AUTODONE | TC3XREG_I2CMST_STAT1_AUTOERR,
      TC3XREG_I2CMST_STAT1_AUTOERR | TC3XREG_I2CMST_STAT1_ARBIT_LOST, &Data);

  if (ret != 0)
  {
    TC317X_SlaveIF_I2CMS_Ending (moduleidx, 1, 0x00);
    return TC3X_IO_FAIL;
  }
  else
  {
    retv = TC317XREGGET_I2CMST_TXRRXR (&g_pTC3X[moduleidx][0], 1);
    TC317X_SlaveIF_I2CMS_Ending (moduleidx, 1, 0x00);
    return retv;    // OK
  }
}

TC3X_IO_Err TC317X_SlaveIF_I2CMS_Reg_Write (int32_t moduleidx, int32_t ChipAddr, int32_t RegAddr, uint32_t data)  // use 52cmd
{
  uint32_t Data;
  int32_t       ret;

  TC317X_IO_IF_LOCK (moduleidx);

  // auto cmd start, device addr, target addr, input
  TC317XREGSET_I2CMST_AutoConfig (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_AUTOCONFIG_CMDQUE_INIT, 1);
  TC317XREGSET_I2CMST_DevAddr (&g_pTC3X[moduleidx][0], ChipAddr | TC3XREG_I2CMST_DEV_ADDR_WRITE, 1);
  TC317XREGSET_I2CMST_AutoTargetAddr (&g_pTC3X[moduleidx][0], RegAddr, 1);
  TC317XREGSET_I2CMST_TXRRXR (&g_pTC3X[moduleidx][0], data, 1);
  TC317XREGSET_I2CMST_AutoConfig (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_AUTOCONFIG_CMDQUE_ADD, 1);
  TC317XREGSET_I2CMST_Start (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_AUTOSTART, 1);

  ret = TC317X_SlaveIF_I2CMS_Waiting (moduleidx, TC3XREG_I2CMST_AUTOMODE, TC3XREG_I2CMST_STAT1_AUTODONE | TC3XREG_I2CMST_STAT1_AUTOERR,
      TC3XREG_I2CMST_STAT1_AUTOERR | TC3XREG_I2CMST_STAT1_ARBIT_LOST, &Data);

  if (ret != 0)
  {
    TC317X_SlaveIF_I2CMS_Ending (moduleidx, 1, 0x00);
    return TC3X_IO_FAIL;
  }
  else
  {
    TC317X_SlaveIF_I2CMS_Ending (moduleidx, 1, 0x00);
    return TC3X_IO_SUCCESS; // OK
  }
}

TC3X_IO_Err TC317X_SlaveIF_I2CMS_Reg_ReadEX (int32_t moduleidx, int32_t ChipAddr, int32_t RegAddr, uint8_t *data, int32_t iSize)    // use 53cmd
{
  uint32_t i;
  uint32_t Data;
  uint32_t index;
  int32_t       ret;

  TC317X_IO_IF_LOCK (moduleidx);

  RegAddr = (RegAddr & 0xFF) | Bit7;
  index = 0;

  TC317XREGSET_I2CMST_CTRL (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_INIT | TC3XREG_I2CMST_EN | TC3XREG_I2CMST_NORMALMODE | TC3XREG_I2CMST_SCL_OPENDRAIN, 1);
  TC317XREGSET_I2CMST_TXRRXR (&g_pTC3X[moduleidx][0], ChipAddr | TC3XREG_I2CMST_DEV_ADDR_WRITE, 1);
  TC317XREGSET_I2CMST_Start (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_NORMALSTART | TC3XREG_I2CMST_NORMALWRITE, 1);

  ret = TC317X_SlaveIF_I2CMS_Waiting (moduleidx, TC3XREG_I2CMST_NORMALMODE,
      TC3XREG_I2CMST_STAT0_NORMAL_DONE | TC3XREG_I2CMST_STAT0_NORMAL_ACKIN |
      TC3XREG_I2CMST_STAT0_ARBIT_LOST,
      TC3XREG_I2CMST_STAT0_NORMAL_ACKIN | TC3XREG_I2CMST_STAT0_ARBIT_LOST, &Data);

  if (ret != 0)
  {
    TC317X_SlaveIF_I2CMS_Ending (moduleidx, 1, 0x00);
    return TC3X_IO_FAIL;
  }

  TC317XREGSET_I2CMST_TXRRXR (&g_pTC3X[moduleidx][0], RegAddr, 1);
  TC317XREGSET_I2CMST_Start (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_NORMALWRITE | TC3XREG_I2CMST_NORMALSTOP, 1);

  ret = TC317X_SlaveIF_I2CMS_Waiting (moduleidx, TC3XREG_I2CMST_NORMALMODE,
      TC3XREG_I2CMST_STAT0_NORMAL_DONE | TC3XREG_I2CMST_STAT0_NORMAL_ACKIN |
      TC3XREG_I2CMST_STAT0_ARBIT_LOST,
      TC3XREG_I2CMST_STAT0_NORMAL_ACKIN | TC3XREG_I2CMST_STAT0_ARBIT_LOST, &Data);

  if (ret != 0)
  {
    TC317X_SlaveIF_I2CMS_Ending (moduleidx, 1, 0x00);
    return TC3X_IO_FAIL;
  }

  // ReStart
  TC317XREGSET_I2CMST_TXRRXR (&g_pTC3X[moduleidx][0], ChipAddr | TC3XREG_I2CMST_DEV_ADDR_READ, 1);
  TC317XREGSET_I2CMST_Start (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_NORMALSTART | TC3XREG_I2CMST_NORMALWRITE, 1);

  ret = TC317X_SlaveIF_I2CMS_Waiting (moduleidx, TC3XREG_I2CMST_NORMALMODE,
      TC3XREG_I2CMST_STAT0_NORMAL_DONE | TC3XREG_I2CMST_STAT0_NORMAL_ACKIN |
      TC3XREG_I2CMST_STAT0_ARBIT_LOST,
      TC3XREG_I2CMST_STAT0_NORMAL_ACKIN | TC3XREG_I2CMST_STAT0_ARBIT_LOST, &Data);

  if (ret != 0)
  {
    TC317X_SlaveIF_I2CMS_Ending (moduleidx, 1, 0x00);
    return TC3X_IO_FAIL;
  }

  for (i = 0; i < (uint32_t) (iSize); i++)
  {
    if (i == (uint32_t) (iSize - 1))
      TC317XREGSET_I2CMST_Start (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_NORMALREAD | TC3XREG_I2CMST_NORMALACK | TC3XREG_I2CMST_NORMALSTOP, 1);
    else
      TC317XREGSET_I2CMST_Start (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_NORMALREAD, 1);

    ret = TC317X_SlaveIF_I2CMS_Waiting (moduleidx, TC3XREG_I2CMST_NORMALMODE,
        TC3XREG_I2CMST_STAT0_NORMAL_DONE | TC3XREG_I2CMST_STAT0_ARBIT_LOST,
        TC3XREG_I2CMST_STAT0_ARBIT_LOST, &Data);

    if (ret != 0)
    {
      TC317X_SlaveIF_I2CMS_Ending (moduleidx, 1, 0x00);
      return TC3X_IO_FAIL;
    }

    data[i] = TC317XREGGET_I2CMST_TXRRXR (&g_pTC3X[moduleidx][0], 1);
  }

  TC317X_SlaveIF_I2CMS_Ending (moduleidx, 1, 0x00);
  TC317XREGSET_I2CMST_CTRL (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_INIT | TC3XREG_I2CMST_EN | TC3XREG_I2CMST_AUTOMODE, 0);      // ???󺹿?

  return TC3X_IO_SUCCESS;
}

TC3X_IO_Err TC317X_SlaveIF_I2CMS_Reg_WriteEX (int32_t moduleidx, int32_t ChipAddr, int32_t RegAddr, uint8_t *pCh, int32_t iSize)    // use 53cmd
{
  uint32_t i;
  uint32_t Data;
  uint32_t index;
  int32_t       ret;

  TC317X_IO_IF_LOCK (moduleidx);

  RegAddr = (RegAddr & 0xFF) | Bit7;
  index = 0;

  TC317XREGSET_I2CMST_CTRL (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_INIT | TC3XREG_I2CMST_EN | TC3XREG_I2CMST_NORMALMODE | TC3XREG_I2CMST_SCL_OPENDRAIN, 1);
  TC317XREGSET_I2CMST_TXRRXR (&g_pTC3X[moduleidx][0], ChipAddr | TC3XREG_I2CMST_DEV_ADDR_WRITE, 1);
  TC317XREGSET_I2CMST_Start (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_NORMALSTART | TC3XREG_I2CMST_NORMALWRITE, 1);

  ret = TC317X_SlaveIF_I2CMS_Waiting (moduleidx, TC3XREG_I2CMST_NORMALMODE,
      TC3XREG_I2CMST_STAT0_NORMAL_DONE | TC3XREG_I2CMST_STAT0_NORMAL_ACKIN |
      TC3XREG_I2CMST_STAT0_ARBIT_LOST,
      TC3XREG_I2CMST_STAT0_NORMAL_ACKIN | TC3XREG_I2CMST_STAT0_ARBIT_LOST, &Data);

  if (ret != 0)
  {
    TC317X_SlaveIF_I2CMS_Ending (moduleidx, 1, 0x00);
    return TC3X_IO_FAIL;
  }

  TC317XREGSET_I2CMST_TXRRXR (&g_pTC3X[moduleidx][0], RegAddr, 1);
  TC317XREGSET_I2CMST_Start (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_NORMALWRITE, 1);

  ret = TC317X_SlaveIF_I2CMS_Waiting (moduleidx, TC3XREG_I2CMST_NORMALMODE,
      TC3XREG_I2CMST_STAT0_NORMAL_DONE | TC3XREG_I2CMST_STAT0_NORMAL_ACKIN |
      TC3XREG_I2CMST_STAT0_ARBIT_LOST,
      TC3XREG_I2CMST_STAT0_NORMAL_ACKIN | TC3XREG_I2CMST_STAT0_ARBIT_LOST, &Data);

  if (ret != 0)
  {
    TC317X_SlaveIF_I2CMS_Ending (moduleidx, 1, 0x00);
    return TC3X_IO_FAIL;
  }

  for (i = 0; i < (uint32_t) (iSize); i++)
  {
    TC317XREGSET_I2CMST_TXRRXR (&g_pTC3X[moduleidx][0], pCh[index], 1);
    index++;

    if (i == (uint32_t) (iSize - 1))
      TC317XREGSET_I2CMST_Start (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_NORMALWRITE | TC3XREG_I2CMST_NORMALSTOP, 1);
    else
      TC317XREGSET_I2CMST_Start (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_NORMALWRITE, 1);

    ret = TC317X_SlaveIF_I2CMS_Waiting (moduleidx, TC3XREG_I2CMST_NORMALMODE,
        TC3XREG_I2CMST_STAT0_NORMAL_DONE | TC3XREG_I2CMST_STAT0_NORMAL_ACKIN |
        TC3XREG_I2CMST_STAT0_ARBIT_LOST,
        TC3XREG_I2CMST_STAT0_NORMAL_ACKIN | TC3XREG_I2CMST_STAT0_ARBIT_LOST, &Data);

    if (ret != 0)
    {
      TC317X_SlaveIF_I2CMS_Ending (moduleidx, 1, 0x00);
      return TC3X_IO_FAIL;
    }
  }

  TC317X_SlaveIF_I2CMS_Ending (moduleidx, 1, 0x00);
  TC317XREGSET_I2CMST_CTRL (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_INIT | TC3XREG_I2CMST_EN | TC3XREG_I2CMST_AUTOMODE, 0);      // ???󺹿?

  return TC3X_IO_SUCCESS;
}

// not use semaphore
uint32_t TC317X_SlaveIF_I2CMS_Reg_Read_InnerSem (int32_t moduleidx, int32_t ChipAddr, int32_t RegAddr, TC3X_IO_Err * pError)
{
  uint32_t retv;
  uint32_t Data;
  int32_t       ret;

  retv = 0;

  // auto cmd start, read deviceaddr, target addr
  TC317XREGSET_I2CMST_AutoConfig (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_AUTOCONFIG_CMDQUE_INIT, 1);
  TC317XREGSET_I2CMST_DevAddr (&g_pTC3X[moduleidx][0], ChipAddr | TC3XREG_I2CMST_DEV_ADDR_READ, 1);
  TC317XREGSET_I2CMST_AutoTargetAddr (&g_pTC3X[moduleidx][0], RegAddr, 1);
  TC317XREGSET_I2CMST_AutoConfig (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_AUTOCONFIG_CMDQUE_ADD, 1);
  TC317XREGSET_I2CMST_Start (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_AUTOSTART, 1);

  ret = TC317X_SlaveIF_I2CMS_Waiting (moduleidx, TC3XREG_I2CMST_AUTOMODE, TC3XREG_I2CMST_STAT1_AUTODONE | TC3XREG_I2CMST_STAT1_AUTOERR,
      TC3XREG_I2CMST_STAT1_AUTOERR | TC3XREG_I2CMST_STAT1_ARBIT_LOST, &Data);

  if (ret != 0)
  {
    TC317X_SlaveIF_I2CMS_Ending (moduleidx, 0, 0x00);
    return TC3X_IO_FAIL;
  }

  retv = TC317XREGGET_I2CMST_TXRRXR (&g_pTC3X[moduleidx][0], 1);
  TC317X_SlaveIF_I2CMS_Ending (moduleidx, 0, 0x00);

  return retv;        // OK
}

TC3X_IO_Err TC317X_SlaveIF_I2CMS_Reg_Write_InnerSem (int32_t moduleidx, int32_t ChipAddr, int32_t RegAddr, uint32_t data)
{
  uint32_t Data;
  int32_t       ret;

  // auto cmd start, device addr, target addr, input
  TC317XREGSET_I2CMST_AutoConfig (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_AUTOCONFIG_CMDQUE_INIT, 1);
  TC317XREGSET_I2CMST_DevAddr (&g_pTC3X[moduleidx][0], ChipAddr | TC3XREG_I2CMST_DEV_ADDR_WRITE, 1);
  TC317XREGSET_I2CMST_AutoTargetAddr (&g_pTC3X[moduleidx][0], RegAddr, 1);
  TC317XREGSET_I2CMST_TXRRXR (&g_pTC3X[moduleidx][0], data, 1);
  TC317XREGSET_I2CMST_AutoConfig (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_AUTOCONFIG_CMDQUE_ADD, 1);
  TC317XREGSET_I2CMST_Start (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_AUTOSTART, 1);

  ret = TC317X_SlaveIF_I2CMS_Waiting (moduleidx, TC3XREG_I2CMST_AUTOMODE, TC3XREG_I2CMST_STAT1_AUTODONE | TC3XREG_I2CMST_STAT1_AUTOERR,
      TC3XREG_I2CMST_STAT1_AUTOERR | TC3XREG_I2CMST_STAT1_ARBIT_LOST, &Data);

  if (ret != 0)
  {
    TC317X_SlaveIF_I2CMS_Ending (moduleidx, 0, 0x00);
    return TC3X_IO_FAIL;
  }

  TC317X_SlaveIF_I2CMS_Ending (moduleidx, 0, 0x00);

  return TC3X_IO_SUCCESS;     // OK
}

TC3X_IO_Err TC317X_SlaveIF_I2CMS_Reg_ReadEX_InnerSem (int32_t moduleidx, int32_t ChipAddr, int32_t RegAddr, uint8_t *data, int32_t iSize)
{
  uint32_t i;
  uint32_t Data;
  uint32_t index;
  int32_t       ret;

  RegAddr = (RegAddr & 0xFF) | Bit7;
  index = 0;

  TC317XREGSET_I2CMST_CTRL (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_INIT | TC3XREG_I2CMST_EN | TC3XREG_I2CMST_NORMALMODE | TC3XREG_I2CMST_SCL_OPENDRAIN, 1);
  TC317XREGSET_I2CMST_TXRRXR (&g_pTC3X[moduleidx][0], ChipAddr | TC3XREG_I2CMST_DEV_ADDR_WRITE, 1);
  TC317XREGSET_I2CMST_Start (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_NORMALSTART | TC3XREG_I2CMST_NORMALWRITE, 1);

  ret = TC317X_SlaveIF_I2CMS_Waiting (moduleidx, TC3XREG_I2CMST_NORMALMODE,
      TC3XREG_I2CMST_STAT0_NORMAL_DONE | TC3XREG_I2CMST_STAT0_NORMAL_ACKIN |
      TC3XREG_I2CMST_STAT0_ARBIT_LOST,
      TC3XREG_I2CMST_STAT0_NORMAL_ACKIN | TC3XREG_I2CMST_STAT0_ARBIT_LOST, &Data);

  if (ret != 0)
  {
    TC317X_SlaveIF_I2CMS_Ending (moduleidx, 0, 0x00);
    return TC3X_IO_FAIL;
  }

  TC317XREGSET_I2CMST_TXRRXR (&g_pTC3X[moduleidx][0], RegAddr, 1);
  TC317XREGSET_I2CMST_Start (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_NORMALWRITE | TC3XREG_I2CMST_NORMALSTOP, 1);

  ret = TC317X_SlaveIF_I2CMS_Waiting (moduleidx, TC3XREG_I2CMST_NORMALMODE,
      TC3XREG_I2CMST_STAT0_NORMAL_DONE | TC3XREG_I2CMST_STAT0_NORMAL_ACKIN |
      TC3XREG_I2CMST_STAT0_ARBIT_LOST,
      TC3XREG_I2CMST_STAT0_NORMAL_ACKIN | TC3XREG_I2CMST_STAT0_ARBIT_LOST, &Data);

  if (ret != 0)
  {
    TC317X_SlaveIF_I2CMS_Ending (moduleidx, 0, 0x00);
    return TC3X_IO_FAIL;
  }

  // ReStart
  TC317XREGSET_I2CMST_TXRRXR (&g_pTC3X[moduleidx][0], ChipAddr | TC3XREG_I2CMST_DEV_ADDR_READ, 1);
  TC317XREGSET_I2CMST_Start (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_NORMALSTART | TC3XREG_I2CMST_NORMALWRITE, 1);

  ret = TC317X_SlaveIF_I2CMS_Waiting (moduleidx, TC3XREG_I2CMST_NORMALMODE,
      TC3XREG_I2CMST_STAT0_NORMAL_DONE | TC3XREG_I2CMST_STAT0_NORMAL_ACKIN |
      TC3XREG_I2CMST_STAT0_ARBIT_LOST,
      TC3XREG_I2CMST_STAT0_NORMAL_ACKIN | TC3XREG_I2CMST_STAT0_ARBIT_LOST, &Data);

  if (ret != 0)
  {
    TC317X_SlaveIF_I2CMS_Ending (moduleidx, 0, 0x00);
    return TC3X_IO_FAIL;
  }

  for (i = 0; i < (uint32_t) (iSize); i++)
  {
    if (i == (uint32_t) (iSize - 1))
      TC317XREGSET_I2CMST_Start (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_NORMALREAD | TC3XREG_I2CMST_NORMALACK | TC3XREG_I2CMST_NORMALSTOP, 1);
    else
      TC317XREGSET_I2CMST_Start (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_NORMALREAD, 1);

    ret = TC317X_SlaveIF_I2CMS_Waiting (moduleidx, TC3XREG_I2CMST_NORMALMODE,
        TC3XREG_I2CMST_STAT0_NORMAL_DONE | TC3XREG_I2CMST_STAT0_ARBIT_LOST,
        TC3XREG_I2CMST_STAT0_ARBIT_LOST, &Data);

    if (ret != 0)
    {
      TC317X_SlaveIF_I2CMS_Ending (moduleidx, 0, 0x00);
      return TC3X_IO_FAIL;
    }

    data[i] = TC317XREGGET_I2CMST_TXRRXR (&g_pTC3X[moduleidx][0], 1);
  }

  TC317X_SlaveIF_I2CMS_Ending (moduleidx, 0, 0x00);
  TC317XREGSET_I2CMST_CTRL (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_INIT | TC3XREG_I2CMST_EN | TC3XREG_I2CMST_AUTOMODE, 1);      // ???󺹿?

  return TC3X_IO_SUCCESS;
}

TC3X_IO_Err TC317X_SlaveIF_I2CMS_Reg_WriteEX_InnerSem (int32_t moduleidx, int32_t ChipAddr, int32_t RegAddr, uint8_t *pCh, int32_t iSize)
{
  uint32_t i;
  uint32_t Data;
  uint32_t index;
  int32_t       ret;

  RegAddr = (RegAddr & 0xFF) | Bit7;
  index = 0;

  TC317XREGSET_I2CMST_CTRL (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_INIT | TC3XREG_I2CMST_EN | TC3XREG_I2CMST_NORMALMODE | TC3XREG_I2CMST_SCL_OPENDRAIN, 1);
  TC317XREGSET_I2CMST_TXRRXR (&g_pTC3X[moduleidx][0], ChipAddr | TC3XREG_I2CMST_DEV_ADDR_WRITE, 1);
  TC317XREGSET_I2CMST_Start (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_NORMALSTART | TC3XREG_I2CMST_NORMALWRITE, 1);

  ret = TC317X_SlaveIF_I2CMS_Waiting (moduleidx, TC3XREG_I2CMST_NORMALMODE,
      TC3XREG_I2CMST_STAT0_NORMAL_DONE | TC3XREG_I2CMST_STAT0_NORMAL_ACKIN |
      TC3XREG_I2CMST_STAT0_ARBIT_LOST,
      TC3XREG_I2CMST_STAT0_NORMAL_ACKIN | TC3XREG_I2CMST_STAT0_ARBIT_LOST, &Data);

  if (ret != 0)
  {
    TC317X_SlaveIF_I2CMS_Ending (moduleidx, 0, 0x00);
    return TC3X_IO_FAIL;
  }

  TC317XREGSET_I2CMST_TXRRXR (&g_pTC3X[moduleidx][0], RegAddr, 1);
  TC317XREGSET_I2CMST_Start (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_NORMALWRITE, 1);

  ret = TC317X_SlaveIF_I2CMS_Waiting (moduleidx, TC3XREG_I2CMST_NORMALMODE,
      TC3XREG_I2CMST_STAT0_NORMAL_DONE | TC3XREG_I2CMST_STAT0_NORMAL_ACKIN |
      TC3XREG_I2CMST_STAT0_ARBIT_LOST,
      TC3XREG_I2CMST_STAT0_NORMAL_ACKIN | TC3XREG_I2CMST_STAT0_ARBIT_LOST, &Data);

  if (ret != 0)
  {
    TC317X_SlaveIF_I2CMS_Ending (moduleidx, 0, 0x00);
    return TC3X_IO_FAIL;
  }

  for (i = 0; i < (uint32_t) (iSize); i++)
  {
    TC317XREGSET_I2CMST_TXRRXR (&g_pTC3X[moduleidx][0], pCh[index], 1);
    index++;

    if (i == (uint32_t) (iSize - 1))
      TC317XREGSET_I2CMST_Start (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_NORMALWRITE | TC3XREG_I2CMST_NORMALSTOP, 1);
    else
      TC317XREGSET_I2CMST_Start (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_NORMALWRITE, 1);

    ret = TC317X_SlaveIF_I2CMS_Waiting (moduleidx, TC3XREG_I2CMST_NORMALMODE,
        TC3XREG_I2CMST_STAT0_NORMAL_DONE | TC3XREG_I2CMST_STAT0_NORMAL_ACKIN |
        TC3XREG_I2CMST_STAT0_ARBIT_LOST,
        TC3XREG_I2CMST_STAT0_NORMAL_ACKIN | TC3XREG_I2CMST_STAT0_ARBIT_LOST, &Data);

    if (ret != 0)
    {
      TC317X_SlaveIF_I2CMS_Ending (moduleidx, 0, 0x00);
      return TC3X_IO_FAIL;
    }
  }

  TC317X_SlaveIF_I2CMS_Ending (moduleidx, 0, 0x00);
  TC317XREGSET_I2CMST_CTRL (&g_pTC3X[moduleidx][0], TC3XREG_I2CMST_INIT | TC3XREG_I2CMST_EN | TC3XREG_I2CMST_AUTOMODE, 1);      // ???󺹿?

  return TC3X_IO_SUCCESS;
}
