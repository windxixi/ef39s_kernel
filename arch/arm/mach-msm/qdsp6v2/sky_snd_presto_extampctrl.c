/************************************************************************************************
**
**    AUDIO EXTERNAL AMP CONTROLLER
**
**    FILE
**        extampctrl.c
**
**    DESCRIPTION
**        This file contains audio external amp api//
**          
**          void snd_extamp_api_Init()//
**          void snd_extamp_api_SetPath()//
**          void snd_extamp_api_SetVolume()//
**          void snd_extamp_api_Sleep()//
**
**    Copyright (c) 2010 by PANTECH Incorporated.  All Rights Reserved.
*************************************************************************************************/

#include "sky_snd_presto_extampctrl.h"
#include <asm/ioctls.h>
#include <linux/uaccess.h>

//extern void snd_extamp_api_SetVolume_ById(uint32_t extamp_dev_id);

#include "snd_presto_sub_fab2200.h"
#include <mach/gpio.h>

#ifdef FEATURE_SKY_SND_EXTAMPCTRL
uint32_t extamp_current_dev_id = 0;
//extern bool LockForTest;

/*********************************************************************************
**********************        DEVELOPER SECTION        ***************************
**********************************************************************************
**
** COMMENT : Customizing for Each Project.
**
*********************************************************************************/
struct extamp_device_info extamp_devices[] = {
  {
	.extamp_dev_id = EXTAMP_SPKR_PHONE,
	.pga_info = 0x01,  // 20110706 jmlee 3dB
	.hp_ramp_speed = 0x00,
	.hp_ramp_en = 0x00,
	.hpl_vol_info = 0x00,
	.hpr_vol_info = 0x00,
	.hp_ng_zra_info = 0x00,
	.hp_ng_thresh_info = 0x00,
	.hp_ng_zrr_info = 0x00,
	.hp_ng_time_info = 0x00,
	.erc_info = 0x01,
	.spk_ramp_speed = 0x00,
	.spk_ramp_en = 0x01,
	.spk_ng_zra_info = 0x00,
	.spk_ng_thresh_info = 0x00,
	.spk_ng_zrr_info = 0x00,
	.spk_ng_time_info = 0x00,
//	.spk_ng_zra_info = 0x01,
//	.spk_ng_thresh_info = 0x01,
//	.spk_ng_zrr_info = 0x01,
//	.spk_ng_time_info = 0x03,
	.thresh_max_info = 0x00,
	.thresh_batt_info = 0x00,
	.agc_attack_info = 0x00,
	.agc_release_info = 0x00,
	.agc_min_info = 0x00,
	.start_gain_info = 0x3b, // 20110629 jmlee --> 20110706 4.0dB
	.mc_ssmt_info = 0x03,
	.ssmt_info = 0x03,
	.rx_voice_vol_0 = 0x27,
	.rx_voice_vol_1 = 0x2A,
	.rx_voice_vol_2 = 0x2D,
	.rx_voice_vol_3 = 0x30,
	.rx_voice_vol_4 = 0x33,
	.rx_voice_vol_5 = 0x36,	
  },
  {
    .extamp_dev_id = EXTAMP_HEADPHONE_STEREO,
	.pga_info = 0x01,  // 20110714 jmlee 3dB
	.hp_ramp_speed = 0x00,
	.hp_ramp_en = 0x01,
	.hpl_vol_info = 0x13, // 20110818  jmlee h/w test -2dB -> -6dB(20110901, MIA)
	.hpr_vol_info = 0x13, // 20110818  jmlee h/w test -2dB -> -6dB(20110901, MIA)
	.hp_ng_zra_info = 0x00,
	.hp_ng_thresh_info = 0x00,
	.hp_ng_zrr_info = 0x0,
	.hp_ng_time_info = 0x00,
//	.hp_ng_zra_info = 0x01,
//	.hp_ng_thresh_info = 0x01,
//	.hp_ng_zrr_info = 0x1,
//	.hp_ng_time_info = 0x03,
	.erc_info = 0x00,
	.spk_ramp_speed = 0x00,
	.spk_ramp_en = 0x00,
	.spk_ng_zra_info = 0x00,
	.spk_ng_thresh_info = 0x00,
	.spk_ng_zrr_info = 0x00,
	.spk_ng_time_info = 0x00,
	.thresh_max_info = 0x00,
	.thresh_batt_info = 0x00,
	.agc_attack_info = 0x00,
	.agc_release_info = 0x00,
	.agc_min_info = 0x00,
	.start_gain_info = 0x00,
	.mc_ssmt_info = 0x00,
	.ssmt_info = 0x00,
	.rx_voice_vol_0 = 0x1a,
	.rx_voice_vol_1 = 0x1d,
	.rx_voice_vol_2 = 0x20,
	.rx_voice_vol_3 = 0x23,
	.rx_voice_vol_4 = 0x26,
	.rx_voice_vol_5 = 0x29,
  },

