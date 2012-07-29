/* Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/debugfs.h>
#include <linux/mfd/pmic8058.h>
#include <linux/pmic8058-othc.h>
#include <linux/mfd/pmic8901.h>
#include <linux/mfd/msm-adie-codec.h>
#include <linux/regulator/pmic8058-regulator.h>
#include <linux/regulator/pmic8901-regulator.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/machine.h>

#include <mach/qdsp6v2/audio_dev_ctl.h>
#include <mach/qdsp6v2/apr_audio.h>
#include <mach/mpp.h>
#include <asm/mach-types.h>
#include <asm/uaccess.h>

#include "snddev_icodec.h"
#include "snddev_ecodec.h"
#ifdef CONFIG_MACH_MSM8X60_PRESTO
// jykim - temp code
#define CONFIG_PANTECH_ASR_PATH
#define CONFIG_PANTECH_BT_GROUPING

#include "timpani_profile_8x60_presto.h"
#else
#include "timpani_profile_8x60.h"
#endif
#include "snddev_hdmi.h"
#include "snddev_mi2s.h"
#include "snddev_virtual.h"

#ifdef CONFIG_MACH_MSM8X60_PRESTO
#include <mach/board.h>
#ifdef CONFIG_PANTECH_AUDIO_PRESTO_FAB2200
#include "snd_presto_sub_fab2200.h"
#endif
#ifdef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020
#include "snd_presto_audience_a2020.h"
#endif
#endif  // CONFIG_MACH_MSM8X60_PRESTO

#ifdef CONFIG_DEBUG_FS
static struct dentry *debugfs_hsed_config;
static void snddev_hsed_config_modify_setting(int type);
static void snddev_hsed_config_restore_setting(void);
#endif

/* GPIO_CLASS_D0_EN */
#define SNDDEV_GPIO_CLASS_D0_EN 227

/* GPIO_CLASS_D1_EN */
#define SNDDEV_GPIO_CLASS_D1_EN 229

#define SNDDEV_GPIO_MIC2_ANCR_SEL 294

static struct resource msm_cdcclk_ctl_resources[] = {
	{
		.name   = "msm_snddev_tx_mclk",
		.start  = 108,
		.end    = 108,
		.flags  = IORESOURCE_IO,
	},
	{
		.name   = "msm_snddev_rx_mclk",
		.start  = 109,
		.end    = 109,
		.flags  = IORESOURCE_IO,
	},
};

static struct platform_device msm_cdcclk_ctl_device = {
	.name   = "msm_cdcclk_ctl",
	.num_resources  = ARRAY_SIZE(msm_cdcclk_ctl_resources),
	.resource       = msm_cdcclk_ctl_resources,
};

static struct resource msm_aux_pcm_resources[] = {

	{
		.name   = "aux_pcm_dout",
		.start  = 111,
		.end    = 111,
		.flags  = IORESOURCE_IO,
	},
	{
		.name   = "aux_pcm_din",
		.start  = 112,
		.end    = 112,
		.flags  = IORESOURCE_IO,
	},
	{
		.name   = "aux_pcm_syncout",
		.start  = 113,
		.end    = 113,
		.flags  = IORESOURCE_IO,
	},
	{
		.name   = "aux_pcm_clkin_a",
		.start  = 114,
		.end    = 114,
		.flags  = IORESOURCE_IO,
	},
};

static struct platform_device msm_aux_pcm_device = {
	.name   = "msm_aux_pcm",
	.num_resources  = ARRAY_SIZE(msm_aux_pcm_resources),
	.resource       = msm_aux_pcm_resources,
};

static struct resource msm_mi2s_gpio_resources[] = {

	{
		.name   = "mi2s_ws",
		.start  = 101,
		.end    = 101,
		.flags  = IORESOURCE_IO,
	},
	{
		.name   = "mi2s_sclk",
		.start  = 102,
		.end    = 102,
		.flags  = IORESOURCE_IO,
	},
	{
		.name   = "mi2s_mclk",
		.start  = 103,
		.end    = 103,
		.flags  = IORESOURCE_IO,
	},
	{
		.name   = "fm_mi2s_sd",
		.start  = 107,
		.end    = 107,
		.flags  = IORESOURCE_IO,
	},
};

static struct platform_device msm_mi2s_device = {
	.name		= "msm_mi2s",
	.num_resources	= ARRAY_SIZE(msm_mi2s_gpio_resources),
	.resource	= msm_mi2s_gpio_resources,
};

/* Must be same size as msm_icodec_gpio_resources */
static int msm_icodec_gpio_defaults[] = {
	0,
	0,
};

static struct resource msm_icodec_gpio_resources[] = {
	{
		.name   = "msm_icodec_speaker_left",
		.start  = SNDDEV_GPIO_CLASS_D0_EN,
		.end    = SNDDEV_GPIO_CLASS_D0_EN,
		.flags  = IORESOURCE_IO,
	},
	{
		.name   = "msm_icodec_speaker_right",
		.start  = SNDDEV_GPIO_CLASS_D1_EN,
		.end    = SNDDEV_GPIO_CLASS_D1_EN,
		.flags  = IORESOURCE_IO,
	},
};

static struct platform_device msm_icodec_gpio_device = {
	.name   = "msm_icodec_gpio",
	.num_resources  = ARRAY_SIZE(msm_icodec_gpio_resources),
	.resource       = msm_icodec_gpio_resources,
	.dev = { .platform_data = &msm_icodec_gpio_defaults },
};

static int msm_qt_icodec_gpio_defaults[] = {
	0,
};

static struct resource msm_qt_icodec_gpio_resources[] = {
	{
		.name   = "msm_icodec_speaker_gpio",
		.start  = SNDDEV_GPIO_CLASS_D0_EN,
		.end    = SNDDEV_GPIO_CLASS_D0_EN,
		.flags  = IORESOURCE_IO,
	},
};

static struct platform_device msm_qt_icodec_gpio_device = {
	.name   = "msm_icodec_gpio",
	.num_resources  = ARRAY_SIZE(msm_qt_icodec_gpio_resources),
	.resource       = msm_qt_icodec_gpio_resources,
	.dev = { .platform_data = &msm_qt_icodec_gpio_defaults },
};

static struct regulator *s3;
static struct regulator *mvs;

static int msm_snddev_enable_dmic_power(void)
{
	int ret;

	s3 = regulator_get(NULL, "8058_s3");
	if (IS_ERR(s3)) {
		ret = -EBUSY;
		goto fail_get_s3;
	}

	ret = regulator_set_voltage(s3, 1800000, 1800000);
	if (ret) {
		pr_err("%s: error setting voltage\n", __func__);
		goto fail_s3;
	}

	ret = regulator_enable(s3);
	if (ret) {
		pr_err("%s: error enabling regulator\n", __func__);
		goto fail_s3;
	}

	mvs = regulator_get(NULL, "8901_mvs0");
	if (IS_ERR(mvs))
		goto fail_mvs0_get;

	ret = regulator_enable(mvs);
	if (ret) {
		pr_err("%s: error setting regulator\n", __func__);
		goto fail_mvs0_enable;
	}
	return ret;

fail_mvs0_enable:
	regulator_put(mvs);
	mvs = NULL;
fail_mvs0_get:
	regulator_disable(s3);
fail_s3:
	regulator_put(s3);
	s3 = NULL;
fail_get_s3:
	return ret;
}

static void msm_snddev_disable_dmic_power(void)
{
	int ret;

	if (mvs) {
		ret = regulator_disable(mvs);
		if (ret < 0)
			pr_err("%s: error disabling vreg mvs\n", __func__);
		regulator_put(mvs);
		mvs = NULL;
	}

	if (s3) {
		ret = regulator_disable(s3);
		if (ret < 0)
			pr_err("%s: error disabling regulator s3\n", __func__);
		regulator_put(s3);
		s3 = NULL;
	}
}

static struct regulator *l11;

static int msm_snddev_enable_qt_dmic_power(void)
{
	int ret;

	l11 = regulator_get(NULL, "8058_l11");
	if (IS_ERR(l11))
		return -EBUSY;

	ret = regulator_set_voltage(l11, 1500000, 1500000);
	if (ret) {
		pr_err("%s: error setting regulator\n", __func__);
		goto fail_l11;
	}
	ret = regulator_enable(l11);
	if (ret) {
		pr_err("%s: error enabling regulator\n", __func__);
		goto fail_l11;
	}
	return 0;

fail_l11:
	regulator_put(l11);
	l11 = NULL;
	return ret;
}


static void msm_snddev_disable_qt_dmic_power(void)
{
	int ret;

	if (l11) {
		ret = regulator_disable(l11);
		if (ret < 0)
			pr_err("%s: error disabling regulator l11\n", __func__);
		regulator_put(l11);
		l11 = NULL;
	}
}

#define PM8901_MPP_3 (2) /* PM8901 MPP starts from 0 */
#ifdef CONFIG_MACH_MSM8X60_PRESTO  // jmlee 
int config_class_d1_gpio(int enable)
#else
static int config_class_d1_gpio(int enable)
#endif
{
	int rc;

	if (enable) {
		rc = gpio_request(SNDDEV_GPIO_CLASS_D1_EN, "CLASSD1_EN");
		if (rc) {
			pr_err("%s: spkr pamp gpio %d request"
			"failed\n", __func__, SNDDEV_GPIO_CLASS_D1_EN);
			return rc;
		}
		gpio_direction_output(SNDDEV_GPIO_CLASS_D1_EN, 1);
	} else {
		gpio_set_value_cansleep(SNDDEV_GPIO_CLASS_D1_EN, 0);
		gpio_free(SNDDEV_GPIO_CLASS_D1_EN);
	}
	return 0;
}

