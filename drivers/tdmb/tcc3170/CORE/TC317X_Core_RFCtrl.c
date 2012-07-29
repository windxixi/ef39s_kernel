//=============================================================================
// Telechips Confidential Proprietary
// Copyright (C) 2005-2009 Telechips Corporation
// All rights are reserved by Telechips Corporation
// File       : TC317X_Core_RFCtrl.c
// Description:
//
// Revision History:
// Version    Date             Author             Description of Changes
//-----------------------------------------------------------------------------
// 1.0.0       2009/04/16   stkim              Draft
//=============================================================================
#include "../IO/TC317X_IO.h"
#ifdef USE_HOST_FREQ_SET_RF
//#include "TCC3170.h"
#endif
#include "TC317X_Core.h"
#include "TC317X_Core_RFCtrl.h"
#include "TC317X_Core_I2CMS.h"
#include "TC317X_Core_RegCtrl.h"
#include "TCC3170RF.h"

void TC317X_SetRFINF (int32_t moduleidx, TC3X_DNUM hBB, int32_t Reserved0)
{
  // Set RF Interface & Find RF Series.
  if (g_pTC3X[moduleidx][hBB].SelRF == TC3X_RF_NONE)
  {
    TC317X_InitI2CMS_Polling (&g_pTC3X[moduleidx][hBB], 100);
    g_pTC3X[moduleidx][hBB].SelRF = TC317X_Find_RF (&g_pTC3X[moduleidx][hBB]);
    TC317X_InitI2CMS_Polling (&g_pTC3X[moduleidx][hBB], 400);
  }
}

uint32_t TC317X_Find_RF (t_TC3X_DEMODULE * pTC317X)
{
  pTC317X->RF_SPECIFIC = 0x3170;

  return TC3X_RF_TCC3170;
}

void TC317X_RFAttach (int32_t moduleidx)
{
#ifdef USE_HOST_FREQ_SET_RF
  TCC3170RF_AttachDevice (moduleidx, g_pTC3X[moduleidx][0].NumOfDemodule);
#else
  CommonRFAttachDevice (moduleidx, g_pTC3X[moduleidx][0].NumOfDemodule);
#endif
}

TC3X_DNUM TC317X_RF_Entry (int32_t moduleidx, t_TC3X_CTRL * pTC317XCtrl)
{
  TC3X_DNUM ret;

#ifdef USE_HOST_FREQ_SET_RF
  ret = TCC3170RF_Entry (moduleidx, pTC317XCtrl->hTC3X, (FN_i_ihi *) & pTC317XCtrl->RF_Init,
      (FN_v_ih *) & pTC317XCtrl->RF_Close, (FN_v_ihii *) & pTC317XCtrl->RF_SetFrequency,
      (FN_v_ihi *) & pTC317XCtrl->RF_CtrlPower, (FN_i_ihivvvv *) & pTC317XCtrl->RF_UserFunction);
#else
  uint8_t rfi2caddr;
  uint8_t numofRF;

  numofRF = (uint8_t) g_pTC3X[moduleidx][0].NumOfDemodule;
  ret = CommonRFEntry (moduleidx, pTC317XCtrl->hTC3X, (FN_i_ihi *) & pTC317XCtrl->RF_Init,
      (FN_v_ih *) & pTC317XCtrl->RF_Close, (FN_v_ihiii *) & pTC317XCtrl->RF_SetFrequency,
      (FN_v_ihi *) & pTC317XCtrl->RF_CtrlPower, (FN_i_ihivvvv *) & pTC317XCtrl->RF_UserFunction);

  rfi2caddr = 0x00;
  g_pTC3XCTRL[moduleidx][0].RF_UserFunction (moduleidx, g_pTC3XCTRL[moduleidx][0].hRF, CMD_RF_SET_RF_ADDR, (void *) &rfi2caddr, (void *) &numofRF, NULL, NULL);
#endif

  return ret;
}

// -------------------------------------------------------
// RF Control
//
uint32_t RF_I2C_Read (int32_t moduleidx, TC3X_DNUM hBB, uint8_t devaddr, uint8_t addr)
{
  uint32_t ret;

  TC317X_IO_IF_LOCK (g_pTC3X[moduleidx][hBB].moduleidx);

  TC317XREGSET_RFCfg(&g_pTC3X[moduleidx][hBB], TC3XREG_RF_MANAGE_ENABLE|TC3XREG_RF_READ, 1);

  TC317XREGSET_RFAddr(&g_pTC3X[moduleidx][hBB], addr, 1);
  TC317XREGSET_RFAction(&g_pTC3X[moduleidx][hBB], TC3XREG_RF_ACTION, 1);
  ret = TC317XREGGET_RFData(&g_pTC3X[moduleidx][hBB], 1);

  TC317XREGSET_RFCfg(&g_pTC3X[moduleidx][hBB], TC3XREG_RF_MANAGE_DISABLE, 1);

  TC317X_IO_IF_UnLOCK (g_pTC3X[moduleidx][hBB].moduleidx);
  return ret;
}

