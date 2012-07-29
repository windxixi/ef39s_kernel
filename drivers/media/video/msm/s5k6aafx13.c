/* Copyright (c) 2009, Code Aurora Forum. All rights reserved.
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
#include <media/msm_camera.h>
#include <mach/gpio.h>
#include "s5k6aafx13.h"
#include <linux/regulator/consumer.h>
#include <linux/regulator/machine.h>

//#include <mach/vreg.h>

#define C_PANTECH_CAMERA_MIN_BRIGHTNESS 0
#define C_PANTECH_CAMERA_MAX_BRIGHTNESS 8

#define C_PANTECH_CAMERA_MIN_PREVIEW_FPS	5
#define C_PANTECH_CAMERA_MAX_PREVIEW_FPS	30

#define F_S5K6AAFX13_POWER
#if (BOARD_REV == PT10)
#define F_ICP_HD_STANDBY
#endif

/* Micron S5K6AAFX13 Registers and their values */
#define SENSOR_DEBUG 0

#define ON  1
#define OFF 0

#define SENSOR_RESET 137
#define SENSOR_STANDBY 139
#ifdef F_ICP_HD_STANDBY
#define SENSOR_RESET_8M 106
#define SENSOR_STANDBY_8M 57
#define ICP_SLAVE_ADDRESS 0x7A>>1
#endif

#define s5k6aafx13_delay_msecs_stream 100//50//200//500
#define S5K6AAFX13_I2C_RETRY	10
#define S5K6AAFX13_I2C_MPERIOD	200

struct s5k6aafx13_work {
	struct work_struct work;
};

static struct  s5k6aafx13_work *s5k6aafx13_sensorw;
static struct  i2c_client *s5k6aafx13_client;

struct s5k6aafx13_ctrl_t {
	const struct msm_camera_sensor_info *sensordata;
};


static struct s5k6aafx13_ctrl_t *s5k6aafx13_ctrl;

static int32_t config_csi;
static bool b_snapshot_flag;

static DECLARE_WAIT_QUEUE_HEAD(s5k6aafx13_wait_queue);
DECLARE_MUTEX(s5k6aafx13_sem);
//static int16_t s5k6aafx13_effect = CAMERA_EFFECT_OFF;


struct s5k6aafx13_vreg_t {
	const char *name;
	unsigned short mvolt;
};

#ifdef F_ICP_HD_STANDBY
static struct regulator *l20a_2p8v_8m;
//static struct regulator *mvs0b_1p8v_8m;
static struct regulator *s2b_1p2v_8m;
static struct regulator *lvs3b_1p8v_8m;
static struct regulator *l14a_2p8v_8m;
#endif

#ifdef F_S5K6AAFX13_POWER
#if (BOARD_REV == PT10)
static struct regulator *lvs1b_1p8v;
static struct regulator *mvs0b_1p8v;
static struct regulator *l9a_2p8v;
static struct regulator *l15a_1p5v;
#else
static struct regulator *mvs0b_1p8v;

static struct regulator *l19a_2p8v;
static struct regulator *s2b_1p5v;
#endif
#endif

/*=============================================================*/
#ifdef F_ICP_HD_STANDBY
static int icp_hd_vreg_init(void)
{
	int rc = 0;
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);

	s2b_1p2v_8m = regulator_get(NULL, "8901_s2");
	if (IS_ERR(s2b_1p2v_8m)) {
		SKYCERR("regulator_get s2b_1p2v fail : 0x%x\n", (unsigned int)s2b_1p2v_8m);
		return -ENODEV;
	}
	rc = regulator_set_voltage(s2b_1p2v_8m, 1300000, 1300000);
	if (rc) {
		SKYCERR("%s: unable to set s2b_1p2v voltage to 1.2V\n", __func__);
		goto fail;
	}

	mvs0b_1p8v = regulator_get(NULL, "8901_mvs0");
	if (IS_ERR(mvs0b_1p8v)) {
		SKYCERR("regulator_get mvs0b_1p8v : 0x%x fail\n", (unsigned int)mvs0b_1p8v);
		return -ENODEV;
	}

	lvs3b_1p8v_8m = regulator_get(NULL, "8901_lvs3");
	if (IS_ERR(lvs3b_1p8v_8m)) {
		SKYCERR("regulator_get lvs3b_1p8v : 0x%x fail\n", (unsigned int)lvs3b_1p8v_8m);
		return -ENODEV;
	}
	
	l20a_2p8v_8m = regulator_get(NULL, "8058_l20");
	if (IS_ERR(l20a_2p8v_8m)) {
		SKYCERR("regulator_get l20a_2p8v : 0x%x fail\n", (unsigned int)l20a_2p8v_8m);
		return -ENODEV;
	}

	rc = regulator_set_voltage(l20a_2p8v_8m, 2800000, 2800000);
	if (rc) {
		SKYCERR("%s: unable to set l20a_2p8v voltage to 2.8V\n", __func__);
		goto fail;
	}

	l14a_2p8v_8m = regulator_get(NULL, "8058_l14");
	if (IS_ERR(l14a_2p8v_8m)) {
		SKYCERR("regulator_get l14a_2p8v : 0x%x fail\n", (unsigned int)l14a_2p8v_8m);
		return -ENODEV;
	}
#if (BOARD_REV >= WS10)
	rc = regulator_set_voltage(l14a_2p8v_8m, 2800000, 2800000);
#else // for SD card
	rc = regulator_set_voltage(l14a_2p8v_8m, 2850000, 2850000);
#endif
	if (rc) {
		SKYCERR("%s: unable to set l14a_2p8v voltage to 2.8V\n", __func__);
		goto fail;
	}
	
	SKYCDBG("%s %s Success!:%d\n", __FILE__, __func__, __LINE__);
	return rc;
fail:
	SKYCDBG("%s %s Failed!:%d\n", __FILE__, __func__, __LINE__);
	if(l20a_2p8v_8m) {
		regulator_put(l20a_2p8v_8m);
	}
	if(s2b_1p2v_8m) {
		regulator_put(s2b_1p2v_8m);
	}
	if(l14a_2p8v_8m) {
		regulator_put(l14a_2p8v_8m);
	}

	return rc;	
}