  {
    .extamp_dev_id = EXTAMP_SPKR_PHONE_AND_HP_STEREO,
	.pga_info = 0x00,
	.hp_ramp_speed = 0x00,
	.hp_ramp_en = 0x01,
	.hpl_vol_info = 0x0c/*0x18*/,  // 20110811 jmlee for test
	.hpr_vol_info = 0x0c/*0x18*/,  // 20110811 jmlee for test
	.hp_ng_zra_info = 0x00,
	.hp_ng_thresh_info = 0x00,
	.hp_ng_zrr_info = 0x0,
	.hp_ng_time_info = 0x00,
//	.hp_ng_zra_info = 0x01,
//	.hp_ng_thresh_info = 0x01,
//	.hp_ng_zrr_info = 0x1,
//	.hp_ng_time_info = 0x03,
	.erc_info = 0x01,
	.spk_ramp_speed = 0x00,
	.spk_ramp_en = 0x01,
	.spk_ng_zra_info = 0x00,
	.spk_ng_thresh_info = 0x00,
	.spk_ng_zrr_info = 0x00,
	.spk_ng_time_info = 0x00,
//	.spk_ng_zra_info = 0x01,
//	.spk_ng_thresh_info = 0x01,
//	.spk_ng_zrr_info = 0x01,
//	.spk_ng_time_info = 0x03,
	.thresh_max_info = 0x00,
	.thresh_batt_info = 0x00,
	.agc_attack_info = 0x00,
	.agc_release_info = 0x00,
	.agc_min_info = 0x00,
	.start_gain_info = 0x3c,
	.mc_ssmt_info = 0x03,
	.ssmt_info = 0x03,
	.rx_voice_vol_0 = 0x1E,
	.rx_voice_vol_1 = 0x23,
	.rx_voice_vol_2 = 0x28,
	.rx_voice_vol_3 = 0x2D,
	.rx_voice_vol_4 = 0x32,
	.rx_voice_vol_5 = 0x37,
  },
 #ifdef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020
 {
	.extamp_dev_id = EXTAMP_NR_ON_FT,
	.pga_info = 0x03, // 20110810 jmlee h/w test bin 9dB     0x02, //20110802 jmlee PGA("pga_info") 6dB
	.hp_ramp_speed = 0x00,
	.hp_ramp_en = 0x00,
	.hpl_vol_info = 0x00,
	.hpr_vol_info = 0x00,
	.hp_ng_zra_info = 0x00,
	.hp_ng_thresh_info = 0x00,
	.hp_ng_zrr_info = 0x00,
	.hp_ng_time_info = 0x00,
	.erc_info = 0x01,
	.spk_ramp_speed = 0x00,
	.spk_ramp_en = 0x01,
	.spk_ng_zra_info = 0x00,
	.spk_ng_thresh_info = 0x00,
	.spk_ng_zrr_info = 0x00,
	.spk_ng_time_info = 0x00,
//	.spk_ng_zra_info = 0x01,
//	.spk_ng_thresh_info = 0x01,
//	.spk_ng_zrr_info = 0x01,
//	.spk_ng_time_info = 0x03,
	.thresh_max_info = 0x00,
	.thresh_batt_info = 0x00,
	.agc_attack_info = 0x00,
	.agc_release_info = 0x00,
	.agc_min_info = 0x00,
	.start_gain_info = 0x3c,  // 20110802 jmlee  4.5dB  ( -25dB ~ 6dB)
	.mc_ssmt_info = 0x03,
	.ssmt_info = 0x03,
	.rx_voice_vol_0 = 0x27,
	.rx_voice_vol_1 = 0x2A,
	.rx_voice_vol_2 = 0x2D,
	.rx_voice_vol_3 = 0x30,
	.rx_voice_vol_4 = 0x35,
	.rx_voice_vol_5 = 0x3a,
  },
  {
	.extamp_dev_id = EXTAMP_NR_OFF_FT,
	.pga_info = 0x03, // 20110810  jmlee h/w test bin 9dB     0x02, //20110802 jmlee PGA("pga_info") 6dB
	.hp_ramp_speed = 0x00,
	.hp_ramp_en = 0x00,
	.hpl_vol_info = 0x00,
	.hpr_vol_info = 0x00,
	.hp_ng_zra_info = 0x00,
	.hp_ng_thresh_info = 0x00,
	.hp_ng_zrr_info = 0x00,
	.hp_ng_time_info = 0x00,
	.erc_info = 0x01,
	.spk_ramp_speed = 0x00,
	.spk_ramp_en = 0x01,
	.spk_ng_zra_info = 0x01,
	.spk_ng_thresh_info = 0x01,
	.spk_ng_zrr_info = 0x01,
	.spk_ng_time_info = 0x03,
	.thresh_max_info = 0x00,
	.thresh_batt_info = 0x00,
	.agc_attack_info = 0x00,
	.agc_release_info = 0x00,
	.agc_min_info = 0x00,
	.start_gain_info = 0x3c,   // 20110802 jmlee  4.5dB  ( -25dB ~ 6dB)
	.mc_ssmt_info = 0x03,
	.ssmt_info = 0x03,
	.rx_voice_vol_0 = 0x22,
	.rx_voice_vol_1 = 0x25,
	.rx_voice_vol_2 = 0x28,
	.rx_voice_vol_3 = 0x2b,
	.rx_voice_vol_4 = 0x2e,
	.rx_voice_vol_5 = 0x31,
  },
  {
	.extamp_dev_id = EXTAMP_NR_ON_DV,
	.pga_info = 0x03, // 20110810  jmlee h/w test bin 9dB     0x02, //20110802 jmlee PGA("pga_info") 6dB
	.hp_ramp_speed = 0x00,
	.hp_ramp_en = 0x00,
	.hpl_vol_info = 0x00,
	.hpr_vol_info = 0x00,
	.hp_ng_zra_info = 0x00,
	.hp_ng_thresh_info = 0x00,
	.hp_ng_zrr_info = 0x00,
	.hp_ng_time_info = 0x00,
	.erc_info = 0x01,
	.spk_ramp_speed = 0x00,
	.spk_ramp_en = 0x01,
	.spk_ng_zra_info = 0x00,
	.spk_ng_thresh_info = 0x00,
	.spk_ng_zrr_info = 0x00,
	.spk_ng_time_info = 0x00,
//	.spk_ng_zra_info = 0x01,
//	.spk_ng_thresh_info = 0x01,
//	.spk_ng_zrr_info = 0x01,
//	.spk_ng_time_info = 0x03,
	.thresh_max_info = 0x00,
	.thresh_batt_info = 0x00,
	.agc_attack_info = 0x00,
	.agc_release_info = 0x00,
	.agc_min_info = 0x00,
	.start_gain_info = 0x3c,   // 20110802 jmlee  4.5dB  ( -25dB ~ 6dB)
	.mc_ssmt_info = 0x03,
	.ssmt_info = 0x03,
	.rx_voice_vol_0 = 0x27,
	.rx_voice_vol_1 = 0x2A,
	.rx_voice_vol_2 = 0x2D,
	.rx_voice_vol_3 = 0x30,
	.rx_voice_vol_4 = 0x35,
	.rx_voice_vol_5 = 0x3a,
  },
  {
	.extamp_dev_id = EXTAMP_NR_OFF_DV,
	.pga_info = 0x03, // 20110810 jmlee h/w test bin 9dB     0x02, //20110802 jmlee PGA("pga_info") 6dB
	.hp_ramp_speed = 0x00,
	.hp_ramp_en = 0x00,
	.hpl_vol_info = 0x00,
	.hpr_vol_info = 0x00,
	.hp_ng_zra_info = 0x00,
	.hp_ng_thresh_info = 0x00,
	.hp_ng_zrr_info = 0x00,
	.hp_ng_time_info = 0x00,
	.erc_info = 0x01,
	.spk_ramp_speed = 0x00,
	.spk_ramp_en = 0x01,
	.spk_ng_zra_info = 0x01,
	.spk_ng_thresh_info = 0x01,
	.spk_ng_zrr_info = 0x01,
	.spk_ng_time_info = 0x03,
	.thresh_max_info = 0x00,
	.thresh_batt_info = 0x00,
	.agc_attack_info = 0x00,
	.agc_release_info = 0x00,
	.agc_min_info = 0x00,
	.start_gain_info = 0x3c,   // 20110802 jmlee  4.5dB  ( -25dB ~ 6dB)
	.mc_ssmt_info = 0x03,
	.ssmt_info = 0x03,
	.rx_voice_vol_0 = 0x22,
	.rx_voice_vol_1 = 0x25,
	.rx_voice_vol_2 = 0x28,
	.rx_voice_vol_3 = 0x2b,
	.rx_voice_vol_4 = 0x2e,
	.rx_voice_vol_5 = 0x31,
  },
#endif
  {
  	.extamp_dev_id = EXTAMP_TTY_HEADPHONE,
	.pga_info = 0x00,
	.hp_ramp_speed = 0x00,
	.hp_ramp_en = 0x00,
	.hpl_vol_info = 0x19,
	.hpr_vol_info = 0x19,
	.hp_ng_zra_info = 0x00,
	.hp_ng_thresh_info = 0x00,
	.hp_ng_zrr_info = 0x00,
	.hp_ng_time_info = 0x00,
	.erc_info = 0x00,
	.spk_ramp_speed = 0x00,
	.spk_ramp_en = 0x00,
	.spk_ng_zra_info = 0x00,
	.spk_ng_thresh_info = 0x00,
	.spk_ng_zrr_info = 0x00,
	.spk_ng_time_info = 0x00,
	.thresh_max_info = 0x00,
	.thresh_batt_info = 0x00,
	.agc_attack_info = 0x00,
	.agc_release_info = 0x00,
	.agc_min_info = 0x00,
	.start_gain_info = 0x00,
	.mc_ssmt_info = 0x00,
	.ssmt_info = 0x00,
	.rx_voice_vol_0 = 0x0,
	.rx_voice_vol_1 = 0x0,
	.rx_voice_vol_2 = 0x0,
	.rx_voice_vol_3 = 0x0,
	.rx_voice_vol_4 = 0x0,
	.rx_voice_vol_5 = 0x0,
  },

#if 1 //def FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP
/* The Table for Setting the Amp Gain */
  {
	.pga_info = 0x00,
	.hp_ramp_speed = 0x00,
	.hp_ramp_en = 0x00,
	.hpl_vol_info = 0x0,
	.hpr_vol_info = 0x0,
	.hp_ng_zra_info = 0x00,
	.hp_ng_thresh_info = 0x00,
	.hp_ng_zrr_info = 0x00,
	.hp_ng_time_info = 0x00,
	.erc_info = 0x00,
	.spk_ramp_speed = 0x00,
	.spk_ramp_en = 0x00,
	.spk_ng_zra_info = 0x00,
	.spk_ng_thresh_info = 0x00,
	.spk_ng_zrr_info = 0x00,
	.spk_ng_time_info = 0x00,
	.thresh_max_info = 0x00,
	.thresh_batt_info = 0x00,
	.agc_attack_info = 0x00,
	.agc_release_info = 0x00,
	.agc_min_info = 0x00,
	.start_gain_info = 0x00,
	.mc_ssmt_info = 0x00,
	.ssmt_info = 0x00,
	.rx_voice_vol_0 = 0x0,
	.rx_voice_vol_1 = 0x0,
	.rx_voice_vol_2 = 0x0,
	.rx_voice_vol_3 = 0x0,
	.rx_voice_vol_4 = 0x0,
	.rx_voice_vol_5 = 0x0,
  },
#endif /* FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP */
};