void RF_I2C_Write (int32_t moduleidx, TC3X_DNUM hBB, uint8_t devaddr, uint8_t addr, uint32_t data)
{
  TC317X_IO_IF_LOCK (g_pTC3X[moduleidx][hBB].moduleidx);

  TC317XREGSET_RFCfg(&g_pTC3X[moduleidx][hBB], TC3XREG_RF_MANAGE_ENABLE|TC3XREG_RF_WRITE, 1);

  TC317XREGSET_RFAddr(&g_pTC3X[moduleidx][hBB], addr, 1);
  TC317XREGSET_RFData(&g_pTC3X[moduleidx][hBB], data, 1);
  TC317XREGSET_RFAction(&g_pTC3X[moduleidx][hBB], TC3XREG_RF_ACTION, 1);

  TC317XREGSET_RFCfg(&g_pTC3X[moduleidx][hBB], TC3XREG_RF_MANAGE_DISABLE, 1);

  TC317X_IO_IF_UnLOCK (g_pTC3X[moduleidx][hBB].moduleidx);
}

void RF_I2C_WriteEX (int32_t moduleidx, TC3X_DNUM hBB, uint8_t devaddr, uint8_t *addr, uint32_t *data, int32_t cnt)
{
  int32_t i;

  for(i=0; i<cnt; i++)
  {
    RF_I2C_Write (moduleidx, hBB, devaddr, addr[i], data[i]);
  }
}

//---------------------------------------------------------
//  Common RF Control

typedef struct
{
  TC3X_DNUM hBB;
  TC3X_DNUM hRF;
  uint8_t I2CADDR;
} tRF_GVar;

//#define COMMON_I2C_ADDR 0xc0

tRF_GVar **pRF_GVar = NULL;

uint32_t (*fnCommonRFIICRead) (int32_t moduleidx, TC3X_DNUM hBB, uint8_t DEVADDR, uint8_t Addr);
void      (*fnCommonRFIICWrite) (int32_t moduleidx, TC3X_DNUM hBB, uint8_t DEVADDR, uint8_t Addr, uint32_t Data);
void      (*fnCommonRFIICWriteEX) (int32_t moduleidx, TC3X_DNUM hBB, uint8_t DEVADDR, uint8_t *pAddr, uint32_t *pData, int32_t cnt);

TC3X_DNUM CommonRFEntry (int32_t moduleidx, TC3X_DNUM hBB,
    FN_i_ihi * RF_Init,
    FN_v_ih * RF_Close, FN_v_ihii * RF_SetFrequency, FN_v_ihi * RF_CtrlPower, FN_i_ihivvvv * RF_UserFunction)
{
  pRF_GVar[moduleidx][hBB].hBB = hBB;
  pRF_GVar[moduleidx][hBB].hRF = hBB;

  if (RF_Init)
    *RF_Init = CommonRFInit;

  if (RF_Close)
    *RF_Close = CommonRFClose;

  if (RF_SetFrequency)
    *RF_SetFrequency = CommonRFSetFrequency;

  if (RF_CtrlPower)
    *RF_CtrlPower = CommonRFCtrlPower;

  if (RF_UserFunction)
    *RF_UserFunction = CommonRFUserFunction;

  return pRF_GVar[moduleidx][hBB].hRF;
}

void CommonRFAttachDevice (int32_t moduleidx, int32_t num)
{
  int32_t       maxbb;
  maxbb = TC317X_IO_Util_GET_MAXBB ();

  if (!pRF_GVar)
  {
    pRF_GVar = (tRF_GVar **) TC317X_IO_malloc (maxbb * sizeof (tRF_GVar));
    TC317X_IO_memset (&pRF_GVar[0], 0, maxbb * sizeof (tRF_GVar));
  }

  if (!pRF_GVar[moduleidx])
  {
    pRF_GVar[moduleidx] = (tRF_GVar *) TC317X_IO_malloc (sizeof (tRF_GVar) * num);
  }

  TC317X_IO_memset (&pRF_GVar[moduleidx][0], 0, sizeof (tRF_GVar) * num);
}

