#ifndef SNDSUBSYSTEM_H
#define SNDSUBSYSTEM_H
/************************************************************************************************
**
**    FAB AUDIO EXTERNAL AMP(SUBSYSTEM)
**
**    FILE
**        snd_sub_fab2200.h
**
**    DESCRIPTION
**        This file contains FAB Audio Subsystem api
**
**    Copyright (c) 2011 by Song Jinhwan <song.jinhwan@pantech.com>
*************************************************************************************************/


/************************************************************************************************
** Includes
*************************************************************************************************/
#include <linux/kernel.h>
/************************************************************************************************
** Definition
*************************************************************************************************/

//#define TPA2051D3_SLAVE_ADDR	  	0x70	//b'1110000 (TI SUBSYSTEM)
#define FAB2200_SLAVE_ADDR	  	0x4D	//b'1001101 (FAB SUBSYSTEM)

#define GPIO_SUBSYSTEM_ENABLE		130


//MSM7x27 Audio Tx/Rx device
#define DEVICE_HANDSET_RX					0	// handset_rx
#define DEVICE_HANDSET_TX					1	//handset_tx
#define DEVICE_SPEAKER_RX					2	//speaker_stereo_rx
#define DEVICE_SPEAKER_TX					3	//speaker_mono_tx
#define DEVICE_HEADSET_RX					4	//headset_stereo_rx
#define DEVICE_HEADSET_TX					5	//headset_mono_tx
#define DEVICE_FMRADIO_HANDSET_RX		6	//fmradio_handset_rx
#define DEVICE_FMRADIO_HEADSET_RX		7	//fmradio_headset_rx
#define DEVICE_FMRADIO_SPEAKER_RX			8	//fmradio_speaker_rx
#define DEVICE_DUALMIC_HANDSET_TX		9	//handset_dual_mic_endfire_tx
#define DEVICE_DUALMIC_SPEAKER_TX			10	//speaker_dual_mic_endfire_tx
#define DEVICE_TTY_HEADSET_MONO_RX		11	//tty_headset_mono_rx
#define DEVICE_TTY_HEADSET_MONO_TX		12	//tty_headset_mono_tx
#define DEVICE_SPEAKER_HEADSET_RX			13	//headset_stereo_speaker_stereo_rx
#define DEVICE_BT_SCO_RX					17	//bt_sco_rx
#define DEVICE_BT_SCO_TX					18	//bt_sco_tx

#ifdef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020 // jmlee
#define DEVICE_SND_DEVICE_NR_CT_TX		20  // Close talk TX
#define DEVICE_SND_DEVICE_NR_CT_RX		21  // Close talk RX
#define DEVICE_SND_DEVICE_NR_FT_TX		22  // Far talk(speaker phone) TX
#define DEVICE_SND_DEVICE_NR_FT_RX		23  // Far talk(speaker phone) RX
#define DEVICE_SND_DEVICE_NR_DV_TX		24  // desk talk(speaker phone) TX
#define DEVICE_SND_DEVICE_NR_DV_RX		25  // desk talk(speaker phone) RX
#define DEVICE_SND_DEVICE_NR_OFF_CT_TX	26  // Close talk TX
#define DEVICE_SND_DEVICE_NR_OFF_CT_RX	27  // Close talk RX
#define DEVICE_SND_DEVICE_NR_OFF_FT_TX	28  // Far talk(speaker phone) TX
#define DEVICE_SND_DEVICE_NR_OFF_FT_RX	29  // Far talk(speaker phone) RX
#define DEVICE_SND_DEVICE_NR_OFF_DV_TX	30  // desk TX
#define DEVICE_SND_DEVICE_NR_OFF_DV_RX	31  // desk RX

#define DEVICE_SND_DEVICE_NR_ON_2MIC_TX	32;  // desk TX
#endif

#define MUTE_VALUE					0x00
#define BIT_CLEAR_VALUE				0x00
#define BIT_CLEAR_MASK				0xFF
#define BIT_SET_VALUE				0xFF

#define SYSTEM_ON					1
#define SYSTEM_OFF					0

#define VOLUME_MUTE				1
#define VOLUME_UNMUTE				0

#define DEFAULT_DATA_REG		0
#define DEFAULT_DATA_REG_0F	0x20

#define START_GAIN_MIN			25

#define RAMP_SPEED_0_25MS		0x00
#define RAMP_SPEED_2MS			0x01
#define RAMP_SPEED_16MS		0x02
#define RAMP_SPEED_128MS		0x03

typedef struct
{
  u8 power_info;		//0x01
  u8 diff_pga_info;	//0x02
  u8 input_spk_hp_zc_info;	//0x03
  u8 input_hp_info;	//0x04
  u8 hpl_vol_info;		//0x05
  u8 hpr_vol_info;		//0x06
  u8 hp_ng_info;	//0x07
  u8 spk_ramp_zc_info;	//0x08
  u8 spk_ng_info;		//0x09
  u8 thresh_info;		//0x0a
  u8 agc_info;		//0x0b
  u8 agc_min_info;	//0x0c
  u8 present_gain_info;	//0x0d
  u8 start_gain_info;	//0x0e
  u8 ssmt_info;		//0x0f
}subsystem_info_t;

