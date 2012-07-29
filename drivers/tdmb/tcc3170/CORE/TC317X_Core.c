//=============================================================================
// Telechips Confidential Proprietary
// Copyright (C) 2005-2009 Telechips Corporation
// All rights are reserved by Telechips Corporation
// File       : TC317X_Core.c
// Description:
//
// Revision History:
// Version    Date             Author             Description of Changes
//-----------------------------------------------------------------------------
// 1.0.0       2009/04/14   stkim              Draft
//=============================================================================
#include "TC317X_Core.h"
#include "TC317X_deviceAPI.h"
#include "TC317X_Core_Mailbox.h"
#include "TC317X_Core_RegCtrl.h"
#include "TC317X_Core_Peri.h"
#include "TC317X_Core_TDMB.h"
#include "TC317X_Core_I2CMS.h"
#include "TC317X_Core_Mailbox.h"
#include "TC317X_Core_Interrupt.h"
#include "TC317X_Core_GPIO.h"
#include "TC317X_Core_Util.h"
#include "TC317X_Core_RFCtrl.h"
#include "TC317X_device_Interrupt_Task.h"
#include "TCC3170RF.h"

#if defined (USE_IF_SRAMLIKE)
#include "../IO/TC317X_IO_SRAMLIKE.h"
#endif // USE_IF_SRAMLIKE
#if defined (USE_IF_I2C)
#include "../IO/TC317X_IO_I2C.h"
#endif // USE_IF_I2C
#if defined (USE_IF_CSPI)
#include "../IO/TC317X_IO_CSPI.h"
#endif // USE_IF_CSPI
#if defined (USE_IF_SDIO)
#include "../IO/TC317X_IO_SDIO.h"
#endif // USE_IF_SDIO

//--------------------------------------------------------------------
// VERSION Control
//
uint8_t CURR_3XDRV_VERSION[] = { 0, 11, 8 };
uint8_t REQ_PCPROGRAM_VERSION[] = { 2, 1, 10 };
uint8_t REQ_ASM_TDMB_VERSION[] = { 1, 4, 8 };

//
//--------------------------------------------------------------------
int32_t prev_band[4] ={ -1, -1,-1,-1};
int32_t curr_band[4] = { -1, -1,-1,-1};

enum
{
  SEL_HP = 0,
  SEL_LP
};

t_TC3X_DEMODULE **g_pTC3X = NULL;
t_StreamSet g_tStreamSet[MAX_TCC3X];

// message que pointer

tc317x_queue_t *m_TC3X_QUE_INTERRUPT[MAX_TCC3X] = {NULL, };

int32_t  tc317x_err = TC3X_IO_SUCCESS;
uint32_t tc317x_cspi_read_size = TDMB_CSPI_READ_SIZE_FIC;

void TC317X_CoreAttach (int32_t moduleidx, int32_t num)
{
  int32_t       i;
  int32_t       maxbb;
  maxbb = TC317X_IO_Util_GET_MAXBB ();

  if (!g_pTC3X)
  {
    g_pTC3X = (t_TC3X_DEMODULE **) TC317X_IO_malloc (sizeof (t_TC3X_DEMODULE) * maxbb);
    TC317X_IO_memset (&g_pTC3X[0], 0, sizeof (t_TC3X_DEMODULE) * maxbb);
  }

  if (g_pTC3X[moduleidx])
  {
    TC317X_IO_free (g_pTC3X[moduleidx]);
    g_pTC3X[moduleidx] = NULL;
  }

  g_pTC3X[moduleidx] = (t_TC3X_DEMODULE *) (TC317X_IO_malloc (sizeof (t_TC3X_DEMODULE) * num));
  TC317X_IO_memset (&g_pTC3X[moduleidx][0], 0, sizeof (t_TC3X_DEMODULE) * num);

  for (i = num - 1; i >= 0; i--)
  {
    g_pTC3X[moduleidx][i].moduleidx = moduleidx;
    g_pTC3X[moduleidx][i].NumOfDemodule = num;
    //    g_pTC3X[moduleidx][i].INT_TRIG = TC3X_INT_EDGE_RISING;      /* org*/
    g_pTC3X[moduleidx][i].INT_TRIG = TC3X_INT_LEVEL_FALLING;    /* added by KST 101215 */
    g_pTC3X[moduleidx][i].NIT.BW_KHz = 8000;
    g_pTC3X[moduleidx][i].NIT.FREQ_KHz = 666000;
    g_pTC3X[moduleidx][i].NIT.Hierarchy = SEL_HP;

    g_pTC3X[moduleidx][i].PLL_M = 0x0f;
    g_pTC3X[moduleidx][i].PLL_P = 0x00;
    g_pTC3X[moduleidx][i].PLL_S = 0x03;
    g_pTC3X[moduleidx][i].OSC_Khz = DEF_OSCCLK_RF;

    g_pTC3X[moduleidx][i].BoardDefined = TC317X_IO_Util_Get_BoardDefine ();
    g_pTC3X[moduleidx][i].mailboxLog = MAILBOX_LOG_NONE;
  }

  TC317X_IO_IF_SemaphoreInit (moduleidx);
  //TC317X_IO_OP_Mailbox_SemaphoreInit (moduleidx);
  //TC317X_IO_MailBox_SemaphoreInit (moduleidx);

  TC317X_IO_PW_RESET (moduleidx);

  for(i=0; i<4; i++)
  {
    prev_band[i] =-1;
    curr_band[i] = -1;
  }
}

void TC317X_CoreDetach (int32_t moduleidx)
{
  int32_t       i;
  int32_t       exist;
  int32_t       maxbb;
  maxbb = TC317X_IO_Util_GET_MAXBB ();
  exist = 0;

  if (g_pTC3XCTRL[moduleidx][0].bPreInited)
  {
    // Stream Stop
    TC317X_Stream_Stop (moduleidx);

    // Stream Buffer Delete
    TC317X_Stream_DeAlloc (moduleidx);

    // RF Ctrl Delete
    g_pTC3XCTRL[moduleidx][0].RF_Close (moduleidx, 0);
  }

  if (g_pTC3X[moduleidx])
    TC317X_IO_free (g_pTC3X[moduleidx]);

  g_pTC3X[moduleidx] = NULL;

  exist = 0;
  for (i = 0; i < maxbb; i++)
  {
    if (g_pTC3X[i])
    {
      exist = 1;
      break;
    }
  }

  if (!exist)
  {
    TC317X_IO_free (g_pTC3X);
    g_pTC3X = NULL;
  }

  TC317X_IO_IF_SemaphoreDelete (moduleidx);
  TC317X_IO_MailBox_SemaphoreDelete (moduleidx);
  TC317X_IO_OP_Mailbox_SemaphoreDelete (moduleidx);
}

void TC317X_Stream_DeAlloc (int32_t moduleidx)
{
  ;
}

t_TC3X_DEMODULE *GetPointerTC317X (int32_t moduleidx, int32_t idx)
{
  return &g_pTC3X[moduleidx][idx];
}

int32_t TC317X_GetChipID (int32_t moduleidx, int32_t idx)
{
  return (TC317XREGGET_CHIPID (&g_pTC3X[moduleidx][idx]));
}

int32_t TC317X_Stream_Reset(int32_t moduleidx, int32_t idx)
{
  /* Disable buffer */
  TC317XREGSET_Manual(&g_pTC3X[moduleidx][idx], 
    TC3XREG_STREAM_CFG0, 0, 0);

  /* Disable interrupt */
  TC317XREGSET_IRQEN(&g_pTC3X[moduleidx][idx], 0);
  TC317XREGSET_IRQCLR (&g_pTC3X[moduleidx][idx], TC3XREG_IRQ_STATCLR_ALL);

  /* Reset interface */
  TC317XREGSET_Manual(&g_pTC3X[moduleidx][idx], TC3XREG_STREAM_CFG3, 0x12, 0);
  TC317XREGSET_SYSRESET (&g_pTC3X[moduleidx][idx], TC3XREG_SYS_RESET_EP);

  /* Enable interrupt */
  TC317XREGSET_IRQEN(&g_pTC3X[moduleidx][idx], 0x8);

  /* Enable buffer */
  TC317XREGSET_Manual(&g_pTC3X[moduleidx][idx], 
    TC3XREG_STREAM_CFG0, STREAM_BUFFER_ALL, 0);

  return 0;
}

int32_t TC317X_Stream_Cfg(int32_t moduleidx, int32_t idx, int32_t threshold)
{
  int32_t shft = threshold>>2;
  tc317x_cspi_read_size = threshold;
  //TCDBG("tc317x_cspi_read_size : %d\n", tc317x_cspi_read_size);

  /* Disable buffer */
  TC317XREGSET_Manual(&g_pTC3X[moduleidx][idx], 
    TC3XREG_STREAM_CFG0, 0, 0);

  /* Change threshold */
  TC317XREGSET_Manual(&g_pTC3X[moduleidx][idx], 
    TC3XREG_STREAM_CFG1, (shft>>8)&0xFF, 0);
  TC317XREGSET_Manual(&g_pTC3X[moduleidx][idx], 
    TC3XREG_STREAM_CFG2, shft&0xFF, 0);

  /* Reset interface */
  TC317XREGSET_Manual(&g_pTC3X[moduleidx][idx], TC3XREG_STREAM_CFG3, 0x12, 0);
  TC317XREGSET_SYSRESET (&g_pTC3X[moduleidx][idx], TC3XREG_SYS_RESET_EP);
  /* Enable buffer */
  TC317XREGSET_Manual(&g_pTC3X[moduleidx][idx], 
    TC3XREG_STREAM_CFG0, STREAM_BUFFER_ALL, 0);
  
#if 0
#ifdef STREAM_MIXER
  if (g_pTC3X[moduleidx][idx].SubIF.SubIFDefine == TC3X_STREAM_IO_MAINIO)
    if(i==0)
      TC317XREGSET_STREAM_CFG (&g_pTC3X[moduleidx][idx],
          (STREAM_BUFFER_ALL<<24)         |
          ((tc317x_cspi_read_size>>2)<<8) |
          (STREAM_CMD_FIFO_INIT|STREAM_CMD_FIFO_ENABLE));
    else
      TC317XREGSET_STREAM_CFG (&g_pTC3X[moduleidx][idx], 
        (STREAM_BUFFER_ALL<<24));
  else
    TC317XREGSET_STREAM_CFG(&g_pTC3X[moduleidx][idx], (STREAM_BUFFER_ALL<<24));
#else
  if (g_pTC3X[moduleidx][idx].SubIF.SubIFDefine == TC3X_STREAM_IO_MAINIO)
    TC317XREGSET_STREAM_CFG(&g_pTC3X[moduleidx][idx],
      (STREAM_BUFFER_ALL<<24)         |
      ((tc317x_cspi_read_size>>2)<<8) |
      (STREAM_CMD_FIFO_INIT|STREAM_CMD_FIFO_ENABLE));
  else
    TC317XREGSET_STREAM_CFG(&g_pTC3X[moduleidx][idx], (STREAM_BUFFER_ALL<<24));
#endif
#endif
  return 0;
}

void TC317X_CoreInit (int32_t moduleidx, int32_t Reserved0)
{
  int32_t       i;
  int32_t       ChipID;

  for (i = 0; i < (int32_t) (g_pTC3X[moduleidx][0].NumOfDemodule); i++)
  {
    ChipID = TC317X_GetChipID (moduleidx, i);
    // After ColdBoot
#ifdef STREAM_MIXER
    if (i != 0)
    {
      g_pTC3X[moduleidx][i].SubIF.SubIFDefine = TC3X_STREAM_IO_SPIMS;
      g_pTC3X[moduleidx][i].SubIF.SubIFValue = TC3XREG_PERI_SEL_SPI;
    }
    TC317X_CoreStreamIOInit (&g_pTC3X[moduleidx][i]);

    if (i == 0)
      TC317X_CoreSetInterruptMode (moduleidx, i);      // Set Only Master
#else
    if (i == 0)
    {
      TC317X_CoreStreamIOInit (&g_pTC3X[moduleidx][i]);     // Set Only Master
      TC317X_CoreSetInterruptMode (moduleidx, i);      // Set Only Master
    }
#endif

    TC317X_CoreGPIOnEnableSystem (moduleidx, g_pTC3XCTRL[moduleidx][i].hTC3X);

    //TC317X_Stream_Cfg(moduleidx, i, TDMB_CSPI_READ_SIZE_FIC);

    TC317X_SetRFINF (moduleidx, g_pTC3XCTRL[moduleidx][i].hTC3X, 0);

    // rf attach & init.
    // in this case rf attach and init at once.
    if (i == 0)
    {
      TC317X_RFAttach (moduleidx);
    }

    g_pTC3XCTRL[moduleidx][i].hRF = TC317X_RF_Entry (moduleidx, &g_pTC3XCTRL[moduleidx][i]);
    g_pTC3XCTRL[moduleidx][i].RF_UserFunction (moduleidx, g_pTC3XCTRL[moduleidx][i].hRF, MSG_SET_FN_RF_READ, (void *) RF_I2C_Read, NULL,
        NULL, NULL);
    g_pTC3XCTRL[moduleidx][i].RF_UserFunction (moduleidx, g_pTC3XCTRL[moduleidx][i].hRF, MSG_SET_FN_RF_WRITE, (void *) RF_I2C_Write,
        NULL, NULL, NULL);
    g_pTC3XCTRL[moduleidx][i].RF_UserFunction (moduleidx, g_pTC3XCTRL[moduleidx][i].hRF, MSG_SET_FN_RF_WRITEEX, (void *) RF_I2C_WriteEX,
        NULL, NULL, NULL);
#ifdef USE_HOST_FREQ_SET_RF
    //    g_pTC3XCTRL[moduleidx][i].RF_Init (moduleidx, g_pTC3XCTRL[moduleidx][i].hRF, g_pTC3X[moduleidx][0].NIT.BW_KHz);
    g_pTC3XCTRL[moduleidx][i].RF_Init (moduleidx, g_pTC3XCTRL[moduleidx][i].hRF, 0);
#endif
  }
}

