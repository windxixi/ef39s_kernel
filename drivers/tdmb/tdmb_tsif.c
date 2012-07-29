//=============================================================================
// File       : T3700_tsif.c
//
// Description: 
//
// Revision History:
//
// Version         Date           Author        Description of Changes
//-----------------------------------------------------------------------------
//  1.0.0       2010/12/06       yschoi         Create
//=============================================================================

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
//#include <linux/i2c.h>
#include <linux/delay.h>

#include "tdmb_comdef.h"
#include "tdmb_dev.h"
#include "tdmb_tsif.h"
#include "tdmb_i2c.h"
#include "tdmb_type.h"

#ifdef FEATURE_TDMB_TSIF_CLK_CTL
#include <../../arch/arm/mach-msm/clock-pcom.h>
#endif



extern void tsif_test_tdmb(void);

/*================================================================== */
/*==============       TDMB TSIF Driver Function     =============== */
/*================================================================== */

/*===========================================================================
FUNCTION       tdmb_tsif_clk_enable
DESCRIPTION
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
===========================================================================*/
void tdmb_tsif_clk_enable(void)
{
#ifdef FEATURE_TDMB_TSIF_CLK_CTL
  int rc;
#endif

  TDMB_MSG_TSIF("[%s]\n", __func__);
  
#ifdef FEATURE_TDMB_TSIF_CLK_CTL
  rc = pc_clk_enable(P_TSIF_CLK);
  if(rc)
  {
    TDMB_MSG_TSIF("[%s] pc_clk_enable fail!!! [%d]\n", __func__, rc);
  }
#endif /* FEATURE_TDMB_TSIF_CLK_CTL */
}


/*===========================================================================
FUNCTION       tdmb_tsif_clk_disable
DESCRIPTION
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
===========================================================================*/
void tdmb_tsif_clk_disable(void)
{
  TDMB_MSG_TSIF("[%s]\n", __func__);

#ifdef FEATURE_TDMB_TSIF_CLK_CTL
  pc_clk_disable(P_TSIF_CLK);
#endif /* FEATURE_TDMB_TSIF_CLK_CTL */
}


/*===========================================================================
FUNCTION       tdmb_tsif_test
DESCRIPTION
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
===========================================================================*/
void tdmb_tsif_test(void)
{
  TDMB_MSG_TSIF("[%s]\n", __func__);

  tsif_test_tdmb();
}

