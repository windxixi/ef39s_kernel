//=============================================================================
// Telechips Confidential Proprietary
// Copyright (C) 2005-2009 Telechips Corporation
// All rights are reserved by Telechips Corporation
// File       : TC317X_Core_I2CMS.h
// Description:
//
// Revision History:
// Version    Date             Author             Description of Changes
//-----------------------------------------------------------------------------
// 1.0.0       2009/04/16   stkim              Draft
//=============================================================================
#ifndef __TC317X_CORE_I2CMS_H__
#define __TC317X_CORE_I2CMS_H__

#include "TC317X_struc.h"

void      TC317X_InitI2CMS_Polling (t_TC3X_DEMODULE * pTC317X, int32_t in_khz);
uint32_t TC317X_I2CMS_Polling_Write8 (t_TC3X_DEMODULE * pTC317X, uint8_t deviceaddr, uint8_t targetaddr, uint8_t input);
uint32_t TC317X_I2CMS_Polling_Read8 (t_TC3X_DEMODULE * pTC317X, uint8_t deviceaddr, uint8_t targetaddr, uint8_t *pData);
uint32_t TC317X_I2CMS_Polling_MultiWrite8 (t_TC3X_DEMODULE * pTC317X, uint8_t deviceaddr, uint8_t *ptargetaddr,
    uint8_t *pinput, int32_t length);
uint32_t TC317X_I2CMS_Polling_MultiRead8 (t_TC3X_DEMODULE * pTC317X, uint8_t deviceaddr, uint8_t *ptargetaddr,
    uint8_t *pData, int32_t length);
void      TC317X_SlaveIF_I2CMS_Init (t_TC3X_DEMODULE * pTC317X, int32_t in_khz);
uint32_t TC317X_SlaveIF_I2CMS_Reg_Read (int32_t moduleidx, int32_t ChipAddr, int32_t RegAddr, TC3X_IO_Err * pError);      // use 52cmd
TC3X_IO_Err TC317X_SlaveIF_I2CMS_Reg_Write (int32_t moduleidx, int32_t ChipAddr, int32_t RegAddr, uint32_t data); // use 52cmd
TC3X_IO_Err TC317X_SlaveIF_I2CMS_Reg_ReadEX (int32_t moduleidx, int32_t ChipAddr, int32_t RegAddr, uint8_t *data, int32_t iSize);   // use 53cmd
TC3X_IO_Err TC317X_SlaveIF_I2CMS_Reg_WriteEX (int32_t moduleidx, int32_t ChipAddr, int32_t RegAddr, uint8_t *pCh, int32_t iSize);   // use 53cmd
uint32_t TC317X_SlaveIF_I2CMS_Reg_Read_InnerSem (int32_t moduleidx, int32_t ChipAddr, int32_t RegAddr, TC3X_IO_Err * pError);
TC3X_IO_Err TC317X_SlaveIF_I2CMS_Reg_Write_InnerSem (int32_t moduleidx, int32_t ChipAddr, int32_t RegAddr, uint32_t data);
TC3X_IO_Err TC317X_SlaveIF_I2CMS_Reg_ReadEX_InnerSem (int32_t moduleidx, int32_t ChipAddr, int32_t RegAddr, uint8_t *data, int32_t iSize);
TC3X_IO_Err TC317X_SlaveIF_I2CMS_Reg_WriteEX_InnerSem (int32_t moduleidx, int32_t ChipAddr, int32_t RegAddr, uint8_t *pCh, int32_t iSize);

#endif
