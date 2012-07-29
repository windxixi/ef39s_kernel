/*
**
**    FAB2200 AUDIO EXTERNAL AMP(SUBSYSTEM)
**
**    FILE
**        snd_sub_fab2200.c
**
**    DESCRIPTION
**        This file contains FAB Audio Subsystem api
**
**          void snd_subsystem_Init()
**          void snd_subsystem_DeInit()
**          void snd_subsystem_SetPath()
**          void snd_subsystem_SetVolume()
**
**          Copyright (c) 2011 by Song Jinhwan <song.jinhwan@pantech.com>
*************************************************************************************************/


/************************************************************************************************
** Includes
*************************************************************************************************/

#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <mach/gpio.h>

#include <mach/board.h>
#include <mach/vreg.h>

#include <linux/regulator/pmic8058-regulator.h>
#include <linux/pmic8058-othc.h>

#include "snd_presto_sub_fab2200.h"
#include "sky_snd_presto_extampctrl.h"  //20110310 jhsong : sub chip cal tool by youngsoo Lim
#ifdef FEATURE_SKY_SND_EXTAMPCTRL  //20110310 jhsong : sub chip cal tool by youngsoo Lim
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/msm_audio.h>
#endif

#if (BOARD_REV <= WS10)  // Presto NR CLK_IN ==> changed with ws20  // jmlee Audience clk err, so test code modify
#ifdef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020
#include "snd_presto_audience_a2020.h"
static int jmlee_adience_clk_err_test = 0;
#endif
#endif

#include <linux/i2c-gpio.h>
/************************************************************************************************
** Definition
*************************************************************************************************/
//#define CONFIG_FAB2200_DEBUG_PRINTK // jmlee 20110823

#define START_GAIN_MAX_INDEX		64
#define HPX_VOL_MAX_INDEX			32

/************************************************************************************************
** Variables
*************************************************************************************************/
static subsystem_info_t tSubsystemInfo;
static struct i2c_client *fab2200_i2c_client = NULL;

static int current_device = 0;

 //Start Gain : -25dB ~ 6dB per 0.5 step (0x00 : mute)
static u8 start_gain[START_GAIN_MAX_INDEX] = {  
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
	0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13,
	0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
	0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
	0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31,
	0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,
	0x3c, 0x3d, 0x3e, 0x3f	 							     
};
/*	 mute, -25 ~ -21dB  	(0 - 9)
	 -20.5 ~ -16dB	   	(10 - 19)
	 -15.5 ~ -11dB	  	(20 - 29)
	 -10.5 ~ -6dB			(30 - 39)
	 -5.5 ~ -1dB			(40 - 49)
	 -0.5 ~ 4dB			(50 - 59)
	   4.5 ~ 6dB			(60 - 63)
*/

 //Hpl/r Volume : -53dB ~ 6dB (0x00 : mute)
static u8 hpx_vol[HPX_VOL_MAX_INDEX] = { 
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
	0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 
	0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
	0x1e, 0x1f										     
}; 

/* mute,-53dB,-49dB,-45dB,-41dB,-37dB,-33dB,-29dB,-25dB,-23dB   (0 - 9)
   -21dB,-19dB,-17dB,-15dB,-13dB,-11dB, -9dB, -8dB, -7dB, -6dB, 	(10 - 19)
   -5dB,   -4dB , -3dB , -2dB , -1dB,   0dB ,  1dB ,  2dB,   3dB,   4dB	(20 - 29)
     5dB,    6dB												(30 - 31)
*/

//Headphone Noise Gate Threshold Vltage (mVpk)
static u8 hp_ng_thresh[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
};
/* disable,   2.8,   5.7,  11.3,  22.6,  45.3, 90.5, 181.0  */

//Headphone Noise Gate Timing  (ms)
static u8 hp_ng_time[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
};
/*	  10,    20,     40,     80,    160,  320,   640,  Reserved */

//Speaker Noise Gate Threshold
static u8 sp_ng_thresh[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
};
/* disable,   2.8,   5.7,  11.3,  22.6,  45.3, 90.5, 181.0  */

//Speaker Noise Gate Timing  (ms)
static u8 sp_ng_time[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
};
/*	  10,    20,     40,     80,    160,  320,   640,  Reserved */

//Speaker AGC Threshold Max  (Vpk)  --> step : 0.2 Vpk (0x00 : disable)
static u8 sp_thresh_max[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
	0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};
/*  disable, 2.2 ~ 3.8	(0 - 9)
	4.0 ~ 5.0			(10 - 15)
*/

//Speaker Vbatt fraction (agc threshold V / vbatt V)   --> step 0.05   (0x00 : disable)
static u8 sp_thresh_vbatt[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
	0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};
/*  disable, 0.90 ~ 1.30		(0 - 9)
	1.35 ~ 1.60			(10 - 15)
*/

//Speaker AGC Attack/Release Speed  (us)	--> step : 2 times per step of previous speed
static u8 sp_agc_attack_release_speed[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
};
/*    12.5,  25,     50,    100,    200,  400,   800,  1600*/

//Speaker Spread Spectrum Modulation Trim of the D-Amp/master clock.
static u8 sp_mcssmt_ssmt[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
};
/*     5.3,  7.0,   10.6,  21.2,   0.0,    3.0,   3.6,  4.2*/

/************************************************************************************************
** Declaration
*************************************************************************************************/
static int fab2200_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int __exit fab2200_remove(struct i2c_client *client);
static int fab2200_suspend(struct i2c_client *client, pm_message_t mesg);
static int fab2200_resume(struct i2c_client *client);
static void fab2200_shutdown(struct i2c_client *client);

//static int snd_subsystem_get_device(void);
static void snd_subsystem_startup(void);
static void snd_subsystem_shutdown(void);

static int snd_subsystem_common_system(int en);
static int snd_subsystem_sp_input_system(int en);
static int snd_subsystem_nr_ft_input_system(int en);
static int snd_subsystem_hp_input_system(int en);
static int snd_subsystem_tty_hp_input_system(int en);
static int snd_subsystem_sp_hp_input_system(int en);
static int snd_subsystem_nr_ft_input_system(int en);
static int snd_subsystem_nr_off_ft_input_system(int en);
static int snd_subsystem_nr_dv_input_system(int en);
static int snd_subsystem_nr_off_dv_input_system(int en);
static int snd_subsystem_sp_ng_ssmt(int en);
static int snd_subsystem_nr_off_ft_ng_ssmt(int en);
static int snd_subsystem_nr_off_dv_ng_ssmt(int en);
static void snd_subsystem_en_init(void);

static int snd_subsystem_i2c_write(u8 reg, u8 data);
//static int snd_subsystem_i2c_read(u8 reg, u8 *data);

#ifdef FEATURE_SKY_SND_EXTAMPCTRL  //20110310 jhsong : sub chip cal tool by youngsoo Lim
#define SND_AMP_IOCTL_MAGIC 'a'
#define SND_SKY_SET_EXTAMPGAIN_BYID   _IOW(SND_AMP_IOCTL_MAGIC, 8, unsigned)

static subsystem_info_t tSubsystemInfo_cal;
static uint32_t extamp_cal_dev_id = 0;

#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP
bool LockForTest = 0;
#define SND_SKY_AUDIOCMD_FOR_INT      _IOW(SND_AMP_IOCTL_MAGIC, 9, unsigned)
#define SND_SKY_AUDIOCMD_FOR_STRING   _IOW(SND_AMP_IOCTL_MAGIC, 10, unsigned)
#endif /* FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP */

void fab2200_set_cal_value_data(struct extamp_device_info *dev_info)
{
//	tSubsystemInfo_cal.power_info = (u8)DEFAULT_DATA_REG;
	extamp_cal_dev_id = dev_info->extamp_dev_id;
	tSubsystemInfo_cal.diff_pga_info = (u8)dev_info->pga_info;
	tSubsystemInfo_cal.input_spk_hp_zc_info = (u8)((dev_info->hp_ramp_speed<<HPRAMP_SPEED_VALUE) | 
												(dev_info->hp_ramp_en&HP_RAMP_VALUE));
//	tSubsystemInfo_cal.input_hp_info = (u8)dev_info->input_hp_info;
	tSubsystemInfo_cal.hpl_vol_info = (u8)dev_info->hpl_vol_info;
	tSubsystemInfo_cal.hpr_vol_info = (u8)dev_info->hpr_vol_info;
	tSubsystemInfo_cal.hp_ng_info = (u8)((dev_info->hp_ng_zra_info&HP_NG_ZRA_VALUE) | (dev_info->hp_ng_thresh_info<<HP_NG_THRESH_VALUE) |
									(dev_info->hp_ng_zrr_info&HP_NG_ZRR_VALUE) | (dev_info->hp_ng_time_info<<HP_NG_TIME_VALUE));
	tSubsystemInfo_cal.spk_ramp_zc_info = (u8)((dev_info->erc_info&ERC_VALUE) | (dev_info->spk_ramp_speed<<SP_RAMP_SPEED_VALUE)
											| (dev_info->spk_ramp_en&SP_RAMP_VALUE));
	tSubsystemInfo_cal.spk_ng_info = (u8)((dev_info->spk_ng_zra_info&SP_NG_ZRA_VALUE) | (dev_info->spk_ng_thresh_info<<SP_NG_THRESH_VALUE) |
									(dev_info->spk_ng_zrr_info&SP_NG_ZRR_VALUE) | (dev_info->spk_ng_time_info<<SP_NG_TIME_VALUE));
	tSubsystemInfo_cal.thresh_info = (u8)((dev_info->thresh_max_info<<THMAX_VALUE) | (dev_info->thresh_batt_info<<THBATT_VALUE));
	tSubsystemInfo_cal.agc_info = (u8)((dev_info->agc_attack_info<<AGC_ATTACK_VALUE) | (dev_info->agc_release_info<<AGC_RELEASE_VALUE));
	tSubsystemInfo_cal.agc_min_info = (u8)dev_info->agc_min_info;
//	tSubsystemInfo_cal.present_gain_info = DEFAULT_DATA_REG;
	tSubsystemInfo_cal.start_gain_info = (u8)dev_info->start_gain_info;
	tSubsystemInfo_cal.ssmt_info = (u8)((dev_info->mc_ssmt_info<<MCSSMT_VALUE) | (dev_info->ssmt_info<<SSMT_VALUE));	

//	printk("@#@#fab2200_set_cal_value_data.......0x02 : 0x%x,    0x05 : 0x%x,     0x06 : 0x%x\n",tSubsystemInfo_cal.diff_pga_info,tSubsystemInfo_cal.hpl_vol_info,tSubsystemInfo_cal.hpr_vol_info );
//	printk("@#@#fab2200_set_cal_value_data.......0x07 : 0x%x,    0x08 : 0x%x,     0x09 : 0x%x\n",tSubsystemInfo_cal.hp_ng_info,tSubsystemInfo_cal.spk_ramp_zc_info,tSubsystemInfo_cal.spk_ng_info );
//	printk("@#@#fab2200_set_cal_value_data.......0x0a : 0x%x,    0x0b : 0x%x,     0x0c : 0x%x\n",tSubsystemInfo_cal.thresh_info,tSubsystemInfo_cal.agc_info,tSubsystemInfo_cal.agc_min_info );
//	printk("@#@#fab2200_set_cal_value_data.......0x0e : 0x%x,    0x0f : 0x%x\n",tSubsystemInfo_cal.start_gain_info,tSubsystemInfo_cal.ssmt_info);
}

void snd_extamp_api_SetVolume_ById(uint32_t extamp_dev_id) {
//	extamp_info_t tCurrentExtampInfo;
	struct extamp_device_info *di;
//	int status = 0;
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	printk(KERN_ERR "[[ MAX97001 ]] ### extamp_dev_id(0x%x)\n", extamp_dev_id);
#endif
#if 0 //def FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP
	if (LockForTest == 1) return;
#endif /* FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP */

	di = extamp_lookup_device(extamp_dev_id);

/*************************************************************************************/
/* Need to modify each Audio sub system.                                             */
/*************************************************************************************/
	fab2200_set_cal_value_data(di);

#if 0
//	status|=snd_subsystem_i2c_write(POWER_REG, tSubsystemInfo.power_info); 					  // 0x01 register set default param
	status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo_cal.diff_pga_info);				  // 0x02 register set default param
	status|=snd_subsystem_i2c_write(INPUT_SPK_HP_ZC_REG, tSubsystemInfo_cal.input_spk_hp_zc_info);  // 0x03 register set default param
	status|=snd_subsystem_i2c_write(INPUT_HP_REG, tSubsystemInfo_cal.input_hp_info);				  // 0x04 register set default param
	status|=snd_subsystem_i2c_write(HPL_VOL_REG, tSubsystemInfo_cal.hpl_vol_info);					  // 0x05 register set default param
	status|=snd_subsystem_i2c_write(HPR_VOL_REG, tSubsystemInfo_cal.hpr_vol_info);				  // 0x06 register set default param
	status|=snd_subsystem_i2c_write(HP_NG_REG, tSubsystemInfo_cal.hp_ng_info);					  // 0x07 register set default param
	status|=snd_subsystem_i2c_write(SPK_RAMP_ZC_REG, tSubsystemInfo_cal.spk_ramp_zc_info);  		  // 0x08 register set default param
	status|=snd_subsystem_i2c_write(SPK_NG_REG, tSubsystemInfo_cal.spk_ng_info);					  // 0x09 register set default param
	status|=snd_subsystem_i2c_write(THRESH_REG, tSubsystemInfo_cal.thresh_info);  					  // 0x0a register set default param
	status|=snd_subsystem_i2c_write(AGC_REG, tSubsystemInfo_cal.agc_info); 						  // 0x0b register set default param
	status|=snd_subsystem_i2c_write(AGC_MIN_REG, tSubsystemInfo_cal.agc_min_info);				  // 0x0c register set default param
