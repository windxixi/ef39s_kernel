//=============================================================================
// Telechips Confidential Proprietary
// Copyright (C) 2005-2009 Telechips Corporation
// All rights are reserved by Telechips Corporation
// File       : TC317X_Core_peri.h
// Description:
//
// Revision History:
// Version    Date             Author             Description of Changes
//-----------------------------------------------------------------------------
// 1.0.0       2009/04/14   stkim              Draft
//=============================================================================
#ifndef __TC317X_CORE_PERI_H__
#define __TC317X_CORE_PERI_H__

#include "TC317X_struc.h"

void      TC317X_Set_PERIperalForStream (t_TC3X_DEMODULE * pTC317X, TC3X_SUBIF_Header SH);
void      TC317X_PERI_EN (t_TC3X_DEMODULE * pTC317X);
void      TC317X_PERI_DN (t_TC3X_DEMODULE * pTC317X);

#endif