void TC317X_CoreIOInit (int32_t moduleidx, TC3X_DNUM hBB, int32_t MainIO, int32_t StreamIO, int32_t PktSize, int32_t PktThrNum)
{
  int32_t       maxbb;
  maxbb = TC317X_IO_Util_GET_MAXBB ();

  if (hBB == 0)
    g_pTC3X[moduleidx][hBB].MasterMode = 1;
  else
    g_pTC3X[moduleidx][hBB].MasterMode = 0;

  g_pTC3X[moduleidx][hBB].MainIF.MainIFDefine = MainIO;
  g_pTC3X[moduleidx][hBB].SubIF.SubIFDefine = StreamIO;
  g_pTC3X[moduleidx][hBB].ChainNum = hBB;

  g_tStreamSet[moduleidx].PktSize = PktSize;
  g_tStreamSet[moduleidx].PktThrNum = PktThrNum;

  switch (MainIO)
  {
    case TC3X_IF_SRAMLIKE:
#if defined (USE_IF_SRAMLIKE)
      g_pTC3X[moduleidx][hBB].DEMOD_AddrBase = 0x50;

      g_pTC3X[moduleidx][hBB].TC3X_Reg_Read = TC317X_IO_SRAMLIKE_Reg_Read;
      g_pTC3X[moduleidx][hBB].TC3X_Reg_Write = TC317X_IO_SRAMLIKE_Reg_Write;
      g_pTC3X[moduleidx][hBB].TC3X_Reg_ReadEx = TC317X_IO_SRAMLIKE_Reg_ReadEx;
      g_pTC3X[moduleidx][hBB].TC3X_Reg_WriteEx = TC317X_IO_SRAMLIKE_Reg_WriteEx;
      g_pTC3X[moduleidx][hBB].TC3X_Reg_Read_InnerSem = TC317X_IO_SRAMLIKE_Reg_Read_InnerSem;
      g_pTC3X[moduleidx][hBB].TC3X_Reg_Write_InnerSem = TC317X_IO_SRAMLIKE_Reg_Write_InnerSem;
      g_pTC3X[moduleidx][hBB].TC3X_Reg_ReadEx_InnerSem = TC317X_IO_SRAMLIKE_Reg_ReadEx_InnerSem;
      g_pTC3X[moduleidx][hBB].TC3X_Reg_WriteEx_InnerSem = TC317X_IO_SRAMLIKE_Reg_WriteEx_InnerSem;
#endif // USE_IF_SRAMLIKE
      break;

    case TC3X_IF_I2C:
#if defined (USE_IF_I2C)
      if (moduleidx == 0)
      {
        g_pTC3X[moduleidx][hBB].DEMOD_AddrBase = TC3X_I2C_DEV_ADDR;
      }
      else
      {
        g_pTC3X[moduleidx][hBB].DEMOD_AddrBase = TC3X_I2C_DEV_ADDR;
      }

      g_pTC3X[moduleidx][hBB].TC3X_Reg_Read = TC317X_IO_I2C_Reg_Read;
      g_pTC3X[moduleidx][hBB].TC3X_Reg_Write = TC317X_IO_I2C_Reg_Write;
      g_pTC3X[moduleidx][hBB].TC3X_Reg_ReadEx = TC317X_IO_I2C_Reg_ReadEx;
      g_pTC3X[moduleidx][hBB].TC3X_Reg_WriteEx = TC317X_IO_I2C_Reg_WriteEx;
      g_pTC3X[moduleidx][hBB].TC3X_Reg_Read_InnerSem = TC317X_IO_I2C_Reg_Read_InnerSem;
      g_pTC3X[moduleidx][hBB].TC3X_Reg_Write_InnerSem = TC317X_IO_I2C_Reg_Write_InnerSem;
      g_pTC3X[moduleidx][hBB].TC3X_Reg_ReadEx_InnerSem = TC317X_IO_I2C_Reg_ReadEx_InnerSem;
      g_pTC3X[moduleidx][hBB].TC3X_Reg_WriteEx_InnerSem = TC317X_IO_I2C_Reg_WriteEx_InnerSem;
#endif // USE_IF_I2C
      break;

    case TC3X_IF_CSPI:
#if defined (USE_IF_CSPI)
      g_pTC3X[moduleidx][hBB].DEMOD_AddrBase = 0x50;
      g_pTC3X[moduleidx][hBB].TC3X_Reg_Read = TC317X_IO_CSPI_Reg_Read;
      g_pTC3X[moduleidx][hBB].TC3X_Reg_Write = TC317X_IO_CSPI_Reg_Write;
      g_pTC3X[moduleidx][hBB].TC3X_Reg_ReadEx = TC317X_IO_CSPI_Reg_ReadEx;
      g_pTC3X[moduleidx][hBB].TC3X_Reg_WriteEx = TC317X_IO_CSPI_Reg_WriteEx;
      g_pTC3X[moduleidx][hBB].TC3X_Reg_Read_InnerSem = TC317X_IO_CSPI_Reg_Read_InnerSem;
      g_pTC3X[moduleidx][hBB].TC3X_Reg_Write_InnerSem = TC317X_IO_CSPI_Reg_Write_InnerSem;
      g_pTC3X[moduleidx][hBB].TC3X_Reg_ReadEx_InnerSem = TC317X_IO_CSPI_Reg_ReadEx_InnerSem;
      g_pTC3X[moduleidx][hBB].TC3X_Reg_WriteEx_InnerSem = TC317X_IO_CSPI_Reg_WriteEx_InnerSem;
#endif // USE_IF_CSPI
      break;

    case TC3X_IF_SDIO1BIT:
    case TC3X_IF_SDIO4BIT:
#if defined (USE_IF_SDIO)
      if (hBB == 0)
      {
        g_pTC3X[moduleidx][hBB].DEMOD_AddrBase = 0x00;
        g_pTC3X[moduleidx][hBB].TC3X_Reg_Read = TC317X_IO_SDIO_Reg_Read;
        g_pTC3X[moduleidx][hBB].TC3X_Reg_Write = TC317X_IO_SDIO_Reg_Write;
        g_pTC3X[moduleidx][hBB].TC3X_Reg_ReadEx = TC317X_IO_SDIO_Reg_ReadEx;
        g_pTC3X[moduleidx][hBB].TC3X_Reg_WriteEx = TC317X_IO_SDIO_Reg_WriteEx;
        g_pTC3X[moduleidx][hBB].TC3X_Reg_Read_InnerSem = TC317X_IO_SDIO_Reg_Read_InnerSem;
        g_pTC3X[moduleidx][hBB].TC3X_Reg_Write_InnerSem = TC317X_IO_SDIO_Reg_Write_InnerSem;
        g_pTC3X[moduleidx][hBB].TC3X_Reg_ReadEx_InnerSem = TC317X_IO_SDIO_Reg_ReadEx_InnerSem;
        g_pTC3X[moduleidx][hBB].TC3X_Reg_WriteEx_InnerSem = TC317X_IO_SDIO_Reg_WriteEx_InnerSem;
      }
      else
      {
        TC317X_SlaveIF_I2CMS_Init (&g_pTC3X[moduleidx][0], 400);

        g_pTC3X[moduleidx][hBB].DEMOD_AddrBase = TC3X_I2C_DEV_ADDR;
        g_pTC3X[moduleidx][hBB].TC3X_Reg_Read = TC317X_SlaveIF_I2CMS_Reg_Read;
        g_pTC3X[moduleidx][hBB].TC3X_Reg_Write = TC317X_SlaveIF_I2CMS_Reg_Write;
        g_pTC3X[moduleidx][hBB].TC3X_Reg_ReadEx = TC317X_SlaveIF_I2CMS_Reg_ReadEX;
        g_pTC3X[moduleidx][hBB].TC3X_Reg_WriteEx = TC317X_SlaveIF_I2CMS_Reg_WriteEX;
        g_pTC3X[moduleidx][hBB].TC3X_Reg_Read_InnerSem = TC317X_SlaveIF_I2CMS_Reg_Read_InnerSem;
        g_pTC3X[moduleidx][hBB].TC3X_Reg_Write_InnerSem = TC317X_SlaveIF_I2CMS_Reg_Write_InnerSem;
        g_pTC3X[moduleidx][hBB].TC3X_Reg_ReadEx_InnerSem = TC317X_SlaveIF_I2CMS_Reg_ReadEX_InnerSem;
        g_pTC3X[moduleidx][hBB].TC3X_Reg_WriteEx_InnerSem = TC317X_SlaveIF_I2CMS_Reg_WriteEX_InnerSem;
      }
#endif // USE_IF_SDIO
      break;
  }
}

void TC317X_PLL_SET (t_TC3X_DEMODULE * pTC317X)
{
  uint8_t    PLL_F, PLL_R, PLL_OD, PLL_M;
  uint32_t OSC_Khz, FOUT;
  uint32_t FVCO;

  pTC317X->PLL_S = (pTC317X->PLL_S & 0x7F);

  PLL_R = ((pTC317X->PLL_P) + 1);
  PLL_F = ((pTC317X->PLL_M) + 1);
  PLL_OD = ((pTC317X->PLL_S) & 0x03);
  PLL_M = (((pTC317X->PLL_S) & 0x04) >> 2);
  OSC_Khz = pTC317X->OSC_Khz;

  FVCO = OSC_Khz * (PLL_F / PLL_R);

  if (PLL_OD)
    FOUT = FVCO >> PLL_OD;
  else
    FOUT = FVCO;

  if (PLL_M)
    FOUT = FOUT >> PLL_M;

  pTC317X->MainClk_Khz = FOUT;

  TC317XREGSET_PLL_7 (pTC317X, (((pTC317X->PLL_P) << 3) | (PLL_OD<<1)));
  TC317XREGSET_PLL_6 (pTC317X, (pTC317X->PLL_M | (PLL_M<<6) | 0x80));

  TC317X_IO_Sleep (1);  // 1ms (orig: 300us)

  PRINTF_LV_0 ("# [%d] PLLSet MainClk[%d]\n", pTC317X->ChainNum, pTC317X->MainClk_Khz);
}

// User Option PLL Set
void TC317X_PreSetPLLSet (t_TC3X_DEMODULE * pTC317X, uint8_t PLL_WaitTime, uint8_t PLL_P, uint8_t PLL_M,
    uint8_t PLL_S, uint32_t OSC_Khz)
{
  pTC317X->PLL_WaitTime = PLL_WaitTime;
  pTC317X->PLL_P = PLL_P;
  pTC317X->PLL_M = PLL_M;
  pTC317X->PLL_S = PLL_S;
  pTC317X->OSC_Khz = OSC_Khz;
  //PRINTF_LV_2 ("# PLL PreSetted\n");
}
uint32_t TC317X_SENDStartMail (t_TC3X_DEMODULE * pTC317X)
{
  tMAILBOX  mailbox;
  uint32_t data;
  int32_t       retmail;

  data = 0x19;    //no power save
  //  data = 0x1b; //power save
#ifndef STREAM_MIXER
  if (pTC317X->NumOfDemodule == 1)
  {
    data = 0x19;        //no power save
    //data = 0x1b; //power save
  }
  else if (pTC317X->MasterMode)
  {
    data = 0x19;
  }
  else
  {
    if (pTC317X->NumOfDemodule == (pTC317X->ChainNum + 1))
      data = 0x15;
    else
      data = 0x11;
  }
#endif

  retmail = TC317X_MB_TxnRead (pTC317X, &mailbox, MBPARA_SYS_START, &data, 1, 0);

  //PRINTF_LV_1 ("# Sending Startmail[ 0x%02x]\n", data);
  if (retmail == -1)
  {
    PRINTF_LV_0 ("# Error - Sending Startmail\n");
  }

  return retmail;
}

uint32_t TC317X_WarmBoot (t_TC3X_DEMODULE * pTC317X, int32_t bStreamStart)
{
  uint32_t data;
  tMAILBOX  mailbox;
  int32_t       retmail;

  data = 0;

  retmail = TC317X_MB_TxnRead (pTC317X, &mailbox, MBPARA_SYS_WARMBOOT, NULL, 0, 1);
  if (retmail == -1)
  {
    PRINTF_LV_0 ("# Error - WarmBoot\n");
  }

  data = mailbox.data_array[0];

  if (data != 0x1acce551)
  {
    PRINTF_LV_0 ("# Error - Can't receive accessmail [0x%x]\n", data);
  }

  if (bStreamStart)
    TC317X_SENDStartMail (pTC317X);

  return data;
}

void TC317X_Set_AGC_Table_Vhf(t_TC3X_DEMODULE * pTC317X)
{
  tMAILBOX  mailbox;
#ifdef TC317X_AGC_IN_BOOT
  mailbox.data_array[0] = 0x0;
  mailbox.data_array[1] = ((195<<16)+(16<<8)+17);
  mailbox.data_array[2] = 0x005a0043;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_JAM, &mailbox.data_array[0], 3);
