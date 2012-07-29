//=============================================================================
// Telechips Confidential Proprietary
// Copyright (C) 2005-2009 Telechips Corporation
// All rights are reserved by Telechips Corporation
// File       : TC317X_deviceAPI.c
// Description:
//
// Revision History:
// Version    Date             Author             Description of Changes
//-----------------------------------------------------------------------------
// 1.0.0       2009/04/14   stkim              Draft
//=============================================================================
#include "TC317X_deviceAPI.h"
#include "TC317X_Core.h"
#include "../IO/TC317X_IO.h"
#include "TC317X_Core_RFCtrl.h"

#include "TC317X_Core_RegCtrl.h"

int32_t       g_NumOfTC3X[MAX_TCC3X];
t_TC3X_CTRL **g_pTC3XCTRL = NULL;

TC3X_DNUM TC317X_Entry (int32_t moduleidx,
    TC3X_DNUM hSTD,
    FN_v_ihv * BB_PreInit,
    FN_i_ivi * BB_ColdBoot,
    FN_v_ihi * BB_Init,
    FN_v_ih * BB_Close,
    FN_i_ihiiv * BB_SetFrequency,
    FN_v_ihiPci * BB_StartService,
    FN_v_ihi * BB_StopService, FN_v_ihi * BB_CtrlPower, FN_i_ihivvvv * BB_UserFunction, FN_v_ihivvvv eventFn, int32_t option)
{
  int32_t       idx;
  idx = hSTD;

  g_pTC3XCTRL[moduleidx][idx].hSTD = hSTD;

  if (eventFn)
    g_pTC3XCTRL[moduleidx][idx].BBEventFn = eventFn;

  g_pTC3XCTRL[moduleidx][idx].hTC3X = idx;

  if (BB_PreInit)
    *BB_PreInit = TC317X_PreInit;
  if (BB_ColdBoot)
    *BB_ColdBoot = TC317X_ColdBoot;
  if (BB_Init)
    *BB_Init = TC317X_Init;
  if (BB_Close)
    *BB_Close = TC317X_Close;
  if (BB_SetFrequency)
    *BB_SetFrequency = TC317X_SetFrequency;
  if (BB_StartService)
    *BB_StartService = TC317X_StartService;
  if (BB_StopService)
    *BB_StopService = TC317X_StopService;
  if (BB_CtrlPower)
    *BB_CtrlPower = TC317X_CtrlPower;
  if (BB_UserFunction)
    *BB_UserFunction = TC317X_UserFunc;

  return idx;
}

void TC317X_AttachDevice (int32_t moduleidx, int32_t num)
{
  int32_t       i;
  int32_t       maxbb;

  maxbb = TC317X_IO_Util_GET_MAXBB ();

  if (!g_pTC3XCTRL)
  {
    g_pTC3XCTRL = (t_TC3X_CTRL **) TC317X_IO_malloc (maxbb * sizeof (t_TC3X_CTRL));
    TC317X_IO_memset (&g_pTC3XCTRL[0], 0, maxbb * sizeof (t_TC3X_CTRL));
  }

  if (!g_pTC3XCTRL[moduleidx])
  {
    g_pTC3XCTRL[moduleidx] = (t_TC3X_CTRL *) TC317X_IO_malloc (sizeof (t_TC3X_CTRL) * num);
  }

  TC317X_IO_memset (&g_pTC3XCTRL[moduleidx][0], 0, sizeof (t_TC3X_CTRL) * num);
  for (i = 0; i < num; i++) {
    g_pTC3XCTRL[moduleidx][i].BBEventFn = TC317X_IO_UTIL_dummy_function9;
    g_pTC3XCTRL[moduleidx][i].RF_Init = TC317X_IO_UTIL_dummy_function3;
    g_pTC3XCTRL[moduleidx][i].RF_Close = TC317X_IO_UTIL_dummy_function4;
    g_pTC3XCTRL[moduleidx][i].RF_SetFrequency = TC317X_IO_UTIL_dummy_function5;
    g_pTC3XCTRL[moduleidx][i].RF_CtrlPower = TC317X_IO_UTIL_dummy_function6;
    g_pTC3XCTRL[moduleidx][i].RF_UserFunction = TC317X_IO_UTIL_dummy_function7;
  }

  TC317X_CoreAttach (moduleidx, num);
  for (i = 0; i < num; i++)
  {
    g_pTC3XCTRL[moduleidx][i].pDemodule = GetPointerTC317X (moduleidx, i);
  }
  g_NumOfTC3X[moduleidx] = num;
}