//	status|=snd_subsystem_i2c_write(PRESENT_GAIN_REG, tSubsystemInfo.present_gain_info);		  // 0x0d register set default param
	status|=snd_subsystem_i2c_write(START_GAIN_REG, tSubsystemInfo_cal.start_gain_info);			  // 0x0e register set default param
	status|=snd_subsystem_i2c_write(SSMT_REG, tSubsystemInfo_cal.ssmt_info);						  // 0x0f register set default param
#endif

/* Debug session */
//	printk(KERN_ERR "[[ FAB2200 ]] ### ID(0x%d) \n",
//	          di->extamp_dev_id);
//	printk(KERN_ERR "[[ FAB2200 ]] ### \n",
//              );
/*************************************************************************************/
}

static int aud_sub_open(struct inode *inode, struct file *file)
{
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	printk("[Audio] aud_sub_open \n");
#endif
	return 0;
}

static int aud_sub_release(struct inode *inode, struct file *file)
{
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	printk("[Audio] aud_sub_release \n");
#endif
	return 0;	
}

static long aud_sub_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
///*static*/ long aud_sub_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    
  	switch (cmd) {
		case SND_SKY_SET_EXTAMPGAIN_BYID:
		{
			uint32_t extamp_dev_id = 0;

		  if (copy_from_user(&extamp_dev_id, (uint32_t *) arg, sizeof(extamp_dev_id))) {
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
		    printk("[Audio] Err:aud_sub_ioctl cmd SND_SKY_SET_MODE_PARAM\n");
#endif
		    return 0;
		  }
			  snd_extamp_api_SetVolume_ById(extamp_dev_id);
		}
		break;
#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP
		case SND_SKY_AUDIOCMD_FOR_INT:
		snd_extamp_api_AudioCmd_ForInt(cmd, arg);
		break;
		case SND_SKY_AUDIOCMD_FOR_STRING:
		snd_extamp_api_AudioCmd_ForString(cmd, arg);
		break;
#endif /* FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP */
  	}
  return 0;
}

//static struct file_operations snd_fops = {
static const struct file_operations snd_fops = {
	.owner		= THIS_MODULE,
	.open		= aud_sub_open,
	.release	= aud_sub_release,
	.unlocked_ioctl	= aud_sub_ioctl,
};

static struct miscdevice miscdev_snd_fab = 
{
	.minor =    MISC_DYNAMIC_MINOR,
	.name =     "aud_sub",
	.fops =     &snd_fops
};
#endif
/*==========================================================================
** fab2200_probe
**=========================================================================*/
void fab2200_set_default_data(void)
{
	tSubsystemInfo.power_info = DEFAULT_DATA_REG;
	tSubsystemInfo.diff_pga_info = DEFAULT_DATA_REG;
	tSubsystemInfo.input_spk_hp_zc_info = DEFAULT_DATA_REG;
	tSubsystemInfo.input_hp_info = DEFAULT_DATA_REG;
	tSubsystemInfo.hpl_vol_info = DEFAULT_DATA_REG;
	tSubsystemInfo.hpr_vol_info = DEFAULT_DATA_REG;
	tSubsystemInfo.hp_ng_info = DEFAULT_DATA_REG;
	tSubsystemInfo.spk_ramp_zc_info = DEFAULT_DATA_REG;
	tSubsystemInfo.spk_ng_info = DEFAULT_DATA_REG;
	tSubsystemInfo.thresh_info = DEFAULT_DATA_REG;
	tSubsystemInfo.agc_info = DEFAULT_DATA_REG;
	tSubsystemInfo.agc_min_info = DEFAULT_DATA_REG;
	tSubsystemInfo.present_gain_info = DEFAULT_DATA_REG;
	tSubsystemInfo.start_gain_info = DEFAULT_DATA_REG;
	tSubsystemInfo.ssmt_info = DEFAULT_DATA_REG_0F;	
}

static int fab2200_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int rc = 0;
	int status = 0;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		fab2200_i2c_client = NULL;
		rc = -1;
	}
	else {
		fab2200_i2c_client = client;

		snd_subsystem_pmic_vreg_l2_on();	


#if 0 //def FEATURE_SKY_SND_EXTAMPCTRL  //20110310 jhsong : sub chip cal tool by youngsoo Lim
		status=misc_register(&miscdev_snd_fab);
		if(status)
		{
			printk(KERN_ERR "aud sub misc_register failed............\n");
			status = 0;
		}
#endif

		snd_subsystem_en_init();
		
		fab2200_set_default_data();

		/* Init fab2200 register */	
		status|=snd_subsystem_i2c_write(POWER_REG, tSubsystemInfo.power_info); 					  // 0x01 register set default param
		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info);				  // 0x02 register set default param
		status|=snd_subsystem_i2c_write(INPUT_SPK_HP_ZC_REG, tSubsystemInfo.input_spk_hp_zc_info);  // 0x03 register set default param
		status|=snd_subsystem_i2c_write(INPUT_HP_REG, tSubsystemInfo.input_hp_info);				  // 0x04 register set default param
		status|=snd_subsystem_i2c_write(HPL_VOL_REG, tSubsystemInfo.hpl_vol_info);					  // 0x05 register set default param
		status|=snd_subsystem_i2c_write(HPR_VOL_REG, tSubsystemInfo.hpr_vol_info);				  // 0x06 register set default param
		status|=snd_subsystem_i2c_write(HP_NG_REG, tSubsystemInfo.hp_ng_info);					  // 0x07 register set default param
		status|=snd_subsystem_i2c_write(SPK_RAMP_ZC_REG, tSubsystemInfo.spk_ramp_zc_info);  		  // 0x08 register set default param
		status|=snd_subsystem_i2c_write(SPK_NG_REG, tSubsystemInfo.spk_ng_info);					  // 0x09 register set default param
		status|=snd_subsystem_i2c_write(THRESH_REG, tSubsystemInfo.thresh_info);  					  // 0x0a register set default param
		status|=snd_subsystem_i2c_write(AGC_REG, tSubsystemInfo.agc_info); 						  // 0x0b register set default param
		status|=snd_subsystem_i2c_write(AGC_MIN_REG, tSubsystemInfo.agc_min_info);				  // 0x0c register set default param
		status|=snd_subsystem_i2c_write(PRESENT_GAIN_REG, tSubsystemInfo.present_gain_info);		  // 0x0d register set default param
		status|=snd_subsystem_i2c_write(START_GAIN_REG, tSubsystemInfo.start_gain_info);			  // 0x0e register set default param
		status|=snd_subsystem_i2c_write(SSMT_REG, tSubsystemInfo.ssmt_info);						  // 0x0f register set default param

	        if(status){
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
			printk("[fab2200] fab2200_probe......fail !!! %d \n", status);
#endif
	              rc = -1;
			return rc;
	        }

		snd_subsystem_shutdown();
	}
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	printk("[fab2200] fab2200_probe %d \n", rc);
#endif
	return rc;
}


/*==========================================================================
** fab2200_remove
**=========================================================================*/
static int __exit fab2200_remove(struct i2c_client *client)
{
	int rc = 0;

	fab2200_i2c_client = NULL;
	//rc = i2c_detach_client(client);
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	printk("[fab2200] fab2200_remove %d \n", rc);	
#endif
	return rc;
}


/*==========================================================================
** fab2200_suspend
**=========================================================================*/
static int fab2200_suspend(struct i2c_client *client, pm_message_t mesg)
{
//	printk("[fab2200] fab2200_suspend\n");
	return 0;
}


/*==========================================================================
** fab2200_resume
**=========================================================================*/
static int fab2200_resume(struct i2c_client *client)
{
//	printk("[fab2200] fab2200_resume\n");

	return 0;
}


/*==========================================================================
** fab2200_shutdown
**=========================================================================*/
static void fab2200_shutdown(struct i2c_client *client)
{
//	int status = 0;

	//snd_subsystem_powerdown();
	snd_subsystem_shutdown();

//	printk("[fab2200] fab2200_shutdown \n" );	
	
//	return status;
}



static const struct i2c_device_id fab2200_id[] = {
	{ "fab2200-amp", 0},
};

static struct i2c_driver fab2200_driver = {
	.id_table	= fab2200_id,
	.probe 		= fab2200_probe,
	.remove		= __exit_p(fab2200_remove),
	.suspend	= fab2200_suspend,
	.resume		= fab2200_resume,
	.shutdown	= fab2200_shutdown,
	.driver 	= {
		.name = "fab2200-amp",
	},
};



/*==========================================================================
** snd_subsystem_Init
**=========================================================================*/
void snd_subsystem_Init(void)
{
	int result = 0;

	result = i2c_add_driver(&fab2200_driver);
	if(result){
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
		printk(KERN_ERR "fab2200_driver add failed.\n");
#endif
	}
}


/*==========================================================================
** snd_subsystem_DeInit
**=========================================================================*/
void snd_subsystem_DeInit(void)
{
#if 0 //def FEATURE_SKY_SND_EXTAMPCTRL  //20110310 jhsong : sub chip cal tool by youngsoo Lim
	misc_deregister(&miscdev_snd_fab);
#endif
	i2c_del_driver(&fab2200_driver);
}

/*==========================================================================
** snd_subsystem_get_device
**=========================================================================*/
int snd_subsystem_get_device(void)
{
	return current_device;
}


/*==========================================================================
** snd_subsystem_SetMute
**=========================================================================*/
int snd_subsystem_SetMute(int new_device, int mute)
{
	int status = 0;
	u8 reg_value=0x00;

	switch(new_device)
	{
		case DEVICE_HEADSET_RX:
		case DEVICE_TTY_HEADSET_MONO_RX:				
			if(mute){
				status|=snd_subsystem_i2c_write(DIFF_PGA_REG, reg_value);  //PGA mute

				status|=snd_subsystem_i2c_write(HPL_VOL_REG, reg_value);  // hpl volume set mute (for hp)
				status|=snd_subsystem_i2c_write(HPR_VOL_REG, reg_value);  // hpr volume set mute (for hp)
			}else{				
				status|=snd_subsystem_i2c_write(DIFF_PGA_REG, (tSubsystemInfo.diff_pga_info));  //PGA unmute

				status|=snd_subsystem_i2c_write(HPL_VOL_REG,  tSubsystemInfo.hpl_vol_info);  // hpl volume set unmute (for hp)
				status|=snd_subsystem_i2c_write(HPR_VOL_REG,  tSubsystemInfo.hpr_vol_info);  // hpr volume set unmute (for hp)				
			}
		break;
		
		case DEVICE_SND_DEVICE_NR_FT_RX:
		case DEVICE_SND_DEVICE_NR_DV_RX:
		case DEVICE_SND_DEVICE_NR_OFF_FT_RX:
		case DEVICE_SND_DEVICE_NR_OFF_DV_RX:
		case DEVICE_SPEAKER_RX:
			if(mute){
				status|=snd_subsystem_i2c_write(DIFF_PGA_REG, reg_value);  //PGA mute
				
				status|=snd_subsystem_i2c_write(START_GAIN_REG, reg_value);  // start gain set mute (for spk)			
			}else{
				status|=snd_subsystem_i2c_write(DIFF_PGA_REG, (tSubsystemInfo.diff_pga_info));  //PGA unmute

				status|=snd_subsystem_i2c_write(START_GAIN_REG, tSubsystemInfo.start_gain_info);  // start gain set unmute (for spk)						
			}
		break;

		case DEVICE_SPEAKER_HEADSET_RX:
			if(mute){
				status|=snd_subsystem_i2c_write(DIFF_PGA_REG, (reg_value));  //PGA mute

				status|=snd_subsystem_i2c_write(HPL_VOL_REG, reg_value);  // hpl volume set mute (for hp)
				status|=snd_subsystem_i2c_write(HPR_VOL_REG, reg_value);  // hpr volume set mute (for hp)
				
				status|=snd_subsystem_i2c_write(START_GAIN_REG, reg_value);  // start gain set mute (for spk)			
			}else{
				status|=snd_subsystem_i2c_write(DIFF_PGA_REG, (tSubsystemInfo.diff_pga_info));  //PGA unmute

				status|=snd_subsystem_i2c_write(HPL_VOL_REG,  tSubsystemInfo.hpl_vol_info);  // hpl volume set unmute (for hp)
				status|=snd_subsystem_i2c_write(HPR_VOL_REG,  tSubsystemInfo.hpr_vol_info);  // hpr volume set unmute (for hp)								

				status|=snd_subsystem_i2c_write(START_GAIN_REG, tSubsystemInfo.start_gain_info);  // start gain set unmute (for spk)						
			}
		break;	
	}

	return status;
}

