//=============================================================================
// File       : tdmb_gpio.h
//
// Description: 
//
// Revision History:
//
// Version         Date           Author        Description of Changes
//-----------------------------------------------------------------------------
//  1.0.0       2010/11/02       yschoi         Create
//=============================================================================

#ifndef _TDMB_GPIO_H_
#define _TDMB_GPIO_H_

#include "tdmb_type.h"

/* ========== Message ID for TDMB ========== */

#define TDMB_MSG_GPIO(fmt, arg...) \
  TDMB_KERN_MSG_ALERT(fmt, ##arg)


#ifdef FEATURE_TDMB_GPIO_INIT
void tdmb_gpio_init(void);
#endif

void tdmb_set_gpio(uint gpio, bool value);

#endif /* _TDMB_GPIO_H_ */