void TC317X_DetachDevice (int32_t moduleidx)
{
  int32_t       i;
  int32_t       exist;
  int32_t       maxbb;

  maxbb = TC317X_IO_Util_GET_MAXBB ();
  exist = 0;


  TC317X_CoreDetach (moduleidx);

  if (g_pTC3XCTRL[moduleidx])
    TC317X_IO_free (g_pTC3XCTRL[moduleidx]);

  g_pTC3XCTRL[moduleidx] = NULL;

  exist = 0;
  for (i = 0; i < maxbb; i++)
  {
    if (g_pTC3XCTRL[i])
    {
      exist = 1;
      break;
    }
  }

  if (!exist)
  {
    TC317X_IO_free (g_pTC3XCTRL);
    g_pTC3XCTRL = NULL;
  }

}

void TC317X_PreInit (int32_t moduleidx, TC3X_DNUM hBB, void *IOSet)
{
  tSTDIOSet *pSTDIOSet;
  uint32_t ChipID;
  pSTDIOSet = (tSTDIOSet *) IOSet;

  TC317X_CoreIOInit (moduleidx, g_pTC3XCTRL[moduleidx][hBB].hTC3X, pSTDIOSet->MainIO, pSTDIOSet->StreamIO, pSTDIOSet->PktSize, pSTDIOSet->PktThrNum);
  g_pTC3XCTRL[moduleidx][hBB].bPreInited = 1;

  ChipID = Uf_GetChipID (moduleidx);
  PRINTF_LV_0 ("# ChipID[0x%02x]\n", ChipID);
}

int32_t TC317X_ColdBoot (int32_t moduleidx, void *pInBin, int32_t option)
{
  TC3X_BOOTBIN *pBootBin;

  uint8_t *pCOLDBOOT;
  uint32_t COLDBOOTSize;
  uint8_t *pDAGU;
  uint32_t DAGUSize;
  uint8_t *pDINT;
  uint32_t DINTSize;
  uint8_t *pRAND;
  uint32_t RANDSize;
  uint8_t *pCOL_ORDER;
  uint32_t COL_ORDERSize;
  int32_t       ret;

  pBootBin = (TC3X_BOOTBIN *) pInBin;
  pCOLDBOOT = pBootBin->pCOLDBOOT;
  COLDBOOTSize = pBootBin->COLDBOOTSize;
  pDAGU = pBootBin->pDAGU;
  DAGUSize = pBootBin->DAGUSize;
  pDINT = pBootBin->pDINT;
  DINTSize = pBootBin->DINTSize;
  pRAND = pBootBin->pRAND;
  RANDSize = pBootBin->RANDSize;
  pCOL_ORDER = pBootBin->pCOL_ORDER;
  COL_ORDERSize = pBootBin->COL_ORDERSize;

  ret = TC317X_CoreColdBoot (moduleidx, pCOLDBOOT, COLDBOOTSize, pDAGU, DAGUSize, pDINT, DINTSize, pRAND, RANDSize, pCOL_ORDER,
      COL_ORDERSize);

  return ret;
}

// First Set Slave and then Master.
void TC317X_Init (int32_t moduleidx, TC3X_DNUM hBB, int32_t Reserved0)
{
  TC317X_CoreInit (moduleidx, 0);
}

void TC317X_Close (int32_t moduleidx, TC3X_DNUM hBB)
{
  if (g_pTC3XCTRL[moduleidx])
    TC317X_DetachDevice (moduleidx);
}