/*==========================================================================
** snd_subsystem_SetPath
**=========================================================================*/
/*
int snd_subsystem_SetPath(int new_device)
{
	int status = 0;
//	u8 reg_value=0;

	current_device = new_device;


	//snd_subsystem_poweron();


//for test
	switch(new_device)
	{
	case DEVICE_HANDSET_RX: //receiver
	case DEVICE_FMRADIO_HANDSET_RX:
		printk("[fab2200] DEVICE_HANDSET_RX %d\n", new_device);		
		break;

	case DEVICE_HEADSET_RX:
//	case DEVICE_FMRADIO_HEADSET_RX:
		printk("[fab2200] DEVICE_HEADSET_RX %d\n", new_device);

		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, (DIFFIN21_VALUE & BIT_CLEAR_VALUE));  //in2,1 input configure to stereo (independent)
		status|=snd_subsystem_i2c_write(INPUT_HP_REG, ((SEL_HPL1_VALUE|SEL_HPR2_VALUE) & BIT_SET_VALUE));	//set input hpl 1, hpr 2 to hp out

		status|=snd_subsystem_i2c_write(INPUT_SPK_HP_ZC_REG, (HP_ZCD_VALUE & BIT_SET_VALUE));  //hp zero cross detection enable
		break;

	case DEVICE_SPEAKER_RX:
//	case DEVICE_FMRADIO_SPEAKER_RX:
		printk("[fab2200] DEVICE_SPEAKER_RX %d\n", new_device);
		
		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, (DIFFIN43_VALUE & BIT_SET_VALUE));  //in4,3 input configure to single ended
		status|=snd_subsystem_i2c_write(INPUT_SPK_HP_ZC_REG, ((SEL_SPA4_VALUE|SEL_SPA3_VALUE) & BIT_SET_VALUE));	//set input to spk out

		status|=snd_subsystem_i2c_write(SPK_RAMP_ZC_REG, (SP_ZCD_VALUE & BIT_SET_VALUE));  //spk zero cross detection enable
		break;

	case DEVICE_SPEAKER_HEADSET_RX:
		printk("[fab2200] DEVICE_SPEAKER_HEADSET_RX %d\n", new_device);
		break;

	case DEVICE_TTY_HEADSET_MONO_RX:	
		printk("[fab2200] DEVICE_TTY_HEADSET_MONO_RX %d\n", new_device);			
		break;	
	
	default:
		break;
	}
	
	return status;
}
*/

/*==========================================================================
** snd_subsystem_get_Startgain_index
**=========================================================================*/
int snd_subsystem_get_Startgain_index(unsigned long long dB)
{
	int _start_gain_index = 0;
	
	_start_gain_index = ((dB + START_GAIN_MIN) * 2) + 1;

#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	printk("[fab2200] snd_subsystem_get_Startgain_index....... start_gain_index : %d \n", _start_gain_index);
#endif
	return _start_gain_index;
}

/*==========================================================================
** snd_subsystem_SetVolume
**=========================================================================*/
int snd_subsystem_SetVolume(void)
{
	int status = 0;
	int start_gain_arr_index = 0;

	switch(snd_subsystem_get_device()/*sub_device*/)
	{
	case DEVICE_HANDSET_RX: //receiver
	case DEVICE_FMRADIO_HANDSET_RX:
		break;
			
	case DEVICE_HEADSET_RX:
	case DEVICE_FMRADIO_HEADSET_RX:
#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.diff_pga_info |= (tSubsystemInfo_cal.diff_pga_info<<PGAIN21_VALUE);

			tSubsystemInfo.hpl_vol_info = tSubsystemInfo_cal.hpl_vol_info<<HPL_VOL_VALUE;  // 0dB
			tSubsystemInfo.hpr_vol_info = tSubsystemInfo_cal.hpr_vol_info<<HPR_VOL_VALUE;  // 0dB			
		}else
#endif	
		{
		tSubsystemInfo.diff_pga_info |= (/*0x00*/0x01<<PGAIN21_VALUE);  //PGA 20110714 jmlee 6dB -> 3dB
			
			tSubsystemInfo.hpl_vol_info = hpx_vol[19]<<HPL_VOL_VALUE;  // 20110818  jmlee h/w test -2dB ->-6dB(20110901)
			tSubsystemInfo.hpr_vol_info = hpx_vol[19]<<HPR_VOL_VALUE; // 20110818  jmlee h/w test -2dB ->-6dB(20110901)
		}		
		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info);  
		status|=snd_subsystem_i2c_write(HPL_VOL_REG, (tSubsystemInfo.hpl_vol_info));  //0dB
		status|=snd_subsystem_i2c_write(HPR_VOL_REG,(tSubsystemInfo.hpr_vol_info));  //0dB
		break;

	case DEVICE_SPEAKER_RX:
	case DEVICE_FMRADIO_SPEAKER_RX:
//		tSubsystemInfo.diff_pga_info |= (0x05<<PGAIN43_VALUE);
		
#ifdef FEATURE_SKY_SND_EXTAMPCTRL  //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.start_gain_info = tSubsystemInfo_cal.start_gain_info<<START_GAIN_VALUE;
//			printk("@#@#snd_subsystem_SetVolume.....lockforTest......startGain : 0x%x",tSubsystemInfo.start_gain_info);
		}else
#endif		
		{
			start_gain_arr_index = snd_subsystem_get_Startgain_index(4.0);  // 20110629 jmlee 2.5dB --> 20110706 4.0dB
			tSubsystemInfo.start_gain_info = start_gain[start_gain_arr_index]<<START_GAIN_VALUE;
//			printk("@#@#snd_subsystem_SetVolume...........startGain : 0x%x",tSubsystemInfo.start_gain_info);
		}
//		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info);  //PGA 15dB
		status|=snd_subsystem_i2c_write(START_GAIN_REG,tSubsystemInfo.start_gain_info);

		break;

	case DEVICE_SPEAKER_HEADSET_RX:
#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.diff_pga_info |= ((tSubsystemInfo_cal.diff_pga_info<<PGAIN43_VALUE) | (tSubsystemInfo_cal.diff_pga_info<<PGAIN21_VALUE)); //PGA 15dB -> 0dB
		}else
#endif		
		{
			tSubsystemInfo.diff_pga_info |= ((0x04/*0x00*/<<PGAIN43_VALUE) | (/*0x05*/0x00<<PGAIN21_VALUE)); //PGA 15dB -> 0dB
		}
		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info);  

#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.start_gain_info = tSubsystemInfo_cal.start_gain_info<<START_GAIN_VALUE;
		}else
#endif		
		{
			start_gain_arr_index = snd_subsystem_get_Startgain_index(/*5*/4.5);  //5dB  ( -25dB ~ 6dB)
			tSubsystemInfo.start_gain_info = start_gain[start_gain_arr_index]<<START_GAIN_VALUE;
		}
		status|=snd_subsystem_i2c_write(START_GAIN_REG,tSubsystemInfo.start_gain_info);
		
#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.hpl_vol_info = tSubsystemInfo_cal.hpl_vol_info<<HPL_VOL_VALUE;  // 0dB
			tSubsystemInfo.hpr_vol_info = tSubsystemInfo_cal.hpr_vol_info<<HPR_VOL_VALUE;  // 0dB
		}else
#endif
		{
			tSubsystemInfo.hpl_vol_info = hpx_vol[/*25*//*24*//*20*/12]<<HPL_VOL_VALUE;  // 20110811 jmlee for test // 0dB -> -1dB (20110414 jhsong)
			tSubsystemInfo.hpr_vol_info = hpx_vol[/*25*//*24*//*20*/12]<<HPR_VOL_VALUE;  // 20110811 jmlee for test // 0dB -> -1dB (20110414 jhsong)
		}		
		status|=snd_subsystem_i2c_write(HPL_VOL_REG, (tSubsystemInfo.hpl_vol_info));  //0dB
		status|=snd_subsystem_i2c_write(HPR_VOL_REG,(tSubsystemInfo.hpr_vol_info));  //0dB
		break;

	case DEVICE_SND_DEVICE_NR_FT_RX:
#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.start_gain_info = tSubsystemInfo_cal.start_gain_info<<START_GAIN_VALUE;
		}else
#endif
		{
			start_gain_arr_index = snd_subsystem_get_Startgain_index(4.5);  // 20110802 jmlee  4.5dB  ( -25dB ~ 6dB)
			tSubsystemInfo.start_gain_info = start_gain[start_gain_arr_index]<<START_GAIN_VALUE;
		}		
//		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info);  //PGA 15dB
		status|=snd_subsystem_i2c_write(START_GAIN_REG,tSubsystemInfo.start_gain_info);
		break;
		
	case DEVICE_SND_DEVICE_NR_DV_RX:
#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.start_gain_info = tSubsystemInfo_cal.start_gain_info<<START_GAIN_VALUE;
		}else
#endif
		{
			start_gain_arr_index = snd_subsystem_get_Startgain_index(4.5);  // 20110802 jmlee  4.5dB  ( -25dB ~ 6dB)
			tSubsystemInfo.start_gain_info = start_gain[start_gain_arr_index]<<START_GAIN_VALUE;
		}		
		
//		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info);  //PGA 15dB
		status|=snd_subsystem_i2c_write(START_GAIN_REG,tSubsystemInfo.start_gain_info);
		break;

	case DEVICE_SND_DEVICE_NR_OFF_FT_RX:
#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.start_gain_info = tSubsystemInfo_cal.start_gain_info<<START_GAIN_VALUE;
		}else
#endif
		{
			start_gain_arr_index = snd_subsystem_get_Startgain_index(4.5);  // 20110802 jmlee  4.5dB  ( -25dB ~ 6dB)
			tSubsystemInfo.start_gain_info = start_gain[start_gain_arr_index]<<START_GAIN_VALUE;
		}		
		
//		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info);  //PGA 15dB
		status|=snd_subsystem_i2c_write(START_GAIN_REG,tSubsystemInfo.start_gain_info);
		break;

	case DEVICE_SND_DEVICE_NR_OFF_DV_RX:
#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.start_gain_info = tSubsystemInfo_cal.start_gain_info<<START_GAIN_VALUE;
		}else
#endif
		{
			start_gain_arr_index = snd_subsystem_get_Startgain_index(4.5);  // 20110802 jmlee  4.5dB  ( -25dB ~ 6dB)
			tSubsystemInfo.start_gain_info = start_gain[start_gain_arr_index]<<START_GAIN_VALUE;
		}		
		
//		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info);  //PGA 15dB
		status|=snd_subsystem_i2c_write(START_GAIN_REG,tSubsystemInfo.start_gain_info);
		break;

	case DEVICE_TTY_HEADSET_MONO_RX :
#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.diff_pga_info |= (tSubsystemInfo_cal.diff_pga_info<<PGAIN21_VALUE);  //PGA 15dB -> 0dB
			
			tSubsystemInfo.hpl_vol_info = tSubsystemInfo_cal.hpl_vol_info<<HPL_VOL_VALUE;  // 0dB
			tSubsystemInfo.hpr_vol_info = tSubsystemInfo_cal.hpr_vol_info<<HPR_VOL_VALUE;  // 0dB
		}else
#endif		
		{
			tSubsystemInfo.diff_pga_info |= (/*0x05*/0x00<<PGAIN21_VALUE);  //PGA 15dB -> 0dB
			
			tSubsystemInfo.hpl_vol_info = hpx_vol[25]<<HPL_VOL_VALUE;  // 0dB
			tSubsystemInfo.hpr_vol_info = hpx_vol[25]<<HPR_VOL_VALUE;  // 0dB
		}		
		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info);  
		status|=snd_subsystem_i2c_write(HPL_VOL_REG, (tSubsystemInfo.hpl_vol_info));  //0dB
		status|=snd_subsystem_i2c_write(HPR_VOL_REG,(tSubsystemInfo.hpr_vol_info));  //0dB
		break;
	
	default:
		break;
			
	}
	return status;
}

