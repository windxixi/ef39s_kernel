//=============================================================================
// Telechips Confidential Proprietary
// Copyright (C) 2005-2009 Telechips Corporation
// All rights are reserved by Telechips Corporation
// File       : TC317X_Core_Interrupt.h
// Description:
//
// Revision History:
// Version    Date             Author             Description of Changes
//-----------------------------------------------------------------------------
// 1.0.0       2009/04/14   stkim              Draft
//=============================================================================
#ifndef __TC317X_CORE_INTERRUPT_H__
#define __TC317X_CORE_INTERRUPT_H__

#include "TC317X_struc.h"

void      InitSPICtrlSet (int32_t moduleidx);
void      SetINTInit (int32_t moduleidx);
void      TC317X_LISR_BBINT (int32_t moduleidx);
void      TC317X_HISR_BBINT (int32_t moduleidx);
void      TC317X_LISR_SPI (int32_t moduleidx, int32_t RcvIndex);
void      TC317X_HISR_SPI (int32_t moduleidx);

int32_t       TC317X_SPI_GetPreReqReadNum (int32_t moduleidx);
void      TC317X_SPI_SetRemainReqReadNum (int32_t moduleidx, int32_t num);
int32_t       TC317X_SPI_GetRemainReqReadNum (int32_t moduleidx);
void      TC317X_SPI_SetPreReqReadNum (int32_t moduleidx);
void      TC317X_SPI_SetPreReadNum (int32_t moduleidx, int32_t num);

#endif