int32_t TC317X_SetFrequency (int32_t moduleidx, TC3X_DNUM hBB, int32_t freq_khz, int32_t bw_khz, void *pOption)
{
  tFreqSet_Option *pFreqSetOption;

  pFreqSetOption = (tFreqSet_Option *) pOption;

  return (TC317X_CoreSetFrequency (moduleidx, g_pTC3XCTRL[moduleidx][0].hTC3X, freq_khz, bw_khz, pFreqSetOption));
}

void TC317X_StartService (int32_t moduleidx, TC3X_DNUM hBB, int32_t bbSrvIdx, uint8_t *pReg, int32_t reg_cnt)
{
}

void TC317X_StopService (int32_t moduleidx, TC3X_DNUM hBB, int32_t bbSrvIdx)
{
}

void TC317X_CtrlPower (int32_t moduleidx, TC3X_DNUM hBB, int32_t setcmd)
{
  switch (setcmd)
  {
    case CMD_BB_PW_RESET:      // reset
      TC317X_IO_PW_RESET (moduleidx);
      break;

    case CMD_BB_PW_DN: // pwdn
      TC317X_IO_PW_DN (moduleidx);
      break;

    case CMD_BB_PW_ON: // pwon
      TC317X_IO_PW_ON (moduleidx);
      break;

    default:
      break;
  }
}