#ifdef CONFIG_MACH_MSM8X60_PRESTO  // jmlee 
int config_class_d0_gpio(int enable)
#else
static int config_class_d0_gpio(int enable)
#endif
{
	int rc;

	if (enable) {
		rc = pm8901_mpp_config_digital_out(PM8901_MPP_3,
			PM8901_MPP_DIG_LEVEL_MSMIO, 1);

		if (rc) {
			pr_err("%s: CLASS_D0_EN failed\n", __func__);
			return rc;
		}

		rc = gpio_request(SNDDEV_GPIO_CLASS_D0_EN, "CLASSD0_EN");

		if (rc) {
			pr_err("%s: spkr pamp gpio pm8901 mpp3 request"
			"failed\n", __func__);
			pm8901_mpp_config_digital_out(PM8901_MPP_3,
			PM8901_MPP_DIG_LEVEL_MSMIO, 0);
			return rc;
		}

		gpio_direction_output(SNDDEV_GPIO_CLASS_D0_EN, 1);
		gpio_set_value(SNDDEV_GPIO_CLASS_D0_EN, 1);

	} else {
		pm8901_mpp_config_digital_out(PM8901_MPP_3,
		PM8901_MPP_DIG_LEVEL_MSMIO, 0);
		gpio_set_value(SNDDEV_GPIO_CLASS_D0_EN, 0);
		gpio_free(SNDDEV_GPIO_CLASS_D0_EN);
	}
	return 0;
}

static atomic_t pamp_ref_cnt;

int msm_snddev_poweramp_on(void)
{
	int rc = 0;
#ifndef CONFIG_MACH_MSM8X60_PRESTO  // jmlee 
	if (atomic_inc_return(&pamp_ref_cnt) > 1)
		return 0;

	pr_debug("%s: enable stereo spkr amp\n", __func__);
	rc = config_class_d0_gpio(1);
	if (rc) {
		pr_err("%s: d0 gpio configuration failed\n", __func__);
		goto config_gpio_fail;
	}
	if (!machine_is_msm8x60_qt()) {
		rc = config_class_d1_gpio(1);
		if (rc) {
			pr_err("%s: d1 gpio configuration failed\n", __func__);
			config_class_d0_gpio(0);
		}
	}
config_gpio_fail:
#endif	
	return rc;
}

void msm_snddev_poweramp_off(void)
{
#ifndef CONFIG_MACH_MSM8X60_PRESTO  // jmlee 
	if (atomic_dec_return(&pamp_ref_cnt) == 0) {
		pr_debug("%s: disable stereo spkr amp\n", __func__);
		config_class_d0_gpio(0);
		if (!machine_is_msm8x60_qt())
			config_class_d1_gpio(0);
		msleep(30);
	}
#endif	
}

#ifdef CONFIG_MACH_MSM8X60_PRESTO
static struct regulator *snddev_reg_l9_switch;

// presto 8058_l9 power enable
int audience_pmic_L9_switch_on(void)
{
	int rc = 0;
	
	snddev_reg_l9_switch = regulator_get(NULL, "8058_l9");
	if (IS_ERR(snddev_reg_l9_switch)) {
		printk("vreg_enable failed(%s) = %d \n", "8058_l9", rc);
	}

	rc = regulator_set_voltage(snddev_reg_l9_switch, 2700000, 2700000);
	if (rc < 0)
		printk("vreg_enable failed(%s) = %d \n", "8058_l9", rc);

	rc = regulator_enable(snddev_reg_l9_switch);
	if (rc < 0)
		printk("vreg_enable failed(%s) = %d \n",	 "8058_l9", rc);

	return rc;
}

// presto 8058_l9  power disable
int audience_pmic_L9_switch_off(void)
{
	int rc = 0;
	if (!snddev_reg_l9_switch)
	{	
		rc = regulator_disable(snddev_reg_l9_switch);
		if (rc < 0)
			printk(KERN_ERR "%s: vreg_enable failed(%s) = %d \n",__func__, "8058_l9", rc);

		regulator_put(snddev_reg_l9_switch);

		snddev_reg_l9_switch = NULL;
	}
	return rc;
}
#endif

#ifndef CONFIG_SKY_SND_CTRL
/* Regulator 8058_l10 supplies regulator 8058_ncp. */
static struct regulator *snddev_reg_ncp;
static struct regulator *snddev_reg_l10;
#endif

static int msm_snddev_voltage_on(void)
{
#ifdef CONFIG_SKY_SND_CTRL
	return 0;
#else
	int rc;
	pr_debug("%s\n", __func__);

	snddev_reg_l10 = regulator_get(NULL, "8058_l10");
	if (IS_ERR(snddev_reg_l10)) {
		pr_err("%s: regulator_get(%s) failed (%ld)\n", __func__,
			"l10", PTR_ERR(snddev_reg_l10));
		return -EBUSY;
	}

	rc = regulator_set_voltage(snddev_reg_l10, 2600000, 2600000);
	if (rc < 0)
		pr_err("%s: regulator_set_voltage(l10) failed (%d)\n",
			__func__, rc);

	rc = regulator_enable(snddev_reg_l10);
	if (rc < 0)
		pr_err("%s: regulator_enable(l10) failed (%d)\n", __func__, rc);

	snddev_reg_ncp = regulator_get(NULL, "8058_ncp");
	if (IS_ERR(snddev_reg_ncp)) {
		pr_err("%s: regulator_get(%s) failed (%ld)\n", __func__,
			"ncp", PTR_ERR(snddev_reg_ncp));
		return -EBUSY;
	}

	rc = regulator_set_voltage(snddev_reg_ncp, 1800000, 1800000);
	if (rc < 0) {
		pr_err("%s: regulator_set_voltage(ncp) failed (%d)\n",
			__func__, rc);
		goto regulator_fail;
	}

	rc = regulator_enable(snddev_reg_ncp);
	if (rc < 0) {
		pr_err("%s: regulator_enable(ncp) failed (%d)\n", __func__, rc);
		goto regulator_fail;
	}

	return rc;

regulator_fail:
	regulator_put(snddev_reg_ncp);
	snddev_reg_ncp = NULL;
	return rc;
#endif    
}

static void msm_snddev_voltage_off(void)
{
#ifndef CONFIG_SKY_SND_CTRL
	int rc;
	pr_debug("%s\n", __func__);

	if (!snddev_reg_ncp)
		goto done;

	rc = regulator_disable(snddev_reg_ncp);
	if (rc < 0)
		pr_err("%s: regulator_disable(ncp) failed (%d)\n",
			__func__, rc);

	regulator_put(snddev_reg_ncp);

	snddev_reg_ncp = NULL;

done:
	if (!snddev_reg_l10)
		return;

	rc = regulator_disable(snddev_reg_l10);
	if (rc < 0)
		pr_err("%s: regulator_disable(l10) failed (%d)\n",
			__func__, rc);

	regulator_put(snddev_reg_l10);

	snddev_reg_l10 = NULL;
#endif    
}

static int msm_snddev_enable_amic_power(void)
{
	int ret = 0;
#ifdef CONFIG_MACH_MSM8X60_PRESTO
              audience_pmic_L9_switch_on();
		ret = pm8058_micbias_enable(OTHC_MICBIAS_0,OTHC_SIGNAL_ALWAYS_ON);
              msm_snddev_mic_sel_QTR();
#else	

#ifdef CONFIG_PMIC8058_OTHC

	if (machine_is_msm8x60_fluid()) {

		ret = pm8058_micbias_enable(OTHC_MICBIAS_0,
				OTHC_SIGNAL_ALWAYS_ON);
		if (ret)
			pr_err("%s: Enabling amic power failed\n", __func__);

		ret = gpio_request(SNDDEV_GPIO_MIC2_ANCR_SEL, "MIC2_ANCR_SEL");
		if (ret) {
			pr_err("%s: spkr pamp gpio %d request failed\n",
				__func__, SNDDEV_GPIO_MIC2_ANCR_SEL);
			return ret;
		}
		gpio_direction_output(SNDDEV_GPIO_MIC2_ANCR_SEL, 0);

	} else {
       #ifdef CONFIG_SKY_SND_CTRL
		ret = pm8058_micbias_enable(OTHC_MICBIAS_0,
				OTHC_SIGNAL_ALWAYS_ON);
	#else
		ret = pm8058_micbias_enable(OTHC_MICBIAS_2,
				OTHC_SIGNAL_ALWAYS_ON);
	#endif       
		if (ret)
			pr_err("%s: Enabling amic power failed\n", __func__);
	}
#endif
#endif
	return ret;
}

static void msm_snddev_disable_amic_power(void)
{
#ifdef CONFIG_MACH_MSM8X60_PRESTO
	int ret;
       audience_pmic_L9_switch_off();
	ret = pm8058_micbias_enable(OTHC_MICBIAS_0, OTHC_SIGNAL_OFF);
	if (ret)
		pr_err("%s: Disabling amic power failed\n", __func__);
#else
#ifdef CONFIG_PMIC8058_OTHC
	int ret;
	if (machine_is_msm8x60_fluid()) {
		ret = pm8058_micbias_enable(OTHC_MICBIAS_0,
				OTHC_SIGNAL_OFF);
		gpio_free(SNDDEV_GPIO_MIC2_ANCR_SEL);
	} else
       #ifdef CONFIG_SKY_SND_CTRL
		ret = pm8058_micbias_enable(OTHC_MICBIAS_0, OTHC_SIGNAL_OFF);
       #else
		ret = pm8058_micbias_enable(OTHC_MICBIAS_2, OTHC_SIGNAL_OFF);
	#endif

	if (ret)
		pr_err("%s: Disabling amic power failed\n", __func__);
#endif
#endif
}


#ifdef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020
int msm_snddev_enable_nr_mic_power(void)
{
	int ret = 0;
       audience_pmic_L9_switch_on();
       ret = pm8058_micbias_enable(OTHC_MICBIAS_0,OTHC_SIGNAL_ALWAYS_ON);
       msm_snddev_mic_sel_a2020();
	return ret;
}

void msm_snddev_disable_nr_mic_power(void)
{

	int ret;
       audience_pmic_L9_switch_off();
	ret = pm8058_micbias_enable(OTHC_MICBIAS_0, OTHC_SIGNAL_OFF);
	if (ret)
		pr_err("%s: Disabling amic power failed\n", __func__);
}
#endif

static int msm_snddev_enable_dmic_sec_power(void)
{
	int ret;

	ret = msm_snddev_enable_dmic_power();
	if (ret) {
		pr_err("%s: Error: Enabling dmic power failed\n", __func__);
		return ret;
	}
#ifdef CONFIG_PMIC8058_OTHC
	ret = pm8058_micbias_enable(OTHC_MICBIAS_2, OTHC_SIGNAL_ALWAYS_ON);
	if (ret) {
		pr_err("%s: Error: Enabling micbias failed\n", __func__);
		msm_snddev_disable_dmic_power();
		return ret;
	}
#endif
	return 0;
}

