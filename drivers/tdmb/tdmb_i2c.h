//=============================================================================
// File       : Tdmb_i2c.h
//
// Description: 
//
// Revision History:
//
// Version         Date           Author        Description of Changes
//-----------------------------------------------------------------------------
//  1.0.0       2009/05/06       yschoi         Create
//=============================================================================

#ifndef _TDMB_I2C_INCLUDES_H_
#define _TDMB_I2C_INCLUDES_H_

#include "tdmb_type.h"


/* ========== Message ID for TDMB ========== */

#define TDMB_MSG_I2C(fmt, arg...) \
  TDMB_KERN_MSG_ALERT(fmt, ##arg)



void tdmb_i2c_api_Init(void);
void tdmb_i2c_api_DeInit(void);

uint8 tdmb_i2c_write_word(uint8 chipid, uint16 reg, uint8 reg_len, uint16 *data, uint16 data_len);
uint8 tdmb_i2c_write_len(uint8 chipid, uint16 reg, uint8 reg_len, uint8 *data, uint16 data_len);
uint8 tdmb_i2c_read_word(uint8 chipid, uint16 reg, uint8 reg_len, uint16 *data, uint16 data_len);
uint8 tdmb_i2c_read_len(uint8 chipid, uint16 reg, uint8 reg_len, uint8 *data, uint16 data_len);

#endif /* _TDMB_I2C_INCLUDES_H_ */