static int icp_hd_power(int on)
{
	int rc = 0;
		SKYCDBG("%s %s:%d power = %d\n", __FILE__, __func__, __LINE__,on);
	if(on) {
		//standby control
		rc = gpio_tlmm_config(GPIO_CFG(SENSOR_STANDBY_8M, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);
		if (!rc) {
			SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
			gpio_set_value(SENSOR_STANDBY_8M,0);					
		}
		
		rc = regulator_enable(s2b_1p2v_8m);
		if (rc) {
			SKYCERR("%s: Enable regulator s2b_1p2v failed\n", __func__);
			goto fail;
		}
		msleep(1);
		
		rc = regulator_enable(lvs3b_1p8v_8m);
		if (rc) {
			SKYCERR("%s: Enable regulator mvs0b_1p8v failed\n", __func__);
			goto fail;
		}
		msleep(1);
		
		rc = regulator_enable(mvs0b_1p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator mvs0b_1p8v failed\n", __func__);
			goto fail;
		}		
		msleep(1);
		
		rc = regulator_enable(l20a_2p8v_8m);
		if (rc) {
			SKYCERR("%s: Enable regulator l20a_2p8v failed\n", __func__);
			goto fail;
		}
		msleep(1);
		
		rc = regulator_enable(l14a_2p8v_8m);
		if (rc) {
			SKYCERR("%s: Enable regulator l14a_2p8v failed\n", __func__);
			goto fail;
		}

		SKYCDBG("%s %s ON Success:%d\n", __FILE__, __func__, __LINE__);
	}
	else {
		SKYCDBG("%s %s:%d power \n", __FILE__, __func__, __LINE__);
#if 0		
		if(1)//s2b_1p2v) 
		{
			rc = regulator_disable(s2b_1p2v_8m);
			if (rc){
				SKYCERR("%s: Disable regulator s2b_1p2v failed\n", __func__);
				goto fail;
			}
			regulator_put(s2b_1p2v_8m);
		}
		
		if(mvs0b_1p8v) {
			rc = regulator_disable(mvs0b_1p8v);
			if (rc){
				SKYCERR("%s: Disable regulator mvs0b_1p8v failed\n", __func__);		
				goto fail;
			}
		}
#endif		
		SKYCDBG("%s %s:%d power \n", __FILE__, __func__, __LINE__);
		if(1)//mvs0b_1p8v) 
		{
			rc = regulator_disable(lvs3b_1p8v_8m);
			if (rc){
				SKYCERR("%s: Disable regulator lvs3b_1p8v failed\n", __func__);
				goto fail;
			}
		}
		SKYCDBG("%s %s:%d power \n", __FILE__, __func__, __LINE__);
		if(1)//l20a_2p8v) 
		{
			rc = regulator_disable(l20a_2p8v_8m);
			if (rc){
				SKYCERR("%s: Disable regulator l20a_2p8v failed\n", __func__);
				goto fail;
			}
			regulator_put(l20a_2p8v_8m);
		}
		SKYCDBG("%s %s:%d power \n", __FILE__, __func__, __LINE__);
		if(1)//l14a_2p8v) 
		{
		rc = regulator_disable(l14a_2p8v_8m);
			if (rc){
				SKYCERR("%s: Disable regulator l14a_2p8v failed\n", __func__);
				goto fail;
			}
			regulator_put(l14a_2p8v_8m);		
		}
		SKYCDBG("%s %s OFF Success:%d\n", __FILE__, __func__, __LINE__);
	}
	
	return rc;
fail:
	SKYCERR("%s %s Failed!:%d\n", __FILE__, __func__, __LINE__);
	if(l20a_2p8v_8m){
		regulator_put(l20a_2p8v_8m);
	}
	if(s2b_1p2v_8m){
		regulator_put(s2b_1p2v_8m);
	}
	if(l14a_2p8v_8m){
		regulator_put(l14a_2p8v_8m);
	}
	return rc;			
}

static int icp_hd_reset(int set)
{
	int rc = 0;

	rc = gpio_tlmm_config(GPIO_CFG(SENSOR_RESET_8M, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);

	if (!rc) {
		SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
		gpio_set_value(SENSOR_RESET_8M,0);		
		if(set){
			gpio_set_value(SENSOR_RESET_8M,1);		
		}
	}
	else{
		SKYCERR("icp_hd_reset gpio_tlmm_config Failed!\n");
		goto reset_fail;
	}

	SKYCDBG("%s %s Success:%d\n", __FILE__, __func__, __LINE__);
	return rc;

reset_fail:
	SKYCERR("%s %s Failed!:%d\n", __FILE__, __func__, __LINE__);
	return rc;
}
#endif

#ifdef F_S5K6AAFX13_POWER
#if (BOARD_REV == PT10)
static int s5k6aafx13_vreg_init(void)
{
	int rc;
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);

	mvs0b_1p8v = regulator_get(NULL, "8901_mvs0");
	if (IS_ERR(mvs0b_1p8v)) {
		SKYCERR("regulator_get mvs0b_1p8v fail\n" );;
		return -ENODEV;
	}
#if 0	
	lvs2b_1p8v = regulator_get(NULL, "8901_lvs2");
	if (IS_ERR(lvs2b_1p8v)) {
		SKYCERR("regulator_get lvs2b_1p8v fail\n" );;
		return -ENODEV;
	}
#endif	
	l9a_2p8v = regulator_get(NULL, "8058_l9");
	if (IS_ERR(l9a_2p8v)) {
		SKYCERR("regulator_get l9a_2p8v fail\n" );;
		return -ENODEV;
	}
	rc = regulator_set_voltage(l9a_2p8v, 2800000, 2800000);
	if (rc) {
		SKYCERR("%s: unable to set l9a_2p8v voltage to 2.8V\n", __func__);
		goto fail;
	}
	
	l15a_1p5v = regulator_get(NULL, "8058_l15");
	if (IS_ERR(l15a_1p5v)) {
		SKYCERR("regulator_get l15a_1p5v fail\n" );;
		return -ENODEV;
	}	
	rc = regulator_set_voltage(l15a_1p5v, 1500000, 1500000);
	if (rc) {
		SKYCERR("%s: unable to set l15a_1p5v voltage to 1.5V\n", __func__);
		goto fail;
	}
	
	lvs1b_1p8v = regulator_get(NULL, "8901_lvs1");
	if (IS_ERR(lvs1b_1p8v)) {
		SKYCERR("regulator_get lvs1b_1p8v fail\n" );;
		return -ENODEV;
	}	
	
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
	return rc;
fail:
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
	if(l15a_1p5v) {
	regulator_put(l15a_1p5v);
	}
	if(l9a_2p8v) {
	regulator_put(l9a_2p8v);
	}
	return rc;	
}
#else
static int s5k6aafx13_vreg_init(void)
{
	int rc;
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);

	mvs0b_1p8v = regulator_get(NULL, "8901_mvs0");
	if (IS_ERR(mvs0b_1p8v)) {
		SKYCERR("regulator_get mvs0b_1p8v fail\n" );;
		return -ENODEV;
	}

	l19a_2p8v = regulator_get(NULL, "8058_l19");
	if (IS_ERR(l19a_2p8v)) {
		SKYCERR("regulator_get l19a_2p8v fail\n" );;
		return -ENODEV;
	}
	rc = regulator_set_voltage(l19a_2p8v, 2800000, 2800000);
	if (rc) {
		SKYCERR("%s: unable to set l19a_2p8v voltage to 2.8V\n", __func__);
		goto fail;
	}
	
	s2b_1p5v = regulator_get(NULL, "8901_s2");
	if (IS_ERR(s2b_1p5v)) {
		SKYCERR("regulator_get s2b_1p5v fail\n" );;
		return -ENODEV;
	}	
	rc = regulator_set_voltage(s2b_1p5v, 1500000, 1500000);
	if (rc) {
		SKYCERR("%s: unable to set s2b_1p5v voltage to 1.5V\n", __func__);
		goto fail;
	}	
#if 0
	mvs0b_1p8v = regulator_get(NULL, "8901_mvs0");
	if (IS_ERR(mvs0b_1p8v)) {
		SKYCERR("regulator_get mvs0b_1p8v fail\n" );;
		return -ENODEV;
	}	
#endif
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
	return rc;
fail:
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
	if(s2b_1p5v) {
	regulator_put(s2b_1p5v);
	}
	if(l19a_2p8v) {
	regulator_put(l19a_2p8v);
	}
	return rc;	
}
#endif