/*==========================================================================
** snd_subsystem_Set_HP_Noise_Gate
**=========================================================================*/
int snd_subsystem_Set_HP_Noise_Gate(int en)
{
	int status = 0;

	if(en)
	{
		 //hpngzra set 1 bit, hpngthresh set 001 bit, hpngzrr set 1 bit, hpngtime set 011 bit 
#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.hp_ng_info = tSubsystemInfo_cal.hp_ng_info;
		}else
#endif
		{
			tSubsystemInfo.hp_ng_info |=	((HP_NG_ZRA_VALUE & BIT_SET_VALUE) | (hp_ng_thresh[1]<<HP_NG_THRESH_VALUE) | 
										 (HP_NG_ZRR_VALUE & BIT_SET_VALUE) | (hp_ng_time[3]<<HP_NG_TIME_VALUE));
		}
	}else{
		tSubsystemInfo.hp_ng_info &= BIT_CLEAR_VALUE;    //hpngzra, hpngthresh, hpngzrr, hpngtime set 0 bit 	
	}

	status|=snd_subsystem_i2c_write(HP_NG_REG, tSubsystemInfo.hp_ng_info); 

	return status;		
}

/*==========================================================================
** snd_subsystem_Set_tty_HP_Noise_Gate
**=========================================================================*/
int snd_subsystem_Set_tty_HP_Noise_Gate(int en)
{
	int status = 0;

	if(en)
	{
		 //hpngzra set 1 bit, hpngthresh set 001 bit, hpngzrr set 1 bit, hpngtime set 011 bit 
#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.hp_ng_info = tSubsystemInfo_cal.hp_ng_info;
		}else
#endif
		{
			tSubsystemInfo.hp_ng_info |=	((HP_NG_ZRA_VALUE & BIT_SET_VALUE) | (hp_ng_thresh[1]<<HP_NG_THRESH_VALUE) | 
										 (HP_NG_ZRR_VALUE & BIT_SET_VALUE) | (hp_ng_time[3]<<HP_NG_TIME_VALUE));
		}		
	}else{
		tSubsystemInfo.hp_ng_info &= BIT_CLEAR_VALUE;    //hpngzra, hpngthresh, hpngzrr, hpngtime set 0 bit 	
	}

	status|=snd_subsystem_i2c_write(HP_NG_REG, tSubsystemInfo.hp_ng_info); 

	return status;		
}

/*==========================================================================
** snd_subsystem_Set_SP_ERC
**=========================================================================*/
int snd_subsystem_Set_SP_ERC(int en)
{
	int status = 0;

	if(en){
#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.spk_ramp_zc_info |= (tSubsystemInfo_cal.spk_ramp_zc_info);
		}else
#endif		
		{
			tSubsystemInfo.spk_ramp_zc_info |= ((ERC_VALUE & BIT_SET_VALUE) | (RAMP_SPEED_0_25MS<<SP_RAMP_SPEED_VALUE)
												| (BIT_SET_VALUE & SP_RAMP_VALUE));
		}
	}else{
		tSubsystemInfo.spk_ramp_zc_info &= BIT_CLEAR_VALUE;
	}

	status|=snd_subsystem_i2c_write(SPK_RAMP_ZC_REG, tSubsystemInfo.spk_ramp_zc_info);  //speaker amplifier edge rate control

	return status;
}

/*==========================================================================
** snd_subsystem_Set_SP_Noise_Gate
**=========================================================================*/
int snd_subsystem_Set_SP_Noise_Gate(int en)
{
	int status = 0;

	if(en)
	{
#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.spk_ng_info = tSubsystemInfo_cal.spk_ng_info;
		}else
#endif		
		{
			switch(snd_subsystem_get_device()/*sub_device*/)
			{
				case DEVICE_SPEAKER_RX:
					//spngzra set 1 bit, spngthresh set 001 bit, spngzrr set 1 bit, spngtime set 011 bit 
					tSubsystemInfo.spk_ng_info |= ((SP_NG_ZRA_VALUE & BIT_SET_VALUE) | (sp_ng_thresh[1]<<SP_NG_THRESH_VALUE) | 
												 (SP_NG_ZRR_VALUE & BIT_SET_VALUE) | (sp_ng_time[3]<<SP_NG_TIME_VALUE));
				break;

				case DEVICE_SND_DEVICE_NR_FT_RX:
					//spngzra set 1 bit, spngthresh set 001 bit, spngzrr set 1 bit, spngtime set 011 bit 
					tSubsystemInfo.spk_ng_info |= ((SP_NG_ZRA_VALUE & BIT_SET_VALUE) | (sp_ng_thresh[1]<<SP_NG_THRESH_VALUE) | 
												 (SP_NG_ZRR_VALUE & BIT_SET_VALUE) | (sp_ng_time[3]<<SP_NG_TIME_VALUE));
				break;

				case DEVICE_SND_DEVICE_NR_DV_RX:
					//spngzra set 1 bit, spngthresh set 001 bit, spngzrr set 1 bit, spngtime set 011 bit 
					tSubsystemInfo.spk_ng_info |= ((SP_NG_ZRA_VALUE & BIT_SET_VALUE) | (sp_ng_thresh[1]<<SP_NG_THRESH_VALUE) | 
												 (SP_NG_ZRR_VALUE & BIT_SET_VALUE) | (sp_ng_time[3]<<SP_NG_TIME_VALUE));
				break;

				case DEVICE_SND_DEVICE_NR_OFF_FT_RX:
					//spngzra set 1 bit, spngthresh set 001 bit, spngzrr set 1 bit, spngtime set 011 bit 
					tSubsystemInfo.spk_ng_info |= ((SP_NG_ZRA_VALUE & BIT_SET_VALUE) | (sp_ng_thresh[1]<<SP_NG_THRESH_VALUE) | 
												 (SP_NG_ZRR_VALUE & BIT_SET_VALUE) | (sp_ng_time[3]<<SP_NG_TIME_VALUE));
				break;

				case DEVICE_SND_DEVICE_NR_OFF_DV_RX:
					//spngzra set 1 bit, spngthresh set 001 bit, spngzrr set 1 bit, spngtime set 011 bit 
					tSubsystemInfo.spk_ng_info |= ((SP_NG_ZRA_VALUE & BIT_SET_VALUE) | (sp_ng_thresh[1]<<SP_NG_THRESH_VALUE) | 
												 (SP_NG_ZRR_VALUE & BIT_SET_VALUE) | (sp_ng_time[3]<<SP_NG_TIME_VALUE));
				break;
			}
		}
	}else{
		tSubsystemInfo.spk_ng_info &= BIT_CLEAR_VALUE;  //spngzra, spngthresh, spngzrr, spngtime set 0 bit 		
	}

	status|=snd_subsystem_i2c_write(SPK_NG_REG, tSubsystemInfo.spk_ng_info);  

	return status;		
}

/*==========================================================================
** snd_subsystem_Set_SP_AGC_Thresh
**=========================================================================*/
int snd_subsystem_Set_SP_AGC_Thresh(int en)
{
	int status = 0;

	if(en)
	{
#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.thresh_info = tSubsystemInfo_cal.thresh_info;
		}else
#endif		
		{
			switch(snd_subsystem_get_device()/*sub_device*/)
			{
				case DEVICE_SPEAKER_RX:
					tSubsystemInfo.thresh_info |=	((sp_thresh_max[0]<<THMAX_VALUE) | (sp_thresh_vbatt[0]<<THBATT_VALUE));  //thmax set 000 bit, thvbatt set 000 bit
				break;

				case DEVICE_SND_DEVICE_NR_FT_RX:
					tSubsystemInfo.thresh_info |=	((sp_thresh_max[0]<<THMAX_VALUE) | (sp_thresh_vbatt[0]<<THBATT_VALUE));  //thmax set 000 bit, thvbatt set 000 bit
				break;

				case DEVICE_SND_DEVICE_NR_DV_RX:
					tSubsystemInfo.thresh_info |=	((sp_thresh_max[0]<<THMAX_VALUE) | (sp_thresh_vbatt[0]<<THBATT_VALUE));  //thmax set 000 bit, thvbatt set 000 bit
				break;

				case DEVICE_SND_DEVICE_NR_OFF_FT_RX:
					tSubsystemInfo.thresh_info |=	((sp_thresh_max[0]<<THMAX_VALUE) | (sp_thresh_vbatt[0]<<THBATT_VALUE));  //thmax set 000 bit, thvbatt set 000 bit
				break;

				case DEVICE_SND_DEVICE_NR_OFF_DV_RX:
					tSubsystemInfo.thresh_info |=	((sp_thresh_max[0]<<THMAX_VALUE) | (sp_thresh_vbatt[0]<<THBATT_VALUE));  //thmax set 000 bit, thvbatt set 000 bit
				break;
			}
		}
	}else{
		tSubsystemInfo.thresh_info &= BIT_CLEAR_VALUE;  //thmax, thvbatt set default bit (000)
	}

	status|=snd_subsystem_i2c_write(THRESH_REG, tSubsystemInfo.thresh_info);  
	
	return status;		
}

/*==========================================================================
** snd_subsystem_Set_SP_AGC_Attack_Release
**=========================================================================*/
int snd_subsystem_Set_SP_AGC_Attack_Release(int en)
{
	int status = 0;

	if(en)
	{
#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.agc_info = tSubsystemInfo_cal.agc_info;
		}else
#endif		
		{
			switch(snd_subsystem_get_device()/*sub_device*/)
			{
				case DEVICE_SPEAKER_RX:
					tSubsystemInfo.agc_info |= ((sp_agc_attack_release_speed[0]<<AGC_ATTACK_VALUE) | 
												(sp_agc_attack_release_speed[0]<<AGC_RELEASE_VALUE));
				break;
				
				case DEVICE_SND_DEVICE_NR_FT_RX:
					tSubsystemInfo.agc_info |= ((sp_agc_attack_release_speed[0]<<AGC_ATTACK_VALUE) | 
												(sp_agc_attack_release_speed[0]<<AGC_RELEASE_VALUE));
				break;

				case DEVICE_SND_DEVICE_NR_DV_RX:
					tSubsystemInfo.agc_info |= ((sp_agc_attack_release_speed[0]<<AGC_ATTACK_VALUE) | 
												(sp_agc_attack_release_speed[0]<<AGC_RELEASE_VALUE));
				break;

				case DEVICE_SND_DEVICE_NR_OFF_FT_RX:
					tSubsystemInfo.agc_info |= ((sp_agc_attack_release_speed[0]<<AGC_ATTACK_VALUE) | 
												(sp_agc_attack_release_speed[0]<<AGC_RELEASE_VALUE));
				break;

				case DEVICE_SND_DEVICE_NR_OFF_DV_RX:
					tSubsystemInfo.agc_info |= ((sp_agc_attack_release_speed[0]<<AGC_ATTACK_VALUE) | 
												(sp_agc_attack_release_speed[0]<<AGC_RELEASE_VALUE));
				break;
			}
		}
	}else{
		tSubsystemInfo.agc_info &= BIT_CLEAR_VALUE;
	}

	status|=snd_subsystem_i2c_write(AGC_REG, tSubsystemInfo.agc_info);  
	
	return status;		
}

/*==========================================================================
** snd_subsystem_Set_SP_AGC_Min
**=========================================================================*/
int snd_subsystem_Set_SP_AGC_Min(int en)
{
	int status = 0;

	if(en)
	{
#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.agc_min_info = tSubsystemInfo_cal.agc_min_info;
		}else
#endif
		{
			switch(snd_subsystem_get_device()/*sub_device*/)
			{
				case DEVICE_SPEAKER_RX:
					tSubsystemInfo.agc_min_info |= (start_gain[1]<<AGC_MIN_VALUE); 	
				break;

				case DEVICE_SND_DEVICE_NR_FT_RX:
					tSubsystemInfo.agc_min_info |= (start_gain[1]<<AGC_MIN_VALUE); 	
				break;
			}
		}
	}else{
		tSubsystemInfo.agc_min_info &= BIT_CLEAR_VALUE; 	
	}

	status|=snd_subsystem_i2c_write(AGC_MIN_REG, tSubsystemInfo.agc_min_info);  
	
	return status;		
}

/*==========================================================================
** snd_subsystem_Set_SP_AGC
**=========================================================================*/
int snd_subsystem_Set_SP_AGC(int en)
{
	int status = 0;

	if(en)
		mdelay(2);
	
	status |= snd_subsystem_Set_SP_AGC_Attack_Release(en);
	status |= snd_subsystem_Set_SP_AGC_Thresh(en);

	return status;
}

