/************************************************************************************************
**
**    AUDIO EXTERNAL AMP
**
**    FILE
**        Sky_snd_ext_amp_max970001.c
**
**    DESCRIPTION
**        This file contains audio external amp api
**          
**          void snd_extamp_api_Init()
**          void snd_extamp_api_SetPath()
**          void snd_extamp_api_SetVolume()
**          void snd_extamp_api_Sleep()
**
**    Copyright (c) 2010 by PANTECH Incorporated.  All Rights Reserved.
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
//#include "dal.h"
//#include "dal_audio.h"
#include "sky_snd_ext_amp_max97001.h"

#ifdef FEATURE_SKY_SND_AUDIO_TEST_COMMAND
#include <asm/ioctls.h>
#include <linux/uaccess.h>
#endif

/************************************************************************************************
** Definition
*************************************************************************************************/
/* Default Register Value */ 
#define DEFAULT_INPUT_MODE_CTL_REG_VAL  ( INADIFF_STEREO_SINGLE | INBDIFF_DIFFERENTIAL | PGAINA_P_0_DB | PGAINB_P_0_DB ) 
#define DEFAULT_HP_MIXER_CTL_REG_VAL ( HPLMIX_NO_INPUT | HPRMIX_NO_INPUT )
#define DEFAULT_SP_MIXER_CTL_REG_VAL  ( SPKMIX_NO_INPUT )
#define DEFAULT_HPLM_VOLUME_CTL_REG_VAL  ( ZCD_DIS | SLEW_DIS | HPLM_MUTE | HPLVOL_M_64_DB )  //mute
#define DEFAULT_HPRM_VOLUME_CTL_REG_VAL  ( HPGAIN_P_0_DB | HPRM_MUTE | HPRVOL_M_64_DB )  //mute
#define DEFAULT_SPK_VOLUME_CTL_REG_VAL  ( FFM_FF_MODE  | SPKM_MUTE | SPKVOL_M_30_DB )  //mute
#define DEFAULT_DISTORTION_LIMITER_CTL_REG_VAL  ( THDCLP_DIS  | THDT1_1_DOT_4_SEC )
#define DEFAULT_POWER_MANAGEMENT_CTL_REG_VAL  ( SHDN_DIS  | LPMODE_DIS | SPKEN_DIS | HPLEN_DIS | HPREN_DIS | BYPEN_CLOSE )
#define DEFAULT_CHARGE_PUMP_CTL_REG_VAL ( CPSEL_0 | F_SUPPLY_MODE )

/************************************************************************************************
** Variables
*************************************************************************************************/
/*static*/ extamp_info_t tExtampInfo;
static unsigned char bHeadphonePath = 0;
static struct i2c_client *max97001_i2c_client = NULL;
static uint32_t CurrDeviceId = 0xffffffff;
static int CurrDevicePwrStatus = 0;


#ifdef FEATURE_SKY_SND_AUDIO_TEST_COMMAND
#define SND_AMP_IOCTL_MAGIC 'a'
#define SND_SKY_EXTAMP_WRITE  _IOW(SND_AMP_IOCTL_MAGIC, 0, unsigned)
#define SND_SKY_EXTAMP_READ  _IOW(SND_AMP_IOCTL_MAGIC, 1, unsigned)
static int current_reg;
static int current_regval;
#endif

#define SND_SKY_EXTAMP_SET_MODE  _IOW(SND_AMP_IOCTL_MAGIC, 3, unsigned)
#define SND_SKY_SET_CALLMODE _IOW(SND_AMP_IOCTL_MAGIC, 4, unsigned)
#define SND_SKY_GET_CALLMODE _IOW(SND_AMP_IOCTL_MAGIC, 5, unsigned)
#define SND_SKY_SET_MIC_MUTE_CTRL _IOW(SND_AMP_IOCTL_MAGIC, 6, unsigned)
#define SND_SKY_SET_AUTOANSWER _IOW(SND_AMP_IOCTL_MAGIC, 7, unsigned)
static int curMode;
static call_mode_param  curCallMode;


/************************************************************************************************
** Declaration
*************************************************************************************************/
static int snd_extamp_i2c_write(u8 reg, u8 data);
//static int snd_extamp_i2c_read(u8 reg, u8 *data);
static int max97001_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int __exit max97001_remove(struct i2c_client *client);
static void max97001_shutdown(struct i2c_client *client);

/*static*/ void snd_extamp_setpath(extamp_inmode_e inpath, extamp_outmode_e outpath, extamp_outfmt_e outfmt);
/*static*/ void snd_extamp_setvolume(extamp_outmode_e outpath, uint32_t volume);
/*static*/ void snd_extamp_setpreampgain(extamp_inmode_e inpath, u8 val);
/*static*/ void snd_extamp_sleep(u8 enable);

int snd_extamp_write_all_reg(extamp_info_t tCurrentExtampInfo);

/*==========================================================================
** max97001_probe
**=========================================================================*/
static int max97001_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
 	int rc = 0;
	int status = 0;
	//test
	//u8 inregval, outregval, volregval,limregval;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		max97001_i2c_client = NULL;
		rc = -1;
	}
	else 
	{
		max97001_i2c_client = client;
		
		tExtampInfo.inp_gain_ctl_reg_val = DEFAULT_INPUT_MODE_CTL_REG_VAL;
		tExtampInfo.hp_mixers_ctl_reg_val = DEFAULT_HP_MIXER_CTL_REG_VAL;
		tExtampInfo.sp_mixer_ctl_reg_val = DEFAULT_SP_MIXER_CTL_REG_VAL;
		tExtampInfo.hpl_ctl_reg_val = DEFAULT_HPLM_VOLUME_CTL_REG_VAL; 
		tExtampInfo.hpr_ctl_reg_val = DEFAULT_HPRM_VOLUME_CTL_REG_VAL; 
		tExtampInfo.sp_ctl_reg_val = DEFAULT_SPK_VOLUME_CTL_REG_VAL; 
		tExtampInfo.limiter_ctl_reg_val = DEFAULT_DISTORTION_LIMITER_CTL_REG_VAL;
		tExtampInfo.power_man_ctl_reg_val = DEFAULT_POWER_MANAGEMENT_CTL_REG_VAL;
		tExtampInfo.charge_pump_ctl_reg_val = DEFAULT_CHARGE_PUMP_CTL_REG_VAL;

		/* Init MAX9877 register */
		status |= snd_extamp_i2c_write(INPUT_GAIN_CTL_REG_ADDR, tExtampInfo.inp_gain_ctl_reg_val);
		status |= snd_extamp_i2c_write(HEADPHONE_MIXERS_CTL_REG_ADDR, tExtampInfo.hp_mixers_ctl_reg_val );
		status |= snd_extamp_i2c_write(SPEAKER_MIXER_CTL_REG_ADDR, tExtampInfo.sp_mixer_ctl_reg_val);
		status |= snd_extamp_i2c_write(HEADPHONE_LEFT_CTL_REG_ADDR, tExtampInfo.hpl_ctl_reg_val);
		status |= snd_extamp_i2c_write(HEADPHONE_RIGHT_CTL_REG_ADDR, tExtampInfo.hpr_ctl_reg_val);
		status |= snd_extamp_i2c_write(SPEAKER_CTL_REG_ADDR, tExtampInfo.sp_ctl_reg_val  );
		status |= snd_extamp_i2c_write(LIMITER_CTL_REG_ADDR, tExtampInfo.limiter_ctl_reg_val);
		status |= snd_extamp_i2c_write(POWER_MANAGEMENT_CTL_REG_ADDR, tExtampInfo.power_man_ctl_reg_val);
		status |= snd_extamp_i2c_write(CHARGE_PUMP_CTL_REG_ADDR, tExtampInfo.charge_pump_ctl_reg_val);
		
		//test
		//snd_extamp_i2c_read(INPUT_GAIN_CTL_REG_ADDR, &inregval);
		//printk("[sky external amp] inreg value = %x\n", inregval);
				
		//snd_extamp_i2c_read(LIMITER_CTL_REG_ADDR, &limregval);
		//printk("[sky external amp] limreg value = %x\n", limregval);

		if(status){
			rc = -1;
		}
	}
	AMP_DBG_MED("  [SKY Audio] max97001_probe %d \n", rc);
	return rc;
    
}

