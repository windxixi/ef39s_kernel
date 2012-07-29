//=============================================================================
// File       : Tdmb_modeldef.h
//
// Description: 
//
// Revision History:
//
// Version         Date           Author        Description of Changes
//-----------------------------------------------------------------------------
//  1.0.0       2010/12/06       yschoi         Create
//                                              (tdmb_dev.h, tdmb_comdef.h 에서 분리)
//=============================================================================

#ifndef _TDMB_MODELDEF_INCLUDES_H_
#define _TDMB_MODELDEF_INCLUDES_H_


/*================================================================== */
/*================     MODEL FEATURE               ================= */
/*================================================================== */

#if 0
/////////////////////////////////////////////////////////////////////////
// TDMB GPIO (depend on model H/W)
// 중복되어 define 된 모델이 있으므로 사용하는 모델을 맨앞으로 가져온다.
/////////////////////////////////////////////////////////////////////////
#elif (defined(CONFIG_SKY_EF40S_BOARD) || defined(CONFIG_SKY_EF40K_BOARD))
  #define DMB_RESET     102
  #define DMB_INT       127
  #define DMB_PWR_EN    86
  #define FEATURE_TDMB_SET_ANT_PATH
  #define DMB_ANT_SEL   42
  #define DMB_ANT_EAR_ACT   1 
#if (BOARD_REV > WS20)
  #define FEATURE_TDMB_PMIC_TCXO_192M
  #define DMB_XO_SEL 19
#endif
  //#define FEATURE_TDMB_PMIC_POWER
  #define FEATURE_TDMB_GPIO_INIT
  #define FEATURE_TDMB_THREAD
#elif defined(CONFIG_SKY_EF39S_BOARD)
  #define DMB_RESET     102
  #define DMB_INT       127
  #define DMB_PWR_EN    86
  #define FEATURE_TDMB_SET_ANT_PATH
  #define DMB_ANT_SEL   42
#if (BOARD_REV > WS10)
  #define DMB_ANT_EAR_ACT   0
#else
  #define DMB_ANT_EAR_ACT   1
#endif
#if (BOARD_REV > WS20)
  #define FEATURE_TDMB_PMIC_TCXO_192M
  #define DMB_XO_SEL 19
#endif
  //#define FEATURE_TDMB_PMIC_POWER
  #define FEATURE_TDMB_GPIO_INIT
  #define FEATURE_TDMB_THREAD
#elif defined(CONFIG_EF35_BOARD)
  #define DMB_RESET     129
  #define DMB_INT       127
  #define DMB_PWR_EN    136
  #define DMB_I2C_SCL    73
  #define DMB_I2C_SDA    72
//#if EF35L_BDVER_L(WS20)
  #define FEATURE_TDMB_SET_ANT_PATH
  #define DMB_ANT_SEL   42
  #define DMB_ANT_EAR_ACT   1
//#endif
  //#define FEATURE_TDMB_PMIC_POWER
  #define FEATURE_TDMB_GPIO_INIT
#elif (defined(CONFIG_EF33_BOARD) || defined(CONFIG_EF34_BOARD))
  #define DMB_RESET     129
  #define DMB_INT       127
  #define DMB_PWR_EN    136
  #define DMB_I2C_SCL    73
  #define DMB_I2C_SDA    72
#if (EF33S_BDVER_L(WS20) || EF34K_BDVER_L(WS20)) // Since WS20 use only retractable Antenna
  #define FEATURE_TDMB_SET_ANT_PATH
  #define DMB_ANT_SEL   42
  #define DMB_ANT_EAR_ACT   1
#endif
  //#define FEATURE_TDMB_PMIC_POWER
  #define FEATURE_TDMB_GPIO_INIT
#elif defined(CONFIG_EF31S_32K_BOARD)
  #define DMB_RESET     26
  #define DMB_INT       30
  #define DMB_PWR_EN    106
  #define DMB_I2C_SDA   28
  #define DMB_I2C_SCL   29
  #define FEATURE_TDMB_PMIC_POWER
  //#define FEATURE_TDMB_TSIF_CLK_CTL
  #define FEATURE_TDMB_GPIO_INIT