/*==========================================================================
** snd_subsystem_Set_SP_SSMT
**=========================================================================*/
int snd_subsystem_Set_SP_SSMT(int en)
{
	int status = 0;

	if(en)
	{
#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.ssmt_info = tSubsystemInfo_cal.ssmt_info;
		}else
#endif		
		{
			switch(snd_subsystem_get_device()/*sub_device*/)
			{
				case DEVICE_SPEAKER_RX:
					tSubsystemInfo.ssmt_info |= ((sp_mcssmt_ssmt[3]<<MCSSMT_VALUE) | (sp_mcssmt_ssmt[3]<<SSMT_VALUE) );
				break;

				case DEVICE_SND_DEVICE_NR_FT_RX:
					tSubsystemInfo.ssmt_info |= ((sp_mcssmt_ssmt[3]<<MCSSMT_VALUE) | (sp_mcssmt_ssmt[3]<<SSMT_VALUE) );
				break;

				case DEVICE_SND_DEVICE_NR_DV_RX:
					tSubsystemInfo.ssmt_info |= ((sp_mcssmt_ssmt[3]<<MCSSMT_VALUE) | (sp_mcssmt_ssmt[3]<<SSMT_VALUE) );
				break;

				case DEVICE_SND_DEVICE_NR_OFF_FT_RX:
					tSubsystemInfo.ssmt_info |= ((sp_mcssmt_ssmt[3]<<MCSSMT_VALUE) | (sp_mcssmt_ssmt[3]<<SSMT_VALUE) );
				break;

				case DEVICE_SND_DEVICE_NR_OFF_DV_RX:
					tSubsystemInfo.ssmt_info |= ((sp_mcssmt_ssmt[3]<<MCSSMT_VALUE) | (sp_mcssmt_ssmt[3]<<SSMT_VALUE) );
				break;
			}
		}
	}else{
		tSubsystemInfo.ssmt_info &= BIT_CLEAR_VALUE;
	}

	status|=snd_subsystem_i2c_write(SSMT_REG, tSubsystemInfo.ssmt_info);  
	
	return status;		
}

#if (BOARD_REV > WS10)
/*==========================================================================
** snd_subsystem_pmic_vreg_l2_on
**=========================================================================*/
static struct regulator *snddev_reg_s3_fab2200;
int snd_subsystem_pmic_vreg_l2_on(void)
{
	int rc = 0;
	snddev_reg_s3_fab2200 = regulator_get(NULL, "8058_s3");
	if (IS_ERR(snddev_reg_s3_fab2200)) {
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
		printk(KERN_ERR "%s: vreg_enable failed(%s) = %d \n",__func__, "8058_s3", rc);
#endif
	}

	rc = regulator_set_voltage(snddev_reg_s3_fab2200, 1800000, 1800000);
	if (rc < 0)
	{
#ifdef CONFIG_FAB2200_DEBUG_PRINTK		
		printk(KERN_ERR "%s: vreg_enable failed(%s) = %d \n",__func__, "8058_s3", rc);
#endif
	}

	rc = regulator_enable(snddev_reg_s3_fab2200);
	if (rc < 0)
	{
#ifdef CONFIG_FAB2200_DEBUG_PRINTK		
		printk(KERN_ERR "%s: vreg_enable failed(%s) = %d \n",	__func__, "8058_s3", rc);
#endif
	}
	return rc;
}


/*==========================================================================
** snd_subsystem_pmic_vreg_l2_off
**=========================================================================*/
void snd_subsystem_pmic_vreg_l2_off(void)
{
	int rc = 0;

	if (!snddev_reg_s3_fab2200)
	{	
		rc = regulator_disable(snddev_reg_s3_fab2200);
		if (rc < 0)
		{
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
			printk(KERN_ERR "%s: vreg_enable failed(%s) = %d \n",__func__, "8058_s3", rc);
#endif
		}

		regulator_put(snddev_reg_s3_fab2200);

		snddev_reg_s3_fab2200 = NULL;
	}

}
#else

/*==========================================================================
** snd_subsystem_pmic_vreg_l2_on
**=========================================================================*/
static struct regulator *snddev_reg_l8_fab2200;
int snd_subsystem_pmic_vreg_l2_on(void)
{
	int rc = 0;
	snddev_reg_l8_fab2200 = regulator_get(NULL, "8058_l8");
	if (IS_ERR(snddev_reg_l8_fab2200)) {
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
		printk(KERN_ERR "%s: vreg_enable failed(%s) = %d \n",__func__, "8058_l8", rc);
#endif
	}

	rc = regulator_set_voltage(snddev_reg_l8_fab2200, 1800000, 1800000);
	if (rc < 0)
	{
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
		printk(KERN_ERR "%s: vreg_enable failed(%s) = %d \n",__func__, "8058_l8", rc);
#endif
	}
	rc = regulator_enable(snddev_reg_l8_fab2200);
	if (rc < 0)
	{
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
		printk(KERN_ERR "%s: vreg_enable failed(%s) = %d \n",	__func__, "8058_l8", rc);
#endif
	}
	return rc;
}


/*==========================================================================
** snd_subsystem_pmic_vreg_l2_off
**=========================================================================*/
void snd_subsystem_pmic_vreg_l2_off(void)
{
	int rc = 0;

	if (!snddev_reg_l8_fab2200)
	{	
		rc = regulator_disable(snddev_reg_l8_fab2200);
		if (rc < 0)
		{
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
			printk(KERN_ERR "%s: vreg_enable failed(%s) = %d \n",__func__, "8058_l8", rc);
#endif
		}

		regulator_put(snddev_reg_l8_fab2200);

		snddev_reg_l8_fab2200 = NULL;
	}

}
#endif
 int config_GPIO_SUBSYSTEM_ENABLE(int enable)
{
	int rc;

	gpio_free(GPIO_SUBSYSTEM_ENABLE);

	if (enable) {
		rc = gpio_request(GPIO_SUBSYSTEM_ENABLE, "GPIO_SUBSYSTEM_ENABLE");
		if (rc) {
			pr_err("%s: spkr pamp gpio %d request"
			"failed\n", __func__, GPIO_SUBSYSTEM_ENABLE);
			return rc;
		}
		gpio_direction_output(GPIO_SUBSYSTEM_ENABLE, 1);
	} else {
		rc = gpio_request(GPIO_SUBSYSTEM_ENABLE, "GPIO_SUBSYSTEM_ENABLE");
		if (rc) {
			pr_err("%s: spkr pamp gpio %d request"
			"failed\n", __func__, GPIO_SUBSYSTEM_ENABLE);
			return rc;
		}
		gpio_direction_output(GPIO_SUBSYSTEM_ENABLE, 0);
	}
	return 0;
}


/*==========================================================================
** snd_subsystem_en_init
**=========================================================================*/
static void snd_subsystem_en_init(void)
{
	int rc = 0;
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	printk("[fab2200] @@@snd_subsystem_en_init.....audio_sub_en GPIO init \n");
#endif
       rc = gpio_tlmm_config(GPIO_CFG(GPIO_SUBSYSTEM_ENABLE, 0, GPIO_CFG_OUTPUT,	GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), GPIO_CFG_ENABLE);

       gpio_request(GPIO_SUBSYSTEM_ENABLE, "GPIO_SUBSYSTEM_ENABLE");
       gpio_direction_output(GPIO_SUBSYSTEM_ENABLE,1);
       config_GPIO_SUBSYSTEM_ENABLE(1);
	
	snd_subsystem_startup();

    	
	if(rc){
#ifdef CONFIG_FAB2200_DEBUG_PRINTK		
		printk("[fab2200] @@@snd_subsystem_en_init.....audio_sub_en GPIO init......Fail !!!");
#endif
	}	
}

/*==========================================================================
** snd_subsystem_startup
**=========================================================================*/
static void snd_subsystem_startup(void)
{
	snd_subsystem_pmic_vreg_l2_on();  //20110406 jhsong

       config_GPIO_SUBSYSTEM_ENABLE(1);
#ifdef CONFIG_FAB2200_DEBUG_PRINTK	   
	printk("[fab2200] @@@POWER ON \n");
#endif
}



/*==========================================================================
** snd_subsystem_shutdown
**=========================================================================*/
static void snd_subsystem_shutdown(void)
{

	snd_subsystem_pmic_vreg_l2_off();  //20110406 jhsong

	config_GPIO_SUBSYSTEM_ENABLE(0);
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	printk("[fab2200] ###POWER OFF \n");
#endif
}

/*==========================================================================
** snd_subsystem_sp_poweron
**=========================================================================*/
int snd_subsystem_sp_poweron(void)
{
	int status = 0;

	snd_subsystem_startup();
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028  //jmlee
	msm_snddev_nr_out_sel_A1028Reciever();  //20110328 jhsong : remove data leakage 
#endif

#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
	if(LockForTest){
		snd_extamp_api_SetVolume_ById(EXTAMP_SPKR_PHONE);
#ifdef CONFIG_FAB2200_DEBUG_PRINTK		
		printk("[fab2200]snd_subsystem_sp_poweron............cal tool (status:%d)\n", status);
#endif
	}
#endif
	status|=snd_subsystem_sp_input_system(SYSTEM_ON);
	status|= snd_subsystem_SetVolume();
	status|= snd_subsystem_sp_ng_ssmt(SYSTEM_ON);
	status|=snd_subsystem_common_system(SYSTEM_ON);
#ifdef CONFIG_FAB2200_DEBUG_PRINTK	
	printk("[fab2200]snd_subsystem_sp_poweron (status:%d)\n", status);
#endif

#if (BOARD_REV <= WS10)  // Presto NR CLK_IN ==> changed with ws20  // jmlee Audience clk err, so test code modify
        if(!jmlee_adience_clk_err_test)
      	{
#ifdef CONFIG_FAB2200_DEBUG_PRINTK      	
	      printk("[JMLEE LOG] snd_audience_a2020_api_Init ~~~~~~~ start \n");		  
#endif
             snd_audience_a2020_api_Init();
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	      printk("[JMLEE LOG] snd_audience_a2020_api_Init ~~~~~~~ end\n");
#endif
	      jmlee_adience_clk_err_test = 1;

       }
#endif
	return status;
}



/*==========================================================================
** snd_subsystem_hp_poweron
**=========================================================================*/
int snd_subsystem_hp_poweron(void)
{
	int status = 0;

	snd_subsystem_startup();

#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
	if(LockForTest){
		snd_extamp_api_SetVolume_ById(EXTAMP_HEADPHONE_STEREO);
#ifdef CONFIG_FAB2200_DEBUG_PRINTK		
		printk("[fab2200]snd_subsystem_hp_poweron ..............cal tool(status:%d)\n", status);
#endif
	}
#endif
	status|=snd_subsystem_hp_input_system(SYSTEM_ON);	
	status|=snd_subsystem_SetVolume();	
//	status|=snd_subsystem_Set_HP_Noise_Gate(SYSTEM_ON);
	status|=snd_subsystem_Set_HP_Noise_Gate(SYSTEM_OFF);  //20110413 jhsong : audio cal
	status|=snd_subsystem_common_system(SYSTEM_ON);
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	printk("[fab2200]snd_subsystem_hp_poweron (status:%d)\n", status);
#endif

	return status;
}

/*==========================================================================
** snd_subsystem_tty_hp_poweron
**=========================================================================*/
int snd_subsystem_tty_hp_poweron(void)
{
	int status = 0;

	snd_subsystem_startup();

#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
	if(LockForTest){
		snd_extamp_api_SetVolume_ById(EXTAMP_TTY_HEADPHONE);
#ifdef CONFIG_FAB2200_DEBUG_PRINTK		
		printk("[fab2200]snd_subsystem_tty_hp_poweron.............cal tool (status:%d)\n", status);
#endif
	}
#endif
	status|=snd_subsystem_tty_hp_input_system(SYSTEM_ON);	
	status|=snd_subsystem_SetVolume();	
//		status|=snd_subsystem_Set_tty_HP_Noise_Gate(SYSTEM_ON);
	status|=snd_subsystem_common_system(SYSTEM_ON);
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	printk("[fab2200]snd_subsystem_tty_hp_poweron (status:%d)\n", status);
#endif

	return status;
}

/*==========================================================================
** snd_subsystem_sp_hp_poweron
**=========================================================================*/
int snd_subsystem_sp_hp_poweron(void)
{
	int status = 0;

	snd_subsystem_startup();
#ifdef CONFIG_PANTECH_MINI_AUDIO_AUDIENCE1028  //jmlee
	msm_snddev_nr_out_sel_A1028Reciever();  //20110328 jhsong : remove data leakage 
#endif

#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
	if(LockForTest){
		snd_extamp_api_SetVolume_ById(EXTAMP_SPKR_PHONE_AND_HP_STEREO);
#ifdef CONFIG_FAB2200_DEBUG_PRINTK		
		printk("[fab2200]snd_subsystem_sp_hp_poweron.............cal tool (status:%d)\n", status);
#endif
	}
#endif
	status|=snd_subsystem_sp_hp_input_system(SYSTEM_ON);	
	status|=snd_subsystem_SetVolume();	
	status|=snd_subsystem_sp_ng_ssmt(SYSTEM_ON);
//	status|=snd_subsystem_Set_HP_Noise_Gate(SYSTEM_ON);
	status|=snd_subsystem_Set_HP_Noise_Gate(SYSTEM_OFF);  //20110413 jhsong : audio cal
	status|=snd_subsystem_common_system(SYSTEM_ON);
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	printk("[fab2200]snd_subsystem_sp_hp_poweron (status:%d)\n", status);
#endif

	return status;
}