/*==========================================================================
** max97001_remove
**=========================================================================*/
static int __exit max97001_remove(struct i2c_client *client)
{
	int rc = 0;
	max97001_i2c_client = NULL;
	/*rc = i2c_detach_client(client);*/
	return rc;
}

static int max97001_suspend(struct device *dev)
{
      unsigned char SuspendOk = 0;
      unsigned char spk_vol, hpl_vol, hpr_vol;

      spk_vol = tExtampInfo.sp_ctl_reg_val & SPKM_MUTE ? 1:0;
      hpl_vol =  tExtampInfo.hpl_ctl_reg_val & HPLM_MUTE ? 1:0;
      hpr_vol =  tExtampInfo.hpr_ctl_reg_val & HPRM_MUTE ? 1:0;
#if 0
	if(sky_amp_log[1])        printk("[SKY Audio] AMP suspend(%d)(%d)(%d)\n", spk_vol, hpl_vol, hpr_vol);
    
       if(!g_call_status)   {
            if(!spk_vol)     {
                tExtampInfo.sp_ctl_reg_val = FFM_FF_MODE  | SPKM_MUTE | SPKVOL_M_30_DB;
                snd_extamp_i2c_write(SPEAKER_CTL_REG_ADDR, tExtampInfo.sp_ctl_reg_val);
                spk_vol = 1;
            }
            if(!hpl_vol)     {
                tExtampInfo.hpl_ctl_reg_val = ZCD_EN | SLEW_EN | HPLM_MUTE | HPLVOL_M_64_DB;
                snd_extamp_i2c_write(HEADPHONE_LEFT_CTL_REG_ADDR, tExtampInfo.hpl_ctl_reg_val);
                hpl_vol = 1;
            }
            if(!hpr_vol)     {
                tExtampInfo.hpr_ctl_reg_val = LPGAIN_P_0_DB | HPRM_MUTE | HPRVOL_M_64_DB;                
                snd_extamp_i2c_write(HEADPHONE_RIGHT_CTL_REG_ADDR, tExtampInfo.hpr_ctl_reg_val);
                hpr_vol = 1;
            }
       }
#endif	   
       SuspendOk = spk_vol & hpl_vol & hpr_vol;

       if(SuspendOk) {
           tExtampInfo.power_man_ctl_reg_val = SHDN_DIS | SPKEN_DIS | HPLEN_DIS | HPREN_DIS | BYPEN_CLOSE;
   	   snd_extamp_i2c_write(POWER_MANAGEMENT_CTL_REG_ADDR, tExtampInfo.power_man_ctl_reg_val);
       }
	AMP_DBG_MED("[SKY Audio] AMP suspend(%d)\n", SuspendOk);
	
	return 0;
}

static int max97001_resume(struct device *dev)
{

	AMP_DBG_MED("[SKY Audio] AMP resume()\n");
//	snd_extamp_sleep(1);
	return 0;
}

static void max97001_shutdown(struct i2c_client *client)
{
	tExtampInfo.power_man_ctl_reg_val &= ~SHDN_EN;
	tExtampInfo.power_man_ctl_reg_val = tExtampInfo.power_man_ctl_reg_val | SHDN_DIS;
	
	snd_extamp_i2c_write(POWER_MANAGEMENT_CTL_REG_ADDR, tExtampInfo.power_man_ctl_reg_val);
	AMP_DBG_MED("  [SKY Audio] AMP Shutdown for power-off\n");
}

static const struct i2c_device_id max97001_id[] = {
	{ "max97001-amp", 0},
};

static struct dev_pm_ops i2c_device_max977001_pm_ops = {
             .suspend = max97001_suspend, 
             .resume = max97001_resume,
};

static struct i2c_driver max97001_driver = {
	.id_table = max97001_id,
	.probe  = max97001_probe,
	.remove = __exit_p(max97001_remove),
	.shutdown = max97001_shutdown,
	.driver = {
		.name = "max97001-amp",
                .pm = &i2c_device_max977001_pm_ops,
	},
};

/*==========================================================================
** snd_extamp_get_current_callmode
**=========================================================================*/
int snd_extamp_get_current_callmode(void)
{
       int result=curCallMode.mode;
       return result;
}
/*==========================================================================
** snd_extamp_get_mode
**=========================================================================*/
int snd_extamp_get_mode(void)
{
       int result=curMode;
       return result;
}
/*==========================================================================
** snd_extamp_api_reset
**=========================================================================*/
void snd_extamp_api_reset(void)
{
       curCallMode.mode= MODE_DEFAULT;
       curCallMode.param=0;
       curMode=0;
       AMP_DBG_HIGH("Rest max97001 parameters.");
}
/*==========================================================================
** snd_extamp_api_Init
**=========================================================================*/
void snd_extamp_api_Init(void)
{
	int result = 0;
	AMP_DBG_MED( "  [SKY Audio] init max97001\n");

#ifdef FEATURE_SKY_SND_AUDIO_TEST_COMMAND
       current_reg=0;
       current_regval=0;
#endif
       
#ifdef FEATURE_SKY_SND_MISC_DEVICE
       snd_misc_device_init();
#endif

	result = i2c_add_driver(&max97001_driver);
	if(result){
		AMP_ERR( "  [SKY Audio] init max97001 Fail\n");
	}
}

/*==========================================================================
** snd_extamp_api_DeInit
**=========================================================================*/
void snd_extamp_api_DeInit(void)
{
#ifdef FEATURE_SKY_SND_MISC_DEVICE
       snd_misc_device_deinit();
#endif
	i2c_del_driver(&max97001_driver);
	AMP_DBG_MED( "  [SKY Audio] DeInit max97001\n");
}

/*==========================================================================
** snd_extamp_write_all_reg
**=========================================================================*/
int snd_extamp_write_all_reg(extamp_info_t tCurrentExtampInfo)
{
	int i;
	int status[9] = {0,0,0,0,0,0,0,0,0};
	int write_status=0;
	//const u8 DEALY_DN = 15; /*ms*/

	//test
	//u8 limregval;

	if(tExtampInfo.inp_gain_ctl_reg_val != tCurrentExtampInfo.inp_gain_ctl_reg_val)	{
		status[0] = snd_extamp_i2c_write(INPUT_GAIN_CTL_REG_ADDR, tCurrentExtampInfo.inp_gain_ctl_reg_val);
		write_status |= 0x1;
	}
	if(tExtampInfo.hp_mixers_ctl_reg_val != tCurrentExtampInfo.hp_mixers_ctl_reg_val)	{
		status[1] = snd_extamp_i2c_write(HEADPHONE_MIXERS_CTL_REG_ADDR, tCurrentExtampInfo.hp_mixers_ctl_reg_val );
		write_status |= 0x2;
	}
	if(tExtampInfo.sp_mixer_ctl_reg_val != tCurrentExtampInfo.sp_mixer_ctl_reg_val)	{
		status[2] = snd_extamp_i2c_write(SPEAKER_MIXER_CTL_REG_ADDR, tCurrentExtampInfo.sp_mixer_ctl_reg_val);
		write_status |= 0x4;
	}
	if(tExtampInfo.hpl_ctl_reg_val != tCurrentExtampInfo.hpl_ctl_reg_val)	{
		status[3] = snd_extamp_i2c_write(HEADPHONE_LEFT_CTL_REG_ADDR, tCurrentExtampInfo.hpl_ctl_reg_val);
		write_status |= 0x8;
	}
	if(tExtampInfo.hpr_ctl_reg_val != tCurrentExtampInfo.hpr_ctl_reg_val)	{
		status[4] = snd_extamp_i2c_write(HEADPHONE_RIGHT_CTL_REG_ADDR, tCurrentExtampInfo.hpr_ctl_reg_val);
		write_status |= 0x10;
	}
	if(tExtampInfo.sp_ctl_reg_val != tCurrentExtampInfo.sp_ctl_reg_val)	{
		status[5] = snd_extamp_i2c_write(SPEAKER_CTL_REG_ADDR, tCurrentExtampInfo.sp_ctl_reg_val  );
		write_status |= 0x20;
	}
	if(tExtampInfo.limiter_ctl_reg_val != tCurrentExtampInfo.limiter_ctl_reg_val)	{
		status[6] = snd_extamp_i2c_write(LIMITER_CTL_REG_ADDR, tCurrentExtampInfo.limiter_ctl_reg_val);
		write_status |= 0x40;
	}
	if(tExtampInfo.power_man_ctl_reg_val != tCurrentExtampInfo.power_man_ctl_reg_val)	{
		status[7] = snd_extamp_i2c_write(POWER_MANAGEMENT_CTL_REG_ADDR, tCurrentExtampInfo.power_man_ctl_reg_val);
		write_status |= 0x80;
	}
	if(tExtampInfo.charge_pump_ctl_reg_val != tCurrentExtampInfo.charge_pump_ctl_reg_val)	{
		status[8] = snd_extamp_i2c_write(CHARGE_PUMP_CTL_REG_ADDR, tCurrentExtampInfo.charge_pump_ctl_reg_val);
		write_status |= 0x100;
	}
	
	//test
	//snd_extamp_i2c_read(LIMITER_CTL_REG_ADDR, &limregval);
	//printk("[sky external amp] limreg value = %x\n", limregval);

	memcpy(&tExtampInfo, &tCurrentExtampInfo, sizeof(extamp_info_t));
	for(i=0;i<9;i++)	{
		if(status[i])	{
			AMP_ERR( "[SKY Audio] snd_extamp_api_SetDevice Fail (%d Reg.)\n", i);
			return 0;
		}
	}

	if(write_status!=0)  {
		AMP_DBG_MED( "[SKY Audio] snd_extamp_write_all_reg (Write Status:%x)\n", write_status);
		return 1;
	}
	else {
		AMP_DBG_MED( "[SKY Audio] snd_extamp_write_all_reg (No change)  \n");
		return 0;
	}
}