#elif defined(CONFIG_EF18_BOARD)
  #define DMB_RESET     163
  #define DMB_INT       40
  #define DMB_PWR_EN    164
  #define DMB_ANT_SEL   172
  #define DMB_ANT_EAR_ACT   0
  #define FEATURE_TDMB_SET_ANT_PATH 
#elif defined(CONFIG_EF14_BOARD)
  #define DMB_RESET     155
  #define DMB_INT       152
  #define DMB_PWR_EN    151
  #define DMB_ANT_SEL   21
  #define DMB_ANT_EAR_ACT   1 
  #define FEATURE_TDMB_SET_ANT_PATH 
#elif defined(CONFIG_EF13_BOARD)
  #define DMB_RESET     87
  #define DMB_INT       84
  #define DMB_PWR_EN    85
  #define DMB_ANT_SEL   79
  #define DMB_ANT_EAR_ACT   0 // Earjack Low active
  #define FEATURE_TDMB_SET_ANT_PATH
#elif defined(CONFIG_EF12_BOARD)
  #define DMB_RESET     155
  #define DMB_INT       98
  //#define DMB_PWR_EN    57 // not used
  //#define DMB_ANT_SEL   58 // not used
  //#define DMB_ANT_EAR_ACT   1 // not used
  //#define FEATURE_TDMB_SET_ANT_PATH
  #define FEATURE_TDMB_PMIC_POWER
#elif defined(CONFIG_SP33_BOARD)
  #define DMB_RESET     23
  #define DMB_INT       18
  #define DMB_PWR_EN    26
  #define DMB_ANT_SEL   72
  #define DMB_ANT_EAR_ACT   1 // Earjack High active
  #define FEATURE_TDMB_SET_ANT_PATH
#elif defined(CONFIG_EF10_BOARD)
  #define DMB_RESET     155
  #define DMB_INT       98
  //#define DMB_PWR_EN    57 // not used
  #define DMB_ANT_SEL   58
  #define DMB_ANT_EAR_ACT   1 // Earjack active (1 : High, 0 : low)
  #define FEATURE_TDMB_SET_ANT_PATH
  #define FEATURE_TDMB_PMIC_POWER
#else
  #error
#endif


/*================================================================== */
/*================     TEST FEATURE                ================= */
/*================================================================== */

//#define FEATURE_TS_PKT_MSG // Single CH : 모든 패킷을 보여줌, Mulch CH : 읽은 데이터의 첫 패킷만 보여줌.
//#define FEATURE_TEST_ON_BOOT
//#define FEATURE_NETBER_TEST_ON_BOOT
//#define FEATURE_NETBER_TEST_ON_AIR
//#define FEATURE_APP_CALL_TEST_FUNC
//#define FEAUTRE_USE_FIXED_BER_DATA
//#define FEAUTRE_USE_FIXED_FIC_DATA
//#define FEATURE_GPIO_DEBUG
//#define FEATURE_COMMAND_TEST_ON_BOOT
//#define FEATURE_I2C_WRITE_CHECK
//#define FEATURE_I2C_DBG_MSG
//#define FEATURE_EBI_WRITE_CHECK
//#define FEATURE_HW_INPUT_MATCHING_TEST

#if (defined(FEATURE_TEST_ON_BOOT) && !defined(FEATURE_TDMB_THREAD)) // 부팅테스트시 쓰레드로 돌려 데이터 계속 읽음
#define FEATURE_TDMB_THREAD
#endif

#ifdef CONFIG_SKY_TDMB_INC_BB
#if (defined(FEATURE_TEST_ON_BOOT) || defined(FEATURE_NETBER_TEST_ON_BOOT) || defined(FEATURE_APP_CALL_TEST_FUNC))
#define INC_FICDEC_USE // 테스트시 채널정보들이 정확하지 않을때 필요
#define FEATURE_INC_FIC_UPDATE_MSG
#endif
#endif /* CONFIG_SKY_TDMB_INC_BB */


#endif /* _TDMB_MODELDEF_INCLUDES_H_ */