#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP
struct bitmask reg_bit_mask[] = {
/* pga_info */
  [0] = {
    .min = 0x0,
    .bitmask = 0x07,
  },
/* hp_ramp_speed */
  [1] = {
    .min = 0x0,
    .bitmask = 0x03,
  },  
/* hp_ramp_en */
  [2] = {
    .min = 0x0,
    .bitmask = 0x01,
  },  
/* hpl_vol_info */
  [3] = {
    .min = 0x0,
    .bitmask = 0x1f,
  },
/* hpr_vol_info */
  [4] = {
    .min = 0x0,
    .bitmask = 0x1f,
  },
/* hp_ng_zra_info */
  [5] = {
    .min = 0x0,
    .bitmask = 0x01,
  },
/* hp_ng_thresh_info */
  [6] = {
    .min = 0x0,
    .bitmask = 0x07,
  },
/* hp_ng_zrr_info */
  [7] = {
    .min = 0x0,
    .bitmask = 0x01,
  },
/* hp_ng_time_info */
  [8] = {
    .min = 0x0,
    .bitmask = 0x07,
  },
/* erc_info */
  [9] = {
    .min = 0x0,
    .bitmask = 0x01,
  },
/* spk_ramp_speed */
  [10] = {
    .min = 0x0,
    .bitmask = 0x03,
  },  
/* spk_ramp_en */
  [11] = {
    .min = 0x0,
    .bitmask = 0x01,
  },  
/* spk_ng_zra_info */
  [12] = {
    .min = 0x0,
    .bitmask = 0x01,
  },
/* spk_ng_thresh_info */
  [13] = {
    .min = 0x0,
    .bitmask = 0x07,
  },
/* spk_ng_zrr_info */
  [14] = {
    .min = 0x0,
    .bitmask = 0x01,
  },
/* spk_ng_time_info */
  [15] = {
    .min = 0x0,
    .bitmask = 0x07,
  },
/* thresh_max_info */
  [16] = {
    .min = 0x0,
    .bitmask = 0x0f,
  },
/* thresh_batt_info */
  [17] = {
    .min = 0x0,
    .bitmask = 0x0f,
  },
/* agc_attack_info */
  [18] = {
    .min = 0x0,
    .bitmask = 0x07,
  },
/* agc_release_info */
  [19] = {
    .min = 0x0,
    .bitmask = 0x07,
  },
/* agc_min_info */
  [20] = {
    .min = 0x0,
    .bitmask = 0x3f,
  },
/* start_gain_info */
  [21] = {
    .min = 0x0,
    .bitmask = 0x3f,
  },
/* mc_ssmt_info */
  [22] = {
    .min = 0x0,
    .bitmask = 0x07,
  },
/* ssmt_info */
  [23] = {
    .min = 0x0,
    .bitmask = 0x07,
  },  
/* rx_voice_vol_0 */
  [24] = {
    .min = 0x0,
    .bitmask = 0x3f,
  },
/* rx_voice_vol_1 */
  [25] = {
    .min = 0x0,
    .bitmask = 0x3f,
  },
/* rx_voice_vol_2 */
  [26] = {
    .min = 0x0,
    .bitmask = 0x3f,
  },
/* rx_voice_vol_3 */
  [27] = {
    .min = 0x0,
    .bitmask = 0x3f,
  },
/* rx_voice_vol_4 */
  [28] = {
    .min = 0x0,
    .bitmask = 0x3f,
  },
/* rx_voice_vol_5 */
  [29] = {
    .min = 0x0,
    .bitmask = 0x3f,
  },
};