/*==========================================================================
** snd_extamp_make_current
**=========================================================================*/
void snd_extamp_make_current(extamp_info_t* tCurrentExtampInfo)
{
    memcpy(tCurrentExtampInfo, &tExtampInfo, sizeof(extamp_info_t));
}

/*==========================================================================
** snd_extamp_api_SetDevice
**=========================================================================*/
void snd_extamp_api_SetDevice(int on, uint32_t cad_device)
{
	extamp_info_t tCurrentExtampInfo;
	int result;
	//const u8 DEALY_DN = 15; /*ms*/

	if(CurrDeviceId == cad_device && CurrDevicePwrStatus == on) 
		return;


        #if 0
	    CurrDeviceId = cad_device;
	    CurrDevicePwrStatus = on;
	    printk(KERN_ERR "  [SKY Audio] set max97001 on(%d), path(%d)\n", on, cad_device);
        #else
        //20110425-jhpark : Rx mute ...in auto-answer mode
            if(on && curCallMode.mode == MODE_AUTOANSWER)
            {
                if(curCallMode.param)
                {
                    CurrDeviceId = cad_device;
                    CurrDevicePwrStatus = on;
                    cad_device = SND_SKY_DEVICE_MUTE_RX;
                    AMP_DBG_MED("MODE_AUTOANSWER  ON      CurrDeviceId(%d),  cad_device(%d)", CurrDeviceId, cad_device);
                }else {
                    cad_device = CurrDeviceId;
                    curCallMode.mode = MODE_DEFAULT;
                    AMP_DBG_MED("MODE_AUTOANSWER  OFF      CurrDeviceId(%d)", CurrDeviceId);
                }
            }else {
	CurrDeviceId = cad_device;
	CurrDevicePwrStatus = on;

               AMP_DBG_MED("[SKY Audio] set max97001 on(%d), path(%d), curMode(%d)", on, cad_device, curMode);
            }
        #endif

	snd_extamp_make_current(&tCurrentExtampInfo);

	switch(cad_device)
	{
	    case SND_DEVICE_HANDSET_RX:
	          tCurrentExtampInfo.power_man_ctl_reg_val = SHDN_EN  | LPMODE_DIS | SPKEN_DIS | HPLEN_DIS | HPREN_DIS |BYPEN_CLOSE;
		break;
	    case SND_DEVICE_HEADSET_RX: /* Headset stereo(Bell) */
		if (on) 
		{
	          /* HP PGAINA 3dB, HP(L/R) vol -12dB, HP class gain 3dB */
	          tCurrentExtampInfo.inp_gain_ctl_reg_val  = INADIFF_STEREO_SINGLE | INBDIFF_DIFFERENTIAL | PGAINA_P_3_DB | PGAINB_P_0_DB;
	          tCurrentExtampInfo.hp_mixers_ctl_reg_val = HPLMIX_INA2 | HPRMIX_INA1;
       	          if(curMode==MODE_IN_CALL) {
	              tCurrentExtampInfo.hpl_ctl_reg_val       = ZCD_DIS | SLEW_DIS | HPLM_UNMUTE | HPLVOL_M_4_DB;
	              tCurrentExtampInfo.hpr_ctl_reg_val       = HPGAIN_P_3_DB | HPRM_UNMUTE | HPRVOL_M_4_DB;
                  }else {
	              tCurrentExtampInfo.hpl_ctl_reg_val       = ZCD_DIS | SLEW_DIS | HPLM_UNMUTE | HPLVOL_M_6_DB;
	              tCurrentExtampInfo.hpr_ctl_reg_val       = HPGAIN_P_3_DB | HPRM_UNMUTE | HPRVOL_M_6_DB;
                  }
	          tCurrentExtampInfo.power_man_ctl_reg_val = SHDN_EN  | LPMODE_DIS | SPKEN_DIS | HPLEN_EN | HPREN_EN | BYPEN_OPEN;
		}
		else
		{
	          tCurrentExtampInfo.inp_gain_ctl_reg_val  = INADIFF_STEREO_SINGLE | INBDIFF_DIFFERENTIAL | PGAINA_P_0_DB | PGAINB_P_0_DB;
	          tCurrentExtampInfo.hp_mixers_ctl_reg_val = HPLMIX_NO_INPUT | HPRMIX_NO_INPUT;
	          tCurrentExtampInfo.hpl_ctl_reg_val       = ZCD_DIS | SLEW_DIS | HPLM_MUTE | HPLVOL_M_64_DB;
	          tCurrentExtampInfo.hpr_ctl_reg_val       = HPGAIN_P_0_DB | HPRM_MUTE | HPRVOL_M_64_DB;
	          tCurrentExtampInfo.power_man_ctl_reg_val = SHDN_EN  | LPMODE_DIS | SPKEN_DIS | HPLEN_DIS | HPREN_DIS | BYPEN_CLOSE;
		}
		break;
	    case SND_DEVICE_SPEAKER_RX: /* Speaker stereo(Bell) */
		if (on)
		{
	         if(curMode==MODE_IN_CALL)      
	          tCurrentExtampInfo.inp_gain_ctl_reg_val  = INADIFF_STEREO_SINGLE  | INBDIFF_DIFFERENTIAL | PGAINA_P_0_DB | PGAINB_P_0_DB;
                 else
	             tCurrentExtampInfo.inp_gain_ctl_reg_val  = INADIFF_STEREO_SINGLE  | INBDIFF_DIFFERENTIAL | PGAINA_P_0_DB | PGAINB_P_3_DB;
             
	          tCurrentExtampInfo.sp_mixer_ctl_reg_val  = SPKMIX_INB2;
              
	         if(curMode==MODE_IN_CALL)      
                     tCurrentExtampInfo.sp_ctl_reg_val        = FFM_FF_MODE  | SPKM_UNMUTE | SPKVOL_M_4_DB;
                 else
                     tCurrentExtampInfo.sp_ctl_reg_val        = FFM_FF_MODE  | SPKM_UNMUTE | SPKVOL_M_4_DB;
                 
	         if(curMode==MODE_IN_CALL)      
                     tCurrentExtampInfo.limiter_ctl_reg_val = THDCLP_DIS |THDT1_1_DOT_4_SEC;
                 else
                     tCurrentExtampInfo.limiter_ctl_reg_val = THDCLP_BELOW_12_PER |THDT1_1_DOT_4_SEC;
                 
	          tCurrentExtampInfo.power_man_ctl_reg_val = SHDN_EN  | LPMODE_DIS | SPKEN_EN | HPLEN_DIS | HPREN_DIS | BYPEN_OPEN;
		}
		else
		{
 	          tCurrentExtampInfo.inp_gain_ctl_reg_val  = INADIFF_STEREO_SINGLE | INBDIFF_DIFFERENTIAL | PGAINA_P_0_DB | PGAINB_P_0_DB;
 	          tCurrentExtampInfo.sp_mixer_ctl_reg_val  = SPKMIX_NO_INPUT;
 	          tCurrentExtampInfo.sp_ctl_reg_val        = FFM_FF_MODE  | SPKM_MUTE | SPKVOL_M_30_DB;
	          tCurrentExtampInfo.limiter_ctl_reg_val = THDCLP_DIS |THDT1_1_DOT_4_SEC;
 	          tCurrentExtampInfo.power_man_ctl_reg_val = SHDN_EN  | LPMODE_DIS | SPKEN_DIS | HPLEN_DIS | HPREN_DIS | BYPEN_CLOSE;
		}
		break;
	    case SND_DEVICE_SPEAKER_HEADSET_RX:
		if (on)
		{
	          tCurrentExtampInfo.inp_gain_ctl_reg_val  = INADIFF_STEREO_SINGLE | INBDIFF_DIFFERENTIAL | PGAINA_P_0_DB | PGAINB_P_3_DB;
	          tCurrentExtampInfo.hp_mixers_ctl_reg_val = HPLMIX_INA2 | HPRMIX_INA1;
	          tCurrentExtampInfo.sp_mixer_ctl_reg_val  = SPKMIX_INB2;
	          tCurrentExtampInfo.hpl_ctl_reg_val       = ZCD_DIS | SLEW_DIS | HPLM_UNMUTE | HPLVOL_M_19_DB;
	          tCurrentExtampInfo.hpr_ctl_reg_val       = HPGAIN_P_3_DB | HPRM_UNMUTE | HPRVOL_M_19_DB;
	          tCurrentExtampInfo.sp_ctl_reg_val        = FFM_FF_MODE  | SPKM_UNMUTE | SPKVOL_M_4_DB;
	          tCurrentExtampInfo.limiter_ctl_reg_val = THDCLP_BELOW_8_PER | THDT1_1_DOT_4_SEC;
	          tCurrentExtampInfo.power_man_ctl_reg_val = SHDN_EN  | LPMODE_DIS | SPKEN_EN | HPLEN_EN | HPREN_EN | BYPEN_OPEN;
		}
		else
		{
	          tCurrentExtampInfo.inp_gain_ctl_reg_val  = INADIFF_STEREO_SINGLE | INBDIFF_DIFFERENTIAL | PGAINA_P_0_DB | PGAINB_P_0_DB;
	          tCurrentExtampInfo.hp_mixers_ctl_reg_val = HPLMIX_NO_INPUT | HPRMIX_NO_INPUT;
	          tCurrentExtampInfo.sp_mixer_ctl_reg_val  = SPKMIX_NO_INPUT;
	          tCurrentExtampInfo.hpl_ctl_reg_val       = ZCD_DIS | SLEW_DIS | HPLM_MUTE | HPLVOL_M_64_DB;
	          tCurrentExtampInfo.hpr_ctl_reg_val       = HPGAIN_P_0_DB | HPRM_MUTE | HPRVOL_M_64_DB;
	          tCurrentExtampInfo.sp_ctl_reg_val        = FFM_FF_MODE  | SPKM_MUTE | SPKVOL_M_30_DB;
	          tCurrentExtampInfo.limiter_ctl_reg_val = THDCLP_DIS | THDT1_1_DOT_4_SEC;
	          tCurrentExtampInfo.power_man_ctl_reg_val = SHDN_EN  | LPMODE_DIS | SPKEN_DIS | HPLEN_DIS | HPREN_DIS | BYPEN_CLOSE;
		}
		break;
	    case SND_SKY_DEVICE_VT_SPEAKER_RX: /* Speaker mono(VT) */
		if (on) 
		{
	          tCurrentExtampInfo.inp_gain_ctl_reg_val  = INADIFF_STEREO_SINGLE  | INBDIFF_DIFFERENTIAL | PGAINA_P_0_DB | PGAINB_P_3_DB;
                  tCurrentExtampInfo.sp_mixer_ctl_reg_val  = SPKMIX_INB2;
                  tCurrentExtampInfo.sp_ctl_reg_val        = FFM_FF_MODE  | SPKM_UNMUTE | SPKVOL_P_7_DB;
                  tCurrentExtampInfo.limiter_ctl_reg_val = THDCLP_BELOW_12_PER |THDT1_1_DOT_4_SEC;
    	          tCurrentExtampInfo.power_man_ctl_reg_val = SHDN_EN  | LPMODE_DIS | SPKEN_EN | HPLEN_DIS | HPREN_DIS | BYPEN_OPEN;
		}
		else
		{
 	          tCurrentExtampInfo.inp_gain_ctl_reg_val  = INADIFF_STEREO_SINGLE | INBDIFF_DIFFERENTIAL | PGAINA_P_0_DB | PGAINB_P_0_DB;
 	          tCurrentExtampInfo.sp_mixer_ctl_reg_val  = SPKMIX_NO_INPUT;
 	          tCurrentExtampInfo.sp_ctl_reg_val        = FFM_FF_MODE  | SPKM_MUTE | SPKVOL_M_30_DB;
	          tCurrentExtampInfo.limiter_ctl_reg_val = THDCLP_DIS |THDT1_1_DOT_4_SEC;
 	          tCurrentExtampInfo.power_man_ctl_reg_val = SHDN_EN  | LPMODE_DIS | SPKEN_DIS | HPLEN_DIS | HPREN_DIS | BYPEN_CLOSE;
		}
		break;        
	    case SND_SKY_DEVICE_LOOPBACK_HEADSET_RX:
		if (on)
		{
	          tCurrentExtampInfo.inp_gain_ctl_reg_val  = INADIFF_STEREO_SINGLE | INBDIFF_DIFFERENTIAL |PGAINA_P_0_DB | PGAINB_P_0_DB;
	          tCurrentExtampInfo.hp_mixers_ctl_reg_val = HPLMIX_INB2 | HPRMIX_INB2;
	          /* 20101216 frogLove - HP(L/R) vol 3dB, HP class gain 3dB */
	          tCurrentExtampInfo.hpl_ctl_reg_val       = ZCD_DIS | SLEW_DIS | HPLM_UNMUTE | HPLVOL_P_3_DB;
	          tCurrentExtampInfo.hpr_ctl_reg_val       = HPGAIN_P_3_DB | HPRM_UNMUTE | HPRVOL_P_3_DB;
	          tCurrentExtampInfo.power_man_ctl_reg_val = SHDN_EN  | LPMODE_DIS | SPKEN_DIS | HPLEN_EN | HPREN_EN | BYPEN_OPEN;
		}
		else
		{
	          tCurrentExtampInfo.inp_gain_ctl_reg_val  = INADIFF_STEREO_SINGLE | INBDIFF_DIFFERENTIAL | PGAINA_P_0_DB | PGAINB_P_0_DB;
	          tCurrentExtampInfo.hp_mixers_ctl_reg_val = HPLMIX_NO_INPUT | HPRMIX_NO_INPUT;
	          tCurrentExtampInfo.hpl_ctl_reg_val       = ZCD_DIS | SLEW_DIS | HPLM_MUTE | HPLVOL_M_64_DB;
	          tCurrentExtampInfo.hpr_ctl_reg_val       = HPGAIN_P_0_DB | HPRM_MUTE | HPRVOL_M_64_DB;
	          tCurrentExtampInfo.power_man_ctl_reg_val = SHDN_EN  | LPMODE_DIS | SPKEN_DIS | HPLEN_DIS | HPREN_DIS | BYPEN_CLOSE;
		}
		break;
	    case SND_SKY_DEVICE_MUTE_RX:
		if (on)
		{
	          tCurrentExtampInfo.inp_gain_ctl_reg_val  = INADIFF_STEREO_SINGLE | INBDIFF_DIFFERENTIAL | PGAINA_P_0_DB | PGAINB_P_0_DB;
	          tCurrentExtampInfo.hp_mixers_ctl_reg_val = HPLMIX_NO_INPUT | HPRMIX_NO_INPUT;
	          tCurrentExtampInfo.hpl_ctl_reg_val       = ZCD_DIS | SLEW_DIS | HPLM_MUTE | HPLVOL_M_64_DB;
	          tCurrentExtampInfo.hpr_ctl_reg_val       = HPGAIN_P_0_DB | HPRM_MUTE | HPRVOL_M_64_DB;
	          tCurrentExtampInfo.power_man_ctl_reg_val = SHDN_EN  | LPMODE_DIS | SPKEN_DIS | HPLEN_DIS | HPREN_DIS | BYPEN_OPEN;
		}
		else
		{
	          tCurrentExtampInfo.power_man_ctl_reg_val = SHDN_EN  | LPMODE_DIS | SPKEN_DIS | HPLEN_DIS | HPREN_DIS |BYPEN_CLOSE;
		}
		break;
	    case SND_SKY_DEVICE_VOICE_HEADSET_RX: /* Headset mono(Voice) */
		if (on)
		{
	          tCurrentExtampInfo.inp_gain_ctl_reg_val  = INADIFF_STEREO_SINGLE | INBDIFF_DIFFERENTIAL | PGAINA_P_3_DB | PGAINB_P_0_DB;
	          tCurrentExtampInfo.hp_mixers_ctl_reg_val = HPLMIX_INA2 | HPRMIX_INA1;
	          /* HP PGAINA 0dB, HP(L/R) vol -12dB, HP class gain 3dB */
	          tCurrentExtampInfo.hpl_ctl_reg_val       = ZCD_DIS | SLEW_DIS | HPLM_UNMUTE | HPLVOL_M_12_DB;
	          tCurrentExtampInfo.hpr_ctl_reg_val       = HPGAIN_P_3_DB | HPRM_UNMUTE | HPRVOL_M_12_DB;
	          tCurrentExtampInfo.power_man_ctl_reg_val = SHDN_EN  | LPMODE_DIS | SPKEN_DIS | HPLEN_EN | HPREN_EN | BYPEN_OPEN;
		}
		else
		{
	          tCurrentExtampInfo.inp_gain_ctl_reg_val  = INADIFF_STEREO_SINGLE | INBDIFF_DIFFERENTIAL | PGAINA_P_0_DB | PGAINB_P_0_DB;
	          tCurrentExtampInfo.hp_mixers_ctl_reg_val = HPLMIX_NO_INPUT | HPRMIX_NO_INPUT;
	          tCurrentExtampInfo.hpl_ctl_reg_val       = ZCD_DIS | SLEW_DIS | HPLM_MUTE | HPLVOL_M_64_DB;
	          tCurrentExtampInfo.hpr_ctl_reg_val       = HPGAIN_P_0_DB | HPRM_MUTE | HPRVOL_M_64_DB;
	          tCurrentExtampInfo.power_man_ctl_reg_val = SHDN_EN  | LPMODE_DIS | SPKEN_DIS | HPLEN_DIS | HPREN_DIS | BYPEN_CLOSE;
		}
		break;
	    case SND_SKY_DEVICE_VOIP_HANDSET_RX: /* VoIP Handset */
	          tCurrentExtampInfo.power_man_ctl_reg_val = SHDN_EN  | LPMODE_DIS | SPKEN_DIS | HPLEN_DIS | HPREN_DIS |BYPEN_CLOSE;
		break;
	    case SND_SKY_DEVICE_VOIP_SPEAKER_RX: /* VoIP speaker */
		if (on) 
		{
	          tCurrentExtampInfo.inp_gain_ctl_reg_val  = INADIFF_STEREO_SINGLE  | INBDIFF_DIFFERENTIAL | PGAINA_P_0_DB | PGAINB_P_0_DB;
	          tCurrentExtampInfo.sp_mixer_ctl_reg_val  = SPKMIX_INB2;
	          tCurrentExtampInfo.sp_ctl_reg_val        = FFM_FF_MODE  | SPKM_UNMUTE | SPKVOL_M_4_DB;
                  tCurrentExtampInfo.limiter_ctl_reg_val = THDCLP_DIS |THDT1_1_DOT_4_SEC;
	          tCurrentExtampInfo.power_man_ctl_reg_val = SHDN_EN  | LPMODE_DIS | SPKEN_EN | HPLEN_DIS | HPREN_DIS | BYPEN_OPEN;
		}
		else
		{
 	          tCurrentExtampInfo.inp_gain_ctl_reg_val  = INADIFF_STEREO_SINGLE | INBDIFF_DIFFERENTIAL | PGAINA_P_0_DB | PGAINB_P_0_DB;
 	          tCurrentExtampInfo.sp_mixer_ctl_reg_val  = SPKMIX_NO_INPUT;
 	          tCurrentExtampInfo.sp_ctl_reg_val        = FFM_FF_MODE  | SPKM_MUTE | SPKVOL_M_30_DB;
	          tCurrentExtampInfo.limiter_ctl_reg_val = THDCLP_DIS |THDT1_1_DOT_4_SEC;
 	          tCurrentExtampInfo.power_man_ctl_reg_val = SHDN_EN  | LPMODE_DIS | SPKEN_DIS | HPLEN_DIS | HPREN_DIS | BYPEN_CLOSE;
		}
		break;        
	    case SND_SKY_DEVICE_VOIP_HEADSET_RX: /* VoIP Headset */
		if (on) 
		{
	          tCurrentExtampInfo.inp_gain_ctl_reg_val  = INADIFF_STEREO_SINGLE | INBDIFF_DIFFERENTIAL | PGAINA_P_0_DB | PGAINB_P_0_DB;
	          tCurrentExtampInfo.hp_mixers_ctl_reg_val = HPLMIX_INA2 | HPRMIX_INA1;
	          tCurrentExtampInfo.hpl_ctl_reg_val       = ZCD_DIS | SLEW_DIS | HPLM_UNMUTE | HPLVOL_M_12_DB;
	          tCurrentExtampInfo.hpr_ctl_reg_val       = HPGAIN_P_0_DB | HPRM_UNMUTE | HPRVOL_M_12_DB;
	          tCurrentExtampInfo.power_man_ctl_reg_val = SHDN_EN  | LPMODE_DIS | SPKEN_DIS | HPLEN_EN | HPREN_EN | BYPEN_OPEN;
		}
		else
		{
	          tCurrentExtampInfo.inp_gain_ctl_reg_val  = INADIFF_STEREO_SINGLE | INBDIFF_DIFFERENTIAL | PGAINA_P_0_DB | PGAINB_P_0_DB;
	          tCurrentExtampInfo.hp_mixers_ctl_reg_val = HPLMIX_NO_INPUT | HPRMIX_NO_INPUT;
	          tCurrentExtampInfo.hpl_ctl_reg_val       = ZCD_DIS | SLEW_DIS | HPLM_MUTE | HPLVOL_M_64_DB;
	          tCurrentExtampInfo.hpr_ctl_reg_val       = HPGAIN_P_0_DB | HPRM_MUTE | HPRVOL_M_64_DB;
	          tCurrentExtampInfo.power_man_ctl_reg_val = SHDN_EN  | LPMODE_DIS | SPKEN_DIS | HPLEN_DIS | HPREN_DIS | BYPEN_CLOSE;
		}
		break;

	    default:
		break;
	}

	result = snd_extamp_write_all_reg(tCurrentExtampInfo); 

}

