//=============================================================================
// Telechips Confidential Proprietary
// Copyright (C) 2005-2009 Telechips Corporation
// All rights are reserved by Telechips Corporation
// File       : TC317X_Core_Interrupt.c
// Description:
//
// Revision History:
// Version    Date             Author             Description of Changes
//-----------------------------------------------------------------------------
// 1.0.0       2009/04/14   stkim              Draft
//=============================================================================

#include "TC317X_Core_Interrupt.h"
#include "TC317X_deviceAPI.h"
#include "TC317X_device_Interrupt_Task.h"
#include "../IO/TC317X_IO.h"
#include "TC317X_Core.h"
#include "TC317X_Core_TDMB.h"

#if defined (USE_SDIO_DATA_INT)
void      TC317X_IO_SDIO_Func0_Write (int32_t moduleidx, uint32_t RegAddr, uint8_t Data, int32_t InnerSem);
#endif // USE_SDIO_DATA_INT

typedef struct
{
  uint32_t widx;
  uint32_t ridx;
} t_SPIStreamCtrl;

int32_t       g_SPI_StreamIdx[MAX_TCC3X];
t_SPIStreamCtrl gSPIStreamCtrl[MAX_TCC3X];

extern tc317x_queue_t *m_TC3X_QUE_INTERRUPT[MAX_TCC3X];

int32_t       mSPIPreReadReqNum[MAX_TCC3X];
int32_t       mSPIPreReadNum[MAX_TCC3X];
int32_t       mSPIPreRemainReqNum[MAX_TCC3X];

void InitSPICtrlSet (int32_t moduleidx)
{
  mSPIPreReadReqNum[moduleidx] = 0;
  mSPIPreReadNum[moduleidx] = 0;
  mSPIPreRemainReqNum[moduleidx] = 0;

  g_SPI_StreamIdx[moduleidx] = 0;
  TC317X_IO_memset (&gSPIStreamCtrl[moduleidx], 0, sizeof (t_SPIStreamCtrl));
  mSPIPreRemainReqNum[moduleidx] = 0;
}

void SetINTInit (int32_t moduleidx)
{
  if (g_pTC3X[moduleidx][0].SubIF.SubIFDefine == TC3X_STREAM_IO_PTS ||
      g_pTC3X[moduleidx][0].SubIF.SubIFDefine == TC3X_STREAM_IO_STS || 
      g_pTC3X[moduleidx][0].SubIF.SubIFDefine == TC3X_STREAM_IO_SPIMS)
  {
    TC317X_IO_Set_SPIHandler (moduleidx, (FN_v_ii) TC317X_LISR_SPI, (FN_v_i) TC317X_HISR_SPI);
  }

  TC317X_IO_Set_BBIntHandler (moduleidx, TC317X_LISR_BBINT, TC317X_HISR_BBINT);
}

void TC317X_LISR_BBINT (int32_t moduleidx)
{
  if (g_pTC3X[moduleidx][0].INT_TRIG == TC3X_INT_LEVEL_FALLING || g_pTC3X[moduleidx][0].INT_TRIG == TC3X_INT_LEVEL_RISING
      || g_pTC3X[moduleidx][0].INT_TRIG == TC3X_INT_EDGE_FALLING)
  {
#ifdef USE_SDIO_DATA_INT
    if (g_pTC3X[moduleidx][0].MainIF.MainIFDefine == TC3X_IF_SDIO1BIT || g_pTC3X[moduleidx][0].MainIF.MainIFDefine == TC3X_IF_SDIO4BIT)
    {
      TC317X_IO_SDIO_Func0_Write (moduleidx, 0x04, 0x02, 0);
      TC317X_IO_SDIO_INT_Ctrl (moduleidx, CMD_SDIO_INT_PAUSE);
    }
    else
    {
      TC317X_IO_BB_INT_Ctrl (moduleidx, CMD_BB_INT_PAUSE, g_pTC3X[moduleidx][0].INT_TRIG);
    }
#else
    TC317X_IO_BB_INT_Ctrl (moduleidx, CMD_BB_INT_PAUSE, g_pTC3X[moduleidx][0].INT_TRIG);
#endif
  }
}