static char *DebugDeviceId(uint32_t DevId) {
  switch (DevId) {
    case EXTAMP_SPKR_PHONE                   : return "SPKR_PHONE";
    case EXTAMP_HEADPHONE_STEREO   : return "HEADPHONE_STEREO";
    case EXTAMP_SPKR_PHONE_AND_HP_STEREO         : return "SPKR_PHONE_AND_HP_STEREO";
 #ifdef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020
    case EXTAMP_NR_ON_FT:		return "NR_ON_FT";	
    case EXTAMP_NR_OFF_FT:		return "NR_OFF_FT";	
    case EXTAMP_NR_ON_DV:		return "NR_ON_DV";	
    case EXTAMP_NR_OFF_DV:		return "NR_OFF_DV";	
#endif
    case EXTAMP_TTY_HEADPHONE:	 return "TTY_HEADPHONE";
    case EXTAMP_TEST_ID                   : return "EXTAMP_TEST_ID";
    default                               : return "Bad Dev ID";
  }
}

static char *DebugStrginRegister(uint32_t DevId) {
  switch (DevId) {
    case 1   : return "PGAGainXX(0x02)";
    case 2   : return "HP_Ramp_Speed(0x03)";
    case 3   : return "HP_Ramp_En(0x03)";	
    case 4   : return "HPVol_Left(0x05)";
    case 5   : return "HPVol_Right(0x06)";
    case 6   : return "HP_NG_ZRA(0x07)";
    case 7   : return "HP_NG_THRESH(0x07)";
    case 8   : return "HP_NG_ZRR(0x07)";
    case 9   : return "HP_NG_TIME(0x07)";
    case 10   : return "erc_info(0x08)";
    case 11   : return "SP_Ramp_Speed(0x08)";
    case 12   : return "SP_Ramp_En(0x08)";	
    case 13   : return "SP_NG_ZRA(0x09)";
    case 14   : return "SP_NG_THRESH(0x09)";
    case 15   : return "SP_NG_ZRR(0x09)";
    case 16   : return "SP_NG_TIME(0x09)";
    case 17   : return "Threshold_MAX(0x0A)";
    case 18   : return "Threshold_BATT(0x0A)";
    case 19   : return "AGC_Attack(0x0B)";
    case 20   : return "AGC_Release(0x0B)";
    case 21   : return "AGC_Min(0x0C)";
    case 22   : return "Start_Gain(0x0E)";
    case 23   : return "MC_SSMT(0x0F)";
    case 24   : return "SSMT(0x0F)";
    case 25   : return "Rx_Voice_Vol_0(Call)";
    case 26   : return "Rx_Voice_Vol_1(Call)";
    case 27   : return "Rx_Voice_Vol_2(Call)";
    case 28   : return "Rx_Voice_Vol_3(Call)";
    case 29   : return "Rx_Voice_Vol_4(Call)";
    case 30   : return "Rx_Voice_Vol_5(Call)";
    default  : return "Bad";
  }
}
#endif //FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP



/*********************************************************************************
********************       COMMON MODULE SECTION        **************************
**********************************************************************************
**
** COMMENT : Common module for the Control.
**
*********************************************************************************/
struct extamp_device_info *extamp_lookup_device(uint32_t extamp_dev_id) {
	struct extamp_device_info *di = extamp_devices;

	for (;;) {
		if (di->extamp_dev_id == extamp_dev_id)
			return di;
		if (di->extamp_dev_id == 0) {
			return di;
		}
		di++;
	}
}

/*static*/ uint32_t snd_extamp_api_GetGainTable(uint32_t DeviceId, uint32_t Register) {
  uint32_t i, *p_reg;
	struct extamp_device_info *di;
  
	di = extamp_lookup_device(DeviceId);
	p_reg = (uint32_t *)di + 1;
	for (i=0; i<sizeof(struct extamp_device_info);i++) {
    if(i == Register) {
      return *p_reg;
    }
    p_reg++;
	}

	return 0;
}

/*static*/ void snd_extamp_api_SetGainTableForTest(uint32_t Register, uint32_t value) {
  uint32_t i, *p_reg;
	struct extamp_device_info *di;

	di = extamp_lookup_device(extamp_current_dev_id);
	p_reg = (uint32_t *)di + 1;
	for (i=0; i<sizeof(struct extamp_device_info);i++) {
    if(i == Register) {
      *p_reg = value;
      return;
    }
    p_reg++;
	}

  return;
}