#else
  mailbox.data_array[0] = 0xC0ACFF44;
  mailbox.data_array[1] = 0x031EFF53;
  mailbox.data_array[2] = 0x011EFF07;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_CFG, &mailbox.data_array[0], 3);

  mailbox.data_array[0] = 0x16112243;
  mailbox.data_array[1] = ((195<<16)+(16<<8)+17);
  mailbox.data_array[2] = 0x005a0043;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_JAM, &mailbox.data_array[0], 3);

  mailbox.data_array[0] = 0x5a220243;
  mailbox.data_array[1] = 0x5e220843;
  mailbox.data_array[2] = 0x60221043;
  mailbox.data_array[3] = 0x62221443;
  mailbox.data_array[4] = 0x64221c43;
  mailbox.data_array[5] = 0x68222043;
  mailbox.data_array[6] = 0x6c222243;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_3, &mailbox.data_array[0], 7);

  mailbox.data_array[0] = 0x70222843;
  mailbox.data_array[1] = 0x2a112c43;
  mailbox.data_array[2] = 0x30113443;
  mailbox.data_array[3] = 0x34113a43;
  mailbox.data_array[4] = 0x38113e43;
  mailbox.data_array[5] = 0x3c114243;
  mailbox.data_array[6] = 0x3e114643;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_4, &mailbox.data_array[0], 7);

  mailbox.data_array[0] = 0x40114c43;
  mailbox.data_array[1] = 0x44115443;
  mailbox.data_array[2] = 0x48115a43;
  mailbox.data_array[3] = 0x4a115e43;
  mailbox.data_array[4] = 0x50116043;
  mailbox.data_array[5] = 0x54116443;
  mailbox.data_array[6] = 0x5a112422;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_5, &mailbox.data_array[0], 7);

  mailbox.data_array[0] = 0x5e112822;
  mailbox.data_array[1] = 0x60113222;
  mailbox.data_array[2] = 0x62113622;
  mailbox.data_array[3] = 0x66113a22;
  mailbox.data_array[4] = 0x6a113e22;
  mailbox.data_array[5] = 0x6e114222;
  mailbox.data_array[6] = 0x72114622;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_6, &mailbox.data_array[0], 7);

  mailbox.data_array[0] = 0x76114822;
  mailbox.data_array[1] = 0x7a115022;
  mailbox.data_array[2] = 0x7e115422;
  mailbox.data_array[3] = 0x7f115622;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_7, &mailbox.data_array[0], 4);

  mailbox.data_array[0] = 0x2a112c43;
  mailbox.data_array[1] = 0x30113443;
  mailbox.data_array[2] = 0x34113a43;
  mailbox.data_array[3] = 0x38113e43;
  mailbox.data_array[4] = 0x3c114243;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_8, &mailbox.data_array[0], 5);

  mailbox.data_array[0] = 0x74222c43;
  mailbox.data_array[1] = 0x78223443;
  mailbox.data_array[2] = 0x7a223a43;
  mailbox.data_array[3] = 0x7e223e43;
  mailbox.data_array[4] = 0x7f224243;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_9, &mailbox.data_array[0], 5);

  mailbox.data_array[0] = 0x5a112422;
  mailbox.data_array[1] = 0x5e112822;
  mailbox.data_array[2] = 0x60113222;
  mailbox.data_array[3] = 0x62113622;
  mailbox.data_array[4] = 0x66113a22;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_A, &mailbox.data_array[0], 5);

  mailbox.data_array[0] = 0x5a116843;
  mailbox.data_array[1] = 0x5e116c43;
  mailbox.data_array[2] = 0x60117243;
  mailbox.data_array[3] = 0x62117643;
  mailbox.data_array[4] = 0x66117a43;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_B, &mailbox.data_array[0], 5);

  mailbox.data_array[0] = 0x0;
  mailbox.data_array[1] = 0x0;
  mailbox.data_array[2] = 0x0;
  mailbox.data_array[3] = 0x0;
  mailbox.data_array[4] = 0x0;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_C, &mailbox.data_array[0], 5);

  mailbox.data_array[0] = 0x0;
  mailbox.data_array[1] = 0x0;
  mailbox.data_array[2] = 0x0;
  mailbox.data_array[3] = 0x0;
  mailbox.data_array[4] = 0x0;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_D, &mailbox.data_array[0], 5);

  mailbox.data_array[0] = 0x0;
  mailbox.data_array[1] = 0x0;
  mailbox.data_array[2] = 0x0;
  mailbox.data_array[3] = 0x0;
  mailbox.data_array[4] = 0x0;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_E, &mailbox.data_array[0], 5);

  mailbox.data_array[0] = 0x0;
  mailbox.data_array[1] = 0x0;
  mailbox.data_array[2] = 0x0;
  mailbox.data_array[3] = 0x0;
  mailbox.data_array[4] = 0x0;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_F, &mailbox.data_array[0], 5);
#endif
}

void TC317X_Set_AGC_Table_Lband (t_TC3X_DEMODULE * pTC317X)
{
  tMAILBOX  mailbox;

#ifdef TC317X_AGC_IN_BOOT
  mailbox.data_array[0] = 0x0;
  mailbox.data_array[1] = ((195<<16)+(23<<8)+24);
  mailbox.data_array[2] = 0x001C0012;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_JAM, &mailbox.data_array[0], 3);
#else
  mailbox.data_array[0] = 0xC0ACFF44;
  mailbox.data_array[1] = 0x031EFF53;
  mailbox.data_array[2] = 0x011EFF07;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_CFG, &mailbox.data_array[0], 3);

  mailbox.data_array[0] = 0x00091223;
  mailbox.data_array[1] = ((195<<16)+(23<<8)+24);
  mailbox.data_array[2] = 0x001C0012;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_JAM, &mailbox.data_array[0], 3);

  mailbox.data_array[0] = 0x52120223;
  mailbox.data_array[1] = 0x58120823;
  mailbox.data_array[2] = 0x5c121223;
  mailbox.data_array[3] = 0x60121c23;
  mailbox.data_array[4] = 0x62122023;
  mailbox.data_array[5] = 0x68122423;
  mailbox.data_array[6] = 0x6a122a23;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_3, &mailbox.data_array[0], 7);

  mailbox.data_array[0] = 0x70123223;
  mailbox.data_array[1] = 0x30093a23;
  mailbox.data_array[2] = 0x32094023;
  mailbox.data_array[3] = 0x3a094823;
  mailbox.data_array[4] = 0x3c095223;
  mailbox.data_array[5] = 0x40095823;
  mailbox.data_array[6] = 0x42095c23;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_4, &mailbox.data_array[0], 7);

  mailbox.data_array[0] = 0x48096023;
  mailbox.data_array[1] = 0x4a096223;
  mailbox.data_array[2] = 0x52096423;
  mailbox.data_array[3] = 0x4e096823;
  mailbox.data_array[4] = 0x58096a23;
  mailbox.data_array[5] = 0x5a097023;
  mailbox.data_array[6] = 0x5e090812;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_5, &mailbox.data_array[0], 7);

  mailbox.data_array[0] = 0x62091212;
  mailbox.data_array[1] = 0x68090e12;
  mailbox.data_array[2] = 0x70091c12;
  mailbox.data_array[3] = 0x72092012;
  mailbox.data_array[4] = 0x78092412;
  mailbox.data_array[5] = 0x7a093012;
  mailbox.data_array[6] = 0x7c093812;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_6, &mailbox.data_array[0], 7);

  mailbox.data_array[0] = 0x7e093a12;
  mailbox.data_array[1] = 0x6f094012;
  mailbox.data_array[2] = 0x73094212;
  mailbox.data_array[3] = 0x7f094a12;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_7, &mailbox.data_array[0], 4);

  mailbox.data_array[0] = 0x30093a23;
  mailbox.data_array[1] = 0x32094023;
  mailbox.data_array[2] = 0x3a094823;
  mailbox.data_array[3] = 0x3c095223;
  mailbox.data_array[4] = 0x40095823;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_8, &mailbox.data_array[0], 5);

  mailbox.data_array[0] = 0x72123a23;
  mailbox.data_array[1] = 0x78124023;
  mailbox.data_array[2] = 0x7a124823;
  mailbox.data_array[3] = 0x7e125223;
  mailbox.data_array[4] = 0x73125823;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_9, &mailbox.data_array[0], 5);

  mailbox.data_array[0] = 0x5e090812;
  mailbox.data_array[1] = 0x62091212;
  mailbox.data_array[2] = 0x68090e12;
  mailbox.data_array[3] = 0x70091c12;
  mailbox.data_array[4] = 0x72092012;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_A, &mailbox.data_array[0], 5);

  mailbox.data_array[0] = 0x5e097223;
  mailbox.data_array[1] = 0x62097823;
  mailbox.data_array[2] = 0x68097a23;
  mailbox.data_array[3] = 0x70097e23;
  mailbox.data_array[4] = 0x72097323;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_B, &mailbox.data_array[0], 5);

  mailbox.data_array[0] = 0x0;
  mailbox.data_array[1] = 0x0;
  mailbox.data_array[2] = 0x0;
  mailbox.data_array[3] = 0x0;
  mailbox.data_array[4] = 0x0;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_C, &mailbox.data_array[0], 5);

  mailbox.data_array[0] = 0x0;
  mailbox.data_array[1] = 0x0;
  mailbox.data_array[2] = 0x0;
  mailbox.data_array[3] = 0x0;
  mailbox.data_array[4] = 0x0;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_D, &mailbox.data_array[0], 5);

  mailbox.data_array[0] = 0x0;
  mailbox.data_array[1] = 0x0;
  mailbox.data_array[2] = 0x0;
  mailbox.data_array[3] = 0x0;
  mailbox.data_array[4] = 0x0;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_E, &mailbox.data_array[0], 5);

  mailbox.data_array[0] = 0x0;
  mailbox.data_array[1] = 0x0;
  mailbox.data_array[2] = 0x0;
  mailbox.data_array[3] = 0x0;
  mailbox.data_array[4] = 0x0;
  TC317X_MB_TxOnly (pTC317X, MBCMD_AGC_DAB_F, &mailbox.data_array[0], 5);
#endif
}

void TC317X_Set_FP_config (t_TC3X_DEMODULE * pTC317X)
{
  tMAILBOX  mailbox;

  mailbox.data_array[0] = 0x0009002a;
  mailbox.data_array[1] = 0x00000025;
  mailbox.data_array[2] = 0x8bf258bf;
  mailbox.data_array[3] = 0x00000000;
  TC317X_MB_TxOnly (pTC317X, MBCMD_FP_DAB_CFG, &mailbox.data_array[0], 4);
}

void TC317X_Set_FP_IIR_Coeff (t_TC3X_DEMODULE * pTC317X, int32_t freq_khz)
{
  tMAILBOX  mailbox;

  if(freq_khz == 183008)
  {
    mailbox.data_array[0] = ((0xCA<<16)|(0x36B));    //183008
    mailbox.data_array[1] = ((0xCE<<16)|(0x368));
  }
  else if(freq_khz == 201008)
  {

    mailbox.data_array[0] = ((0x33A<<16)|(0x366));   //201008
    mailbox.data_array[1] = ((0x336<<16)|(0x363));
  }
  else if(freq_khz == 211280)
  {
    mailbox.data_array[0] = ((0x1E<<16)|(0x307));    //211280
    mailbox.data_array[1] = ((0x1F<<16)|(0x302));
  }
  else if(freq_khz == 181936)
  {
    mailbox.data_array[0] = ((0x35C<<16)|(0x342));    //181936
    mailbox.data_array[1] = ((0x359<<16)|(0x33e));
  }
  else if(freq_khz == 192352)
  {
    mailbox.data_array[0] = ((0x81<<16)|(0x328));     //192.352
    mailbox.data_array[1] = ((0x84<<16)|(0x324));
  }

  else if(freq_khz == 201072)
  {
    mailbox.data_array[0] = ((0x34B<<16)|(0x352));    //201.072
    mailbox.data_array[1] = ((0x347<<16)|(0x34F));
  }

  else if(freq_khz == 211648)
  {
    mailbox.data_array[0] =((0x9F<<16)|(0x33E));      //211.648
    mailbox.data_array[1] =((0xA2<<16)|(0x33A));
  }
  else if(freq_khz == 220352)
  {
    mailbox.data_array[0] =((0x361<<16)|(0x33E));     //220.352
    mailbox.data_array[1] =((0x35E<<16)|(0x33A));
  }
  else if(freq_khz == 230784)
  {
    mailbox.data_array[0] =((0x8B<<16)|(0x32F));      //230.784
    mailbox.data_array[1] =((0x8E<<16)|(0x32B));
  }
  else if(freq_khz == 1459808)
  {
    mailbox.data_array[0] = ((0xCA<<16)|(0x36B));     //1459808
    mailbox.data_array[1] = ((0xCE<<16)|(0x368));
  }
  else if(freq_khz == 1468368)
  {
    mailbox.data_array[0] = ((0x366<<16)|(0x33A));    //1468368
    mailbox.data_array[1] = ((0x363<<16)|(0x336));
  }
  else if(freq_khz == 1478640)
  {
    mailbox.data_array[0] = ((0x5A<<16)|(0x316));     //1478640
    mailbox.data_array[1] = ((0x5C<<16)|(0x311));
  }
  else
  {
    mailbox.data_array[0] = 0;
    mailbox.data_array[1] = 0;
  }

  TC317X_MB_TxOnly (pTC317X, MBCMD_FP_DAB_IIR, &mailbox.data_array[0], 2);
}

void TC317X_CoreColdBoot_SYSSet (t_TC3X_DEMODULE * pTC317X)
{
  uint32_t id = 0;
  //uint32_t temp = 0;

  id = TC317XREGGET_CHIPID (pTC317X);

  //ALL Parts Disable
  TC317XREGSET_SYSEN (pTC317X, 0, 0);

  // ALL Parts Reset
  TC317XREGSET_SYSRESET (pTC317X, TC3XREG_SYS_RESET_DSP | TC3XREG_SYS_RESET_EP);

  // EP Enable
  TC317XREGSET_SYSEN (pTC317X, TC3XREG_SYS_EN_EP, 0);
}