/*==========================================================================
** snd_extamp_api_SetVolume
**=========================================================================*/
void snd_extamp_api_SetVolume(uint32_t hp_vol, uint32_t sp_vol)
{
	extamp_info_t tCurrentExtampInfo;
        u8 val=0;
        int result;

	AMP_DBG_MED( "  [SKY Audio] set Volume hp_vol(%d), sp_vol(%d)\n", hp_vol, sp_vol);
	   
	snd_extamp_make_current(&tCurrentExtampInfo);

	switch(CurrDeviceId){
	    case SND_DEVICE_HANDSET_RX:
		break;
	    case SND_DEVICE_HEADSET_RX:
              val = (u8)(hp_vol);

              tCurrentExtampInfo.hpl_ctl_reg_val  &= ~HPLVOL_P_6_DB;
              tCurrentExtampInfo.hpr_ctl_reg_val  &= ~HPRVOL_P_6_DB;
	          tCurrentExtampInfo.hpl_ctl_reg_val  = tCurrentExtampInfo.hpl_ctl_reg_val |val;
	          tCurrentExtampInfo.hpr_ctl_reg_val  = tCurrentExtampInfo.hpr_ctl_reg_val |val;

		break;
	    case SND_DEVICE_SPEAKER_RX:
	    case SND_SKY_DEVICE_VT_SPEAKER_RX:
              val = (u8)(sp_vol);

	          tCurrentExtampInfo.sp_ctl_reg_val  &= ~SPKVOL_P_20_DB;
	          tCurrentExtampInfo.sp_ctl_reg_val  = tCurrentExtampInfo.sp_ctl_reg_val |val;

		break;
              case SND_DEVICE_SPEAKER_HEADSET_RX:
              val = (u8)(hp_vol);

              tCurrentExtampInfo.hpl_ctl_reg_val  &= ~HPLVOL_P_6_DB;
              tCurrentExtampInfo.hpr_ctl_reg_val  &= ~HPRVOL_P_6_DB;
	          tCurrentExtampInfo.hpl_ctl_reg_val  = tCurrentExtampInfo.hpl_ctl_reg_val |val;
	          tCurrentExtampInfo.hpr_ctl_reg_val  = tCurrentExtampInfo.hpr_ctl_reg_val |val;

              val = (u8)(sp_vol);

	          tCurrentExtampInfo.sp_ctl_reg_val  &= ~SPKVOL_P_20_DB;
	          tCurrentExtampInfo.sp_ctl_reg_val  = tCurrentExtampInfo.sp_ctl_reg_val |val;

		break;
	    default:
		break;
	}
	result = snd_extamp_write_all_reg(tCurrentExtampInfo); 	
}