#if 1   //20110321 jhsong
void snd_extamp_api_call_fab_path(void)
{
	int fab_path = 0;
	int fab_set = 0;

	fab_set = gpio_get_value(GPIO_SUBSYSTEM_ENABLE);
	
	if(fab_set){
		fab_path = snd_subsystem_get_device();

		fab2200_set_default_data();
		
		switch(fab_path){
			case DEVICE_SPEAKER_RX:
				snd_subsystem_sp_poweron();
				break;
			case DEVICE_HEADSET_RX:
				snd_subsystem_hp_poweron();
				break;
			case DEVICE_TTY_HEADSET_MONO_RX:
				snd_subsystem_tty_hp_poweron();
				break;
			case DEVICE_SPEAKER_HEADSET_RX:
				snd_subsystem_sp_hp_poweron();
				break;
 #ifdef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020
			case DEVICE_SND_DEVICE_NR_FT_RX:
				snd_subsystem_nr_ft_poweron();
				break;
			case DEVICE_SND_DEVICE_NR_OFF_FT_RX:
				snd_subsystem_nr_off_ft_poweron();
				break;
			case DEVICE_SND_DEVICE_NR_DV_RX:
				snd_subsystem_nr_dv_poweron();
				break;
			case DEVICE_SND_DEVICE_NR_OFF_DV_RX:
				snd_subsystem_nr_off_dv_poweron();
				break;
#endif
			default:
				break;
		}
	}
}
#endif