#if defined (USE_ANDROID)
int64_t coldst;
int64_t coldet;
int64_t coldTime;

int64_t coldbootst;
int64_t coldbootet;
int64_t coldbootTime;
#else
uint32_t coldst;
uint32_t coldet;
uint32_t coldTime;

uint32_t coldbootst;
uint32_t coldbootet;
uint32_t coldbootTime;
#endif

int32_t TC317X_CoreColdBoot (int32_t moduleidx, uint8_t *pCOLDBOOT, uint32_t COLDBOOTSize,
    uint8_t *pDAGU, uint32_t DAGUSize,
    uint8_t *pDINT, uint32_t DINTSize,
    uint8_t *pRAND, uint32_t RANDSize, uint8_t *pCOL_ORDER, uint32_t COL_ORDERSize)
{
  int32_t       i;
  int32_t       get_crc;
  uint32_t cntBootWrite;
  uint32_t bAddressed;
  tMAILBOX  MailBox;
  uint32_t access_mail = 0;
  uint32_t coldsize = 0;
  //uint32_t remap_pc;

#if defined (USE_ANDROID)
  coldbootst = TC317X_IO_GET_TIMECNT_ms ();
#else
  coldbootst = TC317X_IO_GET_TIMECNT_us ();
#endif // USE_ANDROID

  bAddressed = 0;
  cntBootWrite = 1;

  // cntBootWrite check & addressed check
  if (g_pTC3X[moduleidx][0].BAddressed)
  {
    TC317X_IO_PW_RESET (moduleidx);
    //PRINTF_LV_2 ("#[%d][0] BB Power Resetted\n", moduleidx);
  }

  //PRINTF_LV_2 ("#[%d][0] Device Addressing...\n", moduleidx);
  switch (g_pTC3X[moduleidx][0].MainIF.MainIFDefine)
  {
    case TC3X_IF_SRAMLIKE:
    case TC3X_IF_I2C:
    case TC3X_IF_CSPI:
      cntBootWrite = 1;       // can write at once. master, slave...
      break;

    case TC3X_IF_SDIO1BIT:
    case TC3X_IF_SDIO4BIT:
      if (g_pTC3X[moduleidx][0].NumOfDemodule > 1)
        cntBootWrite = 2;   // slave can write at once slave2, slave3...
      else
        cntBootWrite = 1;
      break;

    default:
      cntBootWrite = 1;       // can write at once. master, slave...
      break;
  }
  bAddressed = 0;

  // PLL Set & Init
  for (i = 0; i < (int32_t) (cntBootWrite); i++)
  {
    TC317X_PLL_SET (&g_pTC3X[moduleidx][i]);
    if (i == 0)
    {
      switch (g_pTC3X[moduleidx][0].MainIF.MainIFDefine)
      {
        case TC3X_IF_SRAMLIKE:
        case TC3X_IF_I2C:
        case TC3X_IF_CSPI:
          break;

        case TC3X_IF_SDIO1BIT:
        case TC3X_IF_SDIO4BIT:
          TC317X_SlaveIF_I2CMS_Init (&g_pTC3X[moduleidx][i], 400);
          break;
      }
    }
    TC317X_CoreColdBoot_SYSSet (&g_pTC3X[moduleidx][i]);
  }

  // Write OP BootBin
  for (i = cntBootWrite - 1; i >= 0; i--)
  {
#if defined (USE_ANDROID)
    coldst = TC317X_IO_GET_TIMECNT_ms ();
#else
    coldst = TC317X_IO_GET_TIMECNT_us ();
#endif // USE_ANDROID

    TC317XREGSET_REMAP (&g_pTC3X[moduleidx][i], 0x00);

    if (pDAGU)
      TC317X_DRV_BootTable_Write (&g_pTC3X[moduleidx][i], TC317X_CODE_TABLEBASE_DAGU, pDAGU, DAGUSize);

    if (pDINT)
      TC317X_DRV_BootTable_Write (&g_pTC3X[moduleidx][i], TC317X_CODE_TABLEBASE_DINT, pDINT, DINTSize);

    if (pRAND)
      TC317X_DRV_BootTable_Write (&g_pTC3X[moduleidx][i], TC317X_CODE_TABLEBASE_RAND, pRAND, RANDSize);

    if (pCOL_ORDER)
      TC317X_DRV_BootTable_Write (&g_pTC3X[moduleidx][i], TC317X_CODE_TABLEBASE_COL_ORDER, pCOL_ORDER, COL_ORDERSize);

    coldsize = COLDBOOTSize - 4;    // Except CRC Size
    TC317X_DRV_OP_Write (&g_pTC3X[moduleidx][i], pCOLDBOOT, coldsize);

    if(tc317x_err != TC3X_IO_SUCCESS)
      return CB_OP_BIN_NOT_READY;

#if defined (USE_ANDROID)
    coldet = TC317X_IO_GET_TIMECNT_ms ();
    coldTime = coldet - coldst;
    //PRINTF_LV_2 ("# ColdBootTime [%lld]\n", coldTime);
#else
    coldet = TC317X_IO_GET_TIMECNT_us ();
    coldTime = coldet - coldst;
    //PRINTF_LV_2 ("# ColdBootTime [%d]\n", coldTime);
#endif // USE_ANDROID
  }

  // First, Set Slaves
  for (i = g_pTC3X[moduleidx][0].NumOfDemodule - 1; i >= 0; i--)
  {
    //Configure EP
    get_crc = TC317XREGGET_CMDDMA_CRC32 (&g_pTC3X[moduleidx][i], 0);

    if (get_crc == *(int32_t *) (&pCOLDBOOT[coldsize]))
    {
      PRINTF_LV_0 ("#[%d][%d] CRC Success\n", moduleidx, i);
      //PRINTF_LV_0 ("#[%d][%d] OP can access Code memory.\n", moduleidx, i);

      switch (g_pTC3X[moduleidx][0].MainIF.MainIFDefine)
      {
        // path through
        case TC3X_IF_CSPI:
        case TC3X_IF_SRAMLIKE:
          {       // Set ChipAddr For Diversity.
            uint8_t ActiveBit;
            uint8_t Addr;

            // 7 : active, 0~6 : address    - default addr?? Write
            ActiveBit = (0x01 << 7);
            Addr = i;

            TC317XREGSET_CHIPAddr (&g_pTC3X[moduleidx][i], ActiveBit | Addr);
            g_pTC3X[moduleidx][i].DEMOD_AddrBase = Addr;
          }
          break;

        case TC3X_IF_I2C:
          {       // Set ChipAddr For Diversity.
            uint8_t ActiveBit;
            uint8_t Addr;
            uint32_t ChipID;

            // 7 : active, 0~6 : address    - default addr?? Write
            ActiveBit = (0x01 << 7);
            if (moduleidx == 0)
            {   // 0xa0 0xa4 0xa6 0xa8
              if (i == 0)
                Addr = TC3X_I2C_DEV_ADDR;
              else
                Addr = (0xA4 | ((i - 1) << 1));
            }
            else
            {   // 0xa2 0xaa 0xac 0xae
              if (i == 0)
                Addr = TC3X_I2C_DEV_ADDR2;
              else
                Addr = (0xAA | ((i - 1) << 1));
            }

            //PRINTF_LV_2 ("# Set ChipAddr [%dth module][%dth] 0x%02x Real[0x%02x]\n", moduleidx, i, Addr, g_pTC3X[moduleidx][i].DEMOD_AddrBase);
            TC317XREGSET_CHIPAddr (&g_pTC3X[moduleidx][i], (ActiveBit | (Addr >> 1)));
            g_pTC3X[moduleidx][i].DEMOD_AddrBase = (Addr);

            ChipID = TC317X_GetChipID (moduleidx, i);
            PRINTF_LV_0("ChipID 0x%02x(%d)\n", ChipID, i);
          }
          break;

          // individualy set
        case TC3X_IF_SDIO1BIT:
        case TC3X_IF_SDIO4BIT:
          {
            // Other Interfaces Can't Set Chip Address seperatly.
            // ex : SDIO,CSPI SLAVE Demodulator should control I2C Master.
            uint8_t ActiveBit;
            uint8_t Addr;
            if (i != 0)
            {
              // 7 : active, 0~6 : address    - default addr?? Write
              ActiveBit = (0x01 << 7);
              Addr = (TC3X_I2C_DEV_ADDR | (i << 1));

              TC317XREGSET_CHIPAddr (&g_pTC3X[moduleidx][i], (ActiveBit | (Addr >> 1)));
              g_pTC3X[moduleidx][i].DEMOD_AddrBase = (Addr);
            }
          }
          break;

        default:
          PRINTF_LV_0 ("# Unknown Interface\n");
          break;
      }

      g_pTC3X[moduleidx][i].BAddressed = 1;

      // PLL Setting


#if defined(TC317X_ES)
      /* ES
       * WARNING code download address = 0x80008000 */
      TC317XREGSET_REMAP_PC (&g_pTC3X[moduleidx][i], 0x02, 0x80, 0x00);
      //PRINTF_LV_0 ("REMAP & PC(0x%04x)====\n", TCC3170_START_PC_opset);
#elif defined(TC317X_SR)
      /* SR
       * WARNING code download address = 0x80008000 */
      TC317XREGSET_REMAP_PC (&g_pTC3X[moduleidx][i], 0x03, 0x80, 0x00);
#else
      TC317XREGSET_REMAP_PC (&g_pTC3X[moduleidx][i], 0x03, 0x00, 0x00);
      //PRINTF_LV_0 ("REMAP & PC(0x%04x)====\n", TCC3170_START_PC);
#endif

      // OP , EP Enable
      TC317XREGSET_SYSEN (&g_pTC3X[moduleidx][i], TC3XREG_SYS_EN_DSP | TC3XREG_SYS_EN_EP, 0);
      access_mail = TC317X_GetAccessMail (&g_pTC3X[moduleidx][i]);

      if (i != 0)
      {
        TC317XREGSET_GPIO_LR (&g_pTC3X[moduleidx][i], 0x1000);
        TC317XREGSET_GPIO_DR (&g_pTC3X[moduleidx][i], 0x1000);
      }

      if (g_pTC3X[moduleidx][0].NumOfDemodule > 1)
      {
#ifdef STREAM_MIXER
        if (i == 0)
        {
          TC317XREGSET_DIVIO (&g_pTC3X[moduleidx][i], 0x02 << 2);
          TC317XREGSET_STREAMMIX (&g_pTC3X[moduleidx][i], 0xC0);
        }
        else if (g_pTC3X[moduleidx][i].NumOfDemodule == (g_pTC3X[moduleidx][i].ChainNum + 1))
        {
          TC317XREGSET_DIVIO (&g_pTC3X[moduleidx][i], 0x01 << 2);
        }
        else
        {
          TC317XREGSET_DIVIO (&g_pTC3X[moduleidx][i], 0x03 << 2);
          TC317XREGSET_STREAMMIX (&g_pTC3X[moduleidx][i], 0xC0);
        }
#else
        if (i == 0)
        {
          TC317XREGSET_DIVIO (&g_pTC3X[moduleidx][i], 0x02);
        }
        else if (g_pTC3X[moduleidx][i].NumOfDemodule == (g_pTC3X[moduleidx][i].ChainNum + 1))
        {
          TC317XREGSET_DIVIO (&g_pTC3X[moduleidx][i], 0x01);
        }
        else
        {
          TC317XREGSET_DIVIO (&g_pTC3X[moduleidx][i], 0x03);
        }
#endif
      }
      else
      {
        TC317XREGSET_DIVIO (&g_pTC3X[moduleidx][i], 0x00);
      }
    }
    else
    {
      PRINTF_LV_0 ("#[%d][%d] [DXBDEV_Coldboot] crc fail: reg: 0x%08x, file : 0x%08x[%d]\n", moduleidx, i, get_crc, *(int32_t *) (&pCOLDBOOT[coldsize]), coldsize);
      return CB_OP_BIN_CRC_FAIL;
    }
  }

  for (i = g_pTC3X[moduleidx][0].NumOfDemodule - 1; i >= 0; i--)
  {
    TC317X_MB_TxnRead (&g_pTC3X[moduleidx][i], &MailBox, MBPARA_SYS_VERSION, NULL, 0, 0);
    g_pTC3X[moduleidx][i].BootCodeVersion = MailBox.data_array[0];
    PRINTF_LV_0 ("#----------------------\n");
    PRINTF_LV_0 ("# Version(%d) : 0x%08X\n", i, MailBox.data_array[0]);
    PRINTF_LV_0 ("#----------------------\n");

    TC317XREGSET_SYSEN (&g_pTC3X[moduleidx][i], TC3XREG_SYS_EN_EP, 0);
  }

#if defined (USE_ANDROID)
  coldbootet = TC317X_IO_GET_TIMECNT_ms ();
  coldbootTime = coldbootet - coldbootst;
  PRINTF_LV_0 ("# TotalColdbootTime[%lld]\n", coldbootTime);
#else
  coldbootet = TC317X_IO_GET_TIMECNT_us ();
  coldbootTime = coldbootet - coldbootst;
  //PRINTF_LV_2 ("# TotalColdbootTime[%d]\n", coldbootTime);
#endif // USE_ANDROID

  return (int32_t) (access_mail);
}