/*==========================================================================
** snd_extamp_api_HeadsetConnected
**=========================================================================*/
void snd_extamp_api_HeadsetConnected(uint32_t connect)
{
	bHeadphonePath = connect;
	//snd_extamp_sleep(1);
}

/*==========================================================================
** snd_extamp_api_SetPreAmpGain
**=========================================================================*/
void snd_extamp_api_SetPreAmpGain(uint32_t gain)
{
	extamp_info_t tCurrentExtampInfo;
        u8 val=0;
        int result;

	AMP_DBG_MED( "  [SKY Audio] set PreAmpGain gain(%d)\n", gain);
	   
	snd_extamp_make_current(&tCurrentExtampInfo);

	switch(CurrDeviceId){
	    case SND_DEVICE_HANDSET_RX:
		break;
	    case SND_DEVICE_HEADSET_RX:
              val = (u8)(gain<<3);
			
              tCurrentExtampInfo.inp_gain_ctl_reg_val  &= ~PGAINA_EXT;
	          tCurrentExtampInfo.inp_gain_ctl_reg_val  = tCurrentExtampInfo.inp_gain_ctl_reg_val |val;

		break;
	    case SND_DEVICE_SPEAKER_RX:
              val = (u8)(gain);
			
	          tCurrentExtampInfo.inp_gain_ctl_reg_val  &= ~PGAINB_EXT;
	          tCurrentExtampInfo.inp_gain_ctl_reg_val  = tCurrentExtampInfo.inp_gain_ctl_reg_val |val;

		break;
              case SND_DEVICE_SPEAKER_HEADSET_RX:
              val = (u8)(gain);
              val |= (u8)(gain<<3);
			  
              tCurrentExtampInfo.inp_gain_ctl_reg_val  &= ~(PGAINA_EXT|PGAINB_EXT);
	          tCurrentExtampInfo.inp_gain_ctl_reg_val  = tCurrentExtampInfo.inp_gain_ctl_reg_val |val;

		break;
	    default:
		break;
	}
	result = snd_extamp_write_all_reg(tCurrentExtampInfo); 
}