#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP
void snd_extamp_api_AudioCmd_ForInt(unsigned int cmd, unsigned long arg) {
	uint32_t buffer[3] = {0};
  
  if(copy_from_user(&buffer, (void *)arg, sizeof(buffer))) {
    printk("[Audio] Err: snd_extamp_api_AudioCmd_ForInt()\n");
    return;
  }
  printk("[Audio] _AudioCmd_ForInt() buffer[] 0x%x 0x%x 0x%x\n"
                          , buffer[0], buffer[1], buffer[2]);
  
  switch (buffer[0]) {
    /* case 0 : Reserved */
    case 1:
      /* buffer[0](cmd)    : Command of 'Get number of Table Colomn' */
      /* buffer[1](value1) : -                                       */
      /* buffer[2](value2) : -                                       */
      buffer[0] = sizeof(extamp_devices)/sizeof(struct extamp_device_info); 
      break;
    case 2:
      /* buffer[0](cmd)    : Command of 'Get a Gain from Table' */
      /* buffer[1](value1) : Device ID */
      /* buffer[2](value2) : Register */
      buffer[0] = snd_extamp_api_GetGainTable(buffer[1], buffer[2]);
	extamp_current_dev_id = buffer[1];
      break;
    case 3:
      /* buffer[0](cmd)    : Command of 'Set a Gain to Test Table'    */
      /* buffer[1](value1) : Register ID */
      /* buffer[2](value2) : Value */
      snd_extamp_api_SetGainTableForTest(buffer[1], buffer[2]);
      break;
    case 4:
      /* buffer[0](cmd)    : Command of 'Lock after setting the Test Table to Amp' */
      /* buffer[1](value1) : Lock Enable/Disable                      */
      /* buffer[2](value2) : -                                        */
      if (buffer[1] == 1) {
	  printk("[Audio] EXTAMP_TEST_ID.........lock\n");
//     	  snd_extamp_api_SetVolume_ById(EXTAMP_TEST_ID);
     	  LockForTest = 1;
      }
      else {
        LockForTest = 0;
      }
	  snd_extamp_api_call_fab_path();
      break;
    case 5:
      /* buffer[0](cmd)    : Command of 'Get a number of Register'    */
      /* buffer[1](value1) : Register ID */
      /* buffer[2](value2) : - */
      buffer[0] = sizeof(struct extamp_device_info)/sizeof(uint32_t)-1;  /* -1 : except Device ID */
      break;
    case 6:
      /* buffer[0](cmd)    : Command of 'Get a min of Register'    */
      /* buffer[1](value1) : Register ID */
      /* buffer[2](value2) : - */
      buffer[0] = reg_bit_mask[buffer[1]].min;
      break;
    case 7:
      /* buffer[0](cmd)    : Command of 'Get a bitmask of Register'    */
      /* buffer[1](value1) : Register ID */
      /* buffer[2](value2) : - */
      buffer[0] = reg_bit_mask[buffer[1]].bitmask;
      break;
    default:
      buffer[0] = 0;
      break;
  }
  printk("[Audio] _AudioCmd_ForInt() return buffer[] 0x%x\n", buffer[0]);
  
	if (copy_to_user((void*) arg, &buffer[0], sizeof(int))) {
		printk("[Audio] _AudioCmd_ForInt() ERROR!!!\n");
	}
}