int32_t TC317X_GetDLR (int32_t moduleidx, int32_t Minkhz, int32_t Maxkhz)
{
  int32_t       mindlr = 0;
  int32_t       maxdlr = 0;
  int32_t       SPIClkSpeed = 0;
  int32_t       minv = -1;
  int32_t       maxv = -1;

  if (g_pTC3X[moduleidx] == NULL)
    return -1;

  mindlr = (g_pTC3X[moduleidx][0].MainClk_Khz / (2 * Minkhz)) - 1;
  maxdlr = (g_pTC3X[moduleidx][0].MainClk_Khz / (2 * Maxkhz)) - 1;

  if (mindlr >= 0 && mindlr < 8)
  {
    minv = mindlr;
  }
  else if (mindlr >= 8)
  {
    minv = 7;
  }

  if (maxdlr >= 0 && maxdlr < 8)
  {
    maxv = maxdlr;
  }
  else if (maxdlr >= 8)
  {
    maxv = 7;
  }

  if (minv == -1 && maxv == -1)
  {
    PRINTF_LV_0 ("# [Error] Can't Find DLR value, dlr set will be zero.\n");
    SPIClkSpeed = g_pTC3X[moduleidx][0].MainClk_Khz / (2 * (1 + 0));
    PRINTF_LV_0 ("# SET SPIClk : %d khz [DLR : %d]\n", SPIClkSpeed, 0);
    return 0;
  }

  if (maxv == -1)
  {
    maxv = minv;
  }
  else if (minv == -1)
  {
    minv = maxv;
  }

#if 0   // MAX DLR
  SPIClkSpeed = g_pTC3X[moduleidx][0].MainClk_Khz / (2 * (1 + maxv));
  //PRINTF_LV_2 ("# SET SPIClk : %d khz [DLR : %d]\n", SPIClkSpeed, maxv);
  return maxv;
#else // MIN DLR
  SPIClkSpeed = g_pTC3X[moduleidx][0].MainClk_Khz / (2 * (1 + minv));
  //PRINTF_LV_2 ("# SET SPIClk : %d khz [DLR : %d]\n", SPIClkSpeed, minv);
  return minv;
#endif
}

int32_t GetSubIFHeaderOn (int32_t moduleidx)
{
  int32_t       retv = 0;

  retv = 1;

  return retv;
}

int32_t GetSubIFSpeed (int32_t moduleidx)
{
  int32_t       retv;
  uint32_t MinKhz;
  uint32_t MaxKhz;

  retv = 0;
  MinKhz = 3000;  // 2.4mbps
  MaxKhz = 10000;

  retv = TC317X_GetDLR (moduleidx, MinKhz, MaxKhz);

  return retv;
}

void TC317X_CoreStreamIOInit (t_TC3X_DEMODULE * pTC317X)
{
  //uint32_t FIFOThr = 0;
  uint32_t BuffCfg = 0;

  switch (pTC317X->SubIF.SubIFDefine)
  {
    case TC3X_STREAM_IO_MAINIO:
      pTC317X->SubIF.SubIFValue = TC3XREG_PERI_SEL_OTHER;
      pTC317X->INT_TRIG = TC3X_INT_LEVEL_FALLING;  //TC3X_INT_LEVEL_RISING;// /* editd by KST 101216 */
      //pTC317X->INT_TRIG = TC3X_INT_EDGE_FALLING;  /* org */
      break;

    case TC3X_STREAM_IO_PTS:
      pTC317X->SubIF.SubIFValue = TC3XREG_PERI_SEL_TS;
      pTC317X->SubIF.HeaderON = GetSubIFHeaderOn (pTC317X->moduleidx);

      pTC317X->SubIF.oTS.MSM_Tssync = TC3XREG_PERI_TS_NORMAL_SLAVE;
      pTC317X->SubIF.oTS.Parallel = TC3XREG_PERI_TS_PTS;
      pTC317X->SubIF.oTS.FastMode = TC3XREG_PERI_TS_FASTON;
      pTC317X->SubIF.oTS.TSClkMask = TC3XREG_PERI_TS_TSCLKMASKON;
      pTC317X->SubIF.oTS.DLR = GetSubIFSpeed (pTC317X->moduleidx);

      pTC317X->SubIF.oTS.TSClk_POL_Low = TC3XREG_PERI_TS_CLKPHASE_POS;
      pTC317X->SubIF.oTS.TSSync_POL_Low = TC3XREG_PERI_TS_SYNC_ACTIVEHIGH;
      pTC317X->SubIF.oTS.TSEN_POL_Low = TC3XREG_PERI_TS_ENPOL_ACTIVEHIGH;
      pTC317X->SubIF.oTS.TSStreamWaitON = TC3XREG_PERI_TS_STREAM_WAIT_ON;
      pTC317X->SubIF.oTS.TSStreamWaitTime = 0x00;
      pTC317X->SubIF.oTS.ByteMSB = 0x00;
      pTC317X->SubIF.oTS.BitMSB = 0x00;
      pTC317X->SubIF.oTS.MSMTSSyncHighTime = 0x00;
      pTC317X->SubIF.oTS.MSMTSSyncLowTime = 0x00;
      pTC317X->INT_TRIG = TC3X_INT_EDGE_RISING;
      break;

    case TC3X_STREAM_IO_STS:
      pTC317X->SubIF.SubIFValue = TC3XREG_PERI_SEL_TS;
      pTC317X->SubIF.HeaderON = GetSubIFHeaderOn (pTC317X->moduleidx);

      pTC317X->SubIF.oTS.MSM_Tssync = TC3XREG_PERI_TS_NORMAL_SLAVE;
      pTC317X->SubIF.oTS.Parallel = TC3XREG_PERI_TS_STS;
      pTC317X->SubIF.oTS.FastMode = TC3XREG_PERI_TS_FASTON;
      pTC317X->SubIF.oTS.TSClkMask = 0x00;
      pTC317X->SubIF.oTS.DLR = GetSubIFSpeed (pTC317X->moduleidx);
#ifdef USE_ANDROID
      pTC317X->SubIF.oTS.TSClk_POL_Low = TC3XREG_PERI_TS_CLKPHASE_POS;
      pTC317X->SubIF.oTS.TSSync_POL_Low = TC3XREG_PERI_TS_SYNC_ACTIVEHIGH;
      pTC317X->SubIF.oTS.TSEN_POL_Low = TC3XREG_PERI_TS_ENPOL_ACTIVEHIGH;
#else
      pTC317X->SubIF.oTS.TSClk_POL_Low = TC3XREG_PERI_TS_CLKPHASE_NEG;
      pTC317X->SubIF.oTS.TSSync_POL_Low = TC3XREG_PERI_TS_SYNC_ACTIVELOW;
      pTC317X->SubIF.oTS.TSEN_POL_Low = TC3XREG_PERI_TS_ENPOL_ACTIVELOW;
#endif
      pTC317X->SubIF.oTS.TSStreamWaitON = TC3XREG_PERI_TS_STREAM_WAIT_ON;
      pTC317X->SubIF.oTS.TSStreamWaitTime = 0x02;
      pTC317X->SubIF.oTS.ByteMSB = 0x00;
      pTC317X->SubIF.oTS.BitMSB = TC3XREG_PERI_TS_BITMSB;
      pTC317X->SubIF.oTS.MSMTSSyncHighTime = 0x00;
      pTC317X->SubIF.oTS.MSMTSSyncLowTime = 0x00;
      pTC317X->INT_TRIG = TC3X_INT_EDGE_RISING;
      break;

    case TC3X_STREAM_IO_SPIMS:
      pTC317X->SubIF.SubIFValue = TC3XREG_PERI_SEL_SPI;
      pTC317X->SubIF.HeaderON = GetSubIFHeaderOn (pTC317X->moduleidx);

      pTC317X->SubIF.oSPI.Protocol = TC3XREG_PERI_SPI_MOTOROLA_SSP;
      pTC317X->SubIF.oSPI.SlaveON = TC3XREG_PERI_SPI_MASTER;
      pTC317X->SubIF.oSPI.FastMode = 0; //TC3XREG_PERI_SPI_FASTON;
      pTC317X->SubIF.oSPI.DLR = GetSubIFSpeed (pTC317X->moduleidx);

      pTC317X->SubIF.oSPI.Size = TC3XREG_PERI_SPI_SIZE32BIT;
      pTC317X->SubIF.oSPI.SPH = TC3XREG_PERI_SPI_CLKINIT_LOW;
      pTC317X->SubIF.oSPI.SPO = TC3XREG_PERI_SPI_CLKPOL_POS;
#ifdef STREAM_MIXER
      if(pTC317X->MasterMode)
      {
        pTC317X->SubIF.oSPI.ByteMSB = 0x00;
        pTC317X->SubIF.oSPI.BitMSB = TC3XREG_PERI_SPI_BITMSB1;
      }
      else
      {
        pTC317X->SubIF.oSPI.ByteMSB = 0x1;
        pTC317X->SubIF.oSPI.BitMSB = 0;
      }
#else
      pTC317X->SubIF.oSPI.ByteMSB = 0x00;
      pTC317X->SubIF.oSPI.BitMSB = TC3XREG_PERI_SPI_BITMSB1;
#endif
      pTC317X->SubIF.oSPI.ReadyTime = 0x00;
      pTC317X->SubIF.oSPI.WaitTime = 0x00;
      pTC317X->INT_TRIG = TC3X_INT_EDGE_RISING;
      break;

    case TC3X_STREAM_IO_SPISLV:
      pTC317X->SubIF.SubIFValue = TC3XREG_PERI_SEL_SPI;
      pTC317X->SubIF.HeaderON = GetSubIFHeaderOn (pTC317X->moduleidx);

      pTC317X->SubIF.oSPI.Protocol = TC3XREG_PERI_SPI_MOTOROLA_SSP;
      pTC317X->SubIF.oSPI.SlaveON = TC3XREG_PERI_SPI_SLAVE;
      pTC317X->SubIF.oSPI.FastMode = 0x00;
      pTC317X->SubIF.oSPI.DLR = GetSubIFSpeed (pTC317X->moduleidx);

      pTC317X->SubIF.oSPI.Size = TC3XREG_PERI_SPI_SIZE32BIT;
      pTC317X->SubIF.oSPI.SPH = TC3XREG_PERI_SPI_CLKINIT_LOW;
      pTC317X->SubIF.oSPI.SPO = TC3XREG_PERI_SPI_CLKPOL_POS;
      pTC317X->SubIF.oSPI.ByteMSB = 0x00;
      pTC317X->SubIF.oSPI.BitMSB = TC3XREG_PERI_SPI_BITMSB1;
      pTC317X->SubIF.oSPI.ReadyTime = 0x00;
      pTC317X->SubIF.oSPI.WaitTime = 0x00;
      pTC317X->INT_TRIG = TC3X_INT_LEVEL_FALLING;
      break;

    case TC3X_STREAM_IO_HPI_HEADERON:
    case TC3X_STREAM_IO_HPI_HEADEROFF:
      pTC317X->SubIF.SubIFValue = TC3XREG_PERI_SEL_HPI;
      pTC317X->SubIF.HeaderON = 1;
      pTC317X->SubIF.oHPI.IntelMode = TC3XREG_PERI_HPI_INTEL;
      pTC317X->SubIF.oHPI.ByteMSB = 0x00;
      pTC317X->SubIF.oHPI.BitMSB = 0x00;
      pTC317X->INT_TRIG = TC3X_INT_LEVEL_FALLING;
      break;
  }

  // Output buffer management
  //FIFOThr = GetFIFOThr_TDMB (pTC317X->moduleidx);

#ifdef STREAM_MIXER
  if (pTC317X->MasterMode)
  {
    TC317XREGSET_OBUFF_D_FIFO_Thr(pTC317X, tc317x_cspi_read_size>>2);
  }
#endif

  TC317XREGSET_OBUFF_A_SAddr (pTC317X, PHY_MEM_ADDR_A_START >> 2);
  TC317XREGSET_OBUFF_A_EAddr (pTC317X, PHY_MEM_ADDR_A_END >> 2);
  TC317XREGSET_OBUFF_Init (pTC317X, TC3XREG_OBUFF_BUFF_A_INIT);

#ifdef STREAM_MIXER
  if (pTC317X->MasterMode)
  {
    BuffCfg = TC3XREG_OBUFF_CONFIG_BUFF_A_EN | TC3XREG_OBUFF_CONFIG_BUFF_A_CIRCULAR |
      TC3XREG_OBUFF_CONFIG_BUFF_B_EN | TC3XREG_OBUFF_CONFIG_BUFF_B_CIRCULAR |
      TC3XREG_OBUFF_CONFIG_BUFF_C_EN | TC3XREG_OBUFF_CONFIG_BUFF_C_CIRCULAR |
      TC3XREG_OBUFF_CONFIG_BUFF_D_EN | TC3XREG_OBUFF_CONFIG_BUFF_D_FIFO;
  }
  else
  {
    BuffCfg = TC3XREG_OBUFF_CONFIG_BUFF_A_EN | TC3XREG_OBUFF_CONFIG_BUFF_A_CIRCULAR |
      TC3XREG_OBUFF_CONFIG_BUFF_B_EN | TC3XREG_OBUFF_CONFIG_BUFF_B_CIRCULAR |
      TC3XREG_OBUFF_CONFIG_BUFF_C_EN | TC3XREG_OBUFF_CONFIG_BUFF_C_CIRCULAR |
      TC3XREG_OBUFF_CONFIG_BUFF_D_EN | TC3XREG_OBUFF_CONFIG_BUFF_D_CIRCULAR;
  }
#else
  BuffCfg = TC3XREG_OBUFF_CONFIG_BUFF_A_EN | TC3XREG_OBUFF_CONFIG_BUFF_A_CIRCULAR ;
#endif

  TC317XREGSET_OBUFF_Config (pTC317X, BuffCfg);

  TC317X_Set_PERIperalForStream (pTC317X, pTC317X->SubIF);
  if(pTC317X->MasterMode)
  {

#ifdef USE_SDIO_DATA_INT
    if (pTC317X->MainIF.MainIFDefine == TC3X_IF_SDIO1BIT || pTC317X->MainIF.MainIFDefine == TC3X_IF_SDIO4BIT)
    {
      TC317X_IO_BB_INT_Ctrl (pTC317X->moduleidx, CMD_BB_INT_INIT, pTC317X->INT_TRIG);
      TC317X_IO_BB_INT_Ctrl (pTC317X->moduleidx, CMD_BB_INT_DISABLE, pTC317X->INT_TRIG);
      TC317X_IO_SDIO_INT_Ctrl (pTC317X->moduleidx, CMD_SDIO_INT_INIT);
    }
    else
    {
      TC317X_IO_BB_INT_Ctrl (pTC317X->moduleidx, CMD_BB_INT_INIT, pTC317X->INT_TRIG);
      TC317X_IO_BB_INT_Ctrl (pTC317X->moduleidx, CMD_BB_INT_DISABLE, pTC317X->INT_TRIG);
    }
#else
    TC317X_IO_BB_INT_Ctrl (pTC317X->moduleidx, CMD_BB_INT_INIT, pTC317X->INT_TRIG);
    TC317X_IO_BB_INT_Ctrl (pTC317X->moduleidx, CMD_BB_INT_DISABLE, pTC317X->INT_TRIG);
#endif

    // Prepare SPI DMA
    switch (pTC317X->SubIF.SubIFDefine)
    {
      case TC3X_STREAM_IO_PTS:
      case TC3X_STREAM_IO_STS:
      case TC3X_STREAM_IO_SPIMS:
      case TC3X_STREAM_IO_SPISLV:
        // Prepare SPI/STS
        TC317X_IO_Stream_SPI_Ctrl (pTC317X->moduleidx, CMD_SPI_ALLOC_BUFF, &g_tStreamSet[pTC317X->moduleidx].pStream,
            g_tStreamSet[pTC317X->moduleidx].PktSize, g_tStreamSet[pTC317X->moduleidx].PktThrNum,
            g_pTC3X[pTC317X->moduleidx][0].SubIF.SubIFDefine);
        TC317X_IO_Stream_SPI_Ctrl (pTC317X->moduleidx, CMD_SPI_INIT, &g_tStreamSet[pTC317X->moduleidx].pStream,
            g_tStreamSet[pTC317X->moduleidx].PktSize, g_tStreamSet[pTC317X->moduleidx].PktThrNum,
            g_pTC3X[pTC317X->moduleidx][0].SubIF.SubIFDefine);
        break;
    }
  }

  TC317X_Stream_DeAlloc (pTC317X->moduleidx);     // Delete prev stream mem
}