static void msm_snddev_disable_dmic_sec_power(void)
{
	msm_snddev_disable_dmic_power();

#ifdef CONFIG_PMIC8058_OTHC
	pm8058_micbias_enable(OTHC_MICBIAS_2, OTHC_SIGNAL_OFF);
#endif
}

static struct adie_codec_action_unit iearpiece_48KHz_osr256_actions[] =
	EAR_PRI_MONO_8000_OSR_256;

static struct adie_codec_hwsetting_entry iearpiece_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = iearpiece_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(iearpiece_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile iearpiece_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = iearpiece_settings,
	.setting_sz = ARRAY_SIZE(iearpiece_settings),
};

#ifdef CONFIG_MACH_MSM8X60_PRESTO  // jmlee jmlee for test code	
static struct regulator *snddev_reg_l9;
static int msm_snddev_enable_iearpiece_on(void)
{
#ifdef CONFIG_PMIC8058_OTHC
	int ret = 0;
	int rc;
       printk(KERN_ERR "%s:[JMLEE LOG] msm_snddev_enable_iearpiece_on\n", __func__);
	snddev_reg_l9 = regulator_get(NULL, "8058_l9");
	if (IS_ERR(snddev_reg_l9)) {
		pr_err("%s: regulator_get(%s) failed (%ld)\n", __func__,
			"l9", PTR_ERR(snddev_reg_l9));
	}

	rc = regulator_set_voltage(snddev_reg_l9, 2700000, 2700000);
	if (rc < 0)
		pr_err("%s: regulator_set_voltage(l9) failed (%d)\n",
			__func__, rc);

	rc = regulator_enable(snddev_reg_l9);
	if (rc < 0)
		pr_err("%s: regulator_enable(l9) failed (%d)\n", __func__, rc);
//////////////
       config_GPIO_NR_OUT_SEL(0);
	return ret;
	
#endif
}

static void msm_snddev_disable_iearpeace_off(void)
{
#ifdef CONFIG_PMIC8058_OTHC
	int rc;
	if (!snddev_reg_l9)
		return;
       printk(KERN_ERR "%s:[JMLEE LOG] msm_snddev_disable_iearpeace_off\n", __func__);

	rc = regulator_disable(snddev_reg_l9);
	if (rc < 0)
		pr_err("%s: regulator_disable(l9) failed (%d)\n",
			__func__, rc);

	regulator_put(snddev_reg_l9);

	snddev_reg_l9 = NULL;
//////////////		
		//gpio_free(57);
#endif
}

#endif
static struct snddev_icodec_data snddev_iearpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "handset_rx",
	.copp_id = 0,
	.profile = &iearpiece_profile,
	.channel_mode = 1,
	.default_sample_rate = 48000,
#ifdef CONFIG_MACH_MSM8X60_PRESTO  // jmlee 	
	.pamp_on = msm_snddev_enable_iearpiece_on,
	.pamp_off = msm_snddev_disable_iearpeace_off,
#endif	
};

static struct platform_device msm_iearpiece_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &snddev_iearpiece_data },
};

static struct adie_codec_action_unit imic_48KHz_osr256_actions[] =
	AMIC_PRI_MONO_OSR_256;

static struct adie_codec_hwsetting_entry imic_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = imic_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(imic_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile imic_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = imic_settings,
	.setting_sz = ARRAY_SIZE(imic_settings),
};

static struct snddev_icodec_data snddev_imic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "handset_tx",
	.copp_id = 1,
	.profile = &imic_profile,
	.channel_mode = 1,
	.default_sample_rate = 48000,
	.pamp_on = msm_snddev_enable_amic_power,
	.pamp_off = msm_snddev_disable_amic_power,
};

static struct platform_device msm_imic_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &snddev_imic_data },
};

static struct snddev_icodec_data snddev_fluid_ispkr_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "speaker_mono_tx",
	.copp_id = PRIMARY_I2S_TX,
	.profile = &imic_profile,
	.channel_mode = 1,
	.default_sample_rate = 48000,
	.pamp_on = msm_snddev_enable_amic_power,
	.pamp_off = msm_snddev_disable_amic_power,
};

static struct platform_device msm_fluid_ispkr_mic_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &snddev_fluid_ispkr_mic_data },
};


static struct adie_codec_action_unit headset_ab_cpls_48KHz_osr256_actions[] =
#ifdef CONFIG_SKY_SND_CTRL /* 20110429-jhpark: RX HPH CLASS AB LEGACY */
	HPH_PRI_AB_LEG_STEREO; 
#else
	HEADSET_AB_CPLS_48000_OSR_256;
#endif

static struct adie_codec_hwsetting_entry headset_ab_cpls_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = headset_ab_cpls_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(headset_ab_cpls_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile headset_ab_cpls_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = headset_ab_cpls_settings,
	.setting_sz = ARRAY_SIZE(headset_ab_cpls_settings),
};

static struct snddev_icodec_data snddev_ihs_stereo_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "headset_stereo_rx",
	.copp_id = 0,
	.profile = &headset_ab_cpls_profile,
	.channel_mode = 2,
	.default_sample_rate = 48000,
	.voltage_on = msm_snddev_voltage_on,
	.voltage_off = msm_snddev_voltage_off,
#ifdef CONFIG_PANTECH_AUDIO_PRESTO_FAB2200
	.pamp_on = &snd_subsystem_hp_poweron,
	.pamp_off = &snd_subsystem_hp_powerdown,
#else
	.pamp_on = NULL,
	.pamp_off = NULL,
#endif
};

static struct platform_device msm_headset_stereo_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &snddev_ihs_stereo_rx_data },
};

static struct adie_codec_action_unit headset_anc_48KHz_osr256_actions[] =
	ANC_HEADSET_CPLS_AMIC1_AUXL_RX1_48000_OSR_256;

static struct adie_codec_hwsetting_entry headset_anc_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = headset_anc_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(headset_anc_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile headset_anc_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = headset_anc_settings,
	.setting_sz = ARRAY_SIZE(headset_anc_settings),
};

static struct snddev_icodec_data snddev_anc_headset_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE | SNDDEV_CAP_ANC),
	.name = "anc_headset_stereo_rx",
	.copp_id = PRIMARY_I2S_RX,
	.profile = &headset_anc_profile,
	.channel_mode = 2,
	.default_sample_rate = 48000,
	.pamp_on = msm_snddev_enable_amic_power,
	.pamp_off = msm_snddev_disable_amic_power,
	.voltage_on = msm_snddev_voltage_on,
	.voltage_off = msm_snddev_voltage_off,
};

static struct platform_device msm_anc_headset_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &snddev_anc_headset_data },
};

static struct adie_codec_action_unit ispkr_stereo_48KHz_osr256_actions[] =
	SPEAKER_PRI_STEREO_48000_OSR_256;

static struct adie_codec_hwsetting_entry ispkr_stereo_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ispkr_stereo_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ispkr_stereo_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile ispkr_stereo_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = ispkr_stereo_settings,
	.setting_sz = ARRAY_SIZE(ispkr_stereo_settings),
};

static struct snddev_icodec_data snddev_ispkr_stereo_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "speaker_stereo_rx",
	.copp_id = 0,
	.profile = &ispkr_stereo_profile,
#ifdef CONFIG_PANTECH_AUDIO_PRESTO_FAB2200 //CONFIG_SKY_SND_EXTAMP /* 20110521-jhpark:If you use 2, it is stereo so please use 1 then L+R will be mixed. */
	.channel_mode = 1,
#else
	.channel_mode = 2,
#endif
	.default_sample_rate = 48000,
#ifdef CONFIG_PANTECH_AUDIO_PRESTO_FAB2200    //20110428 jhsong : feature clearup
	.pamp_on = &snd_subsystem_sp_poweron,
	.pamp_off = &snd_subsystem_sp_powerdown,
#else
	.pamp_on = msm_snddev_poweramp_on,
	.pamp_off = msm_snddev_poweramp_off,
#endif	
};

static struct platform_device msm_ispkr_stereo_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &snddev_ispkr_stereo_data },
};

#ifdef CONFIG_MACH_MSM8X60_PRESTO //def FEATURE_PANTECH_PRESTO_SND // 20110919 jmlee communication test block
static struct adie_codec_action_unit ispkr_stereo_communication_48KHz_osr256_actions[] =
	SPEAKER_PRI_STEREO_48000_OSR_256;

static struct adie_codec_hwsetting_entry ispkr_stereo_communication_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ispkr_stereo_communication_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ispkr_stereo_communication_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile ispkr_stereo_communication_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = ispkr_stereo_communication_settings,
	.setting_sz = ARRAY_SIZE(ispkr_stereo_communication_settings),
};

static struct snddev_icodec_data snddev_ispkr_communication_stereo_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "speaker_stereo_communication_rx",
	.copp_id = 0,
	.profile = &ispkr_stereo_communication_profile,
#ifdef CONFIG_PANTECH_AUDIO_PRESTO_FAB2200 //CONFIG_SKY_SND_EXTAMP /* 20110521-jhpark:If you use 2, it is stereo so please use 1 then L+R will be mixed. */
	.channel_mode = 1,
#else
	.channel_mode = 2,
#endif
	.default_sample_rate = 48000,
#ifdef CONFIG_PANTECH_AUDIO_PRESTO_FAB2200    //20110428 jhsong : feature clearup
	.pamp_on = &snd_subsystem_sp_poweron,
	.pamp_off = &snd_subsystem_sp_powerdown,
#else
	.pamp_on = msm_snddev_poweramp_on,
	.pamp_off = msm_snddev_poweramp_off,
#endif	
};

static struct platform_device msm_ispkr_stereo_communication_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &snddev_ispkr_communication_stereo_data },
};
#endif

static struct adie_codec_action_unit idmic_mono_48KHz_osr256_actions[] =
	DMIC1_PRI_MONO_OSR_256;

static struct adie_codec_hwsetting_entry idmic_mono_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = idmic_mono_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(idmic_mono_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile idmic_mono_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = idmic_mono_settings,
	.setting_sz = ARRAY_SIZE(idmic_mono_settings),
};