int32_t TC317X_UserFunc (int32_t moduleidx, TC3X_DNUM hBB, int32_t message, void *param1, void *param2, void *param3, void *param4)
{
  int32_t       ret = 0;
  uint32_t Addr;
  uint32_t Data;
  int32_t       cmd;
  int32_t       wordcnt;
  uint32_t *dataArray;
  uint8_t *pData;
  uint32_t Size;

  //tFreqSet_Option *pOption;

  TDMBService_t *pTDMBService;
  t_TDMBLock *pLockDMB;

  if (hBB >= g_NumOfTC3X[moduleidx])
  {
    PRINTF_LV_0 ("# [TC317X_UserFunc] InValid Handle\n");
    return 0;
  }

  switch (message)
  {
    case CMD_BB_WARMBOOT:
      ret = UF_WarmBoot (moduleidx, g_pTC3XCTRL[moduleidx][hBB].hTC3X);
      break;

    case CMD_BB_REG_R:
      Addr = *(uint32_t *) param1;
      ret = Uf_RegRead (moduleidx, g_pTC3XCTRL[moduleidx][hBB].hTC3X, Addr);
      break;

    case CMD_BB_REG_W:
      Addr = *(uint32_t *) param1;
      Data = *(uint32_t *) param2;
      Uf_RegWrite (moduleidx, g_pTC3XCTRL[moduleidx][hBB].hTC3X, Addr, Data);
      break;

    case CMD_BB_MAILBOX_R:
      cmd = *(int32_t *) param1;
      // param2 : tmailbox
      Uf_MailBoxRead (moduleidx, g_pTC3XCTRL[moduleidx][hBB].hTC3X, cmd, param2);
      break;

    case CMD_BB_MAILBOX_W:
      cmd = *(int32_t *) param1;
      dataArray = (uint32_t *) param2;
      wordcnt = *(int32_t *) param3;
      Uf_MailBoxWrite (moduleidx, g_pTC3XCTRL[moduleidx][hBB].hTC3X, cmd, dataArray, wordcnt);
      break;

    case CMD_BB_SET_FIFO_SIZE:
      Uf_SetFifoSize(moduleidx, g_pTC3XCTRL[moduleidx][hBB].hTC3X, *(int*)param1);
      break;

    case CMD_BB_STREAMOUT_RESTART:
      UF_RestartStream (moduleidx);
      break;

    case CMD_BB_STREAMOUT_PAUSE:
      UF_PauseStream (moduleidx);
      break;

    case CMD_BB_GET_BOOTCODE_VERSION:
      UF_GetBootCodeVersion (moduleidx, (uint32_t *) param1);
      break;

    case CMD_BB_TDMB_REG:
      pTDMBService = (TDMBService_t *) param1;
      Uf_REG_TDMB_Service (moduleidx, hBB, pTDMBService);
      break;

    case CMD_BB_TDMB_UNREG:
      pTDMBService = (TDMBService_t *) param1;
      ret = Uf_UnREG_TDMB_Service (moduleidx, hBB, pTDMBService);
      break;

    case CMD_GET_TDMB_LOCK:
      pLockDMB = (t_TDMBLock *) param1;
      Uf_GetTDMB_LOCK (moduleidx, hBB, pLockDMB);
      break;

    case CMD_GET_TDMB_SNR_SET:
      Uf_GetTDMB_SNR_SET (moduleidx, hBB,
          (uint32_t *) param1, (uint32_t *) param2, (uint32_t *) param3);
      break;

    case CMD_GET_TDMB_PCBER_SET:
      Uf_GetTDMB_PCBER_SET (moduleidx, hBB,
          (uint32_t *) param1, (uint32_t*) param2);
      break;

    case CMD_GET_TDMB_VITERBIBER_SET:
      Uf_GetTDMB_VITERBIBER_SET (moduleidx, hBB,
          (uint32_t *) param1, (uint32_t *) param2);
      break;

    case CMD_GET_TDMB_TSPER_SET:
      Uf_GetTDMB_TSPER_SET (moduleidx, hBB,
          (uint32_t *) param1, (uint32_t *) param2);
      break;

    case CMD_GET_TDMB_RSSI_SET:
      Uf_GetTDMB_RSSI_SET (moduleidx, hBB,
          (uint32_t *) param1, (uint32_t *) param2);
      break;

    case CMD_PUSH_TDMB_STAT_DATA:
      Size = *(uint32_t *) param2;
      ret = Uf_PushTDMBStatData (moduleidx, hBB, (uint8_t *) param1, Size);
      break;

    case CMD_REQUEST_PARSE_TDMB_DATA:
      Size = *(uint32_t *) param2;
      Uf_PushTDMB_RawData (moduleidx, hBB, (uint8_t *) param1, Size);
      break;

    case CMD_REQUEST_TDMB_RESET_MONITOR_VALUE:
      Uf_PushTDMB_ResetMonitoringValue (moduleidx, hBB);
      break;

    case CMD_BB_CHECK_OP:
      ret = UF_CheckOP (moduleidx);
      break;

    case CMD_BB_GET_CHIPID:
      ret = Uf_GetChipID (moduleidx);
      break;

    case CMD_BB_WRITE_MEM:
      pData = (uint8_t *) param1;
      Addr = *(uint32_t *) param2;
      Size = *(uint32_t *) param3;

      Uf_SetMEMWrite (moduleidx, pData, Addr, Size);
      break;

    case CMD_BB_READ_MEM:
      pData = (uint8_t *) param1;
      Addr = *(uint32_t *) param2;
      Size = *(uint32_t *) param3;

      Uf_GetMEMRead (moduleidx, pData, Addr, Size);
      break;

    case CMD_BB_PLL_SET:
      {
        uint8_t PLL_WaitTime;
        uint8_t PLL_P;
        uint8_t PLL_M;
        uint8_t PLL_S;
        uint32_t OSC_Khz;

        pData = (uint8_t *) param1;
        OSC_Khz = *(uint32_t *) param2;
        PLL_WaitTime = pData[0];
        PLL_P = pData[1];
        PLL_M = pData[2];
        PLL_S = pData[3];

        Uf_SetPLLSet (moduleidx, PLL_WaitTime, PLL_P, PLL_M, PLL_S, OSC_Khz);
      }
      break;

      // RF   - pass-trough
    case CMD_RF_READ_REG_SINGLE:       // equal
    case CMD_RF_WRITE_REG_SINGLE:      // equal
    case CMD_RF_GET_PLL_LOCKSTAT:      // equal
      ret = g_pTC3XCTRL[moduleidx][hBB].RF_UserFunction (moduleidx, g_pTC3XCTRL[moduleidx][hBB].hRF, message, param1, param2, param3,
          param4);
      break;
  }

  return ret;
}

int32_t TC317X_Search ()
{
  return 1;
}
