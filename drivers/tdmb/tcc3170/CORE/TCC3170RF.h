//=============================================================================
// Telechips Confidential Proprietary
// Copyright (C) 2005-2009 Telechips Corporation
// All rights are reserved by Telechips Corporation
// File       : TCC3170.h
// Description:
//
// Revision History:
// Version    Date             Author             Description of Changes
//-----------------------------------------------------------------------------
// 1.0.0       2009/04/16   stkim              Draft
//=============================================================================
#ifndef __TCC3170RF_H__
#define __TCC3170RF_H__

#include "../TC317X_Common.h"
#include "../IO/TC317X_IO_UTIL.h"

#ifndef NULL
#define NULL 0
#endif

typedef struct
{
  TC3X_DNUM hBB;
  TC3X_DNUM hRF;
} tTCC3170RF_GVar;

TC3X_DNUM TCC3170RF_Entry (int32_t moduleidx, TC3X_DNUM hBB,
    FN_i_ihi * RF_Init,
    FN_v_ih * RF_Close, FN_v_ihii * RF_SetFrequency, FN_v_ihi * RF_CtrlPower, FN_i_ihivvvv * RF_UserFunction);

void      TCC3170RF_AttachDevice (int32_t moduleidx, int32_t num);
void      TCC3170RF_DetachDevice (int32_t moduleidx);
int32_t       TCC3170RF_RF_Init (int32_t moduleidx, TC3X_DNUM hRF, int32_t band);
void      TCC3170RF_RF_Close (int32_t moduleidx, TC3X_DNUM hRF);
void      TCC3170RF_RF_SetFrequency (int32_t moduleidx, TC3X_DNUM hRF, int32_t freq_khz, int32_t bw_khz);
void      TCC3170RF_RF_CtrlPower (int32_t moduleidx, TC3X_DNUM hRF, int32_t param1);      // param1 1:on 0:off
int32_t       TCC3170RF_RF_UserFunction (int32_t moduleidx, TC3X_DNUM hRF, int32_t message, void *param1, void *param2, void *param3, void *param4);
uint32_t TCC3170RF_ReadIIC (int32_t moduleidx, TC3X_DNUM hRF, uint8_t bIndex);
void      TCC3170RF_WriteIIC (int32_t moduleidx, TC3X_DNUM hRF, uint8_t bIndex, uint32_t bData);
void      TCC3170RF_WriteIIC_RegData (int32_t moduleidx, TC3X_DNUM hRF, uint8_t Taddr, uint32_t Tdata, int32_t bitposition, int32_t bitcount);
uint32_t       TCC3170RF_ReadIIC_RegData (int32_t moduleidx, TC3X_DNUM hRF, uint8_t Taddr, int32_t bitposition, int32_t bitcount);
void      TCC3170RF_SetOutBW (int32_t moduleidx, TC3X_DNUM hRF, int32_t bw_khz);
int32_t       TCC3170RF_pow2_user (int32_t nval);
void      TCC3170RF_WriteIIC_FormedData (int32_t moduleidx, TC3X_DNUM hRF, uint32_t *pData, uint32_t length);

#endif