void TC317X_CoreSetInterruptMode (int32_t moduleidx, int32_t idx)
{
  int32_t       value;

  /* Set Interrupt Mode. */
  if (g_pTC3X[moduleidx][0].INT_TRIG == TC3X_INT_LEVEL_FALLING)
    value = TC3XREG_IRQ_MODE_LEVEL | TC3XREG_IRQ_MODE_FALLING;
  else if (g_pTC3X[moduleidx][0].INT_TRIG == TC3X_INT_LEVEL_RISING)
    value = TC3XREG_IRQ_MODE_LEVEL | TC3XREG_IRQ_MODE_RISING;
  else if (g_pTC3X[moduleidx][0].INT_TRIG == TC3X_INT_EDGE_FALLING)
    value = TC3XREG_IRQ_MODE_TRIGER | TC3XREG_IRQ_MODE_FALLING;
  else
    value = TC3XREG_IRQ_MODE_TRIGER | TC3XREG_IRQ_MODE_RISING;

  TC317X_INT_Mode (&g_pTC3X[moduleidx][idx], value | TC3XREG_IRQ_MODE_PAD_ENABLE);
  TC317X_Set_INT_Enable (moduleidx);
}

void TC317X_Set_INT_Enable (int32_t moduleidx)
{
  SetINTInit (moduleidx);     // Connect Interrupt Function

  // Set Interrupts.
  if (g_pTC3X[moduleidx][0].SubIF.SubIFDefine == TC3X_STREAM_IO_MAINIO ||
      g_pTC3X[moduleidx][0].SubIF.SubIFDefine == TC3X_STREAM_IO_SPISLV ||
      g_pTC3X[moduleidx][0].SubIF.SubIFDefine == TC3X_STREAM_IO_HPI_HEADERON
      || g_pTC3X[moduleidx][0].SubIF.SubIFDefine == TC3X_STREAM_IO_HPI_HEADEROFF)
  {
    // use Level Interrupt
    TC317X_INT_Enable (&g_pTC3X[moduleidx][0], TC3XREG_IRQ_EN_DATAINT);
  }

}

/* First Set Slave and then Master. */
void TC317X_CoreGPIOnEnableSystem (int32_t moduleidx, TC3X_DNUM hBB)
{
  int32_t access_mail;

  /* IO */
  TC317X_Set_GPIO (&g_pTC3X[moduleidx][hBB], g_pTC3X[moduleidx][0].MainIF, g_pTC3X[moduleidx][0].SubIF);
  /* ALL Parts Disable */
  TC317XREGSET_SYSEN (&g_pTC3X[moduleidx][hBB], 0, 0);
  // ALL Parts Reset
  TC317XREGSET_SYSRESET (&g_pTC3X[moduleidx][hBB], TC3XREG_SYS_RESET_DSP | TC3XREG_SYS_RESET_EP);
  // EP Enable
  TC317XREGSET_SYSEN (&g_pTC3X[moduleidx][hBB], TC3XREG_SYS_EN_EP, 0);

  // ALL System Parts Enable
  TC317XREGSET_SYSEN (&g_pTC3X[moduleidx][hBB], TC3XREG_SYS_EN_EP | TC3XREG_SYS_EN_DSP, 0);
  TC317XREGSET_SYSRESET (&g_pTC3X[moduleidx][hBB], TC3XREG_SYS_RESET_DSP);
  access_mail = TC317X_GetAccessMail (&g_pTC3X[moduleidx][hBB]);

}

void Start_Standard_Process (t_TC3X_DEMODULE * pTC317X)
{
  if (pTC317X->MasterMode)
  {
    Init_TDMB_Process (pTC317X);
  }
}

void Stop_Standard_Process (t_TC3X_DEMODULE * pTC317X)
{
  if (pTC317X->MasterMode)
  {
    Stop_TDMB_Process (pTC317X);
  }
}

// -------------------------------------------------------
// Stream Control
//
#if defined (USE_ANDROID)
int64_t setfreqtime_st;
int64_t setfreqtime_et;
int64_t setfreqtime;
#else
uint32_t setfreqtime_st;
uint32_t setfreqtime_et;
uint32_t setfreqtime;
#endif
extern void ksj_func_init (void);
extern void test_stream_mixer_force(t_TC3X_DEMODULE * pTC317X);
int32_t TC317X_CoreSetFrequency (int32_t moduleidx, TC3X_DNUM hBB, int32_t freq_khz, int32_t bw_khz, tFreqSet_Option * pOption)
{
  int32_t       i;
  int32_t       ret, org_freq;
  uint32_t NumData[7];
  uint32_t FreqData[7];


  ret = 0;
  org_freq = freq_khz;

  for (i = g_pTC3X[moduleidx][0].NumOfDemodule - 1; i >= 0; i--)
  {
    g_pTC3X[moduleidx][i].mailboxLog = MAILBOX_LOG_FREQSET_STOP;
#if defined (USE_ANDROID)
    setfreqtime_st = TC317X_IO_GET_TIMECNT_ms ();
#else
    setfreqtime_st = TC317X_IO_GET_TIMECNT_us ();
#endif // USE_ANDROID

    g_pTC3X[moduleidx][i].NIT.BW_KHz = bw_khz;
    g_pTC3X[moduleidx][i].NIT.FREQ_KHz = freq_khz;

    if (pOption)
    {
      g_pTC3X[moduleidx][i].NIT.Hierarchy = pOption->Hierarchy;
      g_pTC3X[moduleidx][i].USE_PID_Filtering = pOption->EnablePIDFiltering;
    }
    else
    {
      g_pTC3X[moduleidx][i].NIT.Hierarchy = 0;
      g_pTC3X[moduleidx][i].USE_PID_Filtering = 0;
    }

    // StreamStop
    if(i == 0)
      TC317X_Stream_Stop (moduleidx);

    //Stop_Standard_Process (&g_pTC3X[moduleidx][i]);
    //g_pTC3XCTRL[moduleidx][0].BBEventFn (moduleidx, 0, CMD_STREAM_STOPPED, NULL, NULL, NULL, NULL);

#ifdef  STREAM_MIXER
    if(i != 0)
      freq_khz = 205280;
    else
      freq_khz = org_freq;
#endif
    //PRINTF_LV_1 ("\n");
    PRINTF_LV_0 ("# [%d] Freq (%d) | Bandwidth(%d) \n", i, freq_khz, bw_khz);


    if(freq_khz < 1000000)
      curr_band[i] = 0;
    else
      curr_band[i] = 1;
#ifdef USE_HOST_FREQ_SET_RF
    if( curr_band[i] != prev_band[i])
    {
      TCC3170RF_RF_Init(moduleidx, g_pTC3XCTRL[moduleidx][i].hRF, curr_band[i]);
    }
    g_pTC3XCTRL[moduleidx][i].RF_SetFrequency (moduleidx, g_pTC3XCTRL[moduleidx][i].hRF, freq_khz, bw_khz);

    TC317XREGSET_XTAL_BIAS_KEY(&g_pTC3X[moduleidx][i], 0x5e);
    TC317XREGSET_XTAL_BIAS(&g_pTC3X[moduleidx][i], 0x00);

#else
    switch (g_pTC3X[moduleidx][0].MainIF.MainIFDefine)
    {
      case TC3X_IF_I2C:
        //TC3XREGSET_GPIOA_ALT (&g_pTC3X[moduleidx][i], 0x3F);
        TC317X_OP_Enable (&g_pTC3X[moduleidx][i], 0);
        break;

      case TC3X_IF_SRAMLIKE:
      case TC3X_IF_CSPI:
      case TC3X_IF_SDIO1BIT:
      case TC3X_IF_SDIO4BIT:
        TC317X_OP_Enable (&g_pTC3X[moduleidx][i], 0);
        break;

      default:
        TC317X_OP_Enable (&g_pTC3X[moduleidx][i], 0);
        break;
    }

#endif
    if (g_pTC3X[moduleidx][i].MasterMode)
    {
      TC317X_Stream_StartSet (moduleidx);
    }

    TC317X_Set_FP_IIR_Coeff (&g_pTC3X[moduleidx][i], freq_khz);

    TC317X_IO_memset (&FreqData[0], 0, sizeof (uint32_t) * 7);
    TC317X_IO_memset (&NumData[0], 0, sizeof (uint32_t) * 7);

    NumData[0] = 1;
    FreqData[0] = freq_khz;
    g_pTC3X[moduleidx][i].mailboxLog = MAILBOX_LOG_FREQSET_START;
    TC317X_MB_TxOnly (&g_pTC3X[moduleidx][i], MBPARA_SYS_NUM_FREQ, &NumData[0], 1);
    g_pTC3X[moduleidx][i].mailboxLog = MAILBOX_LOG_FREQSET_START_STEP1;
    TC317X_MB_TxOnly (&g_pTC3X[moduleidx][i], MBPARA_SYS_FREQ_0_6, &FreqData[0], 1);
    g_pTC3X[moduleidx][i].mailboxLog = MAILBOX_LOG_FREQSET_START_STEP2;


    if(curr_band[i]!=prev_band[i])
    {
#if defined(TC317X_MULTI_DMB)
      uint32_t cfg[4];//, data;
      cfg[0] = DP_CFG_1_DATA0;
      cfg[1] = DP_CFG_1_DATA1;
      cfg[2] = DP_CFG_1_DATA2;
      cfg[3] = DP_CFG_1_DATA3;
      TC317X_MB_TxOnly (&g_pTC3X[moduleidx][i], MBCMD_DP_DAB_CFG1, &cfg[0], 4);

      cfg[0] = DP_CFG_2_DATA0;
      cfg[1] = DP_CFG_2_DATA1;
      cfg[2] = DP_CFG_2_DATA2;
      cfg[3] = DP_CFG_2_DATA3;
      TC317X_MB_TxOnly (&g_pTC3X[moduleidx][i], MBCMD_DP_DAB_CFG2, &cfg[0], 4);
#endif
      // set agc table
      //PRINTF_LV_2 ("# [%d] AGC Table Write \n", i);
      if(freq_khz < 1000000) 
        TC317X_Set_AGC_Table_Vhf (&g_pTC3X[moduleidx][i]);
      else 
        TC317X_Set_AGC_Table_Lband(&g_pTC3X[moduleidx][i]);
    }
    prev_band[i] = curr_band[i];

    // Send Start Mail
    TC317X_WarmBoot (&g_pTC3X[moduleidx][i], 1);
    SetTDMBStreamOn (&g_pTC3X[moduleidx][i], 1, 1, 1);

    ret = 1;

#if defined (USE_ANDROID)
    setfreqtime_et = TC317X_IO_GET_TIMECNT_ms ();
#else
    setfreqtime_et = TC317X_IO_GET_TIMECNT_us ();
#endif // USE_ANDROID
    setfreqtime = setfreqtime_et - setfreqtime_st;

    //PRINTF_LV_2 ("# FreqSetTime [%d]\n", setfreqtime);
    PRINTF_LV_0 ("# FreqSetTime [%lld]\n", setfreqtime);

    g_pTC3X[moduleidx][i].mailboxLog = MAILBOX_LOG_SERVICE;
  }

  ksj_func_init ();
#ifdef STREAM_MIXER
  test_stream_mixer_force(&g_pTC3X[moduleidx][1]);
#endif

  return ret;
}

