#ifndef SKY_SND_EXT_AMP_MAX97001_H
#define SKY_SND_EXT_AMP_MAX97001_H
/************************************************************************************************
**
**    AUDIO EXTERNAL AMP
**
**    FILE
**        Sky_snd_ext_amp_max97001.h
**
**    DESCRIPTION
**        This file contains audio external amp api
**
**    Copyright (c) 2010 by PANTECH Incorporated.  All Rights Reserved.
*************************************************************************************************/


/************************************************************************************************
** Includes
*************************************************************************************************/
#include <linux/kernel.h>

/************************************************************************************************
** FEATURE Definition
*************************************************************************************************/
#define FEATURE_SKY_SND_MISC_DEVICE
#define FEATURE_SKY_SND_AUDIO_TEST_COMMAND		// SangwonLee 110104

/************************************************************************************************
** Message Definition
*************************************************************************************************/
//#define MSG_LEVEL_LOW   1
//jhpark#define MSG_LEVEL_MED   2
//jhpark#define MSG_LEVEL_HIGH   4

#ifdef MSG_LEVEL_LOW
#define AMP_DBG_LOW(fmt, arg...) printk(KERN_INFO "%s: " fmt "\n", __func__, ## arg)
#else
#define AMP_DBG_LOW(fmt, arg...) do {} while (0)
#endif

#ifdef MSG_LEVEL_MED
#define AMP_DBG_MED(fmt, arg...) printk(KERN_INFO "%s: " fmt "\n", __func__, ## arg)
#else
#define AMP_DBG_MED(fmt, arg...) do {} while (0)
#endif

#ifdef MSG_LEVEL_HIGH
#define AMP_DBG_HIGH(fmt, arg...) printk(KERN_INFO "%s: " fmt "\n", __func__, ## arg)
#else
#define AMP_DBG_HIGH(fmt, arg...) do {} while (0)
#endif

#define AMP_ERR(fmt, arg...) printk(KERN_ERR "%s: " fmt "\n", __func__, ## arg)

/************************************************************************************************
** Definition
*************************************************************************************************/
#define MAX97001_SLAVE_ADDR	0x4D	


/* MAX97001 Control Registers */
#define INPUT_GAIN_CTL_REG_ADDR     		0x00
#define HEADPHONE_MIXERS_CTL_REG_ADDR     0x01	
#define SPEAKER_MIXER_CTL_REG_ADDR   		0x02	
#define HEADPHONE_LEFT_CTL_REG_ADDR   	0x03
#define HEADPHONE_RIGHT_CTL_REG_ADDR    	0x04
#define SPEAKER_CTL_REG_ADDR   			0x05
#define RESERVED_CTL_REG_ADDR    			0x06
#define LIMITER_CTL_REG_ADDR  				0x07
#define POWER_MANAGEMENT_CTL_REG_ADDR 	0x08
#define CHARGE_PUMP_CTL_REG_ADDR 		0x09
#define REV_ID_CTL_REG_ADDR				0x10


/* Input Register */
#define INADIFF_STEREO_SINGLE  0x00
#define INADIFF_DIFFERENTIAL  0x80
#define INBDIFF_STEREO_SINGLE  0x00
#define INBDIFF_DIFFERENTIAL  0x40
#define PGAINA_M_6_DB  0x00
#define PGAINA_M_3_DB  0x08
#define PGAINA_P_0_DB  0x10
#define PGAINA_P_3_DB  0x18
#define PGAINA_P_6_DB  0x20
#define PGAINA_P_9_DB  0x28
#define PGAINA_P_18_DB  0x30
#define PGAINA_EXT  0x038
#define PGAINB_M_6_DB  0x00
#define PGAINB_M_3_DB  0x01
#define PGAINB_P_0_DB  0x02
#define PGAINB_P_3_DB  0x03
#define PGAINB_P_6_DB  0x04
#define PGAINB_P_9_DB  0x05
#define PGAINB_P_18_DB  0x06
#define PGAINB_EXT  0x07


/* Mixers Registers - DATASHEET_FIX */
#define HPLMIX_NO_INPUT  0x00  //0x01
#define HPLMIX_INA1  0x10
#define HPLMIX_INA2  0x20
#define HPLMIX_INB1  0x40
#define HPLMIX_INB2  0x80
#define HPRMIX_NO_INPUT  0x00
#define HPRMIX_INA1  0x01
#define HPRMIX_INA2  0x02
#define HPRMIX_INB1  0x04
#define HPRMIX_INB2  0x08

