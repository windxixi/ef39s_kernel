//=============================================================================
// File       : Tdmb_netber.h
//
// Description: 
//
// Revision History:
//
// Version         Date           Author        Description of Changes
//-----------------------------------------------------------------------------
//  1.0.0       2009/05/06       yschoi         Create
//=============================================================================

#ifndef TDMB_TEST_INCLUDES_H
#define TDMB_TEST_INCLUDES_H

/*===================================================================
                     Include files
====================================================================*/
#include "tdmb_type.h"
#include "tdmb_comdef.h"

#ifdef FEATURE_QTDMB_USE_INC_T3900
#include "t3900/t3900_bb.h"
#endif

#ifdef FEATURE_QTDMB_USE_INC_T3700
#include "t3700/t3700_bb.h"
#endif

#ifdef FEATURE_QTDMB_USE_FCI
#include "fc8050/fc8050_wrapper.h"
#endif /* FEATURE_QTDMB_USE_FCI */

#ifdef FEATURE_QTDMB_USE_RTV
#include "mtv350/mtv350_bb.h"
#endif /* FEATURE_QTDMB_USE_RTV */

#ifdef FEATURE_QTDMB_USE_TCC
#include "tcc3170/tcc3170_bb.h"
#endif

/*===================================================================
                         define
====================================================================*/

/* ========== Message ID for TDMB ========== */

#define TDMB_MSG_TEST(fmt, arg...) \
  TDMB_KERN_MSG_ALERT(fmt, ##arg)


/*===================================================================
                         Structure
====================================================================*/

typedef enum {
  TDMB_U1,         // 8A (181280)
  TDMB_mYTN,       // 8B (183008)
  TDMB_QBS,        // 8C (184736) (1 to 1)
  TDMB_myMBC,      // 12A (205280)
  TDMB_KBS_STAR,  // 12B (207008 )
  TDMB_KBS_HEART, // 12B (207008)
  TDMB_SBS_u_TV,   // 12C (208736)
  TDMB_RADIO,   // 12C (208736)
  TDMB_TEST,        // 10B (195008)
  TDMB_HW_DTV_TEST, // H/W 5층 실험실 DTV 장비 air stream
  TDMB_NETBER       // H/W 5층 실험실 DTV 장비 netber stream
}service_t;

typedef struct
{
  uint16 ensemble_id;
  uint32 service_id;
  uint8  subch_id;
  uint32 freq;
  uint16 frame_num;
} nerber_info_type;

#define TDMB_TEST_CH TDMB_KBS_HEART

/*===================================================================
                     Global variables
====================================================================*/


/*===========================================================================
FUNCTION       netber_init
DESCRIPTION
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
===========================================================================*/
void netber_init(void);

/*===========================================================================
FUNCTION       netber_GetError
DESCRIPTION
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
===========================================================================*/
void netber_GetError(unsigned short buf_size, uint8 *buf);

/*===========================================================================
FUNCTION       netber_GetErrorBitCount
DESCRIPTION
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
===========================================================================*/
unsigned long netber_GetErrorBitCount(unsigned short src, unsigned char dest);

/*===========================================================================
FUNCTION       netber_clr_loop_count
DESCRIPTION
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
===========================================================================*/
void netber_clr_loop_count(void);

void tdmb_ch_test(uint8 ch);

#if (defined(FEATURE_TEST_ON_BOOT) || defined(FEATURE_NETBER_TEST_ON_BOOT))// 부팅중에 테스트
void tdmb_test_on_boot(void);
#endif

int tdmb_get_fixed_chan_info(service_t servicetype, chan_info* pChInfo);


#endif /* TDMB_TEST_INCLUDES_H */

