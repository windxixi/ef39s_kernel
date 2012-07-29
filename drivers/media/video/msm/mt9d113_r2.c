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
#include "mt9d113_r2.h"
#ifdef CONFIG_PANTECH_CAMERA_TUNER
#include "ptune_parser.h"
#endif

/* PANTECH_CAMERA_TODO */
#undef CDBG
#undef CERR
#define CDBG(fmt, args...) printk(KERN_DEBUG fmt, ##args)
#define CERR(fmt, args...) printk(KERN_ERR fmt, ##args)

/* I2C slave address */
/*PANTECH_CAMERA_TODO, we use CE1612 and MT9D112 for EF40 WS20 and
 * they use same I2C slave address. So we register MT9D112 with fake address,
 * and use 0x78 instead of 0x7A in kernel driver. */
#define SI2C_SA	(mt9d113_client->addr)
/*#define SI2C_SA	(0x78 >> 1)*/


typedef struct {
	struct work_struct work;
} mt9d113_work_t;

typedef struct {
	const struct msm_camera_sensor_info *sinfo;
} mt9d113_ctrl_t;


#if (defined(CONFIG_MACH_MSM8X60_EF40S) && (BOARD_REV >= WS20)) || \
    (defined(CONFIG_MACH_MSM8X60_EF40K) && (BOARD_REV >= WS20))
/* CE1612 + MT9D113 */

#define CAMIO_R_RST_N	0
#define CAMIO_R_STB_N	1
#define CAMIO_F_RST_N	2
#define CAMIO_F_STB	3
#define CAMIO_MAX	4

static sgpio_ctrl_t sgpios[CAMIO_MAX] = {
	{CAMIO_R_RST_N, "CAMIO_R_RST_N", 106},
	{CAMIO_R_STB_N, "CAMIO_R_STB_N",  46},
	{CAMIO_F_RST_N, "CAMIO_F_RST_N", 137},
	{CAMIO_F_STB,   "CAMIO_F_STB",   139},
};

#define CAMV_IO_1P8V	0
#define CAMV_CORE_1P8V	1
#define CAMV_A_2P8V	2
#define CAMV_MAX	3

static svreg_ctrl_t svregs[CAMV_MAX] = {
	{CAMV_IO_1P8V,   "8901_mvs0", NULL, 1800}, /* I2C pull-up */
	{CAMV_CORE_1P8V, "8901_lvs3", NULL, 1800},
	{CAMV_A_2P8V,    "8058_l19",  NULL, 2800},
};

#else
#error "unknown machine!"
#endif


extern si2c_const_param_t mt9d113_const_params[SI2C_PID_MAX];
static si2c_param_t mt9d113_params[SI2C_PID_MAX];
#ifdef CONFIG_PANTECH_CAMERA_TUNER
static si2c_param_t mt9d113_tuner_params[SI2C_PID_MAX];
#endif

static mt9d113_ctrl_t *mt9d113_ctrl = NULL;
static struct i2c_client *mt9d113_client = NULL;
static mt9d113_work_t *mt9d113_work = NULL;
DEFINE_MUTEX(mt9d113_mutex);

static int sensor_mode = SENSOR_PREVIEW_MODE;
static bool config_csi_done = false;


static int mt9d113_power_on(void)
{
	int rc = 0;

	CDBG("%s E\n", __func__);

	if (sgpio_ctrl(sgpios, CAMIO_R_RST_N, 0) < 0)	rc = -EIO;
	if (sgpio_ctrl(sgpios, CAMIO_R_STB_N, 0) < 0)	rc = -EIO;
	mdelay(1);

	if (sgpio_ctrl(sgpios, CAMIO_F_STB, 0) < 0)	rc = -EIO;
	if (sgpio_ctrl(sgpios, CAMIO_F_RST_N, 1) < 0)	rc = -EIO;
	mdelay(1);

	if (svreg_ctrl(svregs, CAMV_IO_1P8V, 1) < 0)	rc = -EIO;
	mdelay(2); /* > 1ms */
	if (svreg_ctrl(svregs, CAMV_CORE_1P8V, 1) < 0)	rc = -EIO;
	mdelay(1);
	msm_camio_clk_rate_set(24000000);
	msleep(10); /* > 1 clk (approx. 0.042us) */
	if (sgpio_ctrl(sgpios, CAMIO_F_RST_N, 0) < 0)	rc = -EIO;
	/*PANTECH_CAMERA_TODO, reset was grounded with 10uF cap in WS20*/
	mdelay(10); /* > 10 clks (approx. 0.42us) */
	if (sgpio_ctrl(sgpios, CAMIO_F_RST_N, 1) < 0)	rc = -EIO;
	mdelay(1); /* > 1 clk (apporx. 0.042us) */
	if (svreg_ctrl(svregs, CAMV_A_2P8V, 1) < 0)	rc = -EIO;
	mdelay(2); /* > 6000 clks (approx. 252us) */

	CDBG("%s X (%d)\n", __func__, rc);
	return rc;
}