/*==========================================================================
** snd_extamp_api_SetPath
**=========================================================================*/
/*static*/ void snd_extamp_setpath(extamp_inmode_e inpath, extamp_outmode_e outpath, extamp_outfmt_e outfmt)
{
	u8 inregval = 0;
	u8 hpmixerval = 0;
	u8 spmixerval = 0;
	u8 powerval = 0;
	
/* kernel_warn:	u8 volume; */

	switch(outfmt){
	case EXTAMP_MONO:
		inregval = INADIFF_DIFFERENTIAL | INBDIFF_DIFFERENTIAL;
		break;
	case EXTAMP_STEREO:
		inregval = INADIFF_STEREO_SINGLE | INBDIFF_STEREO_SINGLE;
		break;
	default:
		break;
	}

	switch(inpath){
	case EXTAMP_IN_INA:
		switch(outpath){
		case EXTAMP_OUT_SPK:
			spmixerval = SPKMIX_INA1;
			powerval = BYPEN_CLOSE;
			break;
		case EXTAMP_OUT_HPH_L:
		case EXTAMP_OUT_HPH_R:
		case EXTAMP_OUT_HPH_LR:
			hpmixerval = HPLMIX_INA1 | HPRMIX_INA1;
			powerval = BYPEN_CLOSE;
			break;
		case EXTAMP_OUT_SPK_HPH:
			spmixerval = SPKMIX_INA1; 
			hpmixerval = HPLMIX_INA1 | HPRMIX_INA1;
			powerval = BYPEN_CLOSE;	
			bHeadphonePath = 1;
			break;
		default:
			return;
		}
		break;
	case EXTAMP_IN_INB:
		switch(outpath){
		case EXTAMP_OUT_SPK:
			spmixerval = SPKMIX_INB1;
			powerval = BYPEN_CLOSE;
			break;
		case EXTAMP_OUT_HPH_L:
		case EXTAMP_OUT_HPH_R:
		case EXTAMP_OUT_HPH_LR:
			hpmixerval = HPLMIX_INB1 | HPRMIX_INB1;
			powerval = BYPEN_CLOSE;
			break;
		case EXTAMP_OUT_SPK_HPH:
			spmixerval = SPKMIX_INB1; 
			hpmixerval = HPLMIX_INB1 | HPRMIX_INB1;
			powerval = BYPEN_CLOSE;	
			break;
		default:
			return;
		}
		break;
	case EXTAMP_IN_INAB:
		switch(outpath){
		case EXTAMP_OUT_SPK:
			spmixerval = SPKMIX_INA1 | SPKMIX_INB1;
			powerval = BYPEN_CLOSE;
			break;
		case EXTAMP_OUT_HPH_L:
		case EXTAMP_OUT_HPH_R:
		case EXTAMP_OUT_HPH_LR:
			hpmixerval = HPLMIX_INA1 | HPRMIX_INA1 | HPLMIX_INB1 | HPRMIX_INB1;
			powerval = BYPEN_CLOSE;
			break;
		case EXTAMP_OUT_SPK_HPH:
			hpmixerval = HPLMIX_INA1 | HPRMIX_INA1 | HPLMIX_INB1 | HPRMIX_INB1;
			spmixerval = SPKMIX_INA1 | SPKMIX_INB1;
			powerval = BYPEN_CLOSE;
			break;
		default:
			return;
        	}
		break;
	case EXTAMP_IN_RXIN:
		powerval = BYPEN_OPEN;
		inregval = tExtampInfo.inp_gain_ctl_reg_val & 0xD2;
              break;
	default:
		return;
	}

	/*volume = (bHeadphonePath) ? 0 : tExtampInfo.spk_vol_reg_val;
	volume = (inpath == EXTAMP_IN_RXIN) ? 0 : volume;
	snd_extamp_api_SetVolume(outpath, volume);*/

	tExtampInfo.power_man_ctl_reg_val  = (tExtampInfo.power_man_ctl_reg_val & 0xB0) | powerval;
	tExtampInfo.inp_gain_ctl_reg_val = (tExtampInfo.inp_gain_ctl_reg_val & 0xCF) | inregval;
	tExtampInfo.hp_mixers_ctl_reg_val  =(tExtampInfo.hp_mixers_ctl_reg_val | hpmixerval); 
	tExtampInfo.sp_mixer_ctl_reg_val  =(tExtampInfo.sp_mixer_ctl_reg_val | spmixerval); 
	
	snd_extamp_i2c_write(POWER_MANAGEMENT_CTL_REG_ADDR, tExtampInfo.power_man_ctl_reg_val);
	snd_extamp_i2c_write(INPUT_GAIN_CTL_REG_ADDR, tExtampInfo.inp_gain_ctl_reg_val);
	snd_extamp_i2c_write(HEADPHONE_MIXERS_CTL_REG_ADDR, tExtampInfo.hp_mixers_ctl_reg_val);
	snd_extamp_i2c_write(SPEAKER_MIXER_CTL_REG_ADDR, tExtampInfo.sp_mixer_ctl_reg_val);	
			
}