void snd_extamp_api_AudioCmd_ForString(unsigned int cmd, unsigned long arg) {
  char str[256];
	uint32_t buffer[3] = {0};

  if(copy_from_user(str, (void *)arg, sizeof(str))) {
    printk("[Audio] Err:aud_sub_ioctl cmd SND_SKY_SET_MODE_PARAM\n");
//    return 0;
  }

  buffer[0] = (uint32_t)str[0];
  buffer[1] = (uint32_t)str[1];
  buffer[2] = (uint32_t)str[2];
  
  printk("[Audio] _AudioCmd_ForString() buffer[] 0x%x 0x%x 0x%x\n"
                          , buffer[0], buffer[1], buffer[2]);
                          
  switch (buffer[0]) {
    /* case 0 : Reserved */
    case 1:
      /* buffer[0](cmd)    : Command of 'Get a string of Device Id' */
      /* buffer[1](value1) : Device ID */
      /* buffer[2](value2) : - */
      strcpy(str, DebugDeviceId((uint32_t)buffer[1]));
      break;
    case 2:
      /* buffer[0](cmd)    : Command of 'Get a string of Register' */
      /* buffer[1](value1) : Device ID */
      /* buffer[2](value2) : - */
      strcpy(str, DebugStrginRegister((uint32_t)buffer[1]));
      break;
    default:
      break;
  }

  printk("[Audio] _AudioCmd_ForString() return buffer[] 0x%s\n", str);
  
	if (copy_to_user((void*) arg, str, sizeof(str))) {
		printk("[Audio] aud_sub_ioctl cmd SND_SKY_GET_MODE_PARAM ERROR!!!\n");
	}
}
#endif /* FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP */
#endif /* FEATURE_SKY_SND_EXTAMPCTRL */