// only set Master
void TC317X_Stream_StartSet (int32_t moduleidx)
{
  // Control Que Create
  //TC317XQue_Init (moduleidx);

  // Task Start
  //TC317XTask_Start (moduleidx);

  // Interrupt Enable
#ifdef USE_SDIO_DATA_INT
  if (g_pTC3X[moduleidx][0].MainIF.MainIFDefine == TC3X_IF_SDIO1BIT ||
      g_pTC3X[moduleidx][0].MainIF.MainIFDefine == TC3X_IF_SDIO4BIT) {
    TC317X_IO_SDIO_INT_Ctrl (moduleidx, CMD_SDIO_INT_ENABLE);
    TC317X_IO_SDIO_Func0_Write (moduleidx, 0x04, 0x03, 0);
  } else {
    TC317X_IO_BB_INT_Ctrl (moduleidx, CMD_BB_INT_ENABLE, g_pTC3X[moduleidx][0].INT_TRIG);
  }
#else
  TC317X_IO_BB_INT_Ctrl (moduleidx, CMD_BB_INT_ENABLE, g_pTC3X[moduleidx][0].INT_TRIG);
#endif

#ifdef USE_HOST_FREQ_SET_RF
  // op disable
  TC317X_OP_Disable (&g_pTC3X[moduleidx][0], 0);
  //PERI Enable - modified 2009-03-12
  TC317X_PERI_EN (&g_pTC3X[moduleidx][0]);

#ifdef STREAM_MIXER
  for (i = 1; i < g_pTC3X[moduleidx][0].NumOfDemodule; i++)
  {
    TC317X_PERI_EN (&g_pTC3X[moduleidx][i]);
  }
#endif


  // op enable
  TC317X_OP_Enable (&g_pTC3X[moduleidx][0], 0);
#else
  TC317X_PERI_EN (&g_pTC3X[moduleidx][0]);

#ifdef STREAM_MIXER
  for (i = 1; i < g_pTC3X[moduleidx][0].NumOfDemodule; i++)
  {
    TC317X_PERI_EN (&g_pTC3X[moduleidx][i]);
  }
#endif
#endif

  // Out Buffer Init and Config
  TC317X_OutBuffer_Init_En (&g_pTC3X[moduleidx][0]);
  Start_Standard_Process (&g_pTC3X[moduleidx][0]);    // alloc stream buffer

  switch (g_pTC3X[moduleidx][0].SubIF.SubIFDefine)
  {
    case TC3X_STREAM_IO_PTS:
    case TC3X_STREAM_IO_STS:
    case TC3X_STREAM_IO_SPIMS:
      InitSPICtrlSet (moduleidx);
      TC317X_IO_Stream_SPI_Ctrl (moduleidx, CMD_SPI_START, &g_tStreamSet[moduleidx].pStream, g_tStreamSet[moduleidx].PktSize,
          g_tStreamSet[moduleidx].PktThrNum, g_pTC3X[moduleidx][0].SubIF.SubIFDefine);
      break;
  }

  // TC3X Interrupt Enable
  TC317X_Set_INT_Enable (moduleidx);
}

// only set Master
void TC317X_Stream_Stop (int32_t moduleidx)
{
  //int32_t       i;

  switch (g_pTC3X[moduleidx][0].SubIF.SubIFDefine)
  {
    case TC3X_STREAM_IO_PTS:
    case TC3X_STREAM_IO_STS:
    case TC3X_STREAM_IO_SPIMS:
      // SPI Disable
      TC317X_IO_Stream_SPI_Ctrl (moduleidx, CMD_SPI_STOP,
          &g_tStreamSet[moduleidx].pStream, g_tStreamSet[moduleidx].PktSize,
          g_tStreamSet[moduleidx].PktThrNum, g_pTC3X[moduleidx][0].SubIF.SubIFDefine);
      InitSPICtrlSet (moduleidx);
      break;
  }

#ifdef USE_SDIO_DATA_INT
  if (g_pTC3X[moduleidx][0].MainIF.MainIFDefine == TC3X_IF_SDIO1BIT || g_pTC3X[moduleidx][0].MainIF.MainIFDefine == TC3X_IF_SDIO4BIT)
  {
    TC317X_IO_SDIO_Func0_Write (moduleidx, 0x04, 0x02, 0);
    TC317X_IO_SDIO_INT_Ctrl (moduleidx, CMD_SDIO_INT_DISABLE);
  }
  else
  {
    TC317X_IO_BB_INT_Ctrl (moduleidx, CMD_BB_INT_DISABLE, g_pTC3X[moduleidx][0].INT_TRIG);
  }
#else
  TC317X_IO_BB_INT_Ctrl (moduleidx, CMD_BB_INT_DISABLE, g_pTC3X[moduleidx][0].INT_TRIG);
#endif
  TC317X_INT_Disable (&g_pTC3X[moduleidx][0]);

  // op disable
  TC317X_OP_Disable (&g_pTC3X[moduleidx][0], 0);
  //PERI Disable - modified 2009-03-12
  TC317X_PERI_DN (&g_pTC3X[moduleidx][0]);
#ifdef STREAM_MIXER
  for (i = 1; i < g_pTC3X[moduleidx][0].NumOfDemodule; i++)
  {
    TC317X_PERI_DN (&g_pTC3X[moduleidx][i]);
  }
#endif
  // TS FIFO Clear
  TC317X_OutBuffer_Init_En (&g_pTC3X[moduleidx][0]);
  // Task Stop
  //TC317XTask_Stop (moduleidx);
  // Control Que Delete
  //TC317XQue_Delete (moduleidx);
  Stop_Standard_Process (&g_pTC3X[moduleidx][0]);
}

void TC317XTask_Start (int32_t moduleidx)
{
  if (g_pTC3X[moduleidx][0].SubIF.SubIFDefine == TC3X_STREAM_IO_MAINIO
      || g_pTC3X[moduleidx][0].SubIF.SubIFDefine == TC3X_STREAM_IO_SPISLV
      || g_pTC3X[moduleidx][0].SubIF.SubIFDefine == TC3X_STREAM_IO_HPI_HEADERON
      || g_pTC3X[moduleidx][0].SubIF.SubIFDefine == TC3X_STREAM_IO_HPI_HEADEROFF)
  {
    TC317XTask_Interrupt_Create (moduleidx);
  }
}

void TC317XTask_Stop (int32_t moduleidx)
{
  if (g_pTC3X[moduleidx][0].SubIF.SubIFDefine == TC3X_STREAM_IO_MAINIO
      || g_pTC3X[moduleidx][0].SubIF.SubIFDefine == TC3X_STREAM_IO_SPISLV
      || g_pTC3X[moduleidx][0].SubIF.SubIFDefine == TC3X_STREAM_IO_HPI_HEADERON
      || g_pTC3X[moduleidx][0].SubIF.SubIFDefine == TC3X_STREAM_IO_HPI_HEADEROFF)
  {
    TC317XTask_Interrupt_Delete (moduleidx);
  }
}

void TC317XQue_Init (int32_t moduleidx)
{
  //m_TC3X_QUE_INTERRUPT[moduleidx] = TC317X_IO_QUE_INT_Create (moduleidx);
  TC317X_IO_QUE_INT_Create (moduleidx);
}

void TC317XQue_Delete (int32_t moduleidx)
{
  TC317X_IO_QUE_INT_Delete (moduleidx);
}

void TC317X_OP_Enable (t_TC3X_DEMODULE * pTC317X, uint32_t AdditionalOpt)
{
  uint32_t access_mail;

  // OP Enable
  TC317XREGSET_SYSEN (pTC317X, TC3XREG_SYS_EN_DSP | TC3XREG_SYS_EN_EP | AdditionalOpt, 0);
  TC317XREGSET_SYSRESET (pTC317X, TC3XREG_SYS_RESET_DSP);
  access_mail = TC317X_GetAccessMail (pTC317X);
}

void TC317X_OP_Disable (t_TC3X_DEMODULE * pTC3X, uint32_t AdditionalOpt)
{
  // op disable
  TC317XREGSET_SYSEN (pTC3X, TC3XREG_SYS_EN_EP | AdditionalOpt, 0);
  TC317XREGSET_SYSRESET (pTC3X, TC3XREG_SYS_RESET_DSP);
}

void TC317X_OutBuffer_Init_En (t_TC3X_DEMODULE * pTC317X)
{
  uint32_t BuffCfg = 0;
  TC317XREGSET_OBUFF_Init (pTC317X, TC3XREG_OBUFF_BUFF_A_INIT);
#ifdef STREAM_MIXER
  BuffCfg = TC3XREG_OBUFF_CONFIG_BUFF_A_EN |TC3XREG_OBUFF_CONFIG_BUFF_A_CIRCULAR |
    TC3XREG_OBUFF_CONFIG_BUFF_B_EN |TC3XREG_OBUFF_CONFIG_BUFF_B_CIRCULAR |
    TC3XREG_OBUFF_CONFIG_BUFF_C_EN |TC3XREG_OBUFF_CONFIG_BUFF_C_CIRCULAR |
    TC3XREG_OBUFF_CONFIG_BUFF_D_EN | TC3XREG_OBUFF_CONFIG_BUFF_D_FIFO;
#else
  BuffCfg = TC3XREG_OBUFF_CONFIG_BUFF_A_EN | TC3XREG_OBUFF_CONFIG_BUFF_A_CIRCULAR;
#endif
  TC317XREGSET_OBUFF_Config (pTC317X, BuffCfg);
}

TC3X_IO_Err TC317X_DRV_Mem_Write (t_TC3X_DEMODULE * pTC317X, int32_t iAddr, uint8_t *data, int32_t iSize, int32_t DMAOPT, int32_t CIRMODE)
{
  if (iSize <= 0)
    return TC3X_IO_FAIL;

  TC317X_IO_IF_LOCK (pTC317X->moduleidx);

#if defined (USE_SDIO_DATA_INT)
  if (pTC317X->MainIF.MainIFDefine == TC3X_IF_SDIO1BIT || pTC317X->MainIF.MainIFDefine == TC3X_IF_SDIO4BIT)
    TC317X_IO_SDIO_Func0_Write (pTC317X->moduleidx, 0x04, 0x02, 1);
#endif // USE_SDIO_DATA_INT

  if (DMAOPT & TC3XREG_CMDDMA_CIRCULARMODE)
  {
    TC317XREGSET_TGTBUFF_CIR_MODE (pTC317X, CIRMODE, 1);
  }

  TC317XREGSET_CMDDMA_CTRL (pTC317X, TC3XREG_CMDDMA_DMAEN | TC3XREG_CMDDMA_WRITEMODE | DMAOPT, 1);
  TC317XREGSET_CMDDMA_SourceAddr (pTC317X, iAddr, 1);
  TC317XREGSET_CMDDMA_Size (pTC317X, iSize >> 2, 1);
  TC317XREGSET_CMDDMA_InitNStart (pTC317X, TC3XREG_CMDDMA_START_AUTOCLR | TC3XREG_CMDDMA_INIT_AUTOCLR | TC3XREG_CMDDMA_CRC32INIT_AUTOCLR, 1);
  pTC317X->TC3X_Reg_WriteEx_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_DATA_WIND, data, iSize);

#if defined (USE_SDIO_DATA_INT)
  if (pTC317X->MainIF.MainIFDefine == TC3X_IF_SDIO1BIT || pTC317X->MainIF.MainIFDefine == TC3X_IF_SDIO4BIT)
    TC317X_IO_SDIO_Func0_Write (pTC317X->moduleidx, 0x04, 0x03, 1);
#endif // USE_SDIO_DATA_INT

  TC317X_IO_IF_UnLOCK (pTC317X->moduleidx);

  return TC3X_IO_SUCCESS;
}

//data filed?? 32bit align?? ?¾ƾ? ?Ѵ?.
TC3X_IO_Err TC317X_DRV_Mem_Read (t_TC3X_DEMODULE * pTC317X, int32_t iAddr, uint8_t *data, int32_t iSize, int32_t DMAOPT, int32_t CIRMODE)
{
  if (iSize <= 0)
    return TC3X_IO_FAIL;

  TC317X_IO_IF_LOCK (pTC317X->moduleidx);

#if defined (USE_SDIO_DATA_INT)
  if (pTC317X->MainIF.MainIFDefine == TC3X_IF_SDIO1BIT || pTC317X->MainIF.MainIFDefine == TC3X_IF_SDIO4BIT)
    TC317X_IO_SDIO_Func0_Write (pTC317X->moduleidx, 0x04, 0x02, 1);
#endif // USE_SDIO_DATA_INT

  if (DMAOPT & TC3XREG_CMDDMA_CIRCULARMODE)
  {
    TC317XREGSET_TGTBUFF_CIR_MODE (pTC317X, CIRMODE, 1);
  }

  TC317XREGSET_CMDDMA_CTRL (pTC317X, TC3XREG_CMDDMA_DMAEN | TC3XREG_CMDDMA_READMODE | DMAOPT, 1);
  TC317XREGSET_CMDDMA_SourceAddr (pTC317X, iAddr, 1);
  TC317XREGSET_CMDDMA_Size (pTC317X, iSize >> 2, 1);
  TC317XREGSET_CMDDMA_InitNStart (pTC317X, TC3XREG_CMDDMA_START_AUTOCLR | TC3XREG_CMDDMA_INIT_AUTOCLR | TC3XREG_CMDDMA_CRC32INIT_AUTOCLR, 1);
  pTC317X->TC3X_Reg_ReadEx_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_DATA_WIND, data, iSize);