#define SPKMIX_NO_INPUT  0x00  //0x02
#define SPKMIX_INA1  0x01
#define SPKMIX_INA2  0x02
#define SPKMIX_INB1  0x04
#define SPKMIX_INB2  0x08


/* Volume Control Register */
#define ZCD_EN  0x00
#define ZCD_DIS   0x80
#define SLEW_EN   0x00
#define SLEW_DIS  0x40
#define HPLM_UNMUTE  0x00
#define HPLM_MUTE    0x20
#define HPLVOL_M_64_DB     0x00
#define HPLVOL_M_60_DB     0x01
#define HPLVOL_M_56_DB     0x02
#define HPLVOL_M_52_DB     0x03
#define HPLVOL_M_48_DB     0x04
#define HPLVOL_M_44_DB     0x05
#define HPLVOL_M_40_DB     0x06
#define HPLVOL_M_37_DB     0x07
#define HPLVOL_M_34_DB     0x08
#define HPLVOL_M_31_DB     0x09
#define HPLVOL_M_28_DB     0x0A
#define HPLVOL_M_25_DB     0x0B
#define HPLVOL_M_22_DB     0x0C
#define HPLVOL_M_19_DB     0x0D
#define HPLVOL_M_16_DB     0x0E
#define HPLVOL_M_14_DB     0x0F
#define HPLVOL_M_12_DB     0x10
#define HPLVOL_M_10_DB     0x11
#define HPLVOL_M_8_DB       0x12
#define HPLVOL_M_6_DB       0x13
#define HPLVOL_M_4_DB       0x14
#define HPLVOL_M_2_DB       0x15
#define HPLVOL_M_1_DB       0x16
#define HPLVOL_P_0_DB       0x17
#define HPLVOL_P_1_DB       0x18
#define HPLVOL_P_2_DB       0x19
#define HPLVOL_P_3_DB       0x1A
#define HPLVOL_P_4_DB       0x1B
#define HPLVOL_P_4_DOT_5_DB     0x1C
#define HPLVOL_P_5_DB                  0x1D
#define HPLVOL_P_5_DOT_5_DB     0x1E
#define HPLVOL_P_6_DB                  0x1F

#define HPGAIN_P_0_DB  0x00
#define HPGAIN_P_3_DB  0x80
#define HPRM_UNMUTE  0x00
#define HPRM_MUTE  0x20
#define HPRVOL_M_64_DB     0x00
#define HPRVOL_M_60_DB     0x01
#define HPRVOL_M_56_DB     0x02
#define HPRVOL_M_52_DB     0x03
#define HPRVOL_M_48_DB     0x04
#define HPRVOL_M_44_DB     0x05
#define HPRVOL_M_40_DB     0x06
#define HPRVOL_M_37_DB     0x07
#define HPRVOL_M_34_DB     0x08
#define HPRVOL_M_31_DB     0x09
#define HPRVOL_M_28_DB     0x0A
#define HPRVOL_M_25_DB     0x0B
#define HPRVOL_M_22_DB     0x0C
#define HPRVOL_M_19_DB     0x0D
#define HPRVOL_M_16_DB     0x0E
#define HPRVOL_M_14_DB     0x0F
#define HPRVOL_M_12_DB     0x10
#define HPRVOL_M_10_DB     0x11
#define HPRVOL_M_8_DB       0x12
#define HPRVOL_M_6_DB       0x13
#define HPRVOL_M_4_DB       0x14
#define HPRVOL_M_2_DB       0x15
#define HPRVOL_M_1_DB       0x16
#define HPRVOL_P_0_DB       0x17
#define HPRVOL_P_1_DB       0x18
#define HPRVOL_P_2_DB       0x19
#define HPRVOL_P_3_DB       0x1A
#define HPRVOL_P_4_DB       0x1B
#define HPRVOL_P_4_DOT_5_DB     0x1C
#define HPRVOL_P_5_DB                  0x1D
#define HPRVOL_P_5_DOT_5_DB     0x1E
#define HPRVOL_P_6_DB                  0x1F
  