/*==========================================================================
** snd_subsystem_nr_ft_poweron
**=========================================================================*/
int snd_subsystem_nr_ft_poweron(void)
{
	int status = 0;

	snd_subsystem_startup();

#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
	if(LockForTest){
		snd_extamp_api_SetVolume_ById(EXTAMP_NR_ON_FT);
#ifdef CONFIG_FAB2200_DEBUG_PRINTK		
		printk("[fab2200]snd_subsystem_nr_ft_poweron...........cal tool (status:%d)\n", status);
#endif
	}
#endif
	status|=snd_subsystem_nr_ft_input_system(SYSTEM_ON);	
	status|=snd_subsystem_SetVolume();	
//	status|=snd_subsystem_sp_ng_ssmt(SYSTEM_ON);
	status|=snd_subsystem_common_system(SYSTEM_ON);
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	printk("[fab2200]snd_subsystem_nr_ft_poweron (status:%d)\n", status);
#endif

	return status;
}

/*==========================================================================
** snd_subsystem_nr_off_ft_poweron
**=========================================================================*/
int snd_subsystem_nr_off_ft_poweron(void)
{
	int status = 0;

	snd_subsystem_startup();

#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
	if(LockForTest){
		snd_extamp_api_SetVolume_ById(EXTAMP_NR_OFF_FT);
#ifdef CONFIG_FAB2200_DEBUG_PRINTK		
		printk("[fab2200]snd_subsystem_nr_ft_poweron.............cal tool (status:%d)\n", status);
#endif
	}
#endif
	status|=snd_subsystem_nr_off_ft_input_system(SYSTEM_ON);	
	status|=snd_subsystem_SetVolume();	
	status|=snd_subsystem_nr_off_ft_ng_ssmt(SYSTEM_ON);
	status|=snd_subsystem_common_system(SYSTEM_ON);
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	printk("[fab2200]snd_subsystem_nr_ft_poweron (status:%d)\n", status);
#endif
	return status;
}

/*==========================================================================
** snd_subsystem_nr_dv_poweron
**=========================================================================*/
int snd_subsystem_nr_dv_poweron(void)
{
	int status = 0;

	snd_subsystem_startup();

#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
	if(LockForTest){
		snd_extamp_api_SetVolume_ById(EXTAMP_NR_ON_DV);
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
		printk("[fab2200]snd_subsystem_nr_dv_poweron.............cal tool (status:%d)\n", status);
#endif
	}
#endif
	status|=snd_subsystem_nr_dv_input_system(SYSTEM_ON);	
	status|=snd_subsystem_SetVolume();	
//	status|=snd_subsystem_sp_ng_ssmt(SYSTEM_ON);
	status|=snd_subsystem_common_system(SYSTEM_ON);
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	printk("[fab2200]snd_subsystem_nr_dv_poweron (status:%d)\n", status);
#endif
	return status;
}

/*==========================================================================
** snd_subsystem_nr_off_dv_poweron
**=========================================================================*/
int snd_subsystem_nr_off_dv_poweron(void)
{
	int status = 0;

	snd_subsystem_startup();

#ifdef FEATURE_SKY_SND_EXTAMPCTRL_FOR_APP    //20110310 jhsong : sub chip cal tool by youngsoo Lim
	if(LockForTest){
		snd_extamp_api_SetVolume_ById(EXTAMP_NR_OFF_DV);
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
		printk("[fab2200]snd_subsystem_nr_dv_poweron..............cal tool (status:%d)\n", status);
#endif
	}
#endif
	status|=snd_subsystem_nr_off_dv_input_system(SYSTEM_ON);	
	status|=snd_subsystem_SetVolume();	
	status|=snd_subsystem_nr_off_dv_ng_ssmt(SYSTEM_ON);
	status|=snd_subsystem_common_system(SYSTEM_ON);
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	printk("[fab2200]snd_subsystem_nr_dv_poweron (status:%d)\n", status);
#endif
	return status;
}

/*==========================================================================
** snd_subsystem_sp_powerdown
**=========================================================================*/
void snd_subsystem_sp_powerdown(void)
{
	int status = 0;

	// start gain mute
	snd_subsystem_SetMute(DEVICE_SPEAKER_RX, VOLUME_MUTE);

	// agc / nr set default
	status|= snd_subsystem_sp_ng_ssmt(SYSTEM_OFF);
	
	// input close
	status|= snd_subsystem_sp_input_system(SYSTEM_OFF);

	//Common Circuit Power down
	status|=snd_subsystem_common_system(SYSTEM_OFF);

	
	snd_subsystem_shutdown();
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	printk("[fab2200] Sound Power Down (status:%d)\n", status);
#endif
	//return status;
}



/*==========================================================================
** snd_subsystem_hp_powerdown
**=========================================================================*/
void snd_subsystem_hp_powerdown(void)
{
	int status = 0;

	// hpl, hpr volume mute
	snd_subsystem_SetMute(DEVICE_HEADSET_RX, VOLUME_MUTE);

	// hp noise gate set default
	snd_subsystem_Set_HP_Noise_Gate(SYSTEM_OFF);
	//input close
	status|= snd_subsystem_hp_input_system(SYSTEM_OFF);
	
	//Common Circuit Power down
	status|=snd_subsystem_common_system(SYSTEM_OFF);

	snd_subsystem_shutdown();

#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	printk("[fab2200]snd_subsystem_hp_powerdown (status:%d)\n", status);
#endif
	//return status;
}

/*==========================================================================
** snd_subsystem_tty_hp_powerdown
**=========================================================================*/
void snd_subsystem_tty_hp_powerdown(void)
{
	int status = 0;

	// hpl, hpr volume mute
	snd_subsystem_SetMute(DEVICE_TTY_HEADSET_MONO_RX, VOLUME_MUTE);

	// hp noise gate set default
	snd_subsystem_Set_tty_HP_Noise_Gate(SYSTEM_OFF);
	//input close
	status|= snd_subsystem_tty_hp_input_system(SYSTEM_OFF);
	
	//Common Circuit Power down
	status|=snd_subsystem_common_system(SYSTEM_OFF);

	snd_subsystem_shutdown();

#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	printk("[fab2200]snd_subsystem_hp_powerdown (status:%d)\n", status);
#endif
	//return status;
}

/*==========================================================================
** snd_subsystem_sp_hp_powerdown
**=========================================================================*/
void snd_subsystem_sp_hp_powerdown(void)
{
	int status = 0;

	// spk, hpl, hpr volume mute
	snd_subsystem_SetMute(DEVICE_SPEAKER_HEADSET_RX, VOLUME_MUTE);


	// sp agc / noise gate set default
	status|=snd_subsystem_sp_ng_ssmt(SYSTEM_OFF);

	// hp noise gate set default
	snd_subsystem_Set_HP_Noise_Gate(SYSTEM_OFF);

	//input close
	status|= snd_subsystem_sp_hp_input_system(SYSTEM_OFF);
	
	//Common Circuit Power down
	status|=snd_subsystem_common_system(SYSTEM_OFF);


	snd_subsystem_shutdown();

#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	printk("[fab2200]snd_subsystem_hp_powerdown (status:%d)\n", status);
#endif
	//return status;
}

/*==========================================================================
** snd_subsystem_nr_ft_powerdown
**=========================================================================*/
void snd_subsystem_nr_ft_powerdown(void)
{
	int status = 0;

	// start gain mute
	snd_subsystem_SetMute(DEVICE_SND_DEVICE_NR_FT_RX, VOLUME_MUTE);

	// agc / nr set default
//	status|= snd_subsystem_sp_ng_ssmt(SYSTEM_OFF);
	
	// input close
	status|= snd_subsystem_nr_ft_input_system(SYSTEM_OFF);

	//Common Circuit Power down
	status|=snd_subsystem_common_system(SYSTEM_OFF);

	
	snd_subsystem_shutdown();
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	printk("[fab2200] Sound Power Down (status:%d)\n", status);
#endif
	//return status;
}

/*==========================================================================
** snd_subsystem_nr_off_ft_powerdown
**=========================================================================*/
void snd_subsystem_nr_off_ft_powerdown(void)
{
	int status = 0;

	// start gain mute
	snd_subsystem_SetMute(DEVICE_SND_DEVICE_NR_OFF_FT_RX, VOLUME_MUTE);

	// agc / nr set default
	status|= snd_subsystem_nr_off_ft_ng_ssmt(SYSTEM_OFF);
	
	// input close
	status|= snd_subsystem_nr_off_ft_input_system(SYSTEM_OFF);

	//Common Circuit Power down
	status|=snd_subsystem_common_system(SYSTEM_OFF);

	
	snd_subsystem_shutdown();
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	printk("[fab2200] Sound Power Down (status:%d)\n", status);
#endif
	//return status;
}

/*==========================================================================
** snd_subsystem_nr_dv_powerdown
**=========================================================================*/
void snd_subsystem_nr_dv_powerdown(void)
{
	int status = 0;

	// start gain mute
	snd_subsystem_SetMute(DEVICE_SND_DEVICE_NR_DV_RX, VOLUME_MUTE);

	// agc / nr set default
//	status|= snd_subsystem_sp_ng_ssmt(SYSTEM_OFF);
	
	// input close
	status|= snd_subsystem_nr_dv_input_system(SYSTEM_OFF);

	//Common Circuit Power down
	status|=snd_subsystem_common_system(SYSTEM_OFF);

	
	snd_subsystem_shutdown();
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	printk("[fab2200] Sound Power Down (status:%d)\n", status);
#endif
	//return status;
}

/*==========================================================================
** snd_subsystem_nr_off_dv_powerdown
**=========================================================================*/
void snd_subsystem_nr_off_dv_powerdown(void)
{
	int status = 0;

	// start gain mute
	snd_subsystem_SetMute(DEVICE_SND_DEVICE_NR_OFF_DV_RX, VOLUME_MUTE);

	// agc / nr set default
	status|= snd_subsystem_nr_off_dv_ng_ssmt(SYSTEM_OFF);
	
	// input close
	status|= snd_subsystem_nr_off_dv_input_system(SYSTEM_OFF);

	//Common Circuit Power down
	status|=snd_subsystem_common_system(SYSTEM_OFF);

	
	snd_subsystem_shutdown();
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	printk("[fab2200] Sound Power Down (status:%d)\n", status);
#endif
	//return status;
}

/*==========================================================================
** snd_subsystem_common_system
**=========================================================================*/
static int snd_subsystem_common_system(int en)
{
	int status = 0;
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	int reg_value=0;
#endif
	if(en){
		if((snd_subsystem_get_device() == DEVICE_HEADSET_RX) || (snd_subsystem_get_device() == DEVICE_SPEAKER_HEADSET_RX)
			|| (snd_subsystem_get_device() == DEVICE_TTY_HEADSET_MONO_RX) )
		{
			tSubsystemInfo.power_info |= ((HIZL_VALUE & BIT_SET_VALUE) | (HIZR_VALUE & BIT_SET_VALUE));  // hp high impedance on
			status|=snd_subsystem_i2c_write(POWER_REG, tSubsystemInfo.power_info);

			tSubsystemInfo.power_info |= (SHDNB_VALUE & BIT_SET_VALUE);  // normal operation
			status|=snd_subsystem_i2c_write(POWER_REG, tSubsystemInfo.power_info);

			tSubsystemInfo.power_info &= ~((HIZL_VALUE & BIT_SET_VALUE) | (HIZR_VALUE & BIT_SET_VALUE));  // hp high impedance off		
			status|=snd_subsystem_i2c_write(POWER_REG, tSubsystemInfo.power_info);  

			if(snd_subsystem_get_device() == DEVICE_SPEAKER_HEADSET_RX)
				status |= snd_subsystem_Set_SP_AGC(en);  //20110524 jhsong change sequence

		}else{
			tSubsystemInfo.power_info |= (SHDNB_VALUE & BIT_SET_VALUE);   // normal operation
			
			status|=snd_subsystem_i2c_write(POWER_REG, tSubsystemInfo.power_info); 

			status |= snd_subsystem_Set_SP_AGC(en);  //20110524 jhsong change sequence
		}
	}
	else{
		if((snd_subsystem_get_device() == DEVICE_HEADSET_RX) || (snd_subsystem_get_device() == DEVICE_SPEAKER_HEADSET_RX)
			|| (snd_subsystem_get_device() == DEVICE_TTY_HEADSET_MONO_RX) )
		{
			if(snd_subsystem_get_device() == DEVICE_SPEAKER_HEADSET_RX)
				status |= snd_subsystem_Set_SP_AGC(en);  //20110524 jhsong change sequence

			tSubsystemInfo.power_info |= ((HIZL_VALUE & BIT_SET_VALUE) | (HIZR_VALUE & BIT_SET_VALUE));   // hp high impedance on
			status|=snd_subsystem_i2c_write(POWER_REG, tSubsystemInfo.power_info);  

			tSubsystemInfo.power_info &= BIT_CLEAR_VALUE;  // low power shutdown mode
			status|=snd_subsystem_i2c_write(POWER_REG, tSubsystemInfo.power_info);  

			tSubsystemInfo.power_info &= ~((HIZL_VALUE & BIT_SET_VALUE) | (HIZR_VALUE & BIT_SET_VALUE));  // hp high impedance off	
			status|=snd_subsystem_i2c_write(POWER_REG, tSubsystemInfo.power_info);  
		}else{
			status |= snd_subsystem_Set_SP_AGC(en);  //20110524 jhsong change sequence

			tSubsystemInfo.power_info &= BIT_CLEAR_VALUE; // low power shutdown mode
			
			status|=snd_subsystem_i2c_write(POWER_REG, tSubsystemInfo.power_info );  
		}
	}
#ifdef CONFIG_FAB2200_DEBUG_PRINTK
	printk("[fab2200]COMMON SYSTEM(en:%d, reg_value:%d, status:%d)\n", en, reg_value, status);
#endif
	return status;
}

