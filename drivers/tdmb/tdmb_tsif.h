//=============================================================================
// File       : Tdmb_tsif.h
//
// Description: 
//
// Revision History:
//
// Version         Date           Author        Description of Changes
//-----------------------------------------------------------------------------
//  1.0.0       2010/12/06       yschoi         Create
//=============================================================================

#ifndef _TDMB_TSIF_INCLUDES_H_
#define _TDMB_TSIF_INCLUDES_H_

#include "tdmb_type.h"


/* ========== Message ID for TDMB ========== */

#define TDMB_MSG_TSIF(fmt, arg...) \
  TDMB_KERN_MSG_ALERT(fmt, ##arg)



void tdmb_tsif_clk_enable(void);
void tdmb_tsif_clk_disable(void);

void tdmb_tsif_test(void);

#endif /* _TDMB_TSIF_INCLUDES_H_ */