#define FFM_SS_MODE 0x00
#define FFM_FF_MODE 0x80
#define SPKM_UNMUTE  0x00
#define SPKM_MUTE  0x40
#define SPKVOL_M_30_DB     0x18
#define SPKVOL_M_26_DB     0x19
#define SPKVOL_M_22_DB     0x1A
#define SPKVOL_M_18_DB     0x1B
#define SPKVOL_M_14_DB     0x1C
#define SPKVOL_M_12_DB     0x1D
#define SPKVOL_M_10_DB     0x1E
#define SPKVOL_M_8_DB       0x1F
#define SPKVOL_M_6_DB       0x20
#define SPKVOL_M_4_DB       0x21
#define SPKVOL_M_2_DB       0x22
#define SPKVOL_P_0_DB       0x23
#define SPKVOL_P_1_DB       0x24
#define SPKVOL_P_2_DB       0x25
#define SPKVOL_P_3_DB       0x26
#define SPKVOL_P_4_DB       0x27
#define SPKVOL_P_5_DB       0x28
#define SPKVOL_P_6_DB       0x29
#define SPKVOL_P_7_DB       0x2A
#define SPKVOL_P_8_DB       0x2B
#define SPKVOL_P_9_DB       0x2C
#define SPKVOL_P_10_DB     0x2D
#define SPKVOL_P_11_DB     0x2E
#define SPKVOL_P_12_DB     0x2F
#define SPKVOL_P_12_DOT_5_DB      0x30
#define SPKVOL_P_13_DB                  0x31
#define SPKVOL_P_13_DOT_5_DB      0x32
#define SPKVOL_P_14_DB                  0x33
#define SPKVOL_P_14_DOT_5_DB     0x34
#define SPKVOL_P_15_DB                  0x35
#define SPKVOL_P_15_DOT_5_DB     0x36
#define SPKVOL_P_16_DB                  0x37
#define SPKVOL_P_16_DOT_5_DB     0x38
#define SPKVOL_P_17_DB                  0x39
#define SPKVOL_P_17_DOT_5_DB     0x3A
#define SPKVOL_P_18_DB                  0x3B
#define SPKVOL_P_18_DOT_5_DB     0x3C
#define SPKVOL_P_19_DB                  0x3D
#define SPKVOL_P_19_DOT_5_DB     0x3E
#define SPKVOL_P_20_DB                  0x3F


/* Distortion Limiter Register */
#define THDCLP_DIS 0x00
#define THDCLP_BELOW_4_PER  0x90
#define THDCLP_BELOW_5_PER  0xA0
#define THDCLP_BELOW_6_PER  0xB0
#define THDCLP_BELOW_8_PER  0xC0
#define THDCLP_BELOW_11_PER  0xD0
#define THDCLP_BELOW_12_PER  0xE0
#define THDCLP_BELOW_15_PER  0xF0
#define THDT1_1_DOT_4_SEC  0x00
#define THDT1_2_DOT_8_SEC  0x01


/* Power Management Register */
#define SHDN_EN  0x80
#define SHDN_DIS  0x0
#define LPMODE_DIS  0x00  
#define LPMODE_INA_SINGLE_END  0x20
#define LPMODE_INB_SINGLE_END  0x40
#define LPMODE_INA_DIFFERENTIAL  0x60
#define SPKEN_DIS  0x00
#define SPKEN_EN  0x10
#define HPLEN_DIS  0x00
#define HPLEN_EN  0x04
#define HPREN_DIS  0x00
#define HPREN_EN  0x02
#define BYPEN_OPEN  0x00
#define BYPEN_CLOSE  0x01


/* Charge Pump Control Register */
#define CPSEL_1 0x02  //+-0.9V on HPVDD/HPVSS  //0x09
#define CPSEL_0 0x00  //+-1.8V on HPVDD/HPVSS
#define F_SUPPLY_MODE 0x01  //Fixed Supply Mode
#define CLASS_H_MODE 0x00  //Class H Mode