void CommonRFDetachDevice (int32_t moduleidx)
{
  int32_t       i;
  int32_t       exist;
  int32_t       maxbb;

  maxbb = TC317X_IO_Util_GET_MAXBB ();
  exist = 0;

  if (pRF_GVar[moduleidx])
    TC317X_IO_free (pRF_GVar[moduleidx]);

  pRF_GVar[moduleidx] = NULL;

  exist = 0;
  for (i = 0; i < maxbb; i++)
  {
    if (pRF_GVar[i])
    {
      exist = 1;
      break;
    }
  }

  if (!exist)
  {
    TC317X_IO_free (pRF_GVar);
    pRF_GVar = NULL;
  }
}

uint32_t CommonRFReadIIC (int32_t moduleidx, TC3X_DNUM hRF, uint8_t Addr)
{
  return (fnCommonRFIICRead (moduleidx, pRF_GVar[moduleidx][hRF].hBB, pRF_GVar[moduleidx][0].I2CADDR, Addr));
}

void CommonRFWriteIIC (int32_t moduleidx, TC3X_DNUM hRF, uint8_t Addr, uint32_t Data)
{
  fnCommonRFIICWrite (moduleidx, pRF_GVar[moduleidx][hRF].hBB, pRF_GVar[moduleidx][0].I2CADDR, Addr, Data);
}

void CommonRFWriteIICEX (int32_t moduleidx, TC3X_DNUM hRF, uint8_t *pAddr, uint32_t *pData, int32_t cnt)
{
  fnCommonRFIICWriteEX (moduleidx, pRF_GVar[moduleidx][hRF].hBB, pRF_GVar[moduleidx][0].I2CADDR, pAddr, pData, cnt);
}

int32_t CommonRFInit (int32_t moduleidx, TC3X_DNUM hRF, int32_t band)
{
  return 0;
}

void CommonRFClose (int32_t moduleidx, TC3X_DNUM hRF)
{
  CommonRFDetachDevice (moduleidx);
}

void CommonRFCtrlPower (int32_t moduleidx, TC3X_DNUM hRF, int32_t param1)       // param1 1:on 0:off
{
}

void CommonRFSetFrequency (int32_t moduleidx, TC3X_DNUM hRF, int32_t freq_khz, int32_t bw_khz)
{

}

void CommonRFSetI2CAddr (int32_t moduleidx, uint8_t addr, uint8_t num)
{
  uint32_t i;

  for (i = 0; i < num; i++)
    pRF_GVar[moduleidx][i].I2CADDR = addr;
}

int32_t CommonRFUserFunction (int32_t moduleidx, TC3X_DNUM hRF, int32_t message, void *param1, void *param2, void *param3, void *param4)
{
  int32_t       ret = 0;

  switch (message)
  {
    case CMD_RF_SET_RF_ADDR:
      {
        uint8_t addr;
        uint8_t num;
        addr = *(uint8_t *) param1;
        num = *(uint32_t *) param2;
        CommonRFSetI2CAddr (moduleidx, addr, num);
      }
      break;

    case CMD_RF_READ_REG_SINGLE:
      {
        // param1 : addr
        uint8_t addr;
        addr = *(uint8_t *) param1;
        ret = CommonRFReadIIC (moduleidx, hRF, addr);
      }
      break;

    case CMD_RF_WRITE_REG_SINGLE:
      {
        // param1 : addr, param2 : data
        uint8_t addr;
        uint32_t data;
        addr = *(uint8_t *) param1;
        data = *(uint32_t *) param2;
        CommonRFWriteIIC (moduleidx, hRF, addr, data);
      }
      break;

    case CMD_RF_GET_PLL_LOCKSTAT:
      ret = 0x01;
      break;

    case MSG_SET_FN_RF_READ:
      fnCommonRFIICRead = (uint32_t (*)(int32_t, TC3X_DNUM, uint8_t, uint8_t)) param1;
      break;

    case MSG_SET_FN_RF_WRITE:
      fnCommonRFIICWrite = (void (*)(int32_t, TC3X_DNUM, uint8_t, uint8_t, uint32_t)) param1;
      break;

    case MSG_SET_FN_RF_WRITEEX:
      fnCommonRFIICWriteEX = (void (*)(int32_t, TC3X_DNUM, uint8_t, uint8_t *, uint32_t *, int)) param1;
      break;

    default:
      break;
  }

  return ret;
}

