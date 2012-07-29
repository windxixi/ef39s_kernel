/* Copyright (c) 2011, PANTECH. All rights reserved.
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

#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <media/msm_camera.h>
#include <mach/camera.h>
#include "sensor_i2c.h"
#include "sensor_ctrl.h"
#include "s5k4ecgx_r2.h"
#ifdef CONFIG_PANTECH_CAMERA_TUNER
#include "ptune_parser.h"
#endif

/* PANTECH_CAMERA_TODO */
#undef CDBG
#undef CERR
#define CDBG(fmt, args...) printk(KERN_DEBUG fmt, ##args)
#define CERR(fmt, args...) printk(KERN_ERR fmt, ##args)

/* I2C slave address */
#define SI2C_SA	(s5k4ecgx_client->addr)


typedef struct {
	struct work_struct work;
} s5k4ecgx_work_t;

typedef struct {
	const struct msm_camera_sensor_info *sinfo;
} s5k4ecgx_ctrl_t;


#if defined(CONFIG_MACH_MSM8X60_PRESTO) && (BOARD_REV >= WS20)
/* S5K4ECGX + MT9V113 */

/* S5K4ECGX don't have HW standby. */
#define CAMIO_R_RST_N		0
#define CAMIO_F_RST_N		1
#define CAMIO_F_STB		2
#define CAMIO_FL_DRV_EN		3
#define CAMIO_FL_MODE		4
#define CAMIO_MAX		5

static sgpio_ctrl_t sgpios[CAMIO_MAX] = {
	{CAMIO_R_RST_N,   "CAMIO_R_RST_N",  106},
	{CAMIO_F_RST_N,   "CAMIO_F_RST_N",  137},
	{CAMIO_F_STB,     "CAMIO_F_STB",    139},
	{CAMIO_FL_DRV_EN, "CAMIO_FL_DRV_EN", 31},
	{CAMIO_FL_MODE,   "CAMIO_FL_MODE",   62},
};

#define CAMV_IO_1P8V	0
#define CAMV_CORE_1P2V	1
#define CAMV_A_2P8V	2
#define CAMV_AF_2P8V	3
#define CAMV_MAX	4

static svreg_ctrl_t svregs[CAMV_MAX] = {
	{CAMV_IO_1P8V,   "8901_mvs0", NULL, 1800},
	{CAMV_CORE_1P2V, "8901_l0",   NULL, 1200},
	{CAMV_A_2P8V,    "8901_l2",   NULL, 2800},
	{CAMV_AF_2P8V,   "8901_l3",   NULL, 2800},
};

#else
#error "unknown machine!"
#endif


extern si2c_const_param_t s5k4ecgx_const_params[SI2C_PID_MAX];
static si2c_param_t s5k4ecgx_params[SI2C_PID_MAX];
#ifdef CONFIG_PANTECH_CAMERA_TUNER
static si2c_param_t s5k4ecgx_tuner_params[SI2C_PID_MAX];
#endif

static s5k4ecgx_ctrl_t *s5k4ecgx_ctrl = NULL;
static struct i2c_client *s5k4ecgx_client = NULL;
static s5k4ecgx_work_t *s5k4ecgx_work = NULL;
DEFINE_MUTEX(s5k4ecgx_mutex);

static int sensor_mode = SENSOR_PREVIEW_MODE;
static int flash_mode = 0;
static bool config_csi_done = false;

static bool autofocus = 0;

static int s5k4ecgx_set_flash(int flash);


static int s5k4ecgx_power_on(void)
{
	int rc = 0;

	CDBG("%s E\n", __func__);

	if (sgpio_ctrl(sgpios, CAMIO_R_RST_N, 0) < 0)	rc = -EIO;
	mdelay(1);
	if (svreg_ctrl(svregs, CAMV_CORE_1P2V, 1) < 0)	rc = -EIO;
	mdelay(1); /* > 20us */
	if (svreg_ctrl(svregs, CAMV_A_2P8V, 1) < 0)	rc = -EIO;
	mdelay(1); /* > 15us */
	if (svreg_ctrl(svregs, CAMV_IO_1P8V, 1) < 0)	rc = -EIO;
	mdelay(1); /* > 0ms */
	msm_camio_clk_rate_set(24000000);
	msleep(10); /* > 15us */
	if (sgpio_ctrl(sgpios, CAMIO_R_RST_N, 1) < 0)	rc = -EIO;
	mdelay(1); /* > 60us */

	/*PANTECH_CAMERA_TODO, check correct timing, spec don't have AF */
	if (svreg_ctrl(svregs, CAMV_AF_2P8V, 1) < 0)	rc = -EIO;
	mdelay(1);

	CDBG("%s X (%d)\n", __func__, rc);
	return rc;
}