static int mt9d113_power_off(void)
{
	int rc = 0;

	CDBG("%s E\n", __func__);

	/*PANTECH_CAMERA_TODO, check correct timing, spec don't have off seq. */

	if (sgpio_ctrl(sgpios, CAMIO_F_STB, 0) < 0)	rc = -EIO;
	if (sgpio_ctrl(sgpios, CAMIO_F_RST_N, 0) < 0)	rc = -EIO;
	mdelay(1);

	/* MCLK will be disabled once again after this. */
//	(void)msm_camio_clk_disable(CAMIO_CAM_MCLK_CLK);
	if (svreg_ctrl(svregs, CAMV_IO_1P8V, 0) < 0)	rc = -EIO;
	if (svreg_ctrl(svregs, CAMV_CORE_1P8V, 0) < 0)	rc = -EIO;
	if (svreg_ctrl(svregs, CAMV_A_2P8V, 0) < 0)	rc = -EIO;
	mdelay(1);

	if (sgpio_ctrl(sgpios, CAMIO_R_RST_N, 0) < 0)	rc = -EIO;
	if (sgpio_ctrl(sgpios, CAMIO_R_STB_N, 0) < 0)	rc = -EIO;
	mdelay(1);

	CDBG("%s X (%d)\n", __func__, rc);
	return rc;
}


static int mt9d113_config_csi(void)
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
	csi->lane_cnt    = 1;
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


static int mt9d113_video_config(void)
{
	int rc = 0;

	CDBG("%s E\n", __func__);

	rc = si2c_write_param(SI2C_SA, SI2C_PREVIEW, mt9d113_params);
	if (rc < 0) {
		CERR("%s video config err(%d)\n", __func__, rc);
		return rc;
	}

	sensor_mode = SENSOR_PREVIEW_MODE;

	CDBG("%s X\n", __func__);
	return 0;
}