//FAB2200 Register Address
#define POWER_REG				0x01	
#define DIFF_PGA_REG			0x02	
#define INPUT_SPK_HP_ZC_REG	0x03
#define INPUT_HP_REG			0x04
#define HPL_VOL_REG				0x05
#define HPR_VOL_REG				0x06
#define HP_NG_REG				0x07
#define SPK_RAMP_ZC_REG		0x08
#define SPK_NG_REG				0x09
#define THRESH_REG				0x0A
#define AGC_REG					0x0B
#define AGC_MIN_REG			0x0C
#define PRESENT_GAIN_REG		0x0D
#define START_GAIN_REG			0x0E
#define SSMT_REG				0x0F

//FAB2200 Register Contents
//#1 	(ADR 0x01)
#define HIZL_VALUE					(0x01<<7)
#define HIZR_VALUE					(0x01<<6)
#define BYPEN_VALUE					(0x01<<4)
#define SHDNB_VALUE				(0x01<<0)
//#2 	(ADR 0x02)
#define DIFFIN43_VALUE				(0x01<<7)
#define DIFFIN21_VALUE				(0x01<<6)
#define PGAIN43_VALUE				3
#define PGAIN21_VALUE				0
//#3 	(ADR 0x03)
#define SEL_SPA4_VALUE				(0x01<<7)
#define SEL_SPA3_VALUE				(0x01<<6)
#define SEL_SPA2_VALUE				(0x01<<5)
#define SEL_SPA1_VALUE				(0x01<<4)
#define HPRAMP_SPEED_VALUE		2
#define HP_ZCD_VALUE				(0x01<<1)
#define HP_RAMP_VALUE				(0x01<<0)
//#4 	(ADR 0x04)
#define SEL_HPL4_VALUE				(0x01<<7)
#define SEL_HPL3_VALUE				(0x01<<6)
#define SEL_HPL2_VALUE				(0x01<<5)
#define SEL_HPL1_VALUE				(0x01<<4)
#define SEL_HPR4_VALUE				(0x01<<3)
#define SEL_HPR3_VALUE				(0x01<<2)
#define SEL_HPR2_VALUE				(0x01<<1)
#define SEL_HPR1_VALUE				(0x01<<0)
//#5 	(ADR 0x05)
#define HPL_VOL_VALUE				1
//#6 	(ADR 0x06)
#define HPR_VOL_VALUE				1
//#7 	(ADR 0x07)
#define HP_NG_ZRA_VALUE			(0x01<<7)
#define HP_NG_THRESH_VALUE		4
#define HP_NG_ZRR_VALUE			(0x01<<3)
#define HP_NG_TIME_VALUE			0
//#8 	(ADR 0x08)
#define ERC_VALUE					(0x01<<7)
#define SP_RAMP_SPEED_VALUE		2
#define SP_ZCD_VALUE				(0x01<<1)
#define SP_RAMP_VALUE				(0x01<<0)
//#9 	(ADR 0x09)
#define SP_NG_ZRA_VALUE			(0x01<<7)
#define SP_NG_THRESH_VALUE		4
#define SP_NG_ZRR_VALUE			(0x01<<3)
#define SP_NG_TIME_VALUE			0
//#10 	(ADR 0x0A)
#define THMAX_VALUE				4
#define THBATT_VALUE				0
//#11 	(ADR 0x0B)
#define AGC_ATTACK_VALUE			4
#define AGC_RELEASE_VALUE			0
//#12 	(ADR 0x0C)
#define AGC_MIN_VALUE				0
//#13 	(ADR 0x0D)
#define PRESENT_GAIN_VALUE			0
//#14 	(ADR 0x0E)
#define START_GAIN_VALUE			0
//#15 	(ADR 0x0F)
#define MCSSMT_VALUE				3
#define SSMT_VALUE					0

/************************************************************************************************
** Variables
*************************************************************************************************/

/************************************************************************************************
** Declaration
*************************************************************************************************/
extern void snd_subsystem_Init(void);
extern void snd_subsystem_DeInit(void);
//extern int snd_subsystem_SetPath(int new_device);
extern int snd_subsystem_SetVolume(void);
extern int snd_subsystem_SetMute(int new_device, int mute);

void fab2200_set_default_data(void);
int snd_subsystem_get_device(void);
int snd_subsystem_pmic_vreg_l2_on(void);
void snd_subsystem_pmic_vreg_l2_off(void);

int snd_subsystem_sp_poweron(void);
#ifdef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020 // jmlee
int snd_subsystem_nr_ft_poweron(void);
int snd_subsystem_nr_off_ft_poweron(void);
int snd_subsystem_nr_dv_poweron(void);
int snd_subsystem_nr_off_dv_poweron(void);
#endif
int snd_subsystem_hp_poweron(void);
int snd_subsystem_tty_hp_poweron(void);
int snd_subsystem_sp_hp_poweron(void);

void snd_subsystem_sp_powerdown(void);
#ifdef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020 // jmlee
void snd_subsystem_nr_ft_powerdown(void);
void snd_subsystem_nr_off_ft_powerdown(void);
void snd_subsystem_nr_dv_powerdown(void);
void snd_subsystem_nr_off_dv_powerdown(void);
#endif
void snd_subsystem_hp_powerdown(void);
void snd_subsystem_tty_hp_powerdown(void);
void snd_subsystem_sp_hp_powerdown(void);

 int config_GPIO_SUBSYSTEM_ENABLE(int enable);

#endif /*SNDSUBSYSTEM_H*/