void TC317X_HISR_BBINT (int32_t moduleidx)
{
#if 0
  static TC3X_InterruptCOMMAND_t chcmd_int;
  static uint32_t cmd3X;

  // Level mode?? ???? - SubIF==MainIF, SubIF==SPI_SLV
  // ?? ?????? ?????? interrupt task???? ts?? ?о??? ?ϹǷ? ??= ??ƾ; Ÿ???Ѵ?.
  if (g_pTC3X[moduleidx][0].INT_TRIG == TC3X_INT_LEVEL_FALLING || g_pTC3X[moduleidx][0].INT_TRIG == TC3X_INT_LEVEL_RISING
      || g_pTC3X[moduleidx][0].INT_TRIG == TC3X_INT_EDGE_FALLING)
  {
    chcmd_int.usCommand = CMDTC3X_INTERRUPT;
    cmd3X = (uint32_t) &chcmd_int;

    switch (moduleidx)
    {
      case 0:
        TC317X_IO_QUE_Send (m_TC3X_QUE_INTERRUPT0, &cmd3X);
        break;

      case 1:
        TC317X_IO_QUE_Send (m_TC3X_QUE_INTERRUPT1, &cmd3X);
        break;

      default:
        break;
    }
  }
#endif
}

int32_t TC317X_SPI_GetPreReqReadNum (int32_t moduleidx)
{
  return mSPIPreReadReqNum[moduleidx];
}

void TC317X_SPI_SetRemainReqReadNum (int32_t moduleidx, int32_t num)
{
  mSPIPreRemainReqNum[moduleidx] = num;
}

int32_t TC317X_SPI_GetRemainReqReadNum (int32_t moduleidx)
{
  return (mSPIPreRemainReqNum[moduleidx]);
}

void TC317X_SPI_SetPreReqReadNum (int32_t moduleidx)
{
  mSPIPreReadReqNum[moduleidx] = 0;
}

void TC317X_SPI_SetPreReadNum (int32_t moduleidx, int32_t num)
{
  mSPIPreReadNum[moduleidx] = num;
  mSPIPreReadReqNum[moduleidx] = num;
}

void TC317X_LISR_SPI (int32_t moduleidx, int32_t RcvIndex)
{
  int32_t       nRcvIndex = 0;

  nRcvIndex = RcvIndex;

  if (g_SPI_StreamIdx[moduleidx] == 0)
  {
    if (nRcvIndex >= (g_tStreamSet[moduleidx].PktThrNum >> 1))
    {
      g_SPI_StreamIdx[moduleidx] = 1;
      TC317X_IO_ACTIVE_HISR_SPI (moduleidx);
    }
  }
  else
  {
    if (nRcvIndex < (g_tStreamSet[moduleidx].PktThrNum >> 1))
    {
      g_SPI_StreamIdx[moduleidx] = 0;
      TC317X_IO_ACTIVE_HISR_SPI (moduleidx);
    }
  }
}

void TC317X_HISR_SPI (int32_t moduleidx)
{
  static TC3X_InterruptCOMMAND_t chcmd_int_spi;
  static uint32_t cmd3X_spi;
  chcmd_int_spi.usCommand = CMDTC3X_SPI_STS_DATA_COME;
  cmd3X_spi = (uint32_t) &chcmd_int_spi;

  TC317X_IO_QUE_Send (m_TC3X_QUE_INTERRUPT[moduleidx], &cmd3X_spi);

  TDMB_SPI_Handler (moduleidx, g_SPI_StreamIdx[moduleidx],
      g_tStreamSet[moduleidx].pStream, g_tStreamSet[moduleidx].PktSize,
      g_tStreamSet[moduleidx].PktThrNum);
}