/*==========================================================================
** snd_extamp_setvolume
**=========================================================================*/
/*static*/ void snd_extamp_setvolume(extamp_outmode_e outpath, uint32_t volume)
{
	unsigned int spkvol = 0;
	unsigned int hphvol = 0;
//jhpark	const u8 VOL_STEP = 4;
//jhpark	const u8 VOL_STEP_DN = 4;
//jhpark	const u8 DEALY = 5; /*ms*/
	const u8 DEALY_DN = 15; /*ms*/

    #if 0
	switch(outpath){
	case EXTAMP_OUT_SPK:
		spkvol = (volume&0x001f0000)>>16;
		break;
	case EXTAMP_OUT_HPH_L:
	case EXTAMP_OUT_HPH_R:
	case EXTAMP_OUT_HPH_LR:
		hphvol = (volume&0x0000001f);
		break;
	case EXTAMP_OUT_SPK_HPH:
		spkvol = (volume&0x001f0000)>>16;
		hphvol = (volume&0x0000001f);
		break;
	case EXTAMP_OUT_BYPASS:
		spkvol = 0;
		hphvol = 0;
		break;
	default:
		return;
	}
#endif
       spkvol=0x3f;
       hphvol=0x1f;
       tExtampInfo.sp_ctl_reg_val = spkvol;
       tExtampInfo.hpl_ctl_reg_val = hphvol;
       tExtampInfo.hpr_ctl_reg_val = hphvol;
       
       snd_extamp_i2c_write(SPEAKER_CTL_REG_ADDR, tExtampInfo.sp_ctl_reg_val);
	msleep(DEALY_DN);
	snd_extamp_i2c_write(HEADPHONE_LEFT_CTL_REG_ADDR, tExtampInfo.hpl_ctl_reg_val);
	snd_extamp_i2c_write(HEADPHONE_RIGHT_CTL_REG_ADDR, tExtampInfo.hpr_ctl_reg_val);
	msleep(DEALY_DN);

#if 0
	/* Wakeup AMP */
	if(spkvol > 0 || hphvol > 0){
		snd_extamp_sleep(0);
	}
		
	if(spkvol > tExtampInfo.sp_ctl_reg_val){
		while(tExtampInfo.sp_ctl_reg_val < spkvol){
			if(tExtampInfo.sp_ctl_reg_val + VOL_STEP > spkvol)
				tExtampInfo.sp_ctl_reg_val = (u8)spkvol;
			else
				tExtampInfo.sp_ctl_reg_val += VOL_STEP;

			snd_extamp_i2c_write(SPEAKER_CTL_REG_ADDR, tExtampInfo.sp_ctl_reg_val);
			msleep(DEALY);
		}
	}
	else if(spkvol < tExtampInfo.sp_ctl_reg_val){
		while(tExtampInfo.sp_ctl_reg_val > spkvol){
			if(tExtampInfo.sp_ctl_reg_val < spkvol + VOL_STEP_DN)
				tExtampInfo.sp_ctl_reg_val = (u8)spkvol;
			else
				tExtampInfo.sp_ctl_reg_val -= VOL_STEP_DN;

			snd_extamp_i2c_write(SPEAKER_CTL_REG_ADDR, tExtampInfo.sp_ctl_reg_val);
			msleep(DEALY_DN);
		}
	}

	if(hphvol > tExtampInfo.hpl_ctl_reg_val){
		while(tExtampInfo.hpl_ctl_reg_val < hphvol){
			if(tExtampInfo.hpl_ctl_reg_val + VOL_STEP > hphvol)
				tExtampInfo.hpl_ctl_reg_val = (u8)hphvol;
			else
				tExtampInfo.hpl_ctl_reg_val += VOL_STEP;

			snd_extamp_i2c_write(HEADPHONE_LEFT_CTL_REG_ADDR, tExtampInfo.hpl_ctl_reg_val);
			snd_extamp_i2c_write(HEADPHONE_RIGHT_CTL_REG_ADDR, tExtampInfo.hpl_ctl_reg_val);
			msleep(DEALY);
		}
	}
	else if(hphvol < tExtampInfo.hpl_ctl_reg_val){
		while(tExtampInfo.hpl_ctl_reg_val > hphvol){
			if(tExtampInfo.hpl_ctl_reg_val < hphvol + VOL_STEP_DN)
				tExtampInfo.hpl_ctl_reg_val = (u8)hphvol;
			else
				tExtampInfo.hpl_ctl_reg_val -= VOL_STEP_DN;

			snd_extamp_i2c_write(HEADPHONE_LEFT_CTL_REG_ADDR, tExtampInfo.hpl_ctl_reg_val);
			snd_extamp_i2c_write(HEADPHONE_RIGHT_CTL_REG_ADDR, tExtampInfo.hpl_ctl_reg_val);
			msleep(DEALY_DN);
		}
	}
	tExtampInfo.hpr_ctl_reg_val = tExtampInfo.hpl_ctl_reg_val;
#endif
	AMP_DBG_MED("  [SKY Audio] SetVolume. SPK(%d) HPH(%d)\n", tExtampInfo.hpr_ctl_reg_val, tExtampInfo.hpl_ctl_reg_val);

	/* Sleep AMP */
	if(spkvol == 0 && hphvol == 0){ 
		snd_extamp_sleep(1);
	}
	return;
}

/*==========================================================================
** snd_extamp_setpreampgain
**=========================================================================*/
/*static*/ void snd_extamp_setpreampgain(extamp_inmode_e inpath, u8 val)
{
	int status = 0;
	
	val = (u8)(val & 0x03);

#if 0
	switch(inpath)		
	{
	case EXTAMP_IN_INA:
		tExtampInfo.inp_gain_ctl_reg_val = (tExtampInfo.inp_gain_ctl_reg_val & 0xBF) | (val << 2);
		break;
	case EXTAMP_IN_INB:
		tExtampInfo.inp_gain_ctl_reg_val = (tExtampInfo.inp_gain_ctl_reg_val & 0x7F) | (val);
		break;
	case EXTAMP_IN_INAB:
		tExtampInfo.inp_gain_ctl_reg_val = (tExtampInfo.inp_gain_ctl_reg_val & 0x3F) | (val << 2) | (val);
		break;
	case EXTAMP_IN_RXIN:
	default:
		return;
	}
#endif
       tExtampInfo.inp_gain_ctl_reg_val = 0;
	status = snd_extamp_i2c_write(INPUT_GAIN_CTL_REG_ADDR, tExtampInfo.inp_gain_ctl_reg_val);
	AMP_DBG_MED("  [SKY Audio] snd_extamp_setpreampgain(%d)\n", tExtampInfo.inp_gain_ctl_reg_val);
	if(status)
	{
		AMP_DBG_MED("  [SKY Audio] snd_extamp_setpreampgain fail(%d)\n", status);
	}
}

/*==========================================================================
** snd_extamp_sleep
**=========================================================================*/
/*static*/ void snd_extamp_sleep(u8 enable)
{
	//unsigned char SleepOk = 0;

#if 0 //jhpark-TEST
	
	SleepOk = (enable && !bHeadphonePath && tExtampInfo.spk_vol_reg_val == 0 && tExtampInfo.hpl_vol_reg_val == 0 && tExtampInfo.hpr_vol_reg_val == 0);

	tExtampInfo.outm_ctl_reg_val &= ~SHDN_OPERATION;
	tExtampInfo.outm_ctl_reg_val = SleepOk ? tExtampInfo.outm_ctl_reg_val | SHDN_SHUTDOWN : tExtampInfo.outm_ctl_reg_val | SHDN_OPERATION;
	
	snd_extamp_i2c_write(OUTMODE_CTL_REG_ADDR, tExtampInfo.outm_ctl_reg_val);
#endif	

	//AMP_DBG_MED("[SKY Audio] AMP Sleep(%d)\n", SleepOk);
}

/*==========================================================================
** snd_extamp_i2c_write
**=========================================================================*/
static int snd_extamp_i2c_write(u8 reg, u8 data)
{
	static int ret = 0;
	unsigned char buf[2];
	struct i2c_msg msg[1];

	if(!max97001_i2c_client){
		return -1;
	}

	buf[0] = (unsigned char)reg;
	buf[1] = (unsigned char)data;

	msg[0].addr = max97001_i2c_client->addr;
	msg[0].flags = 0;
	msg[0].len = 2;
	msg[0].buf = buf;
	
	ret = i2c_transfer(max97001_i2c_client->adapter, msg, 1);
	//printk("  [SKY Audio] snd_extamp_i2c_write ret= %d \n", ret);
	if (ret < 0) {
		return -1;
	}

	return 0;
}