static struct snddev_icodec_data snddev_ispkr_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "speaker_mono_tx",
#ifdef CONFIG_SKY_SND_CTRL
	.copp_id = 1,
	.profile = &imic_profile,
#else
	.copp_id = PRIMARY_I2S_TX,
	.profile = &idmic_mono_profile,
#endif
	.channel_mode = 1,
	.default_sample_rate = 48000,
#ifdef CONFIG_SKY_SND_CTRL	
	.pamp_on = msm_snddev_enable_amic_power,
	.pamp_off = msm_snddev_disable_amic_power,
#else
	.pamp_on = msm_snddev_enable_dmic_power,
	.pamp_off = msm_snddev_disable_dmic_power,
#endif	
};

static struct platform_device msm_ispkr_mic_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &snddev_ispkr_mic_data },
};

#ifdef CONFIG_MACH_MSM8X60_PRESTO //def FEATURE_PANTECH_PRESTO_SND // 20110919 jmlee communication test block
#ifdef CONFIG_SKY_SND_CTRL  // 20110919 jmlee communication test block

#else
static struct adie_codec_action_unit idmic_mono_communication_48KHz_osr256_actions[] =
	DMIC1_PRI_MONO_OSR_256;

static struct adie_codec_hwsetting_entry idmic_mono_communication_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = idmic_mono_communication_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(idmic_mono_communication_48KHz_osr256_actions),
	}
};
static struct adie_codec_dev_profile idmic_mono_communication_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = idmic_mono_communication_settings,
	.setting_sz = ARRAY_SIZE(idmic_mono_communication_settings),
};
#endif

static struct snddev_icodec_data snddev_ispkr_communication_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "speaker_mono_communication_tx",
#ifdef CONFIG_SKY_SND_CTRL
	.copp_id = 1,
	.profile = &imic_profile,
#else
	.copp_id = PRIMARY_I2S_TX,
	.profile = &idmic_mono_communication_profile,
#endif
	.channel_mode = 1,
	.default_sample_rate = 48000,
#ifdef CONFIG_SKY_SND_CTRL	
	.pamp_on = msm_snddev_enable_amic_power,
	.pamp_off = msm_snddev_disable_amic_power,
#else
	.pamp_on = msm_snddev_enable_dmic_power,
	.pamp_off = msm_snddev_disable_dmic_power,
#endif	
};

static struct platform_device msm_ispkr_mic_communication_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &snddev_ispkr_communication_mic_data },
};
#endif

static struct adie_codec_action_unit iearpiece_ffa_48KHz_osr256_actions[] =
	EAR_PRI_MONO_8000_OSR_256;

static struct adie_codec_hwsetting_entry iearpiece_ffa_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = iearpiece_ffa_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(iearpiece_ffa_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile iearpiece_ffa_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = iearpiece_ffa_settings,
	.setting_sz = ARRAY_SIZE(iearpiece_ffa_settings),
};

static struct snddev_icodec_data snddev_iearpiece_ffa_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "handset_rx",
	.copp_id = 0,
	.profile = &iearpiece_ffa_profile,
	.channel_mode = 1,
	.default_sample_rate = 48000,
};

static struct platform_device msm_iearpiece_ffa_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &snddev_iearpiece_ffa_data },
};

static struct snddev_icodec_data snddev_imic_ffa_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "handset_tx",
	.copp_id = PRIMARY_I2S_TX,
	.profile = &idmic_mono_profile,
	.channel_mode = 1,
	.default_sample_rate = 48000,
	.pamp_on = msm_snddev_enable_dmic_power,
	.pamp_off = msm_snddev_disable_dmic_power,
};

static struct platform_device msm_imic_ffa_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &snddev_imic_ffa_data },
};

static struct snddev_icodec_data snddev_qt_dual_dmic_d0_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "speaker_mono_tx",
	.copp_id = PRIMARY_I2S_TX,
	.profile = &idmic_mono_profile,
	.channel_mode = 1,
	.default_sample_rate = 48000,
	.pamp_on = msm_snddev_enable_qt_dmic_power,
	.pamp_off = msm_snddev_disable_qt_dmic_power,
};

static struct platform_device msm_qt_dual_dmic_d0_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &snddev_qt_dual_dmic_d0_data },
};

static struct adie_codec_action_unit dual_mic_endfire_8KHz_osr256_actions[] =
	DMIC1_PRI_STEREO_OSR_256;

static struct adie_codec_hwsetting_entry dual_mic_endfire_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = dual_mic_endfire_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(dual_mic_endfire_8KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile dual_mic_endfire_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = dual_mic_endfire_settings,
	.setting_sz = ARRAY_SIZE(dual_mic_endfire_settings),
};

static struct snddev_icodec_data snddev_dual_mic_endfire_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "handset_dual_mic_endfire_tx",
	.copp_id = PRIMARY_I2S_TX,
	.profile = &dual_mic_endfire_profile,
	.channel_mode = 2,
	.default_sample_rate = 48000,
	.pamp_on = msm_snddev_enable_dmic_power,
	.pamp_off = msm_snddev_disable_dmic_power,
};

static struct platform_device msm_hs_dual_mic_endfire_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &snddev_dual_mic_endfire_data },
};

static struct snddev_icodec_data snddev_dual_mic_spkr_endfire_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "speaker_dual_mic_endfire_tx",
	.copp_id = PRIMARY_I2S_TX,
	.profile = &dual_mic_endfire_profile,
	.channel_mode = 2,
	.default_sample_rate = 48000,
	.pamp_on = msm_snddev_enable_dmic_power,
	.pamp_off = msm_snddev_disable_dmic_power,
};

static struct platform_device msm_spkr_dual_mic_endfire_device = {
	.name = "snddev_icodec",
	.id = 15,
	.dev = { .platform_data = &snddev_dual_mic_spkr_endfire_data },
};

static struct adie_codec_action_unit dual_mic_broadside_8osr256_actions[] =
	HS_DMIC2_STEREO_OSR_256;

static struct adie_codec_hwsetting_entry dual_mic_broadside_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = dual_mic_broadside_8osr256_actions,
		.action_sz = ARRAY_SIZE(dual_mic_broadside_8osr256_actions),
	}
};

static struct adie_codec_dev_profile dual_mic_broadside_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = dual_mic_broadside_settings,
	.setting_sz = ARRAY_SIZE(dual_mic_broadside_settings),
};

static struct snddev_icodec_data snddev_hs_dual_mic_broadside_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "handset_dual_mic_broadside_tx",
	.copp_id = PRIMARY_I2S_TX,
	.profile = &dual_mic_broadside_profile,
	.channel_mode = 2,
	.default_sample_rate = 48000,
	.pamp_on = msm_snddev_enable_dmic_sec_power,
	.pamp_off = msm_snddev_disable_dmic_sec_power,
};

static struct platform_device msm_hs_dual_mic_broadside_device = {
	.name = "snddev_icodec",
	.id = 21,
	.dev = { .platform_data = &snddev_hs_dual_mic_broadside_data },
};

static struct snddev_icodec_data snddev_spkr_dual_mic_broadside_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "speaker_dual_mic_broadside_tx",
	.copp_id = PRIMARY_I2S_TX,
	.profile = &dual_mic_broadside_profile,
	.channel_mode = 2,
	.default_sample_rate = 48000,
	.pamp_on = msm_snddev_enable_dmic_sec_power,
	.pamp_off = msm_snddev_disable_dmic_sec_power,
};

static struct platform_device msm_spkr_dual_mic_broadside_device = {
	.name = "snddev_icodec",
	.id = 18,
	.dev = { .platform_data = &snddev_spkr_dual_mic_broadside_data },
};

static struct snddev_hdmi_data snddev_hdmi_stereo_rx_data = {
	.capability = SNDDEV_CAP_RX ,
	.name = "hdmi_stereo_rx",
	.copp_id = HDMI_RX,
	.channel_mode = 0,
	.default_sample_rate = 48000,
};

static struct platform_device msm_snddev_hdmi_stereo_rx_device = {
	.name = "snddev_hdmi",
	.dev = { .platform_data = &snddev_hdmi_stereo_rx_data },
};

static struct snddev_mi2s_data snddev_mi2s_fm_tx_data = {
	.capability = SNDDEV_CAP_TX ,
	.name = "fmradio_stereo_tx",
	.copp_id = MI2S_TX,
	.channel_mode = 2, /* stereo */
	.sd_lines = MI2S_SD3, /* sd3 */
	.sample_rate = 48000,
};

static struct platform_device msm_mi2s_fm_tx_device = {
	.name = "snddev_mi2s",
	.dev = { .platform_data = &snddev_mi2s_fm_tx_data },
};

static struct snddev_mi2s_data snddev_mi2s_fm_rx_data = {
	.capability = SNDDEV_CAP_RX ,
	.name = "fmradio_stereo_rx",
	.copp_id = MI2S_RX,
	.channel_mode = 2, /* stereo */
	.sd_lines = MI2S_SD3, /* sd3 */
	.sample_rate = 48000,
};

static struct platform_device msm_mi2s_fm_rx_device = {
	.name = "snddev_mi2s",
	.id = 1,
	.dev = { .platform_data = &snddev_mi2s_fm_rx_data },
};

static struct adie_codec_action_unit iheadset_mic_tx_osr256_actions[] =
	HEADSET_AMIC2_TX_MONO_PRI_OSR_256;

static struct adie_codec_hwsetting_entry iheadset_mic_tx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = iheadset_mic_tx_osr256_actions,
		.action_sz = ARRAY_SIZE(iheadset_mic_tx_osr256_actions),
	}
};

static struct adie_codec_dev_profile iheadset_mic_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = iheadset_mic_tx_settings,
	.setting_sz = ARRAY_SIZE(iheadset_mic_tx_settings),
};

static struct snddev_icodec_data snddev_headset_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "headset_mono_tx",
	.copp_id = PRIMARY_I2S_TX,
	.profile = &iheadset_mic_profile,
	.channel_mode = 1,
	.default_sample_rate = 48000,
};

static struct platform_device msm_headset_mic_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &snddev_headset_mic_data },
};

static struct adie_codec_action_unit
	ihs_stereo_speaker_stereo_rx_48KHz_osr256_actions[] =
	SPEAKER_HPH_AB_CPL_PRI_STEREO_48000_OSR_256;

