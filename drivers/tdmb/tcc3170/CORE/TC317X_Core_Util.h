//=============================================================================
// Telechips Confidential Proprietary
// Copyright (C) 2005-2009 Telechips Corporation
// All rights are reserved by Telechips Corporation
// File       : TC317X_Core_Util.h
// Description:
//
// Revision History:
// Version    Date             Author             Description of Changes
//-----------------------------------------------------------------------------
// 1.0.0       2009/04/15   stkim              Draft
//=============================================================================
#ifndef __TC317X_CORE_UTIL_H__
#define __TC317X_CORE_UTIL_H__

#include "TC317X_struc.h"

void      StreamBuffCtrl_Init (int32_t moduleidx, int32_t MAXCNT);
void      buffctrl_init (int32_t moduleidx, uint32_t maxpktnum);
void      buffctrl_push (int32_t moduleidx, uint8_t *ptr);
uint8_t *buffctrl_get (int32_t moduleidx);
void      buffctrl_flush (int32_t moduleidx);
uint32_t buffctrl_GetPacketNum (int32_t moduleidx);
int32_t       StreamBuffCtrl_GetCurr (int32_t moduleidx);
int32_t       StreamBuffCtrl_GetAvail (int32_t moduleidx);
int32_t       StreamBuffCtrl_GetRoop (int32_t moduleidx, int32_t cnt);
void      StreamBuffCtrl_PushCnt (int32_t moduleidx, int32_t cnt);
int32_t       TSSyncCheck (uint8_t *src);
uint32_t buffctrl_GetAvailBurstWNum (int32_t moduleidx);
uint32_t buffctrl_GetWriteIdx (int32_t moduleidx);
#if defined (USE_ANDROID)
uint64_t TC317X_GetTickCnt (void);
uint64_t TC317X_GetInterval (uint64_t startTick);
#else
uint32_t TC317X_GetTickCnt (void);
uint32_t TC317X_GetInterval (uint32_t startTick);
#endif // USE_ANDROID

uint32_t TC317X_Pow2 (int32_t pow);
uint32_t TC317X_log10_ (uint32_t val);
uint32_t  TC317X_moving_avg_d (uint32_t *pArray, uint32_t input, uint32_t cnt, int32_t MAX_ARRAY);

void      Parse_TDMBSyncStat (t_TDMBLock * pTDMBLock, uint8_t input);

#endif