#if (BOARD_REV == PT10)
static int s5k6aafx13_power(int on)
{
	int rc = 0;
	SKYCDBG("%s %s:%d power = %d\n", __FILE__, __func__, __LINE__,on);
	if(on) {
		rc = regulator_enable(mvs0b_1p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator mvs0b_1p8v failed\n", __func__);
			goto fail;
		}
		mdelay(1);
#if 0		
		rc = regulator_enable(lvs2b_1p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator lvs2b_1p8v failed\n", __func__);
			goto fail;
		}
#endif		
		rc = regulator_enable(l9a_2p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator l9a_2p8v failed\n", __func__);
			goto fail;
		}
		mdelay(1);
		rc = regulator_enable(l15a_1p5v);
		if (rc) {
			SKYCERR("%s: Enable regulator l15a_1p5v failed\n", __func__);
			goto fail;
		}
		mdelay(1);
		rc = regulator_enable(lvs1b_1p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator lvs1b_1p8v failed\n", __func__);
			goto fail;
		}
		mdelay(1);
	}
	else {
		rc = regulator_disable(mvs0b_1p8v);
		if (rc)
			SKYCERR("%s: Disable regulator mvs0b_1p8v failed\n", __func__);
#if 0
		rc = regulator_disable(lvs2b_1p8v);
		if (rc)
			SKYCERR("%s: Disable regulator lvs2b_1p8v failed\n", __func__);
#endif
		rc = regulator_disable(lvs1b_1p8v);
		if (rc)
			SKYCERR("%s: Disable regulator lvs1b_1p8v failed\n", __func__);

		rc = regulator_disable(l15a_1p5v);
		if (rc)
			SKYCDBG("%s: Disable regulator l15a_1p5v failed\n", __func__);
		regulator_put(l15a_1p5v);
		
		rc = regulator_disable(l9a_2p8v);
		if (rc)
			SKYCERR("%s: Disable regulator l9a_2p8v failed\n", __func__);
		regulator_put(l9a_2p8v);		
		
	}
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
	return rc;
fail:
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
	if(l9a_2p8v)
		regulator_put(l9a_2p8v);
	if(l15a_1p5v)
		regulator_put(l15a_1p5v);
	return rc;			
}
#else

static int s5k6aafx13_power(int on)
{
	int rc = 0;
	SKYCDBG("%s:%d power = %d\n", __func__, __LINE__,on);
	if(on) {
		rc = regulator_enable(mvs0b_1p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator mvs0b_1p8v failed\n", __func__);
			goto fail;
		}
		mdelay(1);

		rc = regulator_enable(l19a_2p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator l19a_2p8v failed\n", __func__);
			goto fail;
		}
		mdelay(1);
		rc = regulator_enable(s2b_1p5v);
		if (rc) {
			SKYCERR("%s: Enable regulator s2b_1p5v failed\n", __func__);
			goto fail;
		}
		mdelay(1);
#if 0
		rc = regulator_enable(lvs1b_1p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator lvs1b_1p8v failed\n", __func__);
			goto fail;
		}
		msleep(1);
#endif
	}
	else {
		rc = regulator_disable(mvs0b_1p8v);
		if (rc)
			SKYCERR("%s: Disable regulator mvs0b_1p8v failed\n", __func__);
#if 0
		rc = regulator_disable(lvs1b_1p8v);
		if (rc)
			SKYCERR("%s: Disable regulator lvs1b_1p8v failed\n", __func__);
#endif
		rc = regulator_disable(s2b_1p5v);
		if (rc)
			SKYCDBG("%s: Disable regulator s2b_1p5v failed\n", __func__);
		regulator_put(s2b_1p5v);
		
		rc = regulator_disable(l19a_2p8v);
		if (rc)
			SKYCERR("%s: Disable regulator l19a_2p8v failed\n", __func__);
		regulator_put(l19a_2p8v);		
		
	}
	SKYCDBG("%s:%d\n", __func__, __LINE__);
	return rc;
fail:
	SKYCDBG("%s:%d\n", __func__, __LINE__);
	if(l19a_2p8v)
		regulator_put(l19a_2p8v);
	if(s2b_1p5v)
		regulator_put(s2b_1p5v);
	return rc;			
}

#endif
#endif