static struct adie_codec_hwsetting_entry
	ihs_stereo_speaker_stereo_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ihs_stereo_speaker_stereo_rx_48KHz_osr256_actions,
		.action_sz =
		ARRAY_SIZE(ihs_stereo_speaker_stereo_rx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile ihs_stereo_speaker_stereo_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = ihs_stereo_speaker_stereo_rx_settings,
	.setting_sz = ARRAY_SIZE(ihs_stereo_speaker_stereo_rx_settings),
};

static struct snddev_icodec_data snddev_ihs_stereo_speaker_stereo_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "headset_stereo_speaker_stereo_rx",
	.copp_id = 0,
	.profile = &ihs_stereo_speaker_stereo_rx_profile,
	.channel_mode = 2,
	.default_sample_rate = 48000,
#ifdef CONFIG_PANTECH_AUDIO_PRESTO_FAB2200  // jmlee 20110505 add	
	.pamp_on = &snd_subsystem_sp_hp_poweron,
	.pamp_off = &snd_subsystem_sp_hp_powerdown,	
	.voltage_on = &snd_subsystem_pmic_vreg_l2_on,
	.voltage_off = &snd_subsystem_pmic_vreg_l2_off,
#else
	.pamp_on = msm_snddev_poweramp_on,
	.pamp_off = msm_snddev_poweramp_off,
	.voltage_on = msm_snddev_voltage_on,
	.voltage_off = msm_snddev_voltage_off,
#endif	
};

static struct platform_device msm_ihs_stereo_speaker_stereo_rx_device = {
	.name = "snddev_icodec",
	.id = 22,
	.dev = { .platform_data = &snddev_ihs_stereo_speaker_stereo_rx_data },
};

/* define the value for BT_SCO */

static struct snddev_ecodec_data snddev_bt_sco_earpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_rx",
	.copp_id = PCM_RX,
	.channel_mode = 1,
};


static struct snddev_ecodec_data snddev_bt_sco_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_tx",
	.copp_id = PCM_TX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_earpiece_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_earpiece_data },
};


struct platform_device msm_bt_sco_mic_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_mic_data },
};

#ifdef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020 // jmlee
// SND_DEVICE_NR_CT
static struct snddev_ecodec_data snddev_nr_ct_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "nr_ct_rx",
	.copp_id = PCM_RX,
	.channel_mode = 1,
	.pamp_on = &audience_a2020_ct_on,
	.pamp_off = &audience_a2020_ct_off,	
};
static struct snddev_ecodec_data snddev_nr_ct_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "nr_ct_tx",
	.copp_id = PCM_TX,
	.channel_mode = 1,
	.pamp_on = &audience_a2020_ct_on,
	.pamp_off = &audience_a2020_ct_off,		
};
struct platform_device msm_nr_ct_rx_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_nr_ct_rx_data },
};
struct platform_device msm_nr_ct_tx_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_nr_ct_tx_data },
};

// SND_DEVICE_NR_FT
static struct snddev_ecodec_data snddev_nr_ft_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "nr_ft_rx",
	.copp_id = PCM_RX,
	.channel_mode = 1,
	.pamp_on = &audience_a2020_ft_on,
	.pamp_off = &audience_a2020_off,	
};
static struct snddev_ecodec_data snddev_nr_ft_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "nr_ft_tx",
	.copp_id = PCM_TX,
	.channel_mode = 1,
	.pamp_on = &audience_a2020_ft_on,
	.pamp_off = &audience_a2020_off,		
};
struct platform_device msm_nr_ft_rx_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_nr_ft_rx_data },
};
struct platform_device msm_nr_ft_tx_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_nr_ft_tx_data },
};

// SND_DEVICE_NR_DV
static struct snddev_ecodec_data snddev_nr_dv_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "nr_dv_rx",
	.copp_id = PCM_RX,
	.channel_mode = 1,
	.pamp_on = &audience_a2020_dv_on,
	.pamp_off = &audience_a2020_off,	
};
static struct snddev_ecodec_data snddev_nr_dv_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "nr_dv_tx",
	.copp_id = PCM_TX,
	.channel_mode = 1,
	.pamp_on = &audience_a2020_dv_on,
	.pamp_off = &audience_a2020_off,		
};
struct platform_device msm_nr_dv_rx_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_nr_dv_rx_data },
};
struct platform_device msm_nr_dv_tx_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_nr_dv_tx_data },
};

// SND_DEVICE_NR_CT_2MIC
static struct snddev_ecodec_data snddev_nr_ct_2mic_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "nr_ct_2mic_tx",
	.copp_id = PCM_TX,
	.channel_mode = 1,
	.pamp_on = &audience_a2020_ct_2mic_on,
	.pamp_off = &audience_a2020_ct_off,		
};
struct platform_device msm_nr_ct_2mic_tx_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_nr_ct_2mic_tx_data },
};

// DEVICE_SND_DEVICE_NR_OFF_CT_RX
#if 1  // jykim110731@PS3
static struct snddev_ecodec_data snddev_e_nr_off_ct_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "nr_off_ct_rx",
	.copp_id = PCM_RX,
	.channel_mode = 1,
	.pamp_on = &audience_a2020_ct_vpoff_on,
	.pamp_off = &audience_a2020_ct_off,
};

static struct platform_device msm_nr_off_ct_rx_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_e_nr_off_ct_rx_data },
};
#else
static struct adie_codec_action_unit i_nr_off_ct_rx_48KHz_osr256_actions[] =
	NR_OFF_CT_RX_MONO_8000_OSR_256;

static struct adie_codec_hwsetting_entry i_nr_off_ct_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = i_nr_off_ct_rx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(i_nr_off_ct_rx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile i_nr_off_ct_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = i_nr_off_ct_rx_settings,
	.setting_sz = ARRAY_SIZE(i_nr_off_ct_rx_settings),
};

static struct snddev_icodec_data snddev_i_nr_off_ct_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "nr_off_ct_rx",
	.copp_id = 0,
	.profile = &i_nr_off_ct_rx_profile,
	.channel_mode = 1,
	.default_sample_rate = 48000,
#ifdef CONFIG_MACH_MSM8X60_PRESTO  // jmlee 	
	.pamp_on = msm_snddev_enable_iearpiece_on,
	.pamp_off = msm_snddev_disable_iearpeace_off,
#endif	
//	.pamp_on = &msm_snddev_nr_out_sel_qtrReceiver_or_A2020Speaker,
//	.pamp_off = &msm_snddev_nr_out_sel_A2020Reciever,
};

static struct platform_device msm_nr_off_ct_rx_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &snddev_i_nr_off_ct_rx_data },
};
#endif

// SND_DEVICE_NR_OFF_CT
#if 1  // jykim110731@PS3
static struct snddev_ecodec_data snddev_e_nr_off_ct_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "nr_off_ct_tx",
	.copp_id = PCM_TX,
	.channel_mode = 1,
	.pamp_on = &audience_a2020_ct_vpoff_on,
	.pamp_off = &audience_a2020_ct_off,	
};

static struct platform_device msm_nr_off_ct_tx_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_e_nr_off_ct_tx_data },
};
#else
static struct adie_codec_action_unit i_nr_off_ct_tx_48KHz_osr256_actions[] =
	i_nr_off_ct_tx_MONO_OSR_256;

static struct adie_codec_hwsetting_entry i_nr_off_ct_tx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = i_nr_off_ct_tx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(i_nr_off_ct_tx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile i_nr_off_ct_tx_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = i_nr_off_ct_tx_settings,
	.setting_sz = ARRAY_SIZE(i_nr_off_ct_tx_settings),
};

static struct snddev_icodec_data snddev_i_nr_off_ct_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "nr_off_ct_tx",
	.copp_id = 1,
	.profile = &i_nr_off_ct_tx_profile,
	.channel_mode = 1,
	.default_sample_rate = 48000,
	.pamp_on = msm_snddev_enable_amic_power,
	.pamp_off = msm_snddev_disable_amic_power,	
//	.pamp_on = &msm_snddev_mic_sel_QTR,
//	.pamp_off = NULL,
};

static struct platform_device msm_nr_off_ct_tx_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &snddev_i_nr_off_ct_tx_data },
};
#endif

// SND_DEVICE_NR_OFF_CT_1MIC // 20111004 jmlee 1mic only receiver path ===>
static struct snddev_ecodec_data snddev_e_nr_off_ct_1mic_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "nr_off_ct_1mic_rx",
	.copp_id = PCM_RX,
	.channel_mode = 1,
	.pamp_on = &audience_a2020_ct_vpoff_1mic_on,
	.pamp_off = &audience_a2020_ct_off,
};

static struct platform_device msm_nr_off_ct_1mic_rx_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_e_nr_off_ct_1mic_rx_data },
};

static struct snddev_ecodec_data snddev_e_nr_off_ct_1mic_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "nr_off_ct_1mic_tx",
	.copp_id = PCM_TX,
	.channel_mode = 1,
	.pamp_on = &audience_a2020_ct_vpoff_1mic_on,
	.pamp_off = &audience_a2020_ct_off,	
};

static struct platform_device msm_nr_off_ct_1mic_tx_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_e_nr_off_ct_1mic_tx_data },
};

// SND_DEVICE_NR_OFF_CT_1MIC // 20111004 jmlee 1mic only receiver path <===


//SND_DEVICE_NR_OFF_FT
#if 1  // jykim110731@PS3
static struct snddev_ecodec_data snddev_e_nr_off_ft_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "nr_off_ft_rx",
	.copp_id = PCM_RX,
	.channel_mode = 1,
	.pamp_on = &audience_a2020_ft_vpoff_on,
	.pamp_off = &audience_a2020_off,
};

static struct platform_device msm_nr_off_ft_rx_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_e_nr_off_ft_rx_data },
};
#else
static struct adie_codec_action_unit i_nr_off_ft_rx_ispkr_stereo_48KHz_osr256_actions[] =
	i_nr_off_ft_rx_SPEAKER_PRI_STEREO_48000_OSR_256;

static struct adie_codec_hwsetting_entry i_nr_off_ft_rx_ispkr_stereo_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = i_nr_off_ft_rx_ispkr_stereo_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(i_nr_off_ft_rx_ispkr_stereo_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile i_nr_off_ft_rx_ispkr_stereo_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = i_nr_off_ft_rx_ispkr_stereo_settings,
	.setting_sz = ARRAY_SIZE(i_nr_off_ft_rx_ispkr_stereo_settings),
};

