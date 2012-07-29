//=============================================================================
// File       : Tdmb_comdef.h
//
// Description: 
//
// Revision History:
//
// Version         Date           Author        Description of Changes
//-----------------------------------------------------------------------------
//  1.0.0       2009/05/06       yschoi         Create
//=============================================================================

#ifndef _TDMB_COMDEF_INCLUDES_H_
#define _TDMB_COMDEF_INCLUDES_H_

#include "tdmb_modeldef.h"

#ifdef CONFIG_SKY_TDMB_MODULE
#define FEATURE_QTDMB_MODULE
#endif /* CONFIG_SKY_TDMB_MODULE */

#ifdef CONFIG_SKY_TDMB_INC_BB // EF10, EF12, SP33K
  #define FEATURE_QTDMB_USE_INC

  #ifdef CONFIG_SKY_TDMB_INC_BB_T3700
  #define FEATURE_QTDMB_USE_INC_T3700
  #endif

  #ifdef CONFIG_SKY_TDMB_INC_BB_T3900
  #define FEATURE_QTDMB_USE_INC_T3900
  #endif

  #define FEATURE_QTDMB_MULTI_CHANNEL_ENABLE
#endif /* CONFIG_TDMB_INC_BB */

#ifdef CONFIG_SKY_TDMB_FCI_BB  // EF13
  #define FEATURE_QTDMB_USE_FCI

  #ifdef CONFIG_SKY_TDMB_FCI_BB_FC8050
  #define FEATURE_QTDMB_USE_FCI_FC8050
  #endif

  #define FEATURE_QTDMB_MULTI_CHANNEL_ENABLE
#endif /* CONFIG_TDMB_FCI_BB */

#ifdef CONFIG_SKY_TDMB_RTV_BB  // EF33
  #define FEATURE_QTDMB_USE_RTV

  #ifdef CONFIG_SKY_TDMB_RTV_BB_MTV350
  #define FEATURE_QTDMB_USE_RTV_MTV350
  #endif

  #define FEATURE_QTDMB_MULTI_CHANNEL_ENABLE
#endif /* CONFIG_TDMB_FCI_BB */

#ifdef CONFIG_SKY_TDMB_TCC_BB
  #define FEATURE_QTDMB_USE_TCC

  #ifdef CONFIG_SKY_TDMB_TCC_BB_TCC3170
  #define FEATURE_QTDMB_USE_TCC_TCC3170
  #endif
  #define FEATURE_QTDMB_MULTI_CHANNEL_ENABLE
#endif /* CONFIG_SKY_TDMB_TCC_BB */

#ifdef CONFIG_SKY_TDMB_EBI_IF
  #define FEATURE_QTDMB_EBI_IF

  #ifdef CONFIG_SKY_TDMB_EBI_CMD // EF10, SP33K, EF12S, EF13
  #define FEATURE_QTDMB_EBI_CMD
  #endif
#endif /* CONFIG_SKY_TDMB_EBI_IF */

#ifdef CONFIG_SKY_TDMB_TSIF_IF
#define FEATURE_QTDMB_TSIF_IF
#define FEATURE_QTDMB_TSIF_READ_ONCE
#endif

#ifdef CONFIG_SKY_TDMB_I2C_CMD // EF10 초기
  #define FEATURE_QTDMB_I2C_CMD
#endif

#ifdef CONFIG_SKY_TDMB_SPI_IF
  #define FEATURE_QTDMB_SPI_IF

  #ifdef CONFIG_SKY_TDMB_SPI_CMD
    #define FEATURE_QTDMB_SPI_CMD
  #endif

  #define FEATURE_QTDMB_MULTI_CHANNEL_ENABLE
#endif

// driver 에서는 define 하고 framework에서 구분하기로 함.
#if 0//(defined(CONFIG_EF10_BOARD) || defined(CONFIG_EF12_BOARD) || defined(CONFIG_SP33_BOARD))
  // Eclair
  // DMB include visual radio
#else
  // Froyo
  #define FEATURE_TDMB_VISUAL_RADIO_SERVICE
#endif


// TDMB kernel MSG FEATURE
#define FEATURE_TDMB_KERNEL_MSG_ON



/*================================================================== */
/*================     TDMB Kconfig Tree           ================= */
/*================================================================== */
//#
//# TDMB - Terrestrial Digital Multimedia Broadcasting
//#
/*

CONFIG_SKY_TDMB

  CONFIG_SKY_TDMB_MODULE

  CONFIG_SKY_TDMB_INC_BB
    CONFIG_SKY_TDMB_INC_BB_T3700
    CONFIG_SKY_TDMB_INC_BB_T3900
    
  CONFIG_SKY_TDMB_FCI_BB
    CONFIG_SKY_TDMB_FCI_BB_FC8050

  CONFIG_SKY_TDMB_RTV_BB
    CONFIG_SKY_TDMB_RTV_BB_MTV350

  CONFIG_SKY_TDMB_EBI_IF
    CONFIG_SKY_TDMB_EBI_CMD

  CONFIG_SKY_TDMB_TSIF_IF

  CONFIG_SKY_TDMB_I2C_CMD
    CONFIG_SKY_TDMB_I2C_HW
    CONFIG_SKY_TDMB_I2C_GPIO

*/



/*================================================================== */
/*================     Kernel Message for TDMB     ================= */
/*================================================================== */

#ifdef FEATURE_TDMB_KERNEL_MSG_ON
#define TDMB_KERN_MSG_EMERG(fmt, arg...) \
  pr_emerg(fmt, ##arg)

#define TDMB_KERN_MSG_ALERT(fmt, arg...) \
  pr_alert(fmt, ##arg)

#define TDMB_KERN_MSG_CRIT(fmt, arg...) \
  pr_crit(fmt, ##arg)

#define TDMB_KERN_MSG_ERR(fmt, arg...) \
  pr_err(fmt, ##arg)

#define TDMB_KERN_MSG_WARNING(fmt, arg...) \
  pr_warning(fmt, ##arg)

#define TDMB_KERN_MSG_NOTICE(fmt, arg...) \
  pr_notice(fmt, ##arg)

#define TDMB_KERN_MSG_INFO(fmt, arg...) \
  pr_info(fmt, ##arg)
#else
#define TDMB_KERN_MSG_EMERG(fmt, arg...) \
  {}

#define TDMB_KERN_MSG_ALERT(fmt, arg...) \
  {}

#define TDMB_KERN_MSG_CRIT(fmt, arg...) \
  {}

#define TDMB_KERN_MSG_ERR(fmt, arg...) \
  {}

#define TDMB_KERN_MSG_WARNING(fmt, arg...) \
  {}

#define TDMB_KERN_MSG_NOTICE(fmt, arg...) \
  {}

#define TDMB_KERN_MSG_INFO(fmt, arg...) \
  {}
#endif /* FEATURE_TDMB_KERNEL_MSG_ON */

#endif /* _TDMB_COMDEF_INCLUDES_H_ */