#if 0
static int s5k6aafx13_reset(const struct msm_camera_sensor_info *dev)
{
	int rc = 0;

	//rc = gpio_request(dev->sensor_reset, "s5k6aafx13");
	rc = gpio_tlmm_config(GPIO_CFG(SENSOR_RESET, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);

	if (!rc) {
		SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
		gpio_set_value(SENSOR_RESET,0);
		//rc = gpio_direction_output(137, 0);
		mdelay(20);
		gpio_set_value(SENSOR_RESET,1);
		//rc = gpio_direction_output(137, 1);
	}

	//standby
	rc = gpio_tlmm_config(GPIO_CFG(SENSOR_STANDBY, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);

	if (!rc) {
		SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
		gpio_set_value(SENSOR_STANDBY,1);
		//rc = gpio_direction_output(137, 0);
		mdelay(20);		
	}

	//gpio_free(dev->sensor_reset);
	return rc;
}
#else
static int s5k6aafx13_reset(int set)
{
	int rc = 0;

	//reset
	rc = gpio_tlmm_config(GPIO_CFG(SENSOR_RESET, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);

	if (!rc) {
		SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
		gpio_set_value(SENSOR_RESET,0);		
		if(set){
			mdelay(20);
			gpio_set_value(SENSOR_RESET,1);		
		}
	}
	else{
		SKYCERR("ce1612_reset gpio_tlmm_config Failed!\n");
		goto reset_fail;
	}

	//standby
	rc = gpio_tlmm_config(GPIO_CFG(SENSOR_STANDBY, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);

	if (!rc) {
		SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
		gpio_set_value(SENSOR_STANDBY,set);
		mdelay(20);		
	}
	

	SKYCDBG("%s %s Success:%d\n", __FILE__, __func__, __LINE__);
	return rc;

reset_fail:
	SKYCERR("%s %s Failed!:%d\n", __FILE__, __func__, __LINE__);
	return rc;
}
#endif


static int32_t s5k6aafx13_i2c_txdata(unsigned short saddr,
	unsigned char *txdata, int length)
{
	uint32_t i = 0;
	int32_t rc = 0;
	
	struct i2c_msg msg[] = {
		{
			.addr = saddr,
			.flags = 0,
			.len = length,
			.buf = txdata,
		},
	};

#if SENSOR_DEBUG
	if (length == 2)
		SKYCDBG("msm_io_i2c_w: 0x%04x 0x%04x\n",
			*(u16 *) txdata, *(u16 *) (txdata + 2));
	else if (length == 4)
		SKYCDBG("msm_io_i2c_w: 0x%04x\n", *(u16 *) txdata);
	else
		SKYCDBG("msm_io_i2c_w: length = %d\n", length);
#endif
	
	for (i = 0; i < S5K6AAFX13_I2C_RETRY; i++) {
		rc = i2c_transfer(s5k6aafx13_client->adapter, msg, 1); 
		if (rc >= 0) {			
			return 0;
		}
		SKYCDBG("%s: tx retry. [%02x.%02x.%02x] len=%d rc=%d\n", __func__,saddr, *txdata, *(txdata + 1), length, rc);
		msleep(S5K6AAFX13_I2C_MPERIOD);
	}
	return -EIO;	
}

#ifdef F_ICP_HD_STANDBY
static int32_t icp_hd_i2c_write_dw(unsigned short saddr,
	unsigned short waddr, unsigned short wdata1, unsigned short wdata2)
{
	int32_t rc = -EIO;
	unsigned char buf[6];

	memset(buf, 0, sizeof(buf));
	
	buf[0] = (waddr & 0xFF00)>>8;
	buf[1] = (waddr & 0x00FF);
	buf[2] = (wdata1 & 0xFF00)>>8;
	buf[3] = (wdata1 & 0x00FF);
	buf[4] = (wdata2 & 0xFF00)>>8;
	buf[5] = (wdata2 & 0x00FF);

	rc = s5k6aafx13_i2c_txdata(saddr, buf, 6);

	if (rc < 0)
		SKYCERR(
		"i2c_write failed, addr = 0x%x, val1 = 0x%x, val2 = 0x%x!\n",
		waddr, wdata1, wdata1);

	return rc;
}
#endif

static int32_t s5k6aafx13_i2c_write(unsigned short saddr,
	unsigned short waddr, unsigned short wdata, enum s5k6aafx13_width width)
{
	int32_t rc = -EIO;
	unsigned char buf[4];

	memset(buf, 0, sizeof(buf));
	switch (width) {
	case WORD_LEN: {
		buf[0] = (waddr & 0xFF00)>>8;
		buf[1] = (waddr & 0x00FF);
		buf[2] = (wdata & 0xFF00)>>8;
		buf[3] = (wdata & 0x00FF);

		rc = s5k6aafx13_i2c_txdata(saddr, buf, 4);
	}
		break;

	case TRIPLE_LEN: {
		buf[0] = (waddr & 0xFF00)>>8;
		buf[1] = (waddr & 0x00FF);
		buf[2] = wdata;
		rc = s5k6aafx13_i2c_txdata(saddr, buf, 3);
	}
		break;

	case BYTE_LEN: {
		buf[0] = waddr;
		buf[1] = wdata;
		rc = s5k6aafx13_i2c_txdata(saddr, buf, 2);
	}
		break;

	default:
		break;
	}

	if (rc < 0)
		SKYCERR(
		"i2c_write failed, addr = 0x%x, val = 0x%x!\n",
		waddr, wdata);

	return rc;
}

#if 0
static int32_t s5k6aafx13_i2c_write_a2d1(unsigned short waddr, unsigned char wdata)
{
	int32_t rc = -EIO;
	unsigned char buf[3];

	memset(buf, 0, sizeof(buf));

	buf[0] = (waddr & 0xFF00)>>8;
	buf[1] = (waddr & 0x00FF);
	buf[2] = wdata;
	
	rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, buf, 3);

	if (rc < 0)
		SKYCERR(
		"i2c_write failed, saddr= 0x%x, addr = 0x%x, val = 0x%x!\n",
		s5k6aafx13_client->addr, waddr, wdata);

	return rc;
}
#endif

static int32_t s5k6aafx13_i2c_write_table(
	struct s5k6aafx13_i2c_reg_conf const *reg_conf_tbl,
	int num_of_items_in_table)
{
#if 1
	int i;
	int32_t rc = -EIO;

//	uint16_t poll_delay = 0;
//       uint16_t poll_retry = 0;
//       uint16_t poll_mcu_var = 0;
//       uint16_t poll_data = 0;
//       uint16_t poll_mask = 0;
//       uint16_t retry_cnt = 0;
//	uint16_t read_data = 0;
	//OTP 방어코드 추가
//	uint16_t otp_retry_cnt = 0;
//	uint16_t otp_poll_retry = 20;

	for (i = 0; i < num_of_items_in_table; i++) 
	{		
	switch(reg_conf_tbl->width )
	{
		case ZERO_LEN:
		{
			SKYCDBG("ZERO_LEN continue ADDR = 0x%x, VALUE = 0x%x\n",reg_conf_tbl->waddr, reg_conf_tbl->wdata);
			reg_conf_tbl++;		
			rc = 0;
			continue;
		}
#ifdef BURST_MODE_INIT
		case BURST_1:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_1, s5k6aafx13_regs.init_burst_settings_size_1);
			if (rc < 0) {
				SKYCERR("BURST_1 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_2:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_2, s5k6aafx13_regs.init_burst_settings_size_2);
			if (rc < 0) {
				SKYCERR("BURST_2 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_3:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_3, s5k6aafx13_regs.init_burst_settings_size_3);
			if (rc < 0) {
				SKYCERR("BURST_3 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_4:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_4, s5k6aafx13_regs.init_burst_settings_size_4);
			if (rc < 0) {
				SKYCERR("BURST_4 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_5:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_5, s5k6aafx13_regs.init_burst_settings_size_5);
			if (rc < 0) {
				SKYCERR("BURST_5 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_6:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_6, s5k6aafx13_regs.init_burst_settings_size_6);
			if (rc < 0) {
				SKYCERR("BURST_6 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_7:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_7, s5k6aafx13_regs.init_burst_settings_size_7);
			if (rc < 0) {
				SKYCERR("BURST_7 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_8:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_8, s5k6aafx13_regs.init_burst_settings_size_8);
			if (rc < 0) {
				SKYCERR("BURST_8 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_9:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_9, s5k6aafx13_regs.init_burst_settings_size_9);
			if (rc < 0) {
				SKYCERR("BURST_9 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_10:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_10, s5k6aafx13_regs.init_burst_settings_size_10);
			if (rc < 0) {
				SKYCERR("BURST_10 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;

		case BURST_11:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_11, s5k6aafx13_regs.init_burst_settings_size_11);
			if (rc < 0) {
				SKYCERR("BURST_11 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_12:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_12, s5k6aafx13_regs.init_burst_settings_size_12);
			if (rc < 0) {
				SKYCERR("BURST_12 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_13:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_13, s5k6aafx13_regs.init_burst_settings_size_13);
			if (rc < 0) {
				SKYCERR("BURST_13 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_14:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_14, s5k6aafx13_regs.init_burst_settings_size_14);
			if (rc < 0) {
				SKYCERR("BURST_14 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_15:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_15, s5k6aafx13_regs.init_burst_settings_size_15);
			if (rc < 0) {
				SKYCERR("BURST_15 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_16:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_16, s5k6aafx13_regs.init_burst_settings_size_16);
			if (rc < 0) {
				SKYCERR("BURST_16 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_17:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_17, s5k6aafx13_regs.init_burst_settings_size_17);
			if (rc < 0) {
				SKYCERR("BURST_17 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_18:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_18, s5k6aafx13_regs.init_burst_settings_size_18);
			if (rc < 0) {
				SKYCERR("BURST_18 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_19:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_19, s5k6aafx13_regs.init_burst_settings_size_19);
			if (rc < 0) {
				SKYCERR("BURST_19 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_20:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_20, s5k6aafx13_regs.init_burst_settings_size_20);
			if (rc < 0) {
				SKYCERR("BURST_20 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;

		case BURST_21:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_21, s5k6aafx13_regs.init_burst_settings_size_21);
			if (rc < 0) {
				SKYCERR("BURST_21 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_22:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_22, s5k6aafx13_regs.init_burst_settings_size_22);
			if (rc < 0) {
				SKYCERR("BURST_22 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_23:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_23, s5k6aafx13_regs.init_burst_settings_size_23);
			if (rc < 0) {
				SKYCERR("BURST_23 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_24:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_24, s5k6aafx13_regs.init_burst_settings_size_24);
			if (rc < 0) {
				SKYCERR("BURST_24 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_25:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_25, s5k6aafx13_regs.init_burst_settings_size_25);
			if (rc < 0) {
				SKYCERR("BURST_25 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_26:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_26, s5k6aafx13_regs.init_burst_settings_size_26);
			if (rc < 0) {
				SKYCERR("BURST_26 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_27:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_27, s5k6aafx13_regs.init_burst_settings_size_27);
			if (rc < 0) {
				SKYCERR("BURST_27 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_28:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_28, s5k6aafx13_regs.init_burst_settings_size_28);
			if (rc < 0) {
				SKYCERR("BURST_28 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_29:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_29, s5k6aafx13_regs.init_burst_settings_size_29);
			if (rc < 0) {
				SKYCERR("BURST_29 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_30:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_30, s5k6aafx13_regs.init_burst_settings_size_30);
			if (rc < 0) {
				SKYCERR("BURST_30 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;

		case BURST_31:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_31, s5k6aafx13_regs.init_burst_settings_size_31);
			if (rc < 0) {
				SKYCERR("BURST_31 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_32:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_32, s5k6aafx13_regs.init_burst_settings_size_32);
			if (rc < 0) {
				SKYCERR("BURST_32 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_33:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_33, s5k6aafx13_regs.init_burst_settings_size_33);
			if (rc < 0) {
				SKYCERR("BURST_33 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_34:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_34, s5k6aafx13_regs.init_burst_settings_size_34);
			if (rc < 0) {
				SKYCERR("BURST_34 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_35:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_35, s5k6aafx13_regs.init_burst_settings_size_35);
			if (rc < 0) {
				SKYCERR("BURST_35 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_36:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_36, s5k6aafx13_regs.init_burst_settings_size_36);
			if (rc < 0) {
				SKYCERR("BURST_36 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_37:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_37, s5k6aafx13_regs.init_burst_settings_size_37);
			if (rc < 0) {
				SKYCERR("BURST_37 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
		case BURST_38:
			rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
				s5k6aafx13_regs.init_burst_settings_38, s5k6aafx13_regs.init_burst_settings_size_38);
			if (rc < 0) {
				SKYCERR("BURST_38 failed!\n");
				break;
			}
			reg_conf_tbl++;			
			break;
    case BURST_39:
      rc = s5k6aafx13_i2c_txdata(s5k6aafx13_client->addr, 
        s5k6aafx13_regs.init_burst_settings_39, s5k6aafx13_regs.init_burst_settings_size_39);
      if (rc < 0) {
        SKYCERR("BURST_39 failed!\n");
        break;
      }
      reg_conf_tbl++;         
      break;
#endif				
		default:
		{
			rc = s5k6aafx13_i2c_write(s5k6aafx13_client->addr,
								reg_conf_tbl->waddr, reg_conf_tbl->wdata,
								reg_conf_tbl->width);
			//SKYCDBG("I2C WRITE!!! ADDR = 0x%x, VALUE = 0x%x, width = %d, num_of_items_in_table=%d, i=%d\n",
			//	reg_conf_tbl->waddr, reg_conf_tbl->wdata, reg_conf_tbl->width, num_of_items_in_table, i);

			if (rc < 0)
			{
				SKYCERR("s5k6aafx13_i2c_read failed!\n");
				return rc;
			}
			
			if (reg_conf_tbl->mdelay_time != 0)
				mdelay(reg_conf_tbl->mdelay_time);

			reg_conf_tbl++;

			break;
		}			
	}	
	if (rc < 0)
	{
		SKYCERR("s5k6aafx13_i2c_write_table failed!\n");
		break;
	}
	}

	return rc;
#else
	int i;
	int32_t rc = -EIO;

	for (i = 0; i < num_of_items_in_table; i++) {
		rc = s5k6aafx13_i2c_write(s5k6aafx13_client->addr,
			reg_conf_tbl->waddr, reg_conf_tbl->wdata,
			reg_conf_tbl->width);
		if (rc < 0)
			break;
		if (reg_conf_tbl->mdelay_time != 0)
			mdelay(reg_conf_tbl->mdelay_time);
		reg_conf_tbl++;
	}
#endif
	return rc;
}

#if 0
static int s5k6aafx13_i2c_rxdata(unsigned short saddr,
	unsigned char *rxdata, int length)
{
	uint32_t i = 0;
	int32_t rc = 0;
	
	struct i2c_msg msgs[] = {
	{
		.addr   = saddr,
		.flags = 0,
		.len   = 2,
		.buf   = rxdata,
	},
	{
		.addr   = saddr,
		.flags = I2C_M_RD,
		.len   = length,
		.buf   = rxdata,
	},
	};

#if SENSOR_DEBUG
	if (length == 2)
		SKYCDBG("msm_io_i2c_r: 0x%04x 0x%04x\n",
			*(u16 *) rxdata, *(u16 *) (rxdata + 2));
	else if (length == 4)
		SKYCDBG("msm_io_i2c_r: 0x%04x\n", *(u16 *) rxdata);
	else
		SKYCDBG("msm_io_i2c_r: length = %d\n", length);
#endif

	for (i = 0; i < S5K6AAFX13_I2C_RETRY; i++) {
		rc = i2c_transfer(s5k6aafx13_client->adapter, msgs, 2); 
		if (rc >= 0) {			
			return 0;
		}
		SKYCDBG("%s: tx retry. [%02x.%02x.%02x] len=%d rc=%d\n", __func__,saddr, *rxdata, *(rxdata + 1), length, rc);
		msleep(S5K6AAFX13_I2C_MPERIOD);
	}
	return -EIO;
}
#endif

#if 0
static int32_t s5k6aafx13_i2c_read(unsigned short   saddr,
	unsigned short raddr, unsigned short *rdata, enum s5k6aafx13_width width)
{
	int32_t rc = 0;
	unsigned char buf[4];

	if (!rdata)
		return -EIO;

	memset(buf, 0, sizeof(buf));

	switch (width) {
	case WORD_LEN: {
		buf[0] = (raddr & 0xFF00)>>8;
		buf[1] = (raddr & 0x00FF);

		rc = s5k6aafx13_i2c_rxdata(saddr, buf, 2);
		if (rc < 0)
			return rc;

		*rdata = buf[0] << 8 | buf[1];
	}
		break;

	default:
		break;
	}

	if (rc < 0)
		SKYCERR("s5k6aafx13_i2c_read failed!\n");

	return rc;
}
#endif

#if 0
static int32_t s5k6aafx13_set_lens_roll_off(void)
{
	int32_t rc = 0;
#if 0
	rc = s5k6aafx13_i2c_write_table(&s5k6aafx13_regs.rftbl[0],
								 s5k6aafx13_regs.rftbl_size);
#endif
	return rc;
}
#endif

static long s5k6aafx13_reg_init(void)
{
//	int32_t array_length;
//	int32_t i;
	int32_t rc;

	/* PLL Setup Start */
	rc = s5k6aafx13_i2c_write_table(&s5k6aafx13_regs.init_parm[0],
					s5k6aafx13_regs.init_parm_size);

	if (rc < 0)
		return rc;
	/* PLL Setup End   */

	return 0;
}

static long s5k6aafx13_set_effect(int mode, int8_t effect)
{
//	uint16_t reg_addr;
//	uint16_t reg_val;
	int32_t rc = 0;

	SKYCDBG("%s start\n",__func__);

	if(effect < CAMERA_EFFECT_OFF || effect >= CAMERA_EFFECT_MAX){
		SKYCERR("%s error. effect=%d\n", __func__, effect);
		return 0;//-EINVAL;
	}

	rc = s5k6aafx13_i2c_write_table(s5k6aafx13_regs.effect_cfg_settings[effect],
					s5k6aafx13_regs.effect_cfg_settings_size);
	if (rc < 0)
	{
		SKYCDBG("CAMERA_WB I2C FAIL!!! return~~\n");
		return rc;
	}	
	SKYCDBG("%s end\n",__func__);
	return rc;
}

static int32_t s5k6aafx13_set_brightness(int8_t brightness)
{
	int32_t rc = 0;
//	int i = 0;
	SKYCDBG("%s start~ receive brightness = %d\n",__func__, brightness);

	if ((brightness < C_PANTECH_CAMERA_MIN_BRIGHTNESS) || (brightness > C_PANTECH_CAMERA_MAX_BRIGHTNESS)) {
		SKYCERR("%s error. brightness=%d\n", __func__, brightness);
		return 0;//-EINVAL;
	}

	rc = s5k6aafx13_i2c_write_table(s5k6aafx13_regs.bright_cfg_settings[brightness],	
					s5k6aafx13_regs.bright_cfg_settings_size);
	if (rc < 0)
	{
		SKYCDBG("CAMERA_BRIGHTNESS I2C FAIL!!! return~~\n");
		return rc;
	}
	SKYCDBG("%s end\n",__func__);
	return rc;
}

static int32_t s5k6aafx13_set_whitebalance (int32_t whitebalance)
{
	
	int32_t rc = 0;
//	int8_t m_wb = 0;
		
	SKYCDBG("%s start  whitebalance=%d\n",__func__, whitebalance);

	rc = s5k6aafx13_i2c_write_table(s5k6aafx13_regs.wb_cfg_settings[whitebalance-1],
					s5k6aafx13_regs.wb_cfg_settings_size);
	if (rc < 0)
	{
		SKYCDBG("CAMERA_WB I2C FAIL!!! return~~\n");
		return rc;
	}		

	SKYCDBG("%s end\n",__func__);
	return rc;
}

static int32_t s5k6aafx13_set_exposure_mode(int32_t exposure)
{
	int32_t rc = 0;

	SKYCDBG("%s  exposure = %d\n",__func__, exposure);

	if ((exposure < 0) || (exposure >= 4))
	{
		SKYCERR("%s FAIL!!! return~~  exposure = %d\n",__func__,exposure);
		return 0;//-EINVAL;
	}

	rc = s5k6aafx13_i2c_write_table(s5k6aafx13_regs.exposure_cfg_settings[exposure],
					s5k6aafx13_regs.exposure_cfg_settings_size);
	if (rc < 0)
	{
		SKYCDBG("CAMERA_EFFECT_SEPIA I2C FAIL!!! return~~\n");
		return rc;
	}		
	
	SKYCDBG("%s end\n",__func__);

	return rc;
}

static int32_t s5k6aafx13_set_preview_fps(int32_t preview_fps)
{
	/* 0 : variable 30fps, 1 ~ 30 : fixed fps */
	/* default: variable 7 ~ 30fps */
	int32_t rc = 0;	

	if ((preview_fps < C_PANTECH_CAMERA_MIN_PREVIEW_FPS) || 
		(preview_fps > C_PANTECH_CAMERA_MAX_PREVIEW_FPS)) {
		SKYCDBG("%s: -EINVAL, preview_fps=%d\n", 
			__func__, preview_fps);
		return 0;//-EINVAL;
	}

	SKYCDBG("%s: preview_fps=%d\n", __func__, preview_fps);

	rc = s5k6aafx13_i2c_write_table(s5k6aafx13_regs.preview_fps_cfg_settings[preview_fps - C_PANTECH_CAMERA_MIN_PREVIEW_FPS],
					s5k6aafx13_regs.preview_fps_cfg_settings_size);

	SKYCDBG("%s end rc = %d\n",__func__, rc);

	return rc;
}

static int32_t s5k6aafx13_set_reflect(int32_t reflect)
{
	int32_t rc = 0;
//	int32_t i = 0;
//	int8_t npolling = -1;

	SKYCDBG("%s  reflect = %d\n",__func__, reflect);

	if ((reflect < 0) || (reflect >= 4))
	{
		SKYCDBG("%s FAIL!!! return~~  reflect = %d\n",__func__,reflect);
		return 0;//-EINVAL;
	}
	rc = s5k6aafx13_i2c_write_table(s5k6aafx13_regs.reflect_cfg_settings[reflect],
				s5k6aafx13_regs.reflect_cfg_settings_size);
	if (rc < 0)
	{
		SKYCERR("CAMERA_SET_REFLECT I2C FAIL!!! return~~\n");
		return rc;
	}		
	
	SKYCDBG("%s end\n",__func__);

	return rc;
}

static int32_t s5k6aafx13_video_config(void)
{
	int32_t rc = 0;
	
	/* set preview resolution to 1280x960 */
	SKYCDBG("%s start\n",__func__);

	rc = s5k6aafx13_i2c_write_table(&s5k6aafx13_regs.preview_cfg_settings[0],
						s5k6aafx13_regs.preview_cfg_settings_size);

	if (rc < 0)
	{
		SKYCERR("s5k6aafx13_i2c_write_table FAIL!!! return~~\n");
		return rc;
	}
	SKYCDBG("%s end rc = %d\n",__func__, rc);

	return rc;
}

static int32_t s5k6aafx13_snapshot_config(void)
{
	int32_t rc = 0;

	/* set snapshot resolution to 1280x960 */
	SKYCDBG("%s start\n",__func__);
	rc = s5k6aafx13_i2c_write_table(&s5k6aafx13_regs.snapshot_cfg_settings[0],
					s5k6aafx13_regs.snapshot_cfg_settings_size);
	if (rc < 0)
	{
		SKYCERR("s5k6aafx13_i2c_write_table FAIL!!! return~~\n");
		return rc;
	}
	SKYCDBG("%s end rc = %d\n",__func__, rc);
	
	return rc;
}


static long s5k6aafx13_set_sensor_mode(int mode)
{
//	uint16_t clock;
	int32_t rc = 0;
       struct msm_camera_csi_params s5k6aafx13_csi_params;
	   
	SKYCDBG("s5k6aafx13_set_sensor_mode mode = %d!\n",mode);

	switch (mode) {
	case SENSOR_PREVIEW_MODE:
		b_snapshot_flag = false;
#if 0		
		s5k6aafx13_i2c_write_table(&s5k6aafx13_regs.arm_go_cfg_settings[0],
						s5k6aafx13_regs.arm_go_cfg_settings_size);
		s5k6aafx13_i2c_write_table(&s5k6aafx13_regs.standby_enter_cfg_settings[0],
						s5k6aafx13_regs.standby_enter_cfg_settings_size);
		gpio_set_value(SENSOR_STANDBY,0);
		mdelay(20);		
#endif		
#if 1		
		if(config_csi == 0)
		{
			SKYCDBG("s5k6aafx13_set_sensor_mode config_csi E\n");
			s5k6aafx13_csi_params.lane_cnt = 1; // 4;
			s5k6aafx13_csi_params.data_format = CSI_8BIT; //CSI_10BIT;
			s5k6aafx13_csi_params.lane_assign = 0xe4;
			s5k6aafx13_csi_params.dpcm_scheme = 0;
			s5k6aafx13_csi_params.settle_cnt = 0x14;
			rc = msm_camio_csi_config(&s5k6aafx13_csi_params);
			msleep(s5k6aafx13_delay_msecs_stream);
			config_csi = 1;
			SKYCDBG("s5k6aafx13_set_sensor_mode config_csi X\n");
		}
		else {
			rc = s5k6aafx13_video_config();
			SKYCDBG("s5k6aafx13_video_config, rc = %d \n", rc);
			if (rc < 0)
			{
				SKYCERR("s5k6aafx13_video_config FAIL!!! return~~\n");
				return rc;
			}
		}
#endif		
		break;
		
	case SENSOR_SNAPSHOT_MODE:
		if(!b_snapshot_flag) {
			rc = s5k6aafx13_snapshot_config();
			b_snapshot_flag = true;
			SKYCDBG("s5k6aafx13_snapshot_config, rc = %d \n", rc);
			if (rc < 0)
	              {
				SKYCERR("s5k6aafx13_snapshot_config FAIL!!! return~~\n");
				return rc;
			}			
		}
		break;

	case SENSOR_RAW_SNAPSHOT_MODE:
		
		break;

	default:
		return -EINVAL;
	}
	SKYCDBG("s5k6aafx13_set_sensor_mode(%d) X!\n",mode);
	
	return rc;
}

static int s5k6aafx13_sensor_init_probe(const struct msm_camera_sensor_info *data)
{
//	uint16_t model_id = 0;
	int rc = 0;

	SKYCDBG("init entry \n");
	rc = s5k6aafx13_reset(ON);
	if (rc < 0) {
		SKYCERR("reset failed!\n");
		goto init_probe_fail;
	}

	mdelay(5);
#if 1
	SKYCDBG("s5k6aafx13_reg_init E \n");
	rc = s5k6aafx13_reg_init();
	SKYCDBG("s5k6aafx13_reg_init X \n");
	if (rc < 0)
		goto init_probe_fail;
#endif
	return rc;

init_probe_fail:
	s5k6aafx13_reset(OFF);
	return rc;
}

int s5k6aafx13_sensor_init(const struct msm_camera_sensor_info *data)
{
	int rc = 0;

	config_csi = 0;
	b_snapshot_flag = false;
	SKYCDBG(" s5k6aafx13_sensor_init E\n");
#ifdef F_ICP_HD_STANDBY
	icp_hd_vreg_init();
	rc = icp_hd_power(ON);
	if (rc) {		
		SKYCERR(" icp_hd_power failed rc=%d\n",rc);
		goto init_fail; 
	}
	msm_camio_clk_rate_set(24000000);
	icp_hd_reset(ON);
	mdelay(5);
	
	icp_hd_i2c_write_dw(ICP_SLAVE_ADDRESS, 0xf03c,  0x0009, 0x5000);
	icp_hd_i2c_write_dw(ICP_SLAVE_ADDRESS, 0xe000,  0x0000, 0x0000);
	icp_hd_i2c_write_dw(ICP_SLAVE_ADDRESS, 0xe004,  0x0000, 0x8fff);
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
	gpio_set_value(SENSOR_STANDBY_8M,1);					
	
	if(s2b_1p2v_8m) {
		rc = regulator_disable(s2b_1p2v_8m);
		if (rc){
			SKYCERR("%s: Disable regulator s2b_1p2v failed\n", __func__);
			goto init_fail;
		}
		regulator_put(s2b_1p2v_8m);
	}
#endif	

#ifdef F_S5K6AAFX13_POWER	
	s5k6aafx13_vreg_init();
	rc = s5k6aafx13_power(ON);
	if (rc) {		
		SKYCERR(" s5k6aafx13_power failed rc=%d\n",rc);
		goto init_fail; 
	}
#endif
	s5k6aafx13_ctrl = kzalloc(sizeof(struct s5k6aafx13_ctrl_t), GFP_KERNEL);
	if (!s5k6aafx13_ctrl) {
		SKYCERR("s5k6aafx13_init failed!\n");
		rc = -ENOMEM;
		goto init_done;
	}

	if (data)
		s5k6aafx13_ctrl->sensordata = data;

	/* Input MCLK = 24MHz */
	SKYCDBG(" msm_camio_clk_rate_set E\n");
#ifndef F_ICP_HD_STANDBY	
	msm_camio_clk_rate_set(24000000);
#endif
	SKYCDBG(" msm_camio_clk_rate_set X\n");
	mdelay(5);

	rc = s5k6aafx13_sensor_init_probe(data);
	if (rc < 0) {
		SKYCERR("s5k6aafx13_sensor_init failed!\n");
		goto init_fail;
	}

#if 0
	rc = gpio_tlmm_config(GPIO_CFG(SENSOR_STANDBY, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);

	if (!rc) {
		SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
		gpio_set_value(139,1);
		mdelay(20);
        }
#endif
init_done:
	return rc;

init_fail:
	s5k6aafx13_power(OFF);
	
	kfree(s5k6aafx13_ctrl);
	return rc;
}

static int s5k6aafx13_init_client(struct i2c_client *client)
{
	/* Initialize the MSM_CAMI2C Chip */
	init_waitqueue_head(&s5k6aafx13_wait_queue);
	return 0;
}

int s5k6aafx13_sensor_config(void __user *argp)
{
	struct sensor_cfg_data cfg_data;
	int rc = 0;

	if (copy_from_user(&cfg_data,
			(void *)argp,
			sizeof(struct sensor_cfg_data)))
		return -EFAULT;

	/* down(&s5k6aafx13_sem); */

	SKYCDBG("s5k6aafx13_sensor_config, cfgtype = %d, mode = %d\n",
		cfg_data.cfgtype, cfg_data.mode);

		switch (cfg_data.cfgtype) {
		case CFG_SET_MODE:
			rc = s5k6aafx13_set_sensor_mode(
						cfg_data.mode);
			break;
		case CFG_SET_EFFECT:
			rc = s5k6aafx13_set_effect(cfg_data.mode,
						cfg_data.cfg.effect);
			SKYCDBG("s5k6aafx13_set_effect OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.effect =[%d]\n", rc, cfg_data.mode, cfg_data.cfg.effect);			
			break;
		case CFG_SET_BRIGHTNESS:
			rc = s5k6aafx13_set_brightness(cfg_data.cfg.brightness);
			SKYCDBG("s5k6aafx13_set_brightness OK! rc = [%d], cfg_data.cfg.brightness =[%d]\n", rc, cfg_data.cfg.brightness);
			break;
		case CFG_SET_WB:			
			rc = s5k6aafx13_set_whitebalance(cfg_data.cfg.whitebalance);
			SKYCDBG("s5k6aafx13_set_whitebalance OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.whitebalance =[%d]\n", rc, cfg_data.mode, cfg_data.cfg.whitebalance);
			break;
		case CFG_SET_EXPOSURE_MODE:			
			rc = s5k6aafx13_set_exposure_mode(cfg_data.cfg.exposure);
			SKYCDBG("s5k6aafx13_set_exposure_mode OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.exposure =[%d]\n", rc, cfg_data.mode, cfg_data.cfg.exposure);
			break;
		case CFG_SET_PREVIEW_FPS:			
			rc = s5k6aafx13_set_preview_fps(cfg_data.cfg.preview_fps);
			SKYCDBG("s5k6aafx13_set_frame_rate OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.preview_fps =[%d]\n", rc, cfg_data.mode, cfg_data.cfg.preview_fps);
			break;
		case CFG_SET_REFLECT:			
			rc = s5k6aafx13_set_reflect(cfg_data.cfg.reflect);
			SKYCDBG("s5k6aafx13_set_reflect OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.reflect =[%d]\n", rc, cfg_data.mode, cfg_data.cfg.reflect);
			break;
		case CFG_GET_AF_MAX_STEPS:
		default:
			rc = -EINVAL;
			break;
		}

	/* up(&s5k6aafx13_sem); */

	return rc;
}

int s5k6aafx13_sensor_release(void)
{
	int rc = 0;

	/* down(&s5k6aafx13_sem); */
#ifdef F_S5K6AAFX13_POWER	
	SKYCERR(" s5k6aafx13_sensor_release E\n");	
	rc = s5k6aafx13_reset(OFF);
	rc = s5k6aafx13_power(OFF);
	if (rc) {
		SKYCERR(" s5k6aafx13_power failed rc=%d\n",rc);		
	}
#endif	
#ifdef F_ICP_HD_STANDBY
	gpio_set_value_cansleep(SENSOR_STANDBY_8M,0);	
	icp_hd_reset(OFF);
	rc  = icp_hd_power(OFF);
	if (rc) {
		SKYCERR(" icp_hd_power failed rc=%d\n",rc);		
	}
#endif

	kfree(s5k6aafx13_ctrl);
	/* up(&s5k6aafx13_sem); */

	return rc;
}

static int s5k6aafx13_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int rc = 0;
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		rc = -ENOTSUPP;
		goto probe_failure;
	}
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
	s5k6aafx13_sensorw =
		kzalloc(sizeof(struct s5k6aafx13_work), GFP_KERNEL);
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
	if (!s5k6aafx13_sensorw) {
		rc = -ENOMEM;
		goto probe_failure;
	}
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
	i2c_set_clientdata(client, s5k6aafx13_sensorw);
	s5k6aafx13_init_client(client);
	s5k6aafx13_client = client;

	SKYCDBG("s5k6aafx13_probe succeeded!\n");

	return 0;

probe_failure:
	kfree(s5k6aafx13_sensorw);
	s5k6aafx13_sensorw = NULL;
	SKYCERR("s5k6aafx13_probe failed!\n");
	return rc;
}

static const struct i2c_device_id s5k6aafx13_i2c_id[] = {
	{ "s5k6aafx13", 0},
	{ },
};

static struct i2c_driver s5k6aafx13_i2c_driver = {
	.id_table = s5k6aafx13_i2c_id,
	.probe  = s5k6aafx13_i2c_probe,
	.remove = __exit_p(s5k6aafx13_i2c_remove),
	.driver = {
		.name = "s5k6aafx13",
	},
};

static int32_t s5k6aafx13_init_i2c(void)
{
	int32_t rc = 0;

	SKYCDBG("%s start\n",__func__);

	rc = i2c_add_driver(&s5k6aafx13_i2c_driver);
	SKYCDBG("%s s5k6aafx13_i2c_driver rc = %d\n",__func__, rc);
	if (IS_ERR_VALUE(rc))
		goto init_i2c_fail;

	SKYCDBG("%s end\n",__func__);
	return 0;

init_i2c_fail:
	SKYCERR("%s failed! (%d)\n", __func__, rc);
	return rc;
}

static int s5k6aafx13_sensor_probe(const struct msm_camera_sensor_info *info,
				struct msm_sensor_ctrl *s)
{
	int rc = 0;
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);

	rc = s5k6aafx13_init_i2c();	
	if (rc < 0 || s5k6aafx13_client == NULL)
	{
		SKYCERR("%s rc = %d, s5k6aafx13_client = %x\n",__func__, rc, (unsigned int)s5k6aafx13_client);
		goto probe_done;
	}
#if 0	
	int rc = i2c_add_driver(&s5k6aafx13_i2c_driver);
	if (rc < 0 || s5k6aafx13_client == NULL) {
		SKYCERR("%s %s:%d\n", __FILE__, __func__, __LINE__);
		rc = -ENOTSUPP;
		goto probe_done;
	}
#endif
#if 0
	/* Input MCLK = 24MHz */
	msm_camio_clk_rate_set(24000000);
	mdelay(5);

	rc = s5k6aafx13_sensor_init_probe(info);
	if (rc < 0)
		goto probe_done;
#endif	
	//s5k6aafx13_vreg_init();

	s->s_init = s5k6aafx13_sensor_init;
	s->s_release = s5k6aafx13_sensor_release;
	s->s_config  = s5k6aafx13_sensor_config;
	s->s_camera_type = FRONT_CAMERA_2D;
	s->s_mount_angle = 270;//180;//90;

probe_done:
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
	return rc;
}

static int __s5k6aafx13_probe(struct platform_device *pdev)
{
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
	return msm_camera_drv_start(pdev, s5k6aafx13_sensor_probe);
}

static struct platform_driver msm_camera_driver = {
	.probe = __s5k6aafx13_probe,
	.driver = {
		.name = "msm_camera_s5k6aafx13",
		.owner = THIS_MODULE,
	},
};

static int __init s5k6aafx13_init(void)
{
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
	return platform_driver_register(&msm_camera_driver);
}

module_init(s5k6aafx13_init);