#if defined (USE_SDIO_DATA_INT)
  if (pTC317X->MainIF.MainIFDefine == TC3X_IF_SDIO1BIT || pTC317X->MainIF.MainIFDefine == TC3X_IF_SDIO4BIT)
    TC317X_IO_SDIO_Func0_Write (pTC317X->moduleidx, 0x04, 0x03, 1);
#endif // USE_SDIO_DATA_INT

  TC317X_IO_IF_UnLOCK (pTC317X->moduleidx);

  return TC3X_IO_SUCCESS;
}

TC3X_IO_Err TC317X_DRV_Stream_Read (
    t_TC3X_DEMODULE * pTC317X, int32_t iAddr, uint8_t *data, int32_t iSize)
{
  if (iSize <= 0) return TC3X_IO_FAIL;

  TC317X_IO_IF_LOCK (pTC317X->moduleidx);
  pTC317X->TC3X_Reg_ReadEx_InnerSem (pTC317X->moduleidx,
      pTC317X->DEMOD_AddrBase, TC3XREG_STREAM_CFG4, data, iSize);
  TC317X_IO_IF_UnLOCK (pTC317X->moduleidx);

  return TC3X_IO_SUCCESS;
}

void TC317X_DRV_OP_Write (t_TC3X_DEMODULE * pTC317X, uint8_t *data, int32_t iSize)
{
  //int32_t       i;

  if (iSize <= 0)
    return;

  TC317X_IO_IF_LOCK (pTC317X->moduleidx);

#if defined (USE_SDIO_DATA_INT)
  if (pTC317X->MainIF.MainIFDefine == TC3X_IF_SDIO1BIT || pTC317X->MainIF.MainIFDefine == TC3X_IF_SDIO4BIT)
    TC317X_IO_SDIO_Func0_Write (pTC317X->moduleidx, 0x04, 0x02, 1);
#endif // USE_SDIO_DATA_INT

  TC317XREGSET_CMDDMA_CTRL (pTC317X, TC3XREG_CMDDMA_DMAEN | TC3XREG_CMDDMA_WRITEMODE | TC3XREG_CMDDMA_CRC32EN, 1);
  //  TC317XREGSET_CMDDMA_SourceAddr (pTC317X, TC317X_CODEMEMBASE, 1);
  TC317XREGSET_CMDDMA_SourceAddr (pTC317X, TC317X_CODEMEMBASE_8000, 1);
  TC317XREGSET_CMDDMA_Size (pTC317X, iSize >> 2, 1);
  TC317XREGSET_CMDDMA_InitNStart (pTC317X, TC3XREG_CMDDMA_START_AUTOCLR | TC3XREG_CMDDMA_INIT_AUTOCLR | TC3XREG_CMDDMA_CRC32INIT_AUTOCLR, 1);

  //  TC317X_IO_Sleep(7000);

  pTC317X->TC3X_Reg_WriteEx_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_DATA_WIND, data, iSize);

#if defined (USE_SDIO_DATA_INT)
  if (pTC317X->MainIF.MainIFDefine == TC3X_IF_SDIO1BIT || pTC317X->MainIF.MainIFDefine == TC3X_IF_SDIO4BIT)
    TC317X_IO_SDIO_Func0_Write (pTC317X->moduleidx, 0x04, 0x03, 1);
#endif // USE_SDIO_DATA_INT

  TC317X_IO_IF_UnLOCK (pTC317X->moduleidx);
}

void TC317X_DRV_BootTable_Write (t_TC3X_DEMODULE * pTC317X, uint32_t addr, uint8_t *data, int32_t iSize)
{
  if (iSize <= 0)
    return;

  TC317X_IO_IF_LOCK (pTC317X->moduleidx);

#if defined (USE_SDIO_DATA_INT)
  if (pTC317X->MainIF.MainIFDefine == TC3X_IF_SDIO1BIT || pTC317X->MainIF.MainIFDefine == TC3X_IF_SDIO4BIT)
    TC317X_IO_SDIO_Func0_Write (pTC317X->moduleidx, 0x04, 0x02, 1);
#endif // USE_SDIO_DATA_INT

  TC317XREGSET_CMDDMA_CTRL (pTC317X, TC3XREG_CMDDMA_DMAEN | TC3XREG_CMDDMA_WRITEMODE, 1);
  TC317XREGSET_CMDDMA_SourceAddr (pTC317X, addr, 1);
  TC317XREGSET_CMDDMA_Size (pTC317X, iSize >> 2, 1);
  TC317XREGSET_CMDDMA_InitNStart (pTC317X, TC3XREG_CMDDMA_START_AUTOCLR | TC3XREG_CMDDMA_INIT_AUTOCLR | TC3XREG_CMDDMA_CRC32INIT_AUTOCLR, 1);
  pTC317X->TC3X_Reg_WriteEx_InnerSem (pTC317X->moduleidx, pTC317X->DEMOD_AddrBase, TC3XREG_CMDDMA_DATA_WIND, data, iSize);

#if defined (USE_SDIO_DATA_INT)
  if (pTC317X->MainIF.MainIFDefine == TC3X_IF_SDIO1BIT || pTC317X->MainIF.MainIFDefine == TC3X_IF_SDIO4BIT)
    TC317X_IO_SDIO_Func0_Write (pTC317X->moduleidx, 0x04, 0x03, 1);
#endif // USE_SDIO_DATA_INT

  TC317X_IO_IF_UnLOCK (pTC317X->moduleidx);
}

int32_t TC317X_INT_Mode (t_TC3X_DEMODULE * pTC317X, int32_t value)
{
  TC317XREGSET_IRQMODE (pTC317X, value);
  return 1;
}

int32_t TC317X_INT_Enable (t_TC3X_DEMODULE * pTC317X, uint32_t value)
{
  TC317XREGSET_IRQCLR (pTC317X, TC3XREG_IRQ_STATCLR_ALL);
  //interrupt enable
  TC317XREGSET_IRQEN (pTC317X, value);
  return 1;
}

int32_t TC317X_INT_Disable (t_TC3X_DEMODULE * pTC317X)
{
  TC317XREGSET_IRQEN (pTC317X, 0x00);
  return 1;
}

//--------------------------------------------------------------------------------------------
// User Function
//

void UF_RestartStream (int32_t moduleidx)
{
  int32_t       i;

  // PERI init & Outbuffer init
  TC317XREGSET_OBUFF_Init (&g_pTC3X[moduleidx][0], TC3XREG_OBUFF_BUFF_A_INIT);

  for (i = 1; i < g_pTC3X[moduleidx][0].NumOfDemodule; i++)
  {
    TC317X_PERI_EN (&g_pTC3X[moduleidx][i]);
  }

  // stream on mail
  TC317X_TDMBStreamRestart (moduleidx);
}

void UF_PauseStream (int32_t moduleidx)
{
  int32_t       i;

  // stream pause
  TC317X_TDMBStreamPause (moduleidx);

  // PERI init & Outbuffer init
  for (i = 1; i < g_pTC3X[moduleidx][0].NumOfDemodule; i++)
  {
    TC317X_PERI_DN (&g_pTC3X[moduleidx][i]);
  }
  TC317XREGSET_OBUFF_Init (&g_pTC3X[moduleidx][0], 0x07);
  TC317XREGSET_OBUFF_Init (&g_pTC3X[moduleidx][0], TC3XREG_OBUFF_BUFF_A_INIT);
}

void UF_GetBootCodeVersion (int32_t moduleidx, uint32_t *pVersion)
{
  if (pVersion)
    pVersion[0] = g_pTC3X[moduleidx][0].BootCodeVersion;
}

int32_t UF_WarmBoot (int32_t moduleidx, TC3X_DNUM hBB)
{
  int32_t       i;
  int32_t       ret = 0;

  for (i = g_pTC3X[moduleidx][0].NumOfDemodule - 1; i >= 0; i--)
  {
    ret = (int32_t) (TC317X_WarmBoot (&g_pTC3X[moduleidx][i], 1));
  }

  return ret;
}

uint32_t Uf_RegRead (int32_t moduleidx, TC3X_DNUM hBB, int32_t Addr)
{
  return (TC317XREGGET_Manual (&g_pTC3X[moduleidx][hBB], Addr, 0));
}

void Uf_RegWrite (int32_t moduleidx, TC3X_DNUM hBB, int32_t Addr, int32_t Data)
{
  TC317XREGSET_Manual (&g_pTC3X[moduleidx][hBB], Addr, Data, 0);
}

void Uf_SetFifoSize(int32_t moduleidx, TC3X_DNUM hBB, int32_t size)
{
  TC317XREGSET_STREAM_CFG (&g_pTC3X[moduleidx][hBB],
    (STREAM_BUFFER_ALL<<24) | ((size>>2)<<8) |
    (STREAM_CMD_FIFO_INIT|STREAM_CMD_FIFO_ENABLE));

  TC317X_OutBuffer_Init_En (&g_pTC3X[moduleidx][hBB]);
}

void Uf_MailBoxRead (int32_t moduleidx, TC3X_DNUM hBB, int32_t cmd, void *pmailbox)
{
  tMAILBOX *pMailBox;
  pMailBox = (tMAILBOX *) pmailbox;

  TC317X_MB_TxnRead (&g_pTC3X[moduleidx][hBB], pMailBox, cmd, NULL, 0, 0);
}

void Uf_MailBoxWrite (int32_t moduleidx, TC3X_DNUM hBB, int32_t cmd, uint32_t *DataArray, int32_t wordcnt)
{
  TC317X_MB_TxOnly (&g_pTC3X[moduleidx][hBB], cmd, DataArray, wordcnt);
}

uint32_t Uf_GetChipID (int32_t moduleidx)
{
  return (TC317XREGGET_Manual (&g_pTC3X[moduleidx][0], TC3XREG_CHIPID, 0));
}

uint32_t Uf_GetProgramID (int32_t moduleidx)
{
  return (TC317XREGGET_Manual (&g_pTC3X[moduleidx][0], TC3XREG_PROGRAMID, 0));
}

int32_t UF_CheckOP (int32_t moduleidx)
{
  return (TC317XREGGET_SYSEN (&g_pTC3X[moduleidx][0]) & TC3XREG_SYS_EN_DSP);
}

void Uf_SetMEMWrite (int32_t moduleidx, uint8_t *pData, uint32_t Addr, uint32_t Size)
{
  Addr |= PHY_BASE_ADDR;
  TC317X_DRV_Mem_Write (&g_pTC3X[moduleidx][0], Addr, &pData[0], Size, 0, 0);
}

void Uf_GetMEMRead (int32_t moduleidx, uint8_t *pData, uint32_t Addr, uint32_t Size)
{
  Addr |= PHY_BASE_ADDR;
  TC317X_DRV_Mem_Read (&g_pTC3X[moduleidx][0], Addr, &pData[0], Size, 0, 0);
}

void Uf_SetPLLSet (int32_t moduleidx, uint8_t PLL_WaitTime, uint8_t PLL_P, uint8_t PLL_M, uint8_t PLL_S, uint32_t OSC_Khz)
{
  int32_t       i;

  for (i = g_pTC3X[moduleidx][0].NumOfDemodule - 1; i >= 0; i--)
  {
    TC317X_PreSetPLLSet (&g_pTC3X[moduleidx][i], PLL_WaitTime, PLL_P, PLL_M, PLL_S, OSC_Khz);
  }
}

int32_t Uf_REG_TDMB_Service (int32_t moduleidx, TC3X_DNUM hBB, TDMBService_t * pTDMBService)
{
  return (TC317X_SET_TDMB_Service (moduleidx, 1, pTDMBService));
}

int32_t Uf_UnREG_TDMB_Service (int32_t moduleidx, TC3X_DNUM hBB, TDMBService_t * pTDMBService)
{
  return (TC317X_SET_TDMB_Service (moduleidx, 0, pTDMBService));
}

void Uf_GetTDMB_LOCK (int32_t moduleidx, TC3X_DNUM hBB, t_TDMBLock * pLock)
{
  Get_TDMB_LockStatus (moduleidx, pLock);
}

void Uf_GetTDMB_SNR_SET (int32_t moduleidx, TC3X_DNUM hBB,
    uint32_t *param1, uint32_t *param2, uint32_t *param3)
{
  Get_TDMB_SNR (moduleidx, param1, param2, param3);
}

void Uf_GetTDMB_PCBER_SET (int32_t moduleidx, TC3X_DNUM hBB,
    uint32_t *param1, uint32_t *param2)
{
  Get_TDMB_PCBER (moduleidx, param1, param2);
}

void Uf_GetTDMB_VITERBIBER_SET (int32_t moduleidx, TC3X_DNUM hBB,
    uint32_t *param1, uint32_t *param2)
{
  Get_TDMB_VITERBIBER (moduleidx, param1, param2);
}

void Uf_GetTDMB_TSPER_SET (int32_t moduleidx, TC3X_DNUM hBB,
    uint32_t *param1, uint32_t *param2)
{
  Get_TDMB_TSPER (moduleidx, param1, param2);
}

void Uf_GetTDMB_RSSI_SET (int32_t moduleidx, TC3X_DNUM hBB,
    uint32_t *param1, uint32_t *param2)
{
  Get_TDMB_RSSI (moduleidx, param1, param2);
}

int32_t Uf_PushTDMBStatData (int32_t moduleidx, TC3X_DNUM hBB,
    uint8_t *pData, uint32_t Size)
{
  return (ParseNUpdateTDMBStatus (moduleidx, pData, Size));
}

void Uf_PushTDMB_RawData (int32_t moduleidx, TC3X_DNUM hBB, uint8_t *pData, uint32_t Size)
{
  TDMBStreamControlSub (moduleidx, pData, Size);
}

void Uf_PushTDMB_ResetMonitoringValue (int32_t moduleidx, TC3X_DNUM hBB)
{
  TDMB_Reset_MonitorValue (moduleidx);
}