/*==========================================================================
** snd_subsystem_sp_input_system
**=========================================================================*/
static int snd_subsystem_sp_input_system(int en)
{
	int status = 0;

	if(en){
		current_device = DEVICE_SPEAKER_RX;

		tSubsystemInfo.power_info |= (BYPEN_VALUE&BIT_SET_VALUE);  //20110406 jhsong : seperate line out and nr spk
		status|=snd_subsystem_i2c_write(POWER_REG, tSubsystemInfo.power_info );  //20110406 jhsong : seperate line out and nr spk
		
		tSubsystemInfo.diff_pga_info |= (DIFFIN43_VALUE & BIT_SET_VALUE);  // in4,3 configure to differential pair
		tSubsystemInfo.input_spk_hp_zc_info |= ((SEL_SPA4_VALUE & BIT_SET_VALUE)/* | (SEL_SPA3_VALUE & BIT_SET_VALUE)*/);   // in4,3 configure to mono (right)

		tSubsystemInfo.ssmt_info &= BIT_CLEAR_VALUE;	 //20110208 jhsong test	
		status|=snd_subsystem_i2c_write(SSMT_REG, tSubsystemInfo.ssmt_info);   //20110208 jhsong test
		
		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info); 
		status|=snd_subsystem_i2c_write(INPUT_SPK_HP_ZC_REG, tSubsystemInfo.input_spk_hp_zc_info); 

#ifdef FEATURE_SKY_SND_EXTAMPCTRL  //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.diff_pga_info |= (tSubsystemInfo_cal.diff_pga_info<<PGAIN43_VALUE);
#ifdef CONFIG_FAB2200_DEBUG_PRINTK			
			printk("@#@#snd_subsystem_sp_input_system.....lockforTest......preGain : 0x%x",tSubsystemInfo.diff_pga_info);
#endif
		}else
#endif
		{
			tSubsystemInfo.diff_pga_info |= (0x01<<PGAIN43_VALUE);  //20110706 jmlee speaker PGA("pga_info") 3dB
#ifdef CONFIG_FAB2200_DEBUG_PRINTK			
			printk("@#@#snd_subsystem_sp_input_system...........preGain : 0x%x",tSubsystemInfo.diff_pga_info);
#endif
		}
		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info);

		tSubsystemInfo.spk_ramp_zc_info |= (SP_ZCD_VALUE & BIT_SET_VALUE);   // spk zero cross detection enable
		status|=snd_subsystem_i2c_write(SPK_RAMP_ZC_REG, tSubsystemInfo.spk_ramp_zc_info);

	}else{
		tSubsystemInfo.diff_pga_info &= BIT_CLEAR_VALUE;   // in4,3 configure to differential
		tSubsystemInfo.input_spk_hp_zc_info &= BIT_CLEAR_VALUE;  // in4,3 configure to differential
		tSubsystemInfo.spk_ramp_zc_info &= BIT_CLEAR_VALUE;   // spk zero cross detection disable	
		
		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info); 
		status|=snd_subsystem_i2c_write(INPUT_SPK_HP_ZC_REG, tSubsystemInfo.input_spk_hp_zc_info);  
		status|=snd_subsystem_i2c_write(SPK_RAMP_ZC_REG, tSubsystemInfo.spk_ramp_zc_info); 
	}

	return status;
}

/*==========================================================================
** snd_subsystem_nr_ft_input_system
**=========================================================================*/
static int snd_subsystem_nr_ft_input_system(int en)
{
	int status = 0;

	if(en){
		current_device = DEVICE_SND_DEVICE_NR_FT_RX;

		tSubsystemInfo.diff_pga_info |= (DIFFIN43_VALUE & BIT_SET_VALUE);  // in4,3 configure to differential pair
		tSubsystemInfo.input_spk_hp_zc_info |= ((SEL_SPA4_VALUE & BIT_SET_VALUE)/* | (SEL_SPA3_VALUE & BIT_SET_VALUE)*/);   // in4,3 configure to mono (right)

		tSubsystemInfo.ssmt_info &= BIT_CLEAR_VALUE;	 //20110208 jhsong test	
		status|=snd_subsystem_i2c_write(SSMT_REG, tSubsystemInfo.ssmt_info);   //20110208 jhsong test
		
		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info); 
		status|=snd_subsystem_i2c_write(INPUT_SPK_HP_ZC_REG, tSubsystemInfo.input_spk_hp_zc_info); 

#ifdef FEATURE_SKY_SND_EXTAMPCTRL  //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.diff_pga_info |= (tSubsystemInfo_cal.diff_pga_info<<PGAIN43_VALUE);
		}else
#endif
		{
			tSubsystemInfo.diff_pga_info |= (0x03<<PGAIN43_VALUE);   // 20110810 jmlee h/w test bin 9dB (0x03)    //20110802 jmlee PGA("pga_info") 6dB
		}
		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info);

		tSubsystemInfo.spk_ramp_zc_info |= (SP_ZCD_VALUE & BIT_SET_VALUE);   // spk zero cross detection enable
		status|=snd_subsystem_i2c_write(SPK_RAMP_ZC_REG, tSubsystemInfo.spk_ramp_zc_info);
	}else{
		tSubsystemInfo.diff_pga_info &= BIT_CLEAR_VALUE;   // in4,3 configure to differential
		tSubsystemInfo.input_spk_hp_zc_info &= BIT_CLEAR_VALUE;  // in4,3 configure to differential
		tSubsystemInfo.spk_ramp_zc_info &= BIT_CLEAR_VALUE;   // spk zero cross detection disable	
		
		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info); 
		status|=snd_subsystem_i2c_write(INPUT_SPK_HP_ZC_REG, tSubsystemInfo.input_spk_hp_zc_info);  
		status|=snd_subsystem_i2c_write(SPK_RAMP_ZC_REG, tSubsystemInfo.spk_ramp_zc_info); 
	}

	return status;
}

/*==========================================================================
** snd_subsystem_nr_off_ft_input_system
**=========================================================================*/
static int snd_subsystem_nr_off_ft_input_system(int en)
{
	int status = 0;

	if(en){
		current_device = DEVICE_SND_DEVICE_NR_OFF_FT_RX;

		tSubsystemInfo.power_info |= (BYPEN_VALUE&BIT_SET_VALUE);  //20110406 jhsong : seperate line out and nr spk
		status|=snd_subsystem_i2c_write(POWER_REG, tSubsystemInfo.power_info );  //20110406 jhsong : seperate line out and nr spk

		tSubsystemInfo.diff_pga_info |= (DIFFIN43_VALUE & BIT_SET_VALUE);  // in4,3 configure to differential pair
		tSubsystemInfo.input_spk_hp_zc_info |= ((SEL_SPA4_VALUE & BIT_SET_VALUE)/* | (SEL_SPA3_VALUE & BIT_SET_VALUE)*/);   // in4,3 configure to mono (right)

		tSubsystemInfo.ssmt_info &= BIT_CLEAR_VALUE;	 //20110208 jhsong test	
		status|=snd_subsystem_i2c_write(SSMT_REG, tSubsystemInfo.ssmt_info);   //20110208 jhsong test
		
		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info); 
		status|=snd_subsystem_i2c_write(INPUT_SPK_HP_ZC_REG, tSubsystemInfo.input_spk_hp_zc_info); 

#ifdef FEATURE_SKY_SND_EXTAMPCTRL  //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.diff_pga_info |= (tSubsystemInfo_cal.diff_pga_info<<PGAIN43_VALUE);
		}else
#endif
		{
			tSubsystemInfo.diff_pga_info |= (0x03<<PGAIN43_VALUE); // 20110810 jmlee h/w test bin 9dB (0x03)  //20110802 jmlee PGA("pga_info") 6dB
		}
		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info);

		tSubsystemInfo.spk_ramp_zc_info |= (SP_ZCD_VALUE & BIT_SET_VALUE);   // spk zero cross detection enable
		status|=snd_subsystem_i2c_write(SPK_RAMP_ZC_REG, tSubsystemInfo.spk_ramp_zc_info);
	}else{
		tSubsystemInfo.diff_pga_info &= BIT_CLEAR_VALUE;   // in4,3 configure to differential
		tSubsystemInfo.input_spk_hp_zc_info &= BIT_CLEAR_VALUE;  // in4,3 configure to differential
		tSubsystemInfo.spk_ramp_zc_info &= BIT_CLEAR_VALUE;   // spk zero cross detection disable	
		
		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info); 
		status|=snd_subsystem_i2c_write(INPUT_SPK_HP_ZC_REG, tSubsystemInfo.input_spk_hp_zc_info);  
		status|=snd_subsystem_i2c_write(SPK_RAMP_ZC_REG, tSubsystemInfo.spk_ramp_zc_info); 
	}

	return status;
}

/*==========================================================================
** snd_subsystem_nr_dv_input_system
**=========================================================================*/
static int snd_subsystem_nr_dv_input_system(int en)
{
	int status = 0;

	if(en){
		current_device = DEVICE_SND_DEVICE_NR_DV_RX;

		tSubsystemInfo.diff_pga_info |= (DIFFIN43_VALUE & BIT_SET_VALUE);  // in4,3 configure to differential pair
		tSubsystemInfo.input_spk_hp_zc_info |= ((SEL_SPA4_VALUE & BIT_SET_VALUE)/* | (SEL_SPA3_VALUE & BIT_SET_VALUE)*/);   // in4,3 configure to mono (right)

		tSubsystemInfo.ssmt_info &= BIT_CLEAR_VALUE;	 //20110208 jhsong test	
		status|=snd_subsystem_i2c_write(SSMT_REG, tSubsystemInfo.ssmt_info);   //20110208 jhsong test
		
		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info); 
		status|=snd_subsystem_i2c_write(INPUT_SPK_HP_ZC_REG, tSubsystemInfo.input_spk_hp_zc_info); 

#ifdef FEATURE_SKY_SND_EXTAMPCTRL  //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.diff_pga_info |= (tSubsystemInfo_cal.diff_pga_info<<PGAIN43_VALUE);
		}else
#endif
		{
			tSubsystemInfo.diff_pga_info |= (0x03<<PGAIN43_VALUE); // 20110810 jmlee h/w test bin 9dB (0x03)  //20110802 jmlee PGA("pga_info") 6dB
		}
		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info);

		tSubsystemInfo.spk_ramp_zc_info |= (SP_ZCD_VALUE & BIT_SET_VALUE);   // spk zero cross detection enable
		status|=snd_subsystem_i2c_write(SPK_RAMP_ZC_REG, tSubsystemInfo.spk_ramp_zc_info);
	}else{
		tSubsystemInfo.diff_pga_info &= BIT_CLEAR_VALUE;   // in4,3 configure to differential
		tSubsystemInfo.input_spk_hp_zc_info &= BIT_CLEAR_VALUE;  // in4,3 configure to differential
		tSubsystemInfo.spk_ramp_zc_info &= BIT_CLEAR_VALUE;   // spk zero cross detection disable	
		
		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info); 
		status|=snd_subsystem_i2c_write(INPUT_SPK_HP_ZC_REG, tSubsystemInfo.input_spk_hp_zc_info);  
		status|=snd_subsystem_i2c_write(SPK_RAMP_ZC_REG, tSubsystemInfo.spk_ramp_zc_info); 
	}

	return status;
}