static int s5k4ecgx_power_off(void)
{
	int rc = 0;

	CDBG("%s E\n", __func__);

	if (sgpio_ctrl(sgpios, CAMIO_R_RST_N, 0) < 0)	rc = -EIO;
	mdelay(1); /* > 50us */
	/* MCLK will be disabled once again after this. */
//	(void)msm_camio_clk_disable(CAMIO_CAM_MCLK_CLK);
	mdelay(1); /*  > 0ms */

	/*PANTECH_CAMERA_TODO, check correct timing, spec don't have AF */
	if (svreg_ctrl(svregs, CAMV_AF_2P8V, 0) < 0)	rc = -EIO;
	mdelay(1);
	if (svreg_ctrl(svregs, CAMV_A_2P8V, 0) < 0)	rc = -EIO;
	mdelay(1); /* > 0ms */
	if (svreg_ctrl(svregs, CAMV_CORE_1P2V, 0) < 0)	rc = -EIO;
	mdelay(1); /* > 0ms */
	if (svreg_ctrl(svregs, CAMV_IO_1P8V, 0) < 0)	rc = -EIO;
	mdelay(1); /* > 0ms */

	CDBG("%s X (%d)\n", __func__, rc);
	return rc;
}


static int s5k4ecgx_config_csi(void)
{
	struct msm_camera_csi_params *csi = NULL;
	int rc = 0;

	CDBG("%s E\n", __func__);

	csi = kmalloc(sizeof(struct msm_camera_csi_params), GFP_KERNEL);
	if (!csi) {
		CERR("%s err(-ENOMEM)", __func__);
		return -ENOMEM;
	}

	csi->data_format = CSI_8BIT;
	csi->lane_cnt    = 2;
	csi->lane_assign = 0xe4;
	csi->settle_cnt  = 0x14;
	csi->dpcm_scheme = 0;

	rc = msm_camio_csi_config(csi);
	if (rc < 0) {
		kfree(csi);
		CERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

	msleep(10);

	config_csi_done = true;
	kfree(csi);

	CDBG("%s X\n", __func__);
	return 0;
}


static int s5k4ecgx_video_config(void)
{
	int rc = 0;

	CDBG("%s E\n", __func__);

	rc = si2c_write_param(SI2C_SA, SI2C_PREVIEW, s5k4ecgx_params);
	if (rc < 0) {
		CERR("%s video config err(%d)\n", __func__, rc);
		return rc;
	}

	sensor_mode = SENSOR_PREVIEW_MODE;

	CDBG("%s X\n", __func__);
	return 0;
}


static int s5k4ecgx_snapshot_config(void)
{
	int rc = 0;
	uint16_t luxh = 0, luxl = 0;

	si2c_cmd_t cmds_read_luxh[] = {
		{SI2C_WR, SI2C_A2D2, 0xFCFC, 0xD000, 0},
		{SI2C_WR, SI2C_A2D2, 0x002C, 0x7000, 0},
		{SI2C_WR, SI2C_A2D2, 0x002E, 0x2C1A, 0},
		{SI2C_EOC, SI2C_A1D1, 0, 0, 0},
	};
	si2c_cmd_t cmds_read_luxl[] = {
		{SI2C_WR, SI2C_A2D2, 0xFCFC, 0xD000, 0},
		{SI2C_WR, SI2C_A2D2, 0x002C, 0x7000, 0},
		{SI2C_WR, SI2C_A2D2, 0x002E, 0x2C18, 0},
		{SI2C_EOC, SI2C_A1D1, 0, 0, 0},
	};


	CDBG("%s E\n", __func__);


	if( autofocus == 0 )
	{
		if (flash_mode == 1) {
			if (si2c_write_cmds(SI2C_SA, cmds_read_luxh) < 0) rc = -EIO;
			if (si2c_read(SI2C_A2D2, SI2C_SA, 0x0F12, &luxh) < 0) rc = -EIO;
			if (si2c_write_cmds(SI2C_SA, cmds_read_luxl) < 0) rc = -EIO;
			if (si2c_read(SI2C_A2D2, SI2C_SA, 0x0F12, &luxl) < 0) rc = -EIO;
			if (rc < 0) {
				CERR("%s err(-EIO)\n", __func__);
				return rc;
			}

			if ((luxh == 0) && (luxl <= 50))
					s5k4ecgx_set_flash(4);
		} else if (flash_mode == 2) {
			s5k4ecgx_set_flash(4);
		}

		msleep(100);
	}

	rc = si2c_write_param(SI2C_SA, SI2C_SNAPSHOT, s5k4ecgx_params);
	if (rc < 0) {
		CERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

	sensor_mode = SENSOR_SNAPSHOT_MODE;
	autofocus = 0;
	CDBG("%s X\n", __func__);
	return 0;
}


static int s5k4ecgx_auto_focus(void)
{
	uint16_t luxh = 0, luxl = 0;
	uint16_t af1 = 0, af2 = 0;
	int rc = 0;
	int i = 0;

	si2c_cmd_t cmds_read_luxh[] = {
		{SI2C_WR, SI2C_A2D2, 0xFCFC, 0xD000, 0},
		{SI2C_WR, SI2C_A2D2, 0x002C, 0x7000, 0},
		{SI2C_WR, SI2C_A2D2, 0x002E, 0x2C1A, 0},
		{SI2C_EOC, SI2C_A1D1, 0, 0, 0},
	};
	si2c_cmd_t cmds_read_luxl[] = {
		{SI2C_WR, SI2C_A2D2, 0xFCFC, 0xD000, 0},
		{SI2C_WR, SI2C_A2D2, 0x002C, 0x7000, 0},
		{SI2C_WR, SI2C_A2D2, 0x002E, 0x2C18, 0},
		{SI2C_EOC, SI2C_A1D1, 0, 0, 0},
	};

	si2c_cmd_t cmds_read_af1[] = {
		{SI2C_WR, SI2C_A2D2, 0xFCFC, 0xD000, 0},
		{SI2C_WR, SI2C_A2D2, 0x002C, 0x7000, 0},
		{SI2C_WR, SI2C_A2D2, 0x002E, 0x2EEE, 0},
		{SI2C_EOC, SI2C_A1D1, 0, 0, 0},
	};
	si2c_cmd_t cmds_read_af2[] = {
		{SI2C_WR, SI2C_A2D2, 0xFCFC, 0xD000, 0},
		{SI2C_WR, SI2C_A2D2, 0x002C, 0x7000, 0},
		{SI2C_WR, SI2C_A2D2, 0x002E, 0x2007, 0},
		{SI2C_EOC, SI2C_A1D1, 0, 0, 0},
	};

	CDBG("%s E\n", __func__);

	//flash_mode 가 1(auto) 이면 조도를 읽어서 켜고
	//flash_mode 가 2(on) 이면 항상 켜고

	if (flash_mode == 1) {
		if (si2c_write_cmds(SI2C_SA, cmds_read_luxh) < 0) rc = -EIO;
		if (si2c_read(SI2C_A2D2, SI2C_SA, 0x0F12, &luxh) < 0) rc = -EIO;
		if (si2c_write_cmds(SI2C_SA, cmds_read_luxl) < 0) rc = -EIO;
		if (si2c_read(SI2C_A2D2, SI2C_SA, 0x0F12, &luxl) < 0) rc = -EIO;
		if (rc < 0) {
			CERR("%s err(-EIO)\n", __func__);
			return rc;
		}

		if ((luxh == 0) && (luxl <= 50))
				s5k4ecgx_set_flash(4);
	} else if (flash_mode == 2) {
		s5k4ecgx_set_flash(4);
	}

	/* auto focus */
	rc = si2c_write_param(SI2C_SA, SI2C_AF_TRIGGER, s5k4ecgx_params);
	if (rc < 0) {
		CERR("%s err(%d)\n", __func__, rc);
		s5k4ecgx_set_flash(0);
		return rc;
	}

	for(i = 0; i < 255; i++)
	{
		if (si2c_write_cmds(SI2C_SA, cmds_read_af1) < 0) rc = -EIO;
		if (si2c_read(SI2C_A2D2, SI2C_SA, 0x0F12, &af1) < 0) rc = -EIO;
		if( af1 == 0 || ( (af1 >= 3) && ( af1 < 9) ) )
		{
			CERR("%s failed search_value = %d\n", __func__, af1);
			msleep(300);
			return 0;
		}
		if( af1 == 2 )
		{
			if (si2c_write_cmds(SI2C_SA, cmds_read_af2) < 0) rc = -EIO;
			if (si2c_read(SI2C_A2D2, SI2C_SA, 0x0F12, &af2) < 0) rc = -EIO;
			if ( (af2 & 0xff00) == 0)
			{
				CDBG(" success af i = %d\n", i);
				break;
			}
		}
		msleep(10);
	}
	
	/* Last preview frame is not focused for some reason,
	 * so add delay here to view several frames after AF is done. */
//	msleep(150);

#if 0 //ztlee
	if ((flash_mode == 1) || (flash_mode == 2))
		s5k4ecgx_set_flash(0);
#endif
	msleep(300);

	CDBG("%s X\n", __func__);

	return 0;
}


static int s5k4ecgx_set_focus(int focus)
{
	si2c_pid_t pid = SI2C_PID_MAX;
	int rc = 0;

	CDBG("%s focus=%d\n", __func__, focus);

	switch (focus) {
	case 0: pid = SI2C_FOCUS_INFINITY; break;
	default:
/*PANTECH_CAMERA_TODO, 
  CFG_SET_FOCUS_MODE 만들고 대체할것
  렌즈를 그냥 매크로로 밀어버린다. 원래는 모드만 설정해야 한다. */
		pid = SI2C_FOCUS_MACRO;
		break;
	}

	rc = si2c_write_param(SI2C_SA, pid, s5k4ecgx_params);
	if (rc < 0) {
		CERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

	CDBG("%s X\n", __func__);
	return 0;
}


static int s5k4ecgx_set_brightness(int brightness)
{
	si2c_pid_t pid = SI2C_PID_MAX;
	int rc = 0;

	CDBG("%s brightness=%d\n", __func__, brightness);

	switch (brightness) {
	case 0: pid = SI2C_BRIGHTNESS_M4; break;
	case 1: pid = SI2C_BRIGHTNESS_M3; break;
	case 2: pid = SI2C_BRIGHTNESS_M2; break;
	case 3: pid = SI2C_BRIGHTNESS_M1; break;
	case 4: pid = SI2C_BRIGHTNESS_0; break;
	case 5: pid = SI2C_BRIGHTNESS_P1; break;
	case 6: pid = SI2C_BRIGHTNESS_P2; break;
	case 7: pid = SI2C_BRIGHTNESS_P3; break;
	case 8: pid = SI2C_BRIGHTNESS_P4; break;
	default:
		CERR("%s err(-EINVAL)\n", __func__);
		return -EINVAL;
	}

	rc = si2c_write_param(SI2C_SA, pid, s5k4ecgx_params);
	if (rc < 0) {
		CERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

	CDBG("%s X\n", __func__);
	return 0;
}


static int s5k4ecgx_set_effect(int effect)
{
	si2c_pid_t pid = SI2C_PID_MAX;
	int rc = 0;

	CDBG("%s effect=%d\n", __func__, effect);

	switch (effect) {
	case 0: pid = SI2C_EFFECT_OFF; break;
	case 1: pid = SI2C_EFFECT_MONO; break;
	case 2: pid = SI2C_EFFECT_NEGATIVE; break;
	case 3: pid = SI2C_EFFECT_SOLARIZE; break;
	case 4: pid = SI2C_EFFECT_SEPIA; break;
	default:
		CERR("%s err(-EINVAL)\n", __func__);
		return -EINVAL;
	}

	rc = si2c_write_param(SI2C_SA, pid, s5k4ecgx_params);
	if (rc < 0) {
		CERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

	CDBG("%s X\n", __func__);
	return 0;
}


static int s5k4ecgx_set_exposure(int exposure)
{
	si2c_pid_t pid = SI2C_PID_MAX;
	int rc = 0;

	CDBG("%s exposure=%d\n", __func__, exposure);

	switch (exposure) {
	case 1: pid = SI2C_EXPOSURE_AVERAGE; break;
	case 2: 
	case 3: pid = SI2C_EXPOSURE_CENTER; break;
	default:
		CERR("%s err(-EINVAL)\n", __func__);
		return -EINVAL;
	}

	rc = si2c_write_param(SI2C_SA, pid, s5k4ecgx_params);
	if (rc < 0) {
		CERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

	CDBG("%s X\n", __func__);
	return 0;
}


static int s5k4ecgx_set_flash(int flash)
{
	int rc = 0;

	CDBG("%s flash=%d\n", __func__, flash);

	switch (flash) {
	case 0: /* off */
		if (sgpio_ctrl(sgpios, CAMIO_FL_MODE, 0) < 0)	rc = -EIO;
		if (sgpio_ctrl(sgpios, CAMIO_FL_DRV_EN, 0) < 0)	rc = -EIO;
		udelay(500); // mandatory (AAT3176, T-off)
		break;

       /* flash Auto, On설정후 Camcoder -> camera전환시 Flash 계속 켜져있는 현상 수정.*/		
	case 1: /* auto */
	case 2: /* on */
		if (sgpio_ctrl(sgpios, CAMIO_FL_MODE, 0) < 0)	rc = -EIO;
		if (sgpio_ctrl(sgpios, CAMIO_FL_DRV_EN, 0) < 0)	rc = -EIO;
		
//		if (sgpio_ctrl(sgpios, CAMIO_FL_MODE, 1) < 0)	rc = -EIO;
//		if (sgpio_ctrl(sgpios, CAMIO_FL_DRV_EN, 1) < 0)	rc = -EIO;
		break;
	case 3: /* torch */
		if (sgpio_ctrl(sgpios, CAMIO_FL_MODE, 0) < 0)	rc = -EIO;
		if (sgpio_ctrl(sgpios, CAMIO_FL_DRV_EN, 1) < 0)	rc = -EIO;
		udelay(500); // mandatory (AAT3176, T-lat)
		break;
	case 4: // flash On
		if (sgpio_ctrl(sgpios, CAMIO_FL_MODE, 1) < 0)	rc = -EIO;
		if (sgpio_ctrl(sgpios, CAMIO_FL_DRV_EN, 1) < 0)	rc = -EIO;
		break;
	default:
		CERR("%s err(-EINVAL)\n", __func__);
		return -EINVAL;
	}

	if (rc < 0) {
		CERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

	flash_mode = flash;

	CDBG("%s X\n", __func__);
	return 0;
}


static int s5k4ecgx_set_mode(int mode)
{
	int rc = 0;

	CDBG("%s mode=%d\n", __func__, mode);

	if (!config_csi_done) {
#ifdef CONFIG_PANTECH_CAMERA_TUNER
		rc = si2c_write_param(SI2C_SA, SI2C_INIT, s5k4ecgx_params);
		if (rc < 0) {
			CERR("%s init err(%d)\n", __func__, rc);
			return rc;
		}
#endif
		rc = s5k4ecgx_config_csi();
		if (rc < 0) {
			CERR("%s err(-EIO)\n", __func__);
			return -EIO;
		}
	}

	switch (mode) {
	case SENSOR_PREVIEW_MODE:
		if (sensor_mode != SENSOR_PREVIEW_MODE)
			rc = s5k4ecgx_video_config();
		break;
		
	case SENSOR_SNAPSHOT_MODE:
		if (sensor_mode != SENSOR_SNAPSHOT_MODE)
			rc = s5k4ecgx_snapshot_config();
		break;

	default:
		rc = -EINVAL;
		break;
	}

	CDBG("%s X (%d)\n", __func__, rc);
	return rc;
}


static int s5k4ecgx_set_preview_fps(int preview_fps)
{
	si2c_pid_t pid = SI2C_PID_MAX;
	int rc = 0;

	CDBG("%s preview_fps=%d\n", __func__, preview_fps);

	switch (preview_fps) {
	case  0: pid = SI2C_FPS_VARIABLE; break;
	case  7: pid = SI2C_FPS_FIXED7; break;
	case  8: pid = SI2C_FPS_FIXED8; break;
	case 10: pid = SI2C_FPS_FIXED10; break;
	case 14: pid = SI2C_FPS_FIXED14; break;
	case 15: pid = SI2C_FPS_FIXED15; break;
	case 20: pid = SI2C_FPS_FIXED20; break;
	case 24: pid = SI2C_FPS_FIXED24; break;
	case 25: pid = SI2C_FPS_FIXED25; break;
	case 30: pid = SI2C_FPS_FIXED30; break;
//	case 30: pid = SI2C_FPS_VARIABLE; break;
	case 31: pid = SI2C_FPS_VARIABLE; break;
	default:
		//PANTECH_CAMERA_TODO
		return 0;
		//CERR("%s err(-EINVAL)\n", __func__);
		//return -EINVAL;
	}

	rc = si2c_write_param(SI2C_SA, pid, s5k4ecgx_params);
	if (rc < 0) {
		CERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

	CDBG("%s X\n", __func__);
	return 0;
}


static int s5k4ecgx_set_reflect(int reflect)
{
	si2c_pid_t pid = SI2C_PID_MAX;
	int rc = 0;

	CDBG("%s reflect=%d\n", __func__, reflect);

	switch (reflect) {
	case 0: pid = SI2C_REFLECT_OFF; break;
	case 1: pid = SI2C_REFLECT_MIRROR; break;
	case 2: pid = SI2C_REFLECT_WATER; break;
	case 3: pid = SI2C_REFLECT_MIRROR_WATER; break;
	default:
		CERR("%s err(-EINVAL)\n", __func__);
		return -EINVAL;
	}

	rc = si2c_write_param(SI2C_SA, pid, s5k4ecgx_params);
	if (rc < 0) {
		CERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

	CDBG("%s X\n", __func__);
	return 0;
}


#ifdef CONFIG_PANTECH_CAMERA_TUNER
static int s5k4ecgx_set_tuner(struct tuner_cfg tuner)
{
	si2c_cmd_t *cmds = NULL;
	char *fbuf = NULL;

	CDBG("%s fbuf=%p, fsize=%d\n", __func__, tuner.fbuf, tuner.fsize);

	if (!tuner.fbuf || (tuner.fsize == 0)) {
		CERR("%s err(-EINVAL)\n", __func__);
		return -EINVAL;
	}

	fbuf = (char *)kmalloc(tuner.fsize, GFP_KERNEL);
	if (!fbuf) {
		CERR("%s err(-ENOMEM)\n", __func__);
		return -ENOMEM;
	}

	if (copy_from_user(fbuf, tuner.fbuf, tuner.fsize)) {
		CERR("%s err(-EFAULT)\n", __func__);
		kfree(fbuf);
		return -EFAULT;
	}

	cmds = ptune_parse("@init", fbuf);
	if (!cmds) {
		CERR("%s no @init\n", __func__);
		kfree(fbuf);
		return -EFAULT;
	}

	s5k4ecgx_tuner_params[SI2C_INIT].cmds = cmds;
	s5k4ecgx_params[SI2C_INIT].cmds = cmds;

	kfree(fbuf);

	CDBG("%s X\n", __func__);
	return 0;
}
#endif


static int32_t s5k4ecgx_set_wb(int wb)
{
	si2c_pid_t pid = SI2C_PID_MAX;
	int rc = 0;

	CDBG("%s wb=%d\n", __func__, wb);

	switch (wb) {
	case 1: pid = SI2C_WB_AUTO; break;
	case 3: pid = SI2C_WB_INCANDESCENT; break;
	case 4: pid = SI2C_WB_FLUORESCENT; break;
	case 5: pid = SI2C_WB_DAYLIGHT; break;
	case 6: pid = SI2C_WB_CLOUDY; break;
	default:
		CERR("%s err(-EINVAL)\n", __func__);
		return -EINVAL;
	}

	rc = si2c_write_param(SI2C_SA, pid, s5k4ecgx_params);
	if (rc < 0) {
		CERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

	CDBG("%s X\n", __func__);
	return 0;
}


int s5k4ecgx_sensor_init(const struct msm_camera_sensor_info *sinfo)
{
	int rc = 0;

	CDBG("%s E\n", __func__);

	config_csi_done = false;
	sensor_mode = SENSOR_PREVIEW_MODE;
	flash_mode = 0;

	memset(s5k4ecgx_params, 0, sizeof(s5k4ecgx_params));
#ifdef CONFIG_PANTECH_CAMERA_TUNER
	memset(s5k4ecgx_tuner_params, 0, sizeof(s5k4ecgx_tuner_params));
#endif

	//PANTECH_CAMERA_TODO, adapter 설정과 param 설정을 분리할 것. ugly 하다...
	rc = si2c_init(s5k4ecgx_client->adapter, 
			s5k4ecgx_const_params, s5k4ecgx_params);
	if (rc < 0)
		goto sensor_init_fail;

	s5k4ecgx_ctrl = kzalloc(sizeof(s5k4ecgx_ctrl_t), GFP_KERNEL);
	if (!s5k4ecgx_ctrl) {
		CERR("%s err(-ENOMEM)\n", __func__);
		goto sensor_init_fail;
	}

	if (sinfo)
		s5k4ecgx_ctrl->sinfo = sinfo;

	rc = sgpio_init(sgpios, CAMIO_MAX);
	if (rc < 0)
		goto sensor_init_fail;

	rc = svreg_init(svregs, CAMV_MAX);
	if (rc < 0)
		goto sensor_init_fail;

	rc = s5k4ecgx_power_on();
	if (rc < 0)
		goto sensor_init_fail;

#ifdef CONFIG_PANTECH_CAMERA_TUNER
	CERR("%s tuner is enabled, skip writing INIT param!\n", __func__);
#else
	rc = si2c_write_param(SI2C_SA, SI2C_INIT, s5k4ecgx_params);
	if (rc < 0)
		goto sensor_init_fail;
#endif

	CDBG("%s X\n", __func__);
	return 0;

sensor_init_fail:

	if (s5k4ecgx_ctrl) {
		kfree(s5k4ecgx_ctrl);
		s5k4ecgx_ctrl = NULL;
	}
	(void)s5k4ecgx_power_off();
	svreg_release(svregs, CAMV_MAX);
	sgpio_release(sgpios, CAMIO_MAX);
	si2c_release();

	CERR("%s err(%d)\n", __func__, rc);
	return rc;
}


int s5k4ecgx_sensor_config(void __user *argp)
{
	struct sensor_cfg_data sc;
	int rc = 0;

	rc = copy_from_user(&sc, argp, sizeof(struct sensor_cfg_data));
	if (rc) {
		CERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

	/*CDBG("%s type=%d, mode=%d\n", __func__, sc.cfgtype, sc.mode);*/

	mutex_lock(&s5k4ecgx_mutex);

	switch (sc.cfgtype) {
	case CFG_AUTO_FOCUS:
		autofocus = 1;
		rc = s5k4ecgx_auto_focus();
		break;
	case CFG_SET_BRIGHTNESS:
		rc = s5k4ecgx_set_brightness(sc.cfg.brightness);
		break;
	case CFG_SET_EFFECT:
		rc = s5k4ecgx_set_effect(sc.cfg.effect);
		break;
	//PANTECH_CAMERA_TODO, EXPOSURE_MODE -> EXPOSURE
	case CFG_SET_EXPOSURE_MODE:
		rc = s5k4ecgx_set_exposure(sc.cfg.exposure);
		break;
/*PANTECH_CAMERA_TODO, CFG_SET_FOCUS_MODE 만들어서 대체할 것*/
	case CFG_SET_FOCUS_STEP:
		rc = s5k4ecgx_set_focus(sc.cfg.focus_step);
		break;
	case CFG_SET_MODE:
		rc = s5k4ecgx_set_mode(sc.mode);
		break;
	case CFG_SET_PREVIEW_FPS:
		rc = s5k4ecgx_set_preview_fps(sc.cfg.preview_fps);
		break;
	case CFG_SET_REFLECT:
		rc = s5k4ecgx_set_reflect(sc.cfg.reflect);
		break;
#ifdef CONFIG_PANTECH_CAMERA_TUNER
	case CFG_SET_TUNER:
		rc = s5k4ecgx_set_tuner(sc.cfg.tuner);
		break;
#endif
	case CFG_SET_WB:
		//PANTECH_CAMERA_TODO, whitebalance -> wb
		rc = s5k4ecgx_set_wb(sc.cfg.whitebalance);
		break;

/*PANTECH_CAMERA_TODO, led_mode=>flash*/
	case CFG_SET_LED_MODE:
		rc = s5k4ecgx_set_flash(sc.cfg.led_mode);
		break;

	default:
		CDBG("%s err(-EINVAL)\n", __func__);
		rc = -EINVAL;
		break;
	}

	mutex_unlock(&s5k4ecgx_mutex);

	/*CDBG("%s X (%d)\n", __func__, rc);*/
	return rc;
}


int s5k4ecgx_sensor_release(void)
{
#ifdef CONFIG_PANTECH_CAMERA_TUNER
	si2c_pid_t i = 0;
#endif

	CDBG("%s E\n", __func__);

	(void)s5k4ecgx_set_flash(0);

	mutex_lock(&s5k4ecgx_mutex);

	if (s5k4ecgx_ctrl) {
		kfree(s5k4ecgx_ctrl);
		s5k4ecgx_ctrl = NULL;
	}

	(void)s5k4ecgx_power_off();

	svreg_release(svregs, CAMV_MAX);
	sgpio_release(sgpios, CAMIO_MAX);

#ifdef CONFIG_PANTECH_CAMERA_TUNER
	for (i = 0; i < SI2C_PID_MAX; i++) {
		if (s5k4ecgx_tuner_params[i].cmds)
			kfree(s5k4ecgx_tuner_params[i].cmds);
	}
#endif

	si2c_release();

	mutex_unlock(&s5k4ecgx_mutex);

	CDBG("%s X\n", __func__);
	return 0;
}


static int s5k4ecgx_i2c_probe(
	struct i2c_client *client,
	const struct i2c_device_id *id)
{
	CDBG("%s E\n", __func__);

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		CERR("%s (-ENOTSUPP)\n", __func__);
		return -ENOTSUPP;
	}

	s5k4ecgx_work = kzalloc(sizeof(s5k4ecgx_work_t), GFP_KERNEL);
	if (!s5k4ecgx_work) {
		CERR("%s (-ENOMEM)\n", __func__);
		return -ENOMEM;
	}

	i2c_set_clientdata(client, s5k4ecgx_work);
	s5k4ecgx_client = client;

	CDBG("%s X\n", __func__);
	return 0;
}


static const struct i2c_device_id s5k4ecgx_i2c_id[] = {
	{ "s5k4ecgx", 0},
	{ },
};


static struct i2c_driver s5k4ecgx_i2c_driver = {
	.id_table = s5k4ecgx_i2c_id,
	.probe  = s5k4ecgx_i2c_probe,
	.remove = __exit_p(s5k4ecgx_i2c_remove),
	.driver = {
		.name = "s5k4ecgx",
	},
};


static int s5k4ecgx_sensor_probe(
	const struct msm_camera_sensor_info *sinfo,
	struct msm_sensor_ctrl *sctrl)
{
	int rc = 0;

	CDBG("%s E\n", __func__);

	rc = i2c_add_driver(&s5k4ecgx_i2c_driver);
	if (rc < 0 || s5k4ecgx_client == NULL) {
		CERR("%s err(-ENOTSUPP)\n", __func__);
		return -ENOTSUPP;
	}

	sctrl->s_init    = s5k4ecgx_sensor_init;
	sctrl->s_release = s5k4ecgx_sensor_release;
	sctrl->s_config  = s5k4ecgx_sensor_config;

	sctrl->s_camera_type = BACK_CAMERA_2D;
	sctrl->s_mount_angle = 90;

	CDBG("%s X\n", __func__);
	return 0;
}


static int __s5k4ecgx_probe(struct platform_device *pdev)
{
	CDBG("%s EX\n", __func__);
	return msm_camera_drv_start(pdev, s5k4ecgx_sensor_probe);
}


static struct platform_driver msm_camera_driver = {
	.probe = __s5k4ecgx_probe,
	.driver = {
		.name = "msm_camera_s5k4ecgx",
		.owner = THIS_MODULE,
	},
};


static int __init s5k4ecgx_init(void)
{
	CDBG("%s EX\n", __func__);
	return platform_driver_register(&msm_camera_driver);
}
module_init(s5k4ecgx_init);


static void __exit s5k4ecgx_exit(void)
{
	CDBG("%s EX\n", __func__);
	i2c_del_driver(&s5k4ecgx_i2c_driver);
}
module_exit(s5k4ecgx_exit);


MODULE_DESCRIPTION("Samsung 5MP SoC Sensor Driver");
MODULE_LICENSE("GPL v2");