static struct snddev_icodec_data i_nr_off_ft_rx_snddev_ispkr_stereo_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "nr_off_ft_rx",
	.copp_id = 0,
	.profile = &i_nr_off_ft_rx_ispkr_stereo_profile,
#ifdef CONFIG_PANTECH_AUDIO_PRESTO_FAB2200 //CONFIG_SKY_SND_EXTAMP /* 20110521-jhpark:If you use 2, it is stereo so please use 1 then L+R will be mixed. */
	.channel_mode = 1,
#else
	.channel_mode = 2,
#endif
	.default_sample_rate = 48000,
#ifdef CONFIG_PANTECH_AUDIO_PRESTO_FAB2200    //20110428 jhsong : feature clearup
	.pamp_on = &snd_subsystem_nr_off_ft_poweron,
	.pamp_off = &snd_subsystem_nr_off_ft_powerdown,
#else
	.pamp_on = msm_snddev_poweramp_on,
	.pamp_off = msm_snddev_poweramp_off,
#endif	
};

static struct platform_device msm_nr_off_ft_rx_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &i_nr_off_ft_rx_snddev_ispkr_stereo_data },
};
#endif

#if 1  // jykim110731@PS3
static struct snddev_ecodec_data snddev_e_nr_off_ft_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "nr_off_ft_tx",
	.copp_id = PCM_TX,
	.channel_mode = 1,
	.pamp_on = &audience_a2020_ft_vpoff_on,
	.pamp_off = &audience_a2020_off,
};

static struct platform_device msm_nr_off_ft_tx_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_e_nr_off_ft_tx_data },
};
#else
static struct adie_codec_action_unit i_nr_off_ft_tx_imic_48KHz_osr256_actions[] =
	i_nr_off_ft_tx_AMIC_PRI_MONO_OSR_256;

static struct adie_codec_hwsetting_entry i_nr_off_ft_tx_imic_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = i_nr_off_ft_tx_imic_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(i_nr_off_ft_tx_imic_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile i_nr_off_ft_tx_imic_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = i_nr_off_ft_tx_imic_settings,
	.setting_sz = ARRAY_SIZE(i_nr_off_ft_tx_imic_settings),
};

static struct snddev_icodec_data i_nr_off_ft_tx_snddev_imic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "nr_off_ft_tx",
	.copp_id = 1,
	.profile = &i_nr_off_ft_tx_imic_profile,
	.channel_mode = 1,
	.default_sample_rate = 48000,
	.pamp_on = msm_snddev_enable_amic_power,
	.pamp_off = msm_snddev_disable_amic_power,
};

static struct platform_device msm_nr_off_ft_tx_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &i_nr_off_ft_tx_snddev_imic_data },
};
#endif

//SND_DEVICE_NR_OFF_DV
static struct adie_codec_action_unit i_nr_off_dv_rx_ispkr_stereo_48KHz_osr256_actions[] =
	i_nr_off_dv_rx_SPEAKER_PRI_STEREO_48000_OSR_256;

static struct adie_codec_hwsetting_entry i_nr_off_dv_rx_ispkr_stereo_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = i_nr_off_dv_rx_ispkr_stereo_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(i_nr_off_dv_rx_ispkr_stereo_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile i_nr_off_dv_rx_ispkr_stereo_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = i_nr_off_dv_rx_ispkr_stereo_settings,
	.setting_sz = ARRAY_SIZE(i_nr_off_dv_rx_ispkr_stereo_settings),
};

static struct snddev_icodec_data i_nr_off_dv_rx_snddev_ispkr_stereo_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "nr_off_dv_rx",
	.copp_id = 0,
	.profile = &i_nr_off_dv_rx_ispkr_stereo_profile,
#ifdef CONFIG_PANTECH_AUDIO_PRESTO_FAB2200 //CONFIG_SKY_SND_EXTAMP /* 20110521-jhpark:If you use 2, it is stereo so please use 1 then L+R will be mixed. */
	.channel_mode = 1,
#else
	.channel_mode = 2,
#endif
	.default_sample_rate = 48000,
#ifdef CONFIG_PANTECH_AUDIO_PRESTO_FAB2200    //20110428 jhsong : feature clearup
	.pamp_on = &snd_subsystem_nr_off_dv_poweron,
	.pamp_off = &snd_subsystem_nr_off_dv_powerdown,
#else
	.pamp_on = msm_snddev_poweramp_on,
	.pamp_off = msm_snddev_poweramp_off,
#endif	
};

static struct platform_device msm_nr_off_dv_rx_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &i_nr_off_dv_rx_snddev_ispkr_stereo_data },
};

static struct adie_codec_action_unit i_nr_off_dv_tx_imic_48KHz_osr256_actions[] =
	i_nr_off_dv_tx_AMIC_PRI_MONO_OSR_256;

static struct adie_codec_hwsetting_entry i_nr_off_dv_tx_imic_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = i_nr_off_dv_tx_imic_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(i_nr_off_dv_tx_imic_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile i_nr_off_dv_tx_imic_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = i_nr_off_dv_tx_imic_settings,
	.setting_sz = ARRAY_SIZE(i_nr_off_dv_tx_imic_settings),
};

static struct snddev_icodec_data i_nr_off_dv_tx_snddev_imic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "nr_off_dv_tx",
	.copp_id = 1,
	.profile = &i_nr_off_dv_tx_imic_profile,
	.channel_mode = 1,
	.default_sample_rate = 48000,
	.pamp_on = msm_snddev_enable_amic_power,
	.pamp_off = msm_snddev_disable_amic_power,
};

static struct platform_device msm_nr_off_dv_tx_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &i_nr_off_dv_tx_snddev_imic_data },
};

#endif  // end of "#ifdef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020 // jmlee"

#ifdef CONFIG_PANTECH_ASR_PATH
static struct adie_codec_action_unit asr_handset_mic_48KHz_osr256_actions[] =
	ASR_HANDSET_TX_48000_OSR_256;

static struct adie_codec_hwsetting_entry asr_handset_mic_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = asr_handset_mic_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(asr_handset_mic_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile asr_handset_mic_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = asr_handset_mic_settings,
	.setting_sz = ARRAY_SIZE(asr_handset_mic_settings),
};

static struct snddev_icodec_data snddev_asr_handset_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "asr_handset_tx",
	.copp_id = 1,
	.profile = &asr_handset_mic_profile,
	.channel_mode = 1,
	.default_sample_rate = 48000,
	.pamp_on = msm_snddev_enable_amic_power,
	.pamp_off = msm_snddev_disable_amic_power,
};

static struct platform_device msm_asr_handset_tx_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &snddev_asr_handset_mic_data },
};

///////////////////////////////////////

static struct adie_codec_action_unit ihs_asr_mono_tx_48KHz_osr256_actions[] =
	ASR_HEADSET_MONO_TX_48000_OSR_256;

static struct adie_codec_hwsetting_entry ihs_asr_mono_tx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ihs_asr_mono_tx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ihs_asr_mono_tx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile ihs_asr_mono_tx_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = ihs_asr_mono_tx_settings,
	.setting_sz = ARRAY_SIZE(ihs_asr_mono_tx_settings),
};

static struct snddev_icodec_data snddev_asr_ihs_mono_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "asr_headset_tx",
	.copp_id = PRIMARY_I2S_TX,
	.profile = &ihs_asr_mono_tx_profile,
	.channel_mode = 1,
	.default_sample_rate = 48000,
};

static struct platform_device msm_asr_headset_tx_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &snddev_asr_ihs_mono_tx_data },
};
#endif  // CONFIG_PANTECH_ASR_PATH

#ifdef CONFIG_PANTECH_BT_GROUPING
// pantech-bt_a_rx
static struct snddev_ecodec_data snddev_bt_sco_a_earpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_a_rx",
	.copp_id = PCM_RX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_a_earpiece_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_a_earpiece_data },
};

// pantech-bt_a_tx
static struct snddev_ecodec_data snddev_bt_sco_a_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_a_tx",
	.copp_id = PCM_TX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_a_mic_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_a_mic_data },
};

// pantech-bt_b_rx
static struct snddev_ecodec_data snddev_bt_sco_b_earpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_b_rx",
	.copp_id = PCM_RX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_b_earpiece_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_b_earpiece_data },
};

// pantech-bt_b_tx
static struct snddev_ecodec_data snddev_bt_sco_b_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_b_tx",
	.copp_id = PCM_TX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_b_mic_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_b_mic_data },
};

// pantech-bt_c_rx
static struct snddev_ecodec_data snddev_bt_sco_c_earpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_c_rx",
	.copp_id = PCM_RX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_c_earpiece_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_c_earpiece_data },
};

// pantech-bt_c_tx
static struct snddev_ecodec_data snddev_bt_sco_c_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_c_tx",
	.copp_id = PCM_TX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_c_mic_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_c_mic_data },
};

// pantech-bt_d_rx
static struct snddev_ecodec_data snddev_bt_sco_d_earpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_d_rx",
	.copp_id = PCM_RX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_d_earpiece_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_d_earpiece_data },
};

// pantech-bt_d_tx
static struct snddev_ecodec_data snddev_bt_sco_d_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_d_tx",
	.copp_id = PCM_TX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_d_mic_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_d_mic_data },
};

// pantech-bt_e_rx
static struct snddev_ecodec_data snddev_bt_sco_e_earpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_e_rx",
	.copp_id = PCM_RX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_e_earpiece_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_e_earpiece_data },
};

// pantech-bt_e_tx
static struct snddev_ecodec_data snddev_bt_sco_e_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_e_tx",
	.copp_id = PCM_TX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_e_mic_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_e_mic_data },
};

// pantech-bt_f_rx
static struct snddev_ecodec_data snddev_bt_sco_f_earpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_f_rx",
	.copp_id = PCM_RX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_f_earpiece_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_f_earpiece_data },
};

// pantech-bt_f_tx
static struct snddev_ecodec_data snddev_bt_sco_f_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_f_tx",
	.copp_id = PCM_TX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_f_mic_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_f_mic_data },
};