/*==========================================================================
** snd_subsystem_nr_off_dv_input_system
**=========================================================================*/
static int snd_subsystem_nr_off_dv_input_system(int en)
{
	int status = 0;

	if(en){
		current_device = DEVICE_SND_DEVICE_NR_OFF_DV_RX;

		tSubsystemInfo.power_info |= (BYPEN_VALUE&BIT_SET_VALUE);  //20110406 jhsong : seperate line out and nr spk
		status|=snd_subsystem_i2c_write(POWER_REG, tSubsystemInfo.power_info );  //20110406 jhsong : seperate line out and nr spk

		tSubsystemInfo.diff_pga_info |= (DIFFIN43_VALUE & BIT_SET_VALUE);  // in4,3 configure to differential pair
		tSubsystemInfo.input_spk_hp_zc_info |= ((SEL_SPA4_VALUE & BIT_SET_VALUE)/* | (SEL_SPA3_VALUE & BIT_SET_VALUE)*/);   // in4,3 configure to mono (right)

		tSubsystemInfo.ssmt_info &= BIT_CLEAR_VALUE;	 //20110208 jhsong test	
		status|=snd_subsystem_i2c_write(SSMT_REG, tSubsystemInfo.ssmt_info);   //20110208 jhsong test
		
		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info); 
		status|=snd_subsystem_i2c_write(INPUT_SPK_HP_ZC_REG, tSubsystemInfo.input_spk_hp_zc_info); 

#ifdef FEATURE_SKY_SND_EXTAMPCTRL  //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.diff_pga_info |= (tSubsystemInfo_cal.diff_pga_info<<PGAIN43_VALUE);
		}else
#endif
		{
			tSubsystemInfo.diff_pga_info |= (0x03<<PGAIN43_VALUE);  // 20110810 jmlee h/w test bin 9dB (0x03)  //20110802 jmlee PGA("pga_info") 6dB
		}
		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info);

		tSubsystemInfo.spk_ramp_zc_info |= (SP_ZCD_VALUE & BIT_SET_VALUE);   // spk zero cross detection enable
		status|=snd_subsystem_i2c_write(SPK_RAMP_ZC_REG, tSubsystemInfo.spk_ramp_zc_info);
	}else{
		tSubsystemInfo.diff_pga_info &= BIT_CLEAR_VALUE;   // in4,3 configure to differential
		tSubsystemInfo.input_spk_hp_zc_info &= BIT_CLEAR_VALUE;  // in4,3 configure to differential
		tSubsystemInfo.spk_ramp_zc_info &= BIT_CLEAR_VALUE;   // spk zero cross detection disable	
		
		status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info); 
		status|=snd_subsystem_i2c_write(INPUT_SPK_HP_ZC_REG, tSubsystemInfo.input_spk_hp_zc_info);  
		status|=snd_subsystem_i2c_write(SPK_RAMP_ZC_REG, tSubsystemInfo.spk_ramp_zc_info); 
	}

	return status;
}

/*==========================================================================
** snd_subsystem_sp_ng_ssmt
**=========================================================================*/
static int snd_subsystem_sp_ng_ssmt(int en)
{
	int status = 0;

	status|= snd_subsystem_Set_SP_ERC(en);
//	status|= snd_subsystem_Set_SP_AGC(en);
//	status|= snd_subsystem_Set_SP_Noise_Gate(en);
	status|= snd_subsystem_Set_SP_SSMT(en);

	return status;
}

/*==========================================================================
** snd_subsystem_nr_off_ft_ng_ssmt
**=========================================================================*/
static int snd_subsystem_nr_off_ft_ng_ssmt(int en)
{
	int status = 0;

	status|= snd_subsystem_Set_SP_ERC(en);
//	status|= snd_subsystem_Set_SP_AGC(en);
//	status|= snd_subsystem_Set_SP_Noise_Gate(en);
	status|= snd_subsystem_Set_SP_SSMT(en);

	return status;
}

/*==========================================================================
** snd_subsystem_nr_off_dv_ng_ssmt
**=========================================================================*/
static int snd_subsystem_nr_off_dv_ng_ssmt(int en)
{
	int status = 0;

	status|= snd_subsystem_Set_SP_ERC(en);
//	status|= snd_subsystem_Set_SP_AGC(en);
//	status|= snd_subsystem_Set_SP_Noise_Gate(en);
	status|= snd_subsystem_Set_SP_SSMT(en);

	return status;
}

/*==========================================================================
** snd_subsystem_hp_input_system
**=========================================================================*/
static int snd_subsystem_hp_input_system(int en)
{
	int status = 0;

	if(en){
		current_device = DEVICE_HEADSET_RX;

		tSubsystemInfo.diff_pga_info |= (DIFFIN43_VALUE & BIT_SET_VALUE);  // in2,1 configure to independent
		tSubsystemInfo.diff_pga_info &= ~(DIFFIN21_VALUE & BIT_SET_VALUE);  // in2,1 configure to independent
		tSubsystemInfo.input_hp_info |= ((SEL_HPR2_VALUE & BIT_SET_VALUE) | (SEL_HPL1_VALUE & BIT_SET_VALUE));  // hpl in1, hpr in2
#ifdef FEATURE_SKY_SND_EXTAMPCTRL  //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.input_spk_hp_zc_info |= ((HP_ZCD_VALUE & BIT_SET_VALUE) | tSubsystemInfo_cal.input_spk_hp_zc_info);  // hp zero cross detection enable
		}else	
#endif		
		{
			tSubsystemInfo.input_spk_hp_zc_info |= ((HP_ZCD_VALUE & BIT_SET_VALUE) 
														| (RAMP_SPEED_0_25MS<<HPRAMP_SPEED_VALUE) | (BIT_SET_VALUE&HP_RAMP_VALUE));  // hp zero cross detection enable
		}		
	}else{
		tSubsystemInfo.diff_pga_info &= BIT_CLEAR_VALUE;  // in2,1 configure to independent
		tSubsystemInfo.input_hp_info &= BIT_CLEAR_VALUE;
		tSubsystemInfo.input_spk_hp_zc_info &= BIT_CLEAR_VALUE;	
	}

	status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info);  
	status|=snd_subsystem_i2c_write(INPUT_HP_REG, tSubsystemInfo.input_hp_info);  
	status|=snd_subsystem_i2c_write(INPUT_SPK_HP_ZC_REG, tSubsystemInfo.input_spk_hp_zc_info);  

	return status;
}


/*==========================================================================
** snd_subsystem_tty_hp_input_system
**=========================================================================*/
static int snd_subsystem_tty_hp_input_system(int en)
{
	int status = 0;

	if(en){
		current_device = DEVICE_TTY_HEADSET_MONO_RX;

		tSubsystemInfo.diff_pga_info |= (DIFFIN43_VALUE & BIT_SET_VALUE);  // in2,1 configure to independent
		tSubsystemInfo.diff_pga_info &= ~(DIFFIN21_VALUE & BIT_SET_VALUE);  // in2,1 configure to independent
		tSubsystemInfo.input_hp_info |= ((SEL_HPR2_VALUE & BIT_SET_VALUE) | (SEL_HPL1_VALUE & BIT_SET_VALUE));  // hpl in1, hpr in2
#ifdef FEATURE_SKY_SND_EXTAMPCTRL  //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.input_spk_hp_zc_info |= ((HP_ZCD_VALUE & BIT_SET_VALUE) | tSubsystemInfo_cal.input_spk_hp_zc_info);  // hp zero cross detection enable
		}else
#endif		
		{
			tSubsystemInfo.input_spk_hp_zc_info |= ((HP_ZCD_VALUE & BIT_SET_VALUE) 
														| (BIT_CLEAR_VALUE<<HPRAMP_SPEED_VALUE) | (BIT_CLEAR_VALUE&HP_RAMP_VALUE));  // hp zero cross detection enable
		}
	}else{
		tSubsystemInfo.diff_pga_info &= BIT_CLEAR_VALUE;  // in2,1 configure to independent
		tSubsystemInfo.input_hp_info &= BIT_CLEAR_VALUE;
		tSubsystemInfo.input_spk_hp_zc_info &= BIT_CLEAR_VALUE;	
	}

	status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info);  
	status|=snd_subsystem_i2c_write(INPUT_HP_REG, tSubsystemInfo.input_hp_info);  
	status|=snd_subsystem_i2c_write(INPUT_SPK_HP_ZC_REG, tSubsystemInfo.input_spk_hp_zc_info);  

	return status;
}

/*==========================================================================
** snd_subsystem_sp_hp_input_system
**=========================================================================*/
static int snd_subsystem_sp_hp_input_system(int en)
{
	int status = 0;

	if(en){
		current_device = DEVICE_SPEAKER_HEADSET_RX;

		tSubsystemInfo.power_info |= (BYPEN_VALUE&BIT_SET_VALUE);  //20110406 jhsong : seperate line out and nr spk
		status|=snd_subsystem_i2c_write(POWER_REG, tSubsystemInfo.power_info );  //20110406 jhsong : seperate line out and nr spk

		tSubsystemInfo.diff_pga_info &= ~(DIFFIN43_VALUE & BIT_SET_VALUE);  // in4,3 configure to sing ended
		tSubsystemInfo.diff_pga_info &= ~(DIFFIN21_VALUE & BIT_SET_VALUE);  // in2,1 configure to independent
#ifdef FEATURE_SKY_SND_EXTAMPCTRL  //20110310 jhsong : sub chip cal tool by youngsoo Lim
		if(LockForTest){
			tSubsystemInfo.input_spk_hp_zc_info |= ((SEL_SPA4_VALUE & BIT_SET_VALUE) | (SEL_SPA3_VALUE & BIT_SET_VALUE)
													|(HP_ZCD_VALUE & BIT_SET_VALUE) 	| tSubsystemInfo_cal.input_spk_hp_zc_info);  // hp zero cross detection enable
		}else
#endif		
		{
			tSubsystemInfo.input_spk_hp_zc_info |= ((SEL_SPA4_VALUE & BIT_SET_VALUE) | (SEL_SPA3_VALUE & BIT_SET_VALUE) |(HP_ZCD_VALUE & BIT_SET_VALUE) 
													| (RAMP_SPEED_0_25MS<<HPRAMP_SPEED_VALUE) | (BIT_SET_VALUE&HP_RAMP_VALUE));  // hp zero cross detection enable
		}
		tSubsystemInfo.input_hp_info |= ((SEL_HPR2_VALUE & BIT_SET_VALUE) | (SEL_HPL1_VALUE & BIT_SET_VALUE));  // hpl in1, hpr in2
//		tSubsystemInfo.input_spk_hp_zc_info |= (HP_ZCD_VALUE & BIT_SET_VALUE);  // hp zero cross detection enable
		
	}else{
		tSubsystemInfo.diff_pga_info &= BIT_CLEAR_VALUE;  // in2,1 configure to independent
		tSubsystemInfo.input_spk_hp_zc_info &= BIT_CLEAR_VALUE;	
		tSubsystemInfo.input_hp_info &= BIT_CLEAR_VALUE;
	}

	status|=snd_subsystem_i2c_write(DIFF_PGA_REG, tSubsystemInfo.diff_pga_info);  
	status|=snd_subsystem_i2c_write(INPUT_SPK_HP_ZC_REG, tSubsystemInfo.input_spk_hp_zc_info);  
	status|=snd_subsystem_i2c_write(INPUT_HP_REG, tSubsystemInfo.input_hp_info);  

	return status;
}

/*==========================================================================
** snd_subsystem_i2c_write
**=========================================================================*/
static int snd_subsystem_i2c_write(u8 reg, u8 data)
{	
	static int ret = 0;
	unsigned char buf[2];
	//u8 value=0;

	struct i2c_msg msg[1];

	if(!fab2200_i2c_client){
		return -1;
	}

	buf[0] = (unsigned char)reg;
	buf[1] = (unsigned char)data;

	msg[0].addr = fab2200_i2c_client->addr;
	msg[0].flags = 0;
	msg[0].len = 2;
	msg[0].buf = buf;
	
	ret = i2c_transfer(fab2200_i2c_client->adapter, msg, 1);
	if (ret < 0) {
#ifdef CONFIG_FAB2200_DEBUG_PRINTK		
		printk("[fab2200] snd_subsystem_i2c_write fail !!!! ret : %d \n", ret);
#endif
		return -1;
	}
	//printk("[fab2200] snd_subsystem_i2c_write %d \n", ret);
	return 0;
}



/*==========================================================================
** snd_subsystem_i2c_read
**=========================================================================*/
/*
static int snd_subsystem_i2c_read(u8 reg, u8 *data)
{
	static int ret = 0;
	unsigned char buf[1];
	struct i2c_msg msgs[2];

	if(!fab2200_i2c_client){
		return -1;
	}

	buf[0] = reg;

	msgs[0].addr = fab2200_i2c_client->addr;
	msgs[0].flags = 0;
	msgs[0].len = 1;
	msgs[0].buf = buf;

	msgs[1].addr = fab2200_i2c_client->addr;
	msgs[1].flags = I2C_M_RD;
	msgs[1].len = 1;
	msgs[1].buf = buf;

	ret = i2c_transfer(fab2200_i2c_client->adapter, msgs, 2);
	if ( ret < 0) {
		return -1;
	}

	*data = (u8)buf[0];

	printk("[fab2200] snd_subsystem_i2c_read %d \n", ret);

	return 0;
}
*/
#ifdef FEATURE_SKY_SND_EXTAMPCTRL  //20110310 jhsong : sub chip cal tool by youngsoo Lim
static int __init aud_sub_init(void)
{
	return misc_register(&miscdev_snd_fab);
}

device_initcall(aud_sub_init);
#endif