/*==========================================================================
** snd_extamp_i2c_read
**=========================================================================*/
/*
static int snd_extamp_i2c_read(u8 reg, u8 *data)
{
	static int ret = 0;
	unsigned char buf[1];
	struct i2c_msg msgs[2];

	if(!max97001_i2c_client){
		return -1;
	}

	buf[0] = reg;

	msgs[0].addr = max97001_i2c_client->addr;
	msgs[0].flags = 0;
	msgs[0].len = 1;
	msgs[0].buf = buf;

	msgs[1].addr = max97001_i2c_client->addr;
	msgs[1].flags = I2C_M_RD;
	msgs[1].len = 1;
	msgs[1].buf = buf;

	ret = i2c_transfer(max97001_i2c_client->adapter, msgs, 2);
	if ( ret < 0) {
		return -1;
	}

	*data = (u8)buf[0];
	return 0;
}
*/

/*==========================================================================
** miscellaneous device
**=========================================================================*/
#ifdef FEATURE_SKY_SND_MISC_DEVICE
static struct file_operations snd_fops = {
	.owner		= THIS_MODULE,
	.open		= ext_amp_open,
	.release	= ext_amp_release,
	.unlocked_ioctl	= ext_amp_ioctl,
};
static struct miscdevice snd_miscdev = {
	.minor =    MISC_DYNAMIC_MINOR,
	.name =     "ext_amp",
	.fops =     &snd_fops
};
/*static*/ int snd_misc_device_init(void)
{
    int result=0;
    result = misc_register(&snd_miscdev);
    if(result<0)    AMP_DBG_MED("snd_misc_device_init ret= %d ", result);
    return result;
}
/*static*/ int snd_misc_device_deinit(void)
{
    int result=0;
    result = misc_deregister(&snd_miscdev);
    if(result<0)    AMP_DBG_MED("snd_misc_device_deinit ret= %d ", result);
    return result;
}
/*static*/ int ext_amp_open(struct inode *inode, struct file *file)
{
	AMP_DBG_LOW("ext_amp_open ");
	return 0;
}

/*static*/ int ext_amp_release(struct inode *inode, struct file *file)
{
	AMP_DBG_LOW("ext_amp_release ");
	return 0;	
}

/*static*/ long ext_amp_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
       uint32_t value;
	AMP_DBG_LOW("ext_amp_ioctl cmd:0x%x, arg:0x%x ", cmd, arg);
    
    	switch (cmd) {
#ifdef FEATURE_SKY_SND_AUDIO_TEST_COMMAND
            case SND_SKY_EXTAMP_WRITE:
                if (get_user(value, (uint32_t __user *) arg)) {
                    AMP_ERR("Err:aud_sub_ioctl cmd SND_SKY_EXTAMP_WRITE");
                    return -EPERM;
                }
                current_reg= (value&0xff00)>>8;
                current_regval = (value&0xff);
                AMP_DBG_HIGH("SND_SKY_EXTAMP_WRITE (%d, %d)", current_reg, current_regval);
                snd_extamp_write_single_reg(current_reg, current_regval);
            break;

            case SND_SKY_EXTAMP_READ:
                if (get_user(value, (uint32_t __user *) arg)) {
                    AMP_ERR("Err:aud_sub_ioctl cmd SND_SKY_EXTAMP_READ");
                    return -EPERM;
                }
                current_reg = value;
                current_regval = snd_extamp_read_single_reg(value);
                AMP_DBG_HIGH("SND_SKY_EXTAMP_READ (%d, %d)", current_reg, current_regval);
                value = current_regval&0xff;
                if (copy_to_user((void*) arg, &value, sizeof(int))) {
                    AMP_ERR("aud_sub_ioctl cmd SND_SKY_EXTAMP_READ ERROR!!!");
                }
            break;
#endif		
            case SND_SKY_EXTAMP_SET_MODE:
                if (get_user(value, (uint32_t __user *) arg)) {
                    AMP_ERR("Err:aud_sub_ioctl cmd SND_SKY_EXTAMP_SET_MODE");
                    return -EPERM;
                }
                curMode=value; 
                AMP_DBG_MED("SND_SKY_EXTAMP_SET_MODE (%d)", value);
                break;
           case SND_SKY_SET_CALLMODE:
                  if (get_user(value, (uint32_t __user *) arg)) {
                        AMP_ERR("SND_SKY_SET_CALLMODE fail!\n");
			   return -EPERM;
                  }
                      curCallMode.mode = (value&0xff00)>>8;
                      curCallMode.param = (value&0xff);

                  AMP_DBG_MED("SND_SKY_SET_CALLMODE mode=%d, param=%d", curCallMode.mode, curCallMode.param);
              break;
          case SND_SKY_GET_CALLMODE:
              AMP_DBG_MED("SND_SKY_GET_CALLMODE mode=%d, param=%d", curCallMode.mode, curCallMode.param);
		if (copy_to_user((void*) arg, &curCallMode.mode, sizeof(int))) {
			AMP_ERR("SND_SKY_GET_CALLMODE fail!\n");
		}
            break;
          case SND_SKY_SET_AUTOANSWER:
                  if (get_user(value, (uint32_t __user *) arg)) {
                        AMP_ERR("SND_SKY_SET_AUTOANSWER fail!\n");
                        return -EPERM;
                  }
                      curCallMode.mode = (value&0xff00)>>8;
                      curCallMode.param = (value&0xff);

                  AMP_DBG_MED("SND_SKY_SET_AUTOANSWER  mode=%d, param=%d", curCallMode.mode, curCallMode.param);
		  if (curCallMode.param == 0)
                      snd_extamp_api_SetDevice(1, 1);           
              break;

            default:
                  break;
    	}
       return 0;
}
#endif

#ifdef FEATURE_SKY_SND_AUDIO_TEST_COMMAND
/*static*/ int snd_extamp_read_single_reg(int reg)
{
    int regval=0;
    switch(reg)
    {
         case 0:
             regval=tExtampInfo.inp_gain_ctl_reg_val;
             break;
         case 1:
             regval=tExtampInfo.hp_mixers_ctl_reg_val;
             break;
         case 2:
             regval=tExtampInfo.sp_mixer_ctl_reg_val;
             break;
         case 3:
             regval=tExtampInfo.hpl_ctl_reg_val;
             break;
         case 4:
             regval=tExtampInfo.hpr_ctl_reg_val;
             break;
         case 5:
             regval=tExtampInfo.sp_ctl_reg_val;
             break;
         case 6:
             regval=tExtampInfo.limiter_ctl_reg_val;
             break;
         case 7:
             regval=tExtampInfo.power_man_ctl_reg_val;
             break;
         case 8:
             regval=tExtampInfo.charge_pump_ctl_reg_val;
             break;
     }    
    return regval;
}
/*static*/ void snd_extamp_write_single_reg(int reg, int regval)
{
	extamp_info_t tCurrentExtampInfo;
	snd_extamp_make_current(&tCurrentExtampInfo);

       switch(reg)
       {
            case 0:
                tCurrentExtampInfo.inp_gain_ctl_reg_val=regval;
                break;
            case 1:
                tCurrentExtampInfo.hp_mixers_ctl_reg_val=regval;
                break;
            case 2:
                tCurrentExtampInfo.sp_mixer_ctl_reg_val=regval;
                break;
            case 3:
                tCurrentExtampInfo.hpl_ctl_reg_val=regval;
                break;
            case 4:
                tCurrentExtampInfo.hpr_ctl_reg_val=regval;
                break;
            case 5:
                tCurrentExtampInfo.sp_ctl_reg_val=regval;
                break;
            case 6:
                tCurrentExtampInfo.limiter_ctl_reg_val=regval;
                break;
            case 7:
                tCurrentExtampInfo.power_man_ctl_reg_val=regval;
                break;
            case 8:
                tCurrentExtampInfo.charge_pump_ctl_reg_val=regval;
                break;
        }
       snd_extamp_write_all_reg(tCurrentExtampInfo);
}
#endif

