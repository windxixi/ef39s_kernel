//=============================================================================
// Telechips Confidential Proprietary
// Copyright (C) 2005-2009 Telechips Corporation
// All rights are reserved by Telechips Corporation
// File       : TC317X_deviceAPI.h
// Description:
//
// Revision History:
// Version    Date             Author             Description of Changes
//-----------------------------------------------------------------------------
// 1.0.0       2009/04/14   stkim              Draft
//=============================================================================
#ifndef __TC317X_DEVICEAPI_H__
#define __TC317X_DEVICEAPI_H__

#include "../TC317X_Common.h"
#include "../IO/TC317X_IO_UTIL.h"

typedef struct
{
  uint8_t *pStream;
  int32_t       PktSize;
  int32_t       PktThrNum;
} t_StreamSet;

typedef struct
{
  TC3X_DNUM hSTD;
  FN_v_ihivvvv BBEventFn;                                //callback: initial; '?? ?ֻ?'??.

  TC3X_DNUM hTC3X;
  void *pDemodule;
  int32_t Reserved0;
  int32_t bPreInited;

  //RF
  int32_t       rfType;
  TC3X_DNUM hRF;
  int32_t       (*RF_Init) (int32_t moduleidx, TC3X_DNUM hRF, int32_t band);
  void      (*RF_Close) (int32_t moduleidx, TC3X_DNUM hRF);
  void      (*RF_SetFrequency) (int32_t moduleidx, TC3X_DNUM hRF, int32_t freq_khz, int32_t bw_khz);
  void      (*RF_CtrlPower) (int32_t moduleidx, TC3X_DNUM hRF, int32_t param1);
  int32_t       (*RF_UserFunction) (int32_t moduleidx, TC3X_DNUM hRF, int32_t message, void *param1, void *param2, void *param3, void *param4);
} t_TC3X_CTRL;

TC3X_DNUM TC317X_Entry (int32_t moduleidx,
    TC3X_DNUM hSTD,
    FN_v_ihv * BB_PreInit,
    FN_i_ivi * BB_ColdBoot,
    FN_v_ihi * BB_Init,
    FN_v_ih * BB_Close,
    FN_i_ihiiv * BB_SetFrequency,
    FN_v_ihiPci * BB_StartService,
    FN_v_ihi * BB_StopService, FN_v_ihi * BB_CtrlPower, FN_i_ihivvvv * BB_UserFunction, FN_v_ihivvvv eventFn, int32_t option);
void      TC317X_AttachDevice (int32_t moduleidx, int32_t num);
void      TC317X_DetachDevice (int32_t moduleidx);
void      TC317X_PreInit (int32_t moduleidx, TC3X_DNUM hBB, void *IOSet);
int32_t       TC317X_ColdBoot (int32_t moduleidx, void *pInBin, int32_t option);
void      TC317X_Init (int32_t moduleidx, TC3X_DNUM hBB, int32_t Reserved0);
void      TC317X_Close (int32_t moduleidx, TC3X_DNUM hBB);
int32_t       TC317X_SetFrequency (int32_t moduleidx, TC3X_DNUM hBB, int32_t freq_khz, int32_t bw_khz, void *pOption);
void      TC317X_StartService (int32_t moduleidx, TC3X_DNUM hBB, int32_t bbSrvIdx, uint8_t *pReg, int32_t reg_cnt);
void      TC317X_StopService (int32_t moduleidx, TC3X_DNUM hBB, int32_t bbSrvIdx);
void      TC317X_CtrlPower (int32_t moduleidx, TC3X_DNUM hBB, int32_t setcmd);
int32_t       TC317X_UserFunc (int32_t moduleidx, TC3X_DNUM hBB, int32_t message, void *param1, void *param2, void *param3, void *param4);
int32_t       TC317X_Search (void);

extern t_TC3X_CTRL **g_pTC3XCTRL;

#endif