#define SND_DEVICE_HANDSET_RX  0 //handset_rx
#define SND_DEVICE_HANDSET_TX  1 //handset_tx
#define SND_DEVICE_SPEAKER_RX  2 //speaker_stereo_rx
#define SND_DEVICE_SPEAKER_TX  3 //speaker_mono_tx
#define SND_DEVICE_HEADSET_RX  4 //headset_stereo_rx
#define SND_DEVICE_HEADSET_TX  5 //headset_mono_tx
#define SND_DEVICE_FMRADIO_HANDSET_RX 6 //fmradio_handset_rx
#define SND_DEVICE_FMRADIO_HEADSET_RX 7 //fmradio_headset_rx
#define SND_DEVICE_FMRADIO_SPEAKER_RX 8 //fmradio_speaker_rx
#define SND_DEVICE_DUALMIC_HANDSET_TX  9 //handset_dual_mic_endfire_tx
#define SND_DEVICE_DUALMIC_SPEAKER_TX  10  //speaker_dual_mic_endfire_tx
#define SND_DEVICE_TTY_HEADSET_MONO_RX  11 //tty_headset_mono_rx
#define SND_DEVICE_TTY_HEADSET_MONO_TX  12 //tty_headset_mono_tx
#define SND_DEVICE_SPEAKER_HEADSET_RX  13 //headset_stereo_speaker_stereo_rx
#define SND_DEVICE_FMRADIO_STEREO_TX  14
#define SND_DEVICE_HDMI_STERO_RX  15 //hdmi_stereo_rx
#define SND_DEVICE_BT_SCO_RX  17 //bt_sco_rx
#define SND_DEVICE_BT_SCO_TX  18 //bt_sco_tx
#define SND_SKY_DEVICE_VT_SPEAKER_RX  19 
#define SND_SKY_DEVICE_VT_SPEAKER_TX  20
#define SND_SKY_DEVICE_LOOPBACK_HEADSET_RX  21 
#define SND_SKY_DEVICE_MUTE_RX  22 
#define SND_SKY_DEVICE_VOICE_HEADSET_RX  23 
//20110126 frogLove - VoIP Rx/Tx device
#define SND_SKY_DEVICE_VOIP_HANDSET_RX  24 
#define SND_SKY_DEVICE_VOIP_HANDSET_TX  25
#define SND_SKY_DEVICE_VOIP_SPEAKER_RX  26 
#define SND_SKY_DEVICE_VOIP_SPEAKER_TX  27
#define SND_SKY_DEVICE_VOIP_HEADSET_RX  28 
#define SND_SKY_DEVICE_VOIP_HEADSET_TX  29

typedef enum 
{
  EXTAMP_OUT_SPK,
  EXTAMP_OUT_HPH_L,
  EXTAMP_OUT_HPH_R,
  EXTAMP_OUT_HPH_LR,
  EXTAMP_OUT_SPK_HPH,
  EXTAMP_OUT_BYPASS
}extamp_outmode_e;

typedef enum
{
  EXTAMP_IN_INA,
  EXTAMP_IN_INB,
  EXTAMP_IN_INAB,
  EXTAMP_IN_RXIN
}extamp_inmode_e;

typedef enum
{
  EXTAMP_MONO,
  EXTAMP_STEREO
}extamp_outfmt_e;

typedef struct
{
  u8	inp_gain_ctl_reg_val;
  u8	hp_mixers_ctl_reg_val;
  u8	sp_mixer_ctl_reg_val;
  u8	hpl_ctl_reg_val;
  u8	hpr_ctl_reg_val;
  u8 sp_ctl_reg_val;
  u8 limiter_ctl_reg_val;
  u8 power_man_ctl_reg_val; 
  u8 charge_pump_ctl_reg_val;
}extamp_info_t;

typedef enum
{
  MODE_NORMAL,
  MODE_RINGTONE,
  MODE_IN_CALL
}phone_state_e;

typedef enum
{
  MODE_DEFAULT,
  MODE_VT,
  MODE_VOIP,
  MODE_TDMB,
  MODE_AUTOANSWER,
}call_mode_e;

typedef struct
{
    call_mode_e mode;
    int     param;
}call_mode_param;

/************************************************************************************************
** Variables
*************************************************************************************************/

/************************************************************************************************
** Declaration
*************************************************************************************************/
extern void snd_extamp_api_Init(void);
extern void snd_extamp_api_DeInit(void);
extern void snd_extamp_api_SetDevice(int on, uint32_t cad_device);
extern void snd_extamp_api_SetVolume(uint32_t hp_vol, uint32_t sp_vol);
extern void snd_extamp_api_SetPreAmpGain(uint32_t gain);
extern void snd_extamp_api_HeadsetConnected(uint32_t connect);
extern int snd_extamp_get_current_callmode(void);
extern int snd_extamp_get_mode(void);
extern void snd_extamp_api_reset(void);
#ifdef FEATURE_SKY_SND_MISC_DEVICE
/*static*/ int snd_misc_device_init(void);
/*static*/ int snd_misc_device_deinit(void);
/*static*/ int ext_amp_open(struct inode *inode, struct file *file);
/*static*/ int ext_amp_release(struct inode *inode, struct file *file);
/*static*/ long ext_amp_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
#endif

#ifdef FEATURE_SKY_SND_AUDIO_TEST_COMMAND
/*static*/ int snd_extamp_read_single_reg(int reg);
/*static*/ void snd_extamp_write_single_reg(int reg, int regval);
#endif
#endif /*SKY_SND_EXT_AMP_MAX97001_H*/

