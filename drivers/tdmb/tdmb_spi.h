//=============================================================================
// File       : tdmb_spi.h
//
// Description: 
//
// Revision History:
//
// Version         Date           Author        Description of Changes
//-----------------------------------------------------------------------------
//  1.0.0       2011/04/20       yschoi         Create
//=============================================================================

#ifndef _TDMB_SPI_H_
#define _TDMB_SPI_H_

#include "tdmb_type.h"
//#include <linux/spi/spi.h>


/* ========== Message ID for TDMB ========== */

#define TDMB_MSG_SPI(fmt, arg...) \
  TDMB_KERN_MSG_ALERT(fmt, ##arg)



#define TDMB_SPI_DEV_NAME "tdmb_spi"


void tdmb_spi_clear_int(void);
void tdmb_spi_init (void);

void* tdmb_spi_setup(void);

extern int tdmb_spi_write_then_read(u8 *txbuf, unsigned n_tx,u8 *rxbuf, unsigned n_rx);


#endif

