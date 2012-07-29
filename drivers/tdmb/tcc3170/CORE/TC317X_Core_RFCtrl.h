//=============================================================================
// Telechips Confidential Proprietary
// Copyright (C) 2005-2009 Telechips Corporation
// All rights are reserved by Telechips Corporation
// File       : TC317X_Core_RFCtrl.h
// Description:
//
// Revision History:
// Version    Date             Author             Description of Changes
//-----------------------------------------------------------------------------
// 1.0.0       2009/04/16   stkim              Draft
//=============================================================================
#ifndef __TC317X_CORE_RFCTRL_H__
#define __TC317X_CORE_RFCTRL_H__

#include "TC317X_deviceAPI.h"
#include "TC317X_struc.h"
#include "../IO/TC317X_IO_UTIL.h"

//---------------------------------------------------------------------------------------
//      RF Define
typedef enum
{
  TC3X_RF_NONE = 0,
  TC3X_RF_TCC3170
} TC3X_RF_Series;

void      TC317X_SetRFINF (int32_t moduleidx, TC3X_DNUM hBB, int32_t Reserved0);
uint32_t TC317X_Find_RF (t_TC3X_DEMODULE * pTC317X);
void      TC317X_RFAttach (int32_t moduleidx);
TC3X_DNUM TC317X_RF_Entry (int32_t moduleidx, t_TC3X_CTRL * pTC3XCtrl);
uint32_t RF_I2C_Read (int32_t moduleidx, TC3X_DNUM hBB, uint8_t devaddr, uint8_t addr);
void      RF_I2C_Write (int32_t moduleidx, TC3X_DNUM hBB, uint8_t devaddr, uint8_t addr, uint32_t data);
void      RF_I2C_WriteEX (int32_t moduleidx, TC3X_DNUM hBB, uint8_t devaddr, uint8_t *addr, uint32_t *data, int32_t cnt);

TC3X_DNUM CommonRFEntry (int32_t moduleidx, TC3X_DNUM hBB,
    FN_i_ihi * RF_Init,
    FN_v_ih * RF_Close, FN_v_ihii * RF_SetFrequency, FN_v_ihi * RF_CtrlPower, FN_i_ihivvvv * RF_UserFunction);
void      CommonRFAttachDevice (int32_t moduleidx, int32_t num);
void      CommonRFDetachDevice (int32_t moduleidx);
uint32_t CommonRFReadIIC (int32_t moduleidx, TC3X_DNUM hRF, uint8_t Addr);
void      CommonRFWriteIIC (int32_t moduleidx, TC3X_DNUM hRF, uint8_t Addr, uint32_t Data);
void      CommonRFWriteIICEX (int32_t moduleidx, TC3X_DNUM hRF, uint8_t *pAddr, uint32_t *pData, int32_t cnt);
int32_t       CommonRFInit (int32_t moduleidx, TC3X_DNUM hRF, int32_t band);
void      CommonRFClose (int32_t moduleidx, TC3X_DNUM hRF);
void      CommonRFCtrlPower (int32_t moduleidx, TC3X_DNUM hRF, int32_t param1);
void      CommonRFSetFrequency (int32_t moduleidx, TC3X_DNUM hRF, int32_t freq_khz, int32_t bw_khz);
int32_t       CommonRFUserFunction (int32_t moduleidx, TC3X_DNUM hRF, int32_t message, void *param1, void *param2, void *param3, void *param4);

#endif