// pantech-bt_g_rx
static struct snddev_ecodec_data snddev_bt_sco_g_earpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_g_rx",
	.copp_id = PCM_RX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_g_earpiece_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_g_earpiece_data },
};

// pantech-bt_g_tx
static struct snddev_ecodec_data snddev_bt_sco_g_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_g_tx",
	.copp_id = PCM_TX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_g_mic_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_g_mic_data },
};


// pantech-bt_h_rx
static struct snddev_ecodec_data snddev_bt_sco_h_earpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_h_rx",
	.copp_id = PCM_RX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_h_earpiece_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_h_earpiece_data },
};

// pantech-bt_h_tx
static struct snddev_ecodec_data snddev_bt_sco_h_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_h_tx",
	.copp_id = PCM_TX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_h_mic_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_h_mic_data },
};

// pantech-bt_i_rx
static struct snddev_ecodec_data snddev_bt_sco_i_earpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_i_rx",
	.copp_id = PCM_RX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_i_earpiece_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_i_earpiece_data },
};

// pantech-bt_i_tx
static struct snddev_ecodec_data snddev_bt_sco_i_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_i_tx",
	.copp_id = PCM_TX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_i_mic_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_i_mic_data },
};

// pantech-bt_j_rx
static struct snddev_ecodec_data snddev_bt_sco_j_earpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_j_rx",
	.copp_id = PCM_RX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_j_earpiece_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_j_earpiece_data },
};

// pantech-bt_j_tx
static struct snddev_ecodec_data snddev_bt_sco_j_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_j_tx",
	.copp_id = PCM_TX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_j_mic_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_j_mic_data },
};

// pantech-bt_k_rx
static struct snddev_ecodec_data snddev_bt_sco_k_earpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_k_rx",
	.copp_id = PCM_RX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_k_earpiece_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_k_earpiece_data },
};

// pantech-bt_k_tx
static struct snddev_ecodec_data snddev_bt_sco_k_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_k_tx",
	.copp_id = PCM_TX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_k_mic_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_k_mic_data },
};

// pantech-bt_l_rx
static struct snddev_ecodec_data snddev_bt_sco_l_earpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_l_rx",
	.copp_id = PCM_RX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_l_earpiece_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_l_earpiece_data },
};

// pantech-bt_l_tx
static struct snddev_ecodec_data snddev_bt_sco_l_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_class_l_tx",
	.copp_id = PCM_TX,
	.channel_mode = 1,
};

struct platform_device msm_bt_sco_l_mic_device = {
	.name = "msm_snddev_ecodec",
	.dev = { .platform_data = &snddev_bt_sco_l_mic_data },
};
#endif  // CONFIG_PANTECH_BT_GROUPING

static struct adie_codec_action_unit itty_mono_tx_actions[] =
	TTY_HEADSET_MONO_TX_OSR_256;

static struct adie_codec_hwsetting_entry itty_mono_tx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = itty_mono_tx_actions,
		.action_sz = ARRAY_SIZE(itty_mono_tx_actions),
	},
};

static struct adie_codec_dev_profile itty_mono_tx_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = itty_mono_tx_settings,
	.setting_sz = ARRAY_SIZE(itty_mono_tx_settings),
};

static struct snddev_icodec_data snddev_itty_mono_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE | SNDDEV_CAP_TTY),
	.name = "tty_headset_mono_tx",
	.copp_id = PRIMARY_I2S_TX,
	.profile = &itty_mono_tx_profile,
	.channel_mode = 1,
	.default_sample_rate = 48000,
};

static struct platform_device msm_itty_mono_tx_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &snddev_itty_mono_tx_data },
};

static struct adie_codec_action_unit itty_mono_rx_actions[] =
	TTY_HEADSET_MONO_RX_8000_OSR_256;

static struct adie_codec_hwsetting_entry itty_mono_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = itty_mono_rx_actions,
		.action_sz = ARRAY_SIZE(itty_mono_rx_actions),
	},
};

static struct adie_codec_dev_profile itty_mono_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = itty_mono_rx_settings,
	.setting_sz = ARRAY_SIZE(itty_mono_rx_settings),
};

static struct snddev_icodec_data snddev_itty_mono_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE | SNDDEV_CAP_TTY),
	.name = "tty_headset_mono_rx",
	.copp_id = PRIMARY_I2S_RX,
	.profile = &itty_mono_rx_profile,
	.channel_mode = 1,
	.default_sample_rate = 48000,
#ifdef CONFIG_PANTECH_AUDIO_PRESTO_FAB2200    //20110708 jmlee add   <== 20110428 jhsong
	.pamp_on = &snd_subsystem_tty_hp_poweron,
	.pamp_off = &snd_subsystem_tty_hp_powerdown,
#else	
	.voltage_on = msm_snddev_voltage_on,
	.voltage_off = msm_snddev_voltage_off,
#endif	
};

static struct platform_device msm_itty_mono_rx_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &snddev_itty_mono_rx_data },
};

static struct adie_codec_action_unit linein_pri_actions[] =
	LINEIN_PRI_STEREO_OSR_256;

static struct adie_codec_hwsetting_entry linein_pri_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = linein_pri_actions,
		.action_sz = ARRAY_SIZE(linein_pri_actions),
	},
};

static struct adie_codec_dev_profile linein_pri_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = linein_pri_settings,
	.setting_sz = ARRAY_SIZE(linein_pri_settings),
};

static struct snddev_icodec_data snddev_linein_pri_data = {
	.capability = SNDDEV_CAP_TX,
	.name = "linein_pri_tx",
	.copp_id = PRIMARY_I2S_TX,
	.profile = &linein_pri_profile,
	.channel_mode = 2,
	.default_sample_rate = 48000,
	.voltage_on = msm_snddev_voltage_on,
	.voltage_off = msm_snddev_voltage_off,
};

static struct platform_device msm_linein_pri_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &snddev_linein_pri_data },
};

static struct adie_codec_action_unit auxpga_lb_lo_actions[] =
	LB_AUXPGA_LO_STEREO;

static struct adie_codec_hwsetting_entry auxpga_lb_lo_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = auxpga_lb_lo_actions,
		.action_sz = ARRAY_SIZE(auxpga_lb_lo_actions),
	},
};

static struct adie_codec_dev_profile auxpga_lb_lo_profile = {
	.path_type = ADIE_CODEC_LB,
	.settings = auxpga_lb_lo_settings,
	.setting_sz = ARRAY_SIZE(auxpga_lb_lo_settings),
};

static struct snddev_icodec_data snddev_auxpga_lb_lo_data = {
	.capability = SNDDEV_CAP_LB,
	.name = "speaker_stereo_lb",
	.copp_id = PRIMARY_I2S_RX,
	.profile = &auxpga_lb_lo_profile,
	.channel_mode = 2,
	.default_sample_rate = 48000,
	.pamp_on = msm_snddev_poweramp_on,
	.pamp_off = msm_snddev_poweramp_off,
	.dev_vol_type = SNDDEV_DEV_VOL_ANALOG,
};

static struct platform_device msm_auxpga_lb_lo_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &snddev_auxpga_lb_lo_data },
};

static struct adie_codec_action_unit auxpga_lb_hs_actions[] =
	LB_AUXPGA_HPH_AB_CPLS_STEREO;

static struct adie_codec_hwsetting_entry auxpga_lb_hs_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = auxpga_lb_hs_actions,
		.action_sz = ARRAY_SIZE(auxpga_lb_hs_actions),
	},
};

static struct adie_codec_dev_profile auxpga_lb_hs_profile = {
	.path_type = ADIE_CODEC_LB,
	.settings = auxpga_lb_hs_settings,
	.setting_sz = ARRAY_SIZE(auxpga_lb_hs_settings),
};

static struct snddev_icodec_data snddev_auxpga_lb_hs_data = {
	.capability = SNDDEV_CAP_LB,
	.name = "hs_stereo_lb",
	.copp_id = PRIMARY_I2S_RX,
	.profile = &auxpga_lb_hs_profile,
	.channel_mode = 2,
	.default_sample_rate = 48000,
	.voltage_on = msm_snddev_voltage_on,
	.voltage_off = msm_snddev_voltage_off,
	.dev_vol_type = SNDDEV_DEV_VOL_ANALOG,
};

static struct platform_device msm_auxpga_lb_hs_device = {
	.name = "snddev_icodec",
	.dev = { .platform_data = &snddev_auxpga_lb_hs_data },
};

static struct snddev_virtual_data snddev_uplink_rx_data = {
	.capability = SNDDEV_CAP_RX,
	.name = "uplink_rx",
	.copp_id = VOICE_PLAYBACK_TX,
};

static struct platform_device msm_uplink_rx_device = {
	.name = "snddev_virtual",
	.dev = { .platform_data = &snddev_uplink_rx_data },
};

#ifdef CONFIG_DEBUG_FS
static struct adie_codec_action_unit
	ihs_stereo_rx_class_d_legacy_48KHz_osr256_actions[] =
	HPH_PRI_D_LEG_STEREO;

static struct adie_codec_hwsetting_entry
	ihs_stereo_rx_class_d_legacy_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions =
		ihs_stereo_rx_class_d_legacy_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE
		(ihs_stereo_rx_class_d_legacy_48KHz_osr256_actions),
	}
};

static struct adie_codec_action_unit
	ihs_stereo_rx_class_ab_legacy_48KHz_osr256_actions[] =
	HPH_PRI_AB_LEG_STEREO;

static struct adie_codec_hwsetting_entry
	ihs_stereo_rx_class_ab_legacy_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions =
		ihs_stereo_rx_class_ab_legacy_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE
		(ihs_stereo_rx_class_ab_legacy_48KHz_osr256_actions),
	}
};

static void snddev_hsed_config_modify_setting(int type)
{
	struct platform_device *device;
	struct snddev_icodec_data *icodec_data;

	device = &msm_headset_stereo_device;
	icodec_data = (struct snddev_icodec_data *)device->dev.platform_data;

	if (icodec_data) {
		if (type == 1) {
			icodec_data->voltage_on = NULL;
			icodec_data->voltage_off = NULL;
			icodec_data->profile->settings =
				ihs_stereo_rx_class_d_legacy_settings;
			icodec_data->profile->setting_sz =
			ARRAY_SIZE(ihs_stereo_rx_class_d_legacy_settings);
		} else if (type == 2) {
			icodec_data->voltage_on = NULL;
			icodec_data->voltage_off = NULL;
			icodec_data->profile->settings =
				ihs_stereo_rx_class_ab_legacy_settings;
			icodec_data->profile->setting_sz =
			ARRAY_SIZE(ihs_stereo_rx_class_ab_legacy_settings);
		}
	}
}