static int mt9d113_snapshot_config(void)
{
	int rc = 0;

	CDBG("%s E\n", __func__);

	rc = si2c_write_param(SI2C_SA, SI2C_SNAPSHOT, mt9d113_params);
	if (rc < 0) {
		CERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

	sensor_mode = SENSOR_SNAPSHOT_MODE;

	CDBG("%s X\n", __func__);
	return 0;
}


static int mt9d113_set_brightness(int brightness)
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

	rc = si2c_write_param(SI2C_SA, pid, mt9d113_params);
	if (rc < 0) {
		CERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

	CDBG("%s X\n", __func__);
	return 0;
}


static int mt9d113_set_effect(int effect)
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

	rc = si2c_write_param(SI2C_SA, pid, mt9d113_params);
	if (rc < 0) {
		CERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

	CDBG("%s X\n", __func__);
	return 0;
}


static int mt9d113_set_exposure(int exposure)
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

	rc = si2c_write_param(SI2C_SA, pid, mt9d113_params);
	if (rc < 0) {
		CERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

	CDBG("%s X\n", __func__);
	return 0;
}


static int mt9d113_set_mode(int mode)
{
	int rc = 0;

	CDBG("%s mode=%d\n", __func__, mode);

	if (!config_csi_done) {
#ifdef CONFIG_PANTECH_CAMERA_TUNER
		rc = si2c_write_param(SI2C_SA, SI2C_INIT, mt9d113_params);
		if (rc < 0) {
			CERR("%s init err(%d)\n", __func__, rc);
			return rc;
		}
#endif
		rc = mt9d113_config_csi();
		if (rc < 0) {
			CERR("%s err(-EIO)\n", __func__);
			return -EIO;
		}
	}

	switch (mode) {
	case SENSOR_PREVIEW_MODE:
		if (sensor_mode != SENSOR_PREVIEW_MODE)
			rc = mt9d113_video_config();
		break;
		
	case SENSOR_SNAPSHOT_MODE:
		if (sensor_mode != SENSOR_SNAPSHOT_MODE)
			rc = mt9d113_snapshot_config();
		break;

	default:
		rc = -EINVAL;
		break;
	}

	CDBG("%s X (%d)\n", __func__, rc);
	return rc;
}


static int mt9d113_set_preview_fps(int preview_fps)
{
	si2c_pid_t pid = SI2C_PID_MAX;
	int rc = 0;

	CDBG("%s preview_fps=%d\n", __func__, preview_fps);

	switch (preview_fps) {
	case  0: pid = SI2C_FPS_VARIABLE; break;
	case  7: pid = SI2C_FPS_FIXED7; break;
	case  8: pid = SI2C_FPS_FIXED8; break;
	case 10: pid = SI2C_FPS_FIXED10; break;
	case 15: pid = SI2C_FPS_FIXED15; break;
	case 20: pid = SI2C_FPS_FIXED20; break;
	case 24: pid = SI2C_FPS_FIXED24; break;
	case 25: pid = SI2C_FPS_FIXED25; break;
	default:
		//PANTECH_CAMERA_TODO
		return 0;
		//CERR("%s err(-EINVAL)\n", __func__);
		//return -EINVAL;
	}

	rc = si2c_write_param(SI2C_SA, pid, mt9d113_params);
	if (rc < 0) {
		CERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

	CDBG("%s X\n", __func__);
	return 0;
}


static int mt9d113_set_reflect(int reflect)
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

	rc = si2c_write_param(SI2C_SA, pid, mt9d113_params);
	if (rc < 0) {
		CERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

	CDBG("%s X\n", __func__);
	return 0;
}


#ifdef CONFIG_PANTECH_CAMERA_TUNER
static int mt9d113_set_tuner(struct tuner_cfg tuner)
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

	mt9d113_tuner_params[SI2C_INIT].cmds = cmds;
	mt9d113_params[SI2C_INIT].cmds = cmds;

	kfree(fbuf);

	CDBG("%s X\n", __func__);
	return 0;
}
#endif


static int32_t mt9d113_set_wb(int wb)
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

	rc = si2c_write_param(SI2C_SA, pid, mt9d113_params);
	if (rc < 0) {
		CERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

	CDBG("%s X\n", __func__);
	return 0;
}


int mt9d113_sensor_init(const struct msm_camera_sensor_info *sinfo)
{
	int rc = 0;

	CDBG("%s E\n", __func__);

	config_csi_done = false;
	sensor_mode = SENSOR_PREVIEW_MODE;

	memset(mt9d113_params, 0, sizeof(mt9d113_params));
#ifdef CONFIG_PANTECH_CAMERA_TUNER
	memset(mt9d113_tuner_params, 0, sizeof(mt9d113_tuner_params));
#endif

	//PANTECH_CAMERA_TODO, adapter 설정과 param 설정을 분리할 것. ugly 하다...
	rc = si2c_init(mt9d113_client->adapter, 
			mt9d113_const_params, mt9d113_params);
	if (rc < 0)
		goto sensor_init_fail;

	mt9d113_ctrl = kzalloc(sizeof(mt9d113_ctrl_t), GFP_KERNEL);
	if (!mt9d113_ctrl) {
		CERR("%s err(-ENOMEM)\n", __func__);
		goto sensor_init_fail;
	}

	if (sinfo)
		mt9d113_ctrl->sinfo = sinfo;

	rc = sgpio_init(sgpios, CAMIO_MAX);
	if (rc < 0)
		goto sensor_init_fail;

	rc = svreg_init(svregs, CAMV_MAX);
	if (rc < 0)
		goto sensor_init_fail;

	rc = mt9d113_power_on();
	if (rc < 0)
		goto sensor_init_fail;

#ifdef CONFIG_PANTECH_CAMERA_TUNER
	CERR("%s tuner is enabled, skip writing INIT param!\n", __func__);
#else
	rc = si2c_write_param(SI2C_SA, SI2C_INIT, mt9d113_params);
	if (rc < 0)
		goto sensor_init_fail;
#endif

	CDBG("%s X\n", __func__);
	return 0;

sensor_init_fail:

	if (mt9d113_ctrl) {
		kfree(mt9d113_ctrl);
		mt9d113_ctrl = NULL;
	}
	(void)mt9d113_power_off();
	svreg_release(svregs, CAMV_MAX);
	sgpio_release(sgpios, CAMIO_MAX);
	si2c_release();

	CERR("%s err(%d)\n", __func__, rc);
	return rc;
}


int mt9d113_sensor_config(void __user *argp)
{
	struct sensor_cfg_data sc;
	int rc = 0;

	rc = copy_from_user(&sc, argp, sizeof(struct sensor_cfg_data));
	if (rc) {
		CERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

	/*CDBG("%s type=%d, mode=%d\n", __func__, sc.cfgtype, sc.mode);*/

	mutex_lock(&mt9d113_mutex);

	switch (sc.cfgtype) {
	case CFG_SET_BRIGHTNESS:
		rc = mt9d113_set_brightness(sc.cfg.brightness);
		break;
	case CFG_SET_EFFECT:
		rc = mt9d113_set_effect(sc.cfg.effect);
		break;
	//PANTECH_CAMERA_TODO, EXPOSURE_MODE -> EXPOSURE
	case CFG_SET_EXPOSURE_MODE:
		rc = mt9d113_set_exposure(sc.cfg.exposure);
		break;
	case CFG_SET_MODE:
		rc = mt9d113_set_mode(sc.mode);
		break;
	case CFG_SET_PREVIEW_FPS:
		rc = mt9d113_set_preview_fps(sc.cfg.preview_fps);
		break;
	case CFG_SET_REFLECT:
		rc = mt9d113_set_reflect(sc.cfg.reflect);
		break;
#ifdef CONFIG_PANTECH_CAMERA_TUNER
	case CFG_SET_TUNER:
		rc = mt9d113_set_tuner(sc.cfg.tuner);
		break;
#endif
	case CFG_SET_WB:
		//PANTECH_CAMERA_TODO, whitebalance -> wb
		rc = mt9d113_set_wb(sc.cfg.whitebalance);
		break;

	default:
		CDBG("%s err(-EINVAL)\n", __func__);
		/*rc = -EINVAL; PANTECH_CAMERA_TODO*/
		break;
	}

	mutex_unlock(&mt9d113_mutex);

	/*CDBG("%s X (%d)\n", __func__, rc);*/
	return rc;
}


int mt9d113_sensor_release(void)
{
#ifdef CONFIG_PANTECH_CAMERA_TUNER
	si2c_pid_t i = 0;
#endif

	CDBG("%s E\n", __func__);

	mutex_lock(&mt9d113_mutex);

	if (mt9d113_ctrl) {
		kfree(mt9d113_ctrl);
		mt9d113_ctrl = NULL;
	}

	(void)mt9d113_power_off();

	svreg_release(svregs, CAMV_MAX);
	sgpio_release(sgpios, CAMIO_MAX);

#ifdef CONFIG_PANTECH_CAMERA_TUNER
	for (i = 0; i < SI2C_PID_MAX; i++) {
		if (mt9d113_tuner_params[i].cmds)
			kfree(mt9d113_tuner_params[i].cmds);
	}
#endif

	si2c_release();

	mutex_unlock(&mt9d113_mutex);

	CDBG("%s X\n", __func__);
	return 0;
}


static int mt9d113_i2c_probe(
	struct i2c_client *client,
	const struct i2c_device_id *id)
{
	CDBG("%s E\n", __func__);

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		CERR("%s (-ENOTSUPP)\n", __func__);
		return -ENOTSUPP;
	}

	mt9d113_work = kzalloc(sizeof(mt9d113_work_t), GFP_KERNEL);
	if (!mt9d113_work) {
		CERR("%s (-ENOMEM)\n", __func__);
		return -ENOMEM;
	}

	i2c_set_clientdata(client, mt9d113_work);
	mt9d113_client = client;

	CDBG("%s X\n", __func__);
	return 0;
}


static const struct i2c_device_id mt9d113_i2c_id[] = {
	{ "mt9d113", 0},
	{ },
};


static struct i2c_driver mt9d113_i2c_driver = {
	.id_table = mt9d113_i2c_id,
	.probe  = mt9d113_i2c_probe,
	.remove = __exit_p(mt9d113_i2c_remove),
	.driver = {
		.name = "mt9d113",
	},
};


static int mt9d113_sensor_probe(
	const struct msm_camera_sensor_info *sinfo,
	struct msm_sensor_ctrl *sctrl)
{
	int rc = 0;

	CDBG("%s E\n", __func__);

	rc = i2c_add_driver(&mt9d113_i2c_driver);
	if (rc < 0 || mt9d113_client == NULL) {
		CERR("%s err(-ENOTSUPP)\n", __func__);
		return -ENOTSUPP;
	}

	sctrl->s_init    = mt9d113_sensor_init;
	sctrl->s_release = mt9d113_sensor_release;
	sctrl->s_config  = mt9d113_sensor_config;

	sctrl->s_camera_type = FRONT_CAMERA_2D;
	sctrl->s_mount_angle = 0;

	CDBG("%s X\n", __func__);
	return 0;
}


static int __mt9d113_probe(struct platform_device *pdev)
{
	CDBG("%s EX\n", __func__);
	return msm_camera_drv_start(pdev, mt9d113_sensor_probe);
}


static struct platform_driver msm_camera_driver = {
	.probe = __mt9d113_probe,
	.driver = {
		.name = "msm_camera_mt9d113",
		.owner = THIS_MODULE,
	},
};


static int __init mt9d113_init(void)
{
	CDBG("%s EX\n", __func__);
	return platform_driver_register(&msm_camera_driver);
}
module_init(mt9d113_init);


static void __exit mt9d113_exit(void)
{
	CDBG("%s EX\n", __func__);
	i2c_del_driver(&mt9d113_i2c_driver);
}
module_exit(mt9d113_exit);


MODULE_DESCRIPTION("Aptina 2MP SoC Sensor Driver");
MODULE_LICENSE("GPL v2");