static void snddev_hsed_config_restore_setting(void)
{
	struct platform_device *device;
	struct snddev_icodec_data *icodec_data;

	device = &msm_headset_stereo_device;
	icodec_data = (struct snddev_icodec_data *)device->dev.platform_data;

	if (icodec_data) {
#ifdef CONFIG_PANTECH_AUDIO_PRESTO_FAB2200
		icodec_data->voltage_on = &snd_subsystem_pmic_vreg_l2_on;
		icodec_data->voltage_off = &snd_subsystem_pmic_vreg_l2_off;
#else
		icodec_data->voltage_on = msm_snddev_voltage_on;
		icodec_data->voltage_off = msm_snddev_voltage_off;
#endif		
		icodec_data->profile->settings = headset_ab_cpls_settings;
		icodec_data->profile->setting_sz =
			ARRAY_SIZE(headset_ab_cpls_settings);
	}
}

static ssize_t snddev_hsed_config_debug_write(struct file *filp,
	const char __user *ubuf, size_t cnt, loff_t *ppos)
{
	char *lb_str = filp->private_data;
	char cmd;

	if (get_user(cmd, ubuf))
		return -EFAULT;

	if (!strcmp(lb_str, "msm_hsed_config")) {
		switch (cmd) {
		case '0':
			snddev_hsed_config_restore_setting();
			break;

		case '1':
			snddev_hsed_config_modify_setting(1);
			break;

		case '2':
			snddev_hsed_config_modify_setting(2);
			break;

		default:
			break;
		}
	}
	return cnt;
}

static int snddev_hsed_config_debug_open(struct inode *inode, struct file *file)
{
	file->private_data = inode->i_private;
	return 0;
}

static const struct file_operations snddev_hsed_config_debug_fops = {
	.open = snddev_hsed_config_debug_open,
	.write = snddev_hsed_config_debug_write
};
#endif

static struct platform_device *snd_devices_ffa[] __initdata = {
	&msm_iearpiece_ffa_device,
	&msm_imic_ffa_device,
	&msm_ispkr_stereo_device,
	&msm_snddev_hdmi_stereo_rx_device,
	&msm_headset_mic_device,
	&msm_ispkr_mic_device,
	&msm_bt_sco_earpiece_device,
	&msm_bt_sco_mic_device,
	&msm_headset_stereo_device,
	&msm_itty_mono_tx_device,
	&msm_itty_mono_rx_device,
	&msm_mi2s_fm_tx_device,
	&msm_mi2s_fm_rx_device,
	&msm_hs_dual_mic_endfire_device,
	&msm_spkr_dual_mic_endfire_device,
	&msm_hs_dual_mic_broadside_device,
	&msm_spkr_dual_mic_broadside_device,
	&msm_ihs_stereo_speaker_stereo_rx_device,
	&msm_anc_headset_device,
	&msm_auxpga_lb_hs_device,
	&msm_auxpga_lb_lo_device,
	&msm_linein_pri_device,
	&msm_icodec_gpio_device,
};

static struct platform_device *snd_devices_surf[] __initdata = {
	&msm_iearpiece_device,
	&msm_imic_device,
	&msm_ispkr_stereo_device,
	&msm_snddev_hdmi_stereo_rx_device,
	&msm_headset_mic_device,
	&msm_ispkr_mic_device,
	&msm_bt_sco_earpiece_device,
	&msm_bt_sco_mic_device,
	&msm_headset_stereo_device,
	&msm_itty_mono_tx_device,
	&msm_itty_mono_rx_device,
	&msm_mi2s_fm_tx_device,
	&msm_mi2s_fm_rx_device,
	&msm_ihs_stereo_speaker_stereo_rx_device,
	&msm_auxpga_lb_hs_device,
	&msm_auxpga_lb_lo_device,
	&msm_linein_pri_device,
	&msm_icodec_gpio_device,
#ifdef CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020 // jmlees
	&msm_nr_ct_rx_device,
	&msm_nr_ct_tx_device,
	&msm_nr_ft_rx_device,
	&msm_nr_ft_tx_device,	
	&msm_nr_dv_rx_device,
	&msm_nr_dv_tx_device,		
	&msm_nr_off_ct_rx_device,
	&msm_nr_off_ct_tx_device,
	&msm_nr_off_ft_rx_device,
	&msm_nr_off_ft_tx_device,
	&msm_nr_off_dv_rx_device,
	&msm_nr_off_dv_tx_device,
	
	&msm_nr_ct_2mic_tx_device,	
#endif  // CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020
#ifdef CONFIG_PANTECH_ASR_PATH
	&msm_asr_handset_tx_device,
	&msm_asr_headset_tx_device,
#endif
#ifdef CONFIG_PANTECH_BT_GROUPING
	&msm_bt_sco_a_earpiece_device,
	&msm_bt_sco_a_mic_device,
	&msm_bt_sco_b_earpiece_device,
	&msm_bt_sco_b_mic_device,
	&msm_bt_sco_c_earpiece_device,
	&msm_bt_sco_c_mic_device,
	&msm_bt_sco_d_earpiece_device,
	&msm_bt_sco_d_mic_device,
	&msm_bt_sco_e_earpiece_device,
	&msm_bt_sco_e_mic_device,
	&msm_bt_sco_f_earpiece_device,
	&msm_bt_sco_f_mic_device,
	&msm_bt_sco_g_earpiece_device,
	&msm_bt_sco_g_mic_device,
	&msm_bt_sco_h_earpiece_device,
	&msm_bt_sco_h_mic_device,

	&msm_bt_sco_i_earpiece_device,
	&msm_bt_sco_i_mic_device,
	&msm_bt_sco_j_earpiece_device,
	&msm_bt_sco_j_mic_device,
	&msm_bt_sco_k_earpiece_device,
	&msm_bt_sco_k_mic_device,
	&msm_bt_sco_l_earpiece_device,
	&msm_bt_sco_l_mic_device,
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO //def FEATURE_PANTECH_PRESTO_SND // 20110919 jmlee communication test block
	&msm_ispkr_stereo_communication_device,
	&msm_ispkr_mic_communication_device,
	&msm_nr_off_ct_1mic_rx_device,  // 20111004 jmlee 1mic only receiver path
	&msm_nr_off_ct_1mic_tx_device,  // 20111004 jmlee 1mic only receiver path
#endif

};

static struct platform_device *snd_devices_fluid[] __initdata = {
	&msm_iearpiece_device,
	&msm_imic_device,
	&msm_ispkr_stereo_device,
	&msm_snddev_hdmi_stereo_rx_device,
	&msm_headset_stereo_device,
	&msm_headset_mic_device,
	&msm_fluid_ispkr_mic_device,
	&msm_bt_sco_earpiece_device,
	&msm_bt_sco_mic_device,
	&msm_mi2s_fm_tx_device,
	&msm_mi2s_fm_rx_device,
	&msm_anc_headset_device,
	&msm_auxpga_lb_hs_device,
	&msm_auxpga_lb_lo_device,
	&msm_icodec_gpio_device,
};

static struct platform_device *snd_devices_qt[] __initdata = {
	&msm_headset_stereo_device,
	&msm_headset_mic_device,
	&msm_ispkr_stereo_device,
	&msm_qt_dual_dmic_d0_device,
	&msm_snddev_hdmi_stereo_rx_device,
	&msm_qt_icodec_gpio_device,
};

static struct platform_device *snd_devices_common[] __initdata = {
	&msm_aux_pcm_device,
	&msm_cdcclk_ctl_device,
	&msm_mi2s_device,
	&msm_uplink_rx_device,
};

void __init msm_snddev_init(void)
{
	int i;
	int dev_id;

	atomic_set(&pamp_ref_cnt, 0);

	for (i = 0, dev_id = 0; i < ARRAY_SIZE(snd_devices_common); i++)
		snd_devices_common[i]->id = dev_id++;

	platform_add_devices(snd_devices_common,
		ARRAY_SIZE(snd_devices_common));

	/* Auto detect device base on machine info */
	if (machine_is_msm8x60_surf() || machine_is_msm8x60_charm_surf()
#ifdef CONFIG_MACH_MSM8X60_EF39S
			|| machine_is_msm8x60_ef39s()
#endif
#ifdef CONFIG_MACH_MSM8X60_EF40K
			|| machine_is_msm8x60_ef40k()
#endif
#ifdef CONFIG_MACH_MSM8X60_PRESTO
			|| machine_is_msm8x60_presto()
#endif
        ) {
		for (i = 0; i < ARRAY_SIZE(snd_devices_surf); i++)
			snd_devices_surf[i]->id = dev_id++;

		platform_add_devices(snd_devices_surf,
		ARRAY_SIZE(snd_devices_surf));
	} else if (machine_is_msm8x60_ffa() ||
			machine_is_msm8x60_charm_ffa()) {
		for (i = 0; i < ARRAY_SIZE(snd_devices_ffa); i++)
			snd_devices_ffa[i]->id = dev_id++;

		platform_add_devices(snd_devices_ffa,
		ARRAY_SIZE(snd_devices_ffa));
	} else if (machine_is_msm8x60_fluid()) {
		for (i = 0; i < ARRAY_SIZE(snd_devices_fluid); i++)
			snd_devices_fluid[i]->id = dev_id++;

		platform_add_devices(snd_devices_fluid,
		ARRAY_SIZE(snd_devices_fluid));
	} else if (machine_is_msm8x60_qt()) {
		for (i = 0; i < ARRAY_SIZE(snd_devices_qt); i++)
			snd_devices_qt[i]->id = dev_id++;

		platform_add_devices(snd_devices_qt,
		ARRAY_SIZE(snd_devices_qt));
	}

#ifdef CONFIG_DEBUG_FS
	debugfs_hsed_config = debugfs_create_file("msm_hsed_config",
				S_IFREG | S_IRUGO, NULL,
		(void *) "msm_hsed_config", &snddev_hsed_config_debug_fops);
#endif
}
