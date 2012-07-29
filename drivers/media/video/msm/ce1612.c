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
#include "ce1612.h"
#include <linux/regulator/consumer.h>
#include <linux/regulator/machine.h>

//#include <mach/vreg.h>
//#include "camsensor_ce1612_tuneup.h"

//#define F_PANTECH_CAMERA_I2C_LOG

//#define AF_STATE_POLLING

#ifdef CONFIG_PANTECH_CAMERA//IRQ
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#endif

#define F_FW_UPDATE
#if 1
#define CE1612_UPLOADER_INFO		"CE161F00_fw1716prm1716_20111011.h"
#define CE1612_UPLOADER_BIN		"CE161F01_fw1716prm1716_20111011.h"
#define CE1612_FW_INFO				"CE161F02_fw1716prm1716_20111011.h"
#define CE1612_FW_BIN_0				"CE161F03_0_fw1716prm1716_20111011.h"
#define CE1612_FW_BIN_1				"CE161F03_1_fw1716prm1716_20111011.h"
#define CE1612_FW_BIN_2				"CE161F03_2_fw1716prm1716_20111011.h"
#define CE1612_FW_BIN_3				"CE161F03_3_fw1716prm1716_20111011.h"
#define CE1612_FW_BIN_4				"CE161F03_4_fw1716prm1716_20111011.h"
#define CE1612_FW_BIN_5				"CE161F03_5_fw1716prm1716_20111011.h"
#define CE1612_FW_BIN_6				"CE161F03_6_fw1716prm1716_20111011.h"
#define CE1612_FW_BIN_7				"CE161F03_7_fw1716prm1716_20111011.h"
#define CE1612_FW_BIN_8				"CE161F03_8_fw1716prm1716_20111011.h"
#define CE1612_FW_BIN_9				"CE161F03_9_fw1716prm1716_20111011.h"
#else
#define CE1612_UPLOADER_INFO		"CE161F00_fw1704prm0401_20110620.h"
#define CE1612_UPLOADER_BIN		"CE161F01_fw1704prm0401_20110620.h"
#define CE1612_FW_INFO				"CE161F02_fw1704prm0401_20110620.h"
//#define CE1612_FW_BIN				"CE161F03_fw1704prm0401_20110620.h"
#define CE1612_FW_BIN_0				"CE161F03_0_fw1704prm0401_20110620.h"
#define CE1612_FW_BIN_1				"CE161F03_1_fw1704prm0401_20110620.h"
#define CE1612_FW_BIN_2				"CE161F03_2_fw1704prm0401_20110620.h"
#define CE1612_FW_BIN_3				"CE161F03_3_fw1704prm0401_20110620.h"
#define CE1612_FW_BIN_4				"CE161F03_4_fw1704prm0401_20110620.h"
#define CE1612_FW_BIN_5				"CE161F03_5_fw1704prm0401_20110620.h"
#define CE1612_FW_BIN_6				"CE161F03_6_fw1704prm0401_20110620.h"
#define CE1612_FW_BIN_7				"CE161F03_7_fw1704prm0401_20110620.h"
#define CE1612_FW_BIN_8				"CE161F03_8_fw1704prm0401_20110620.h"
//#define CE1612_FW_BIN_9				"CE161F03_9_fw1703prm0301_20110608.h"
//#define CE1612_FW_BIN_10				"CE161F03_10_fw1703prm0301_20110608.h"
#endif

#define CE1612_FW_MAJOR_VER	17
#define CE1612_FW_MINOR_VER	16
#define CE1612_PRM_MAJOR_VER	17
#define CE1612_PRM_MINOR_VER	16

struct ce1612_ver_t {
	uint8_t fw_major_ver;
	uint8_t fw_minor_ver;
	uint8_t prm_major_ver;
	uint8_t prm_minor_ver;
	uint8_t pf_ver;
};

static struct ce1612_ver_t ce1612_ver = {0, 0};

#define C_PANTECH_CAMERA_MIN_PREVIEW_FPS	5
#define C_PANTECH_CAMERA_MAX_PREVIEW_FPS	31

#define F_CE1612_POWER
#if (BOARD_REV >= WS10)
//#define F_L14A_OFF_IN_PROBER
#endif

/* Micron CE1612 Registers and their values */
#define SENSOR_DEBUG 0

#define ON  1
#define OFF 0

#define SENSOR_RESET 106
#define SENSOR_STANDBY 46
#define SENSOR_1P1V_LOWQ 124
#define VDD_CORE_EN 15
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_LED_MODE
#define CE1612_LED_FLASH_ENABLE_GPIO 31
#endif

#define ce1612_delay_msecs_stream 100 //200//500
#define CE1612_I2C_RETRY	10
#define CE1612_I2C_MPERIOD	200
#define CE1612_SNAPSHOT_RETRY 	200//30
#define CE1612_PREVIEW_RETRY 	30
#define CE1612_POLLING_RETRY	 	30

struct ce1612_work {
	struct work_struct work;
};

static struct  ce1612_work *ce1612_sensorw;
static struct  i2c_client *ce1612_client;

struct ce1612_ctrl_t {
	const struct msm_camera_sensor_info *sensordata;
};


static struct ce1612_ctrl_t *ce1612_ctrl = NULL;

#if 1 //def F_PANTECH_CAMERA_ADD_CFG_DIMENSION
#if 1 //def F_PANTECH_CAMERA_1080P_PREVIEW
static struct dimension_cfg ce1612_dimension;
static int32_t ce1612_config_1080P_preview;
#endif
#endif

#if defined(CONFIG_MACH_MSM8X60_EF40S) ||defined(CONFIG_MACH_MSM8X60_EF40K) //EF40S/K
//#if (BOARD_REV > WS20)
static uint16_t ce1612_saddr = 0x3E;
//#else
//static uint16_t ce1612_saddr = 0x3C;
//#endif

#elif defined(CONFIG_MACH_MSM8X60_EF65L)	//EF65L
static uint16_t ce1612_saddr = 0x3E;
#else	// EF39S
static uint16_t ce1612_saddr = 0x3C;
#endif
static uint16_t f_stop_capture = 0;	// test cts


static int32_t config_csi;
static bool b_snapshot_flag;
static int32_t caf_camera_flag = 0;

static DECLARE_WAIT_QUEUE_HEAD(ce1612_wait_queue);
DECLARE_MUTEX(ce1612_sem);

struct ce1612_vreg_t {
	const char *name;
	unsigned short mvolt;
};

#if 1 //def F_PANTECH_CAMERA_FIX_CFG_LED_MODE
//int led_auto;
//uint16_t nbrightness = 0;
#endif
#ifdef F_CE1612_POWER
static struct regulator *l20a_2p8v;		//CAM_AVDD2.8V
static struct regulator *mvs0b_1p8v;	//VDD_HOSTIO1.8V
static struct regulator *l3a_2p8v;		//VDD_SYSIO2.8
static struct regulator *lvs1b_1p8v;		//CAM_IOVDD1.8V
static struct regulator *l14a_2p8v;		//VDD_AF2.8

#ifdef CONFIG_PANTECH_CAMERA//IRQ
static uint32_t ce1612_irq_stat = 0;
#ifdef AF_STATE_POLLING
static uint32_t current_af_state = 0;
#endif
#endif

#ifdef CONFIG_PANTECH_CAMERA_MT9D113
#define C_PANTECH_CAMERA_EF40_MT9D113_STANDBY
#endif

#ifdef C_PANTECH_CAMERA_EF40_MT9D113_STANDBY
static struct regulator *lvs3b_1p8v;		//CAMV_CORE_1P8V
static struct regulator *l19a_2p8v;		//CAMV_A_2P8V

#define MT9D113_RESET 137
#define MT9D113_STANDBY 139
#endif

/*
================================================================================
LOCAL API DECLARATIONS
================================================================================
*/


/*
================================================================================
FUNCTION
================================================================================
*/
static int ce1612_reset(int set)
{
	int rc = 0;

	rc = gpio_tlmm_config(GPIO_CFG(SENSOR_RESET, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);

	if (!rc) {
		SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
		gpio_set_value(SENSOR_RESET,0);		
		if(set){
			gpio_set_value(SENSOR_RESET,1);		
		}
	}
	else{
		SKYCERR("ce1612_reset gpio_tlmm_config Failed!\n");
		goto reset_fail;
	}

	SKYCDBG("%s %s Success:%d\n", __FILE__, __func__, __LINE__);
	return rc;

reset_fail:
	SKYCERR("%s %s Failed!:%d\n", __FILE__, __func__, __LINE__);
	return rc;
}


static int ce1612_vreg_init(void)
{
	int rc = 0;
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);

#ifdef C_PANTECH_CAMERA_EF40_MT9D113_STANDBY
	lvs3b_1p8v = regulator_get(NULL, "8901_lvs3");
	if (IS_ERR(lvs3b_1p8v)) {
		SKYCERR("regulator_get lvs3b_1p8v : 0x%x fail\n", (unsigned int)lvs3b_1p8v);
		return -ENODEV;
	}
	
	l19a_2p8v = regulator_get(NULL, "8058_l19");
	if (IS_ERR(l19a_2p8v)) {
		SKYCERR("regulator_get l19a_2p8v : 0x%x fail\n", (unsigned int)l19a_2p8v);
		return -ENODEV;
	}

	rc = regulator_set_voltage(l19a_2p8v, 2800000, 2800000);
	if (rc) {
		SKYCERR("%s: unable to set l19a_2p8v voltage to 2.8V\n", __func__);
		goto fail;
	}
#endif

	mvs0b_1p8v = regulator_get(NULL, "8901_mvs0");
	if (IS_ERR(mvs0b_1p8v)) {
		SKYCERR("regulator_get mvs0b_1p8v : 0x%x fail\n", (unsigned int)mvs0b_1p8v);
		return -ENODEV;
	}
	
	l20a_2p8v = regulator_get(NULL, "8058_l20");
	if (IS_ERR(l20a_2p8v)) {
		SKYCERR("regulator_get l20a_2p8v : 0x%x fail\n", (unsigned int)l20a_2p8v);
		return -ENODEV;
	}

	rc = regulator_set_voltage(l20a_2p8v, 2800000, 2800000);
	if (rc) {
		SKYCERR("%s: unable to set l20a_2p8v voltage to 2.8V\n", __func__);
		goto fail;
	}

	l3a_2p8v = regulator_get(NULL, "8058_l3");
	if (IS_ERR(l3a_2p8v)) {
		SKYCERR("regulator_get l3a_2p8v : 0x%x fail\n", (unsigned int)l3a_2p8v);
		return -ENODEV;
	}

	rc = regulator_set_voltage(l3a_2p8v, 2800000, 2800000);
	if (rc) {
		SKYCERR("%s: unable to set l3a_2p8v voltage to 2.8V\n", __func__);
		goto fail;
	}

	lvs1b_1p8v = regulator_get(NULL, "8901_lvs1");
	if (IS_ERR(lvs1b_1p8v)) {
		SKYCERR("regulator_get lvs1b_1p8v : 0x%x fail\n", (unsigned int)lvs1b_1p8v);
		return -ENODEV;
	}
	
	l14a_2p8v = regulator_get(NULL, "8058_l14");
	if (IS_ERR(l14a_2p8v)) {
		SKYCERR("regulator_get l14a_2p8v : 0x%x fail\n", (unsigned int)l14a_2p8v);
		return -ENODEV;
	}
#if (BOARD_REV >= WS10)
	rc = regulator_set_voltage(l14a_2p8v, 2800000, 2800000);
#else // for SD card
	rc = regulator_set_voltage(l14a_2p8v, 2850000, 2850000);
#endif
	if (rc) {
		SKYCERR("%s: unable to set l14a_2p8v voltage to 2.8V\n", __func__);
		goto fail;
	}
	
	SKYCDBG("%s %s Success!:%d\n", __FILE__, __func__, __LINE__);
	return rc;
fail:
	SKYCDBG("%s %s Failed!:%d\n", __FILE__, __func__, __LINE__);
#ifdef C_PANTECH_CAMERA_EF40_MT9D113_STANDBY
	if(l19a_2p8v) {
		regulator_put(l19a_2p8v);
	}
#endif
	if(l20a_2p8v) {
		regulator_put(l20a_2p8v);
	}
	if(l3a_2p8v) {
		regulator_put(l3a_2p8v);
	}
	if(l14a_2p8v) {
		regulator_put(l14a_2p8v);
	}
	return rc;	

}

#ifdef F_L14A_OFF_IN_PROBER
static int ce1612_l14a_off(void)
{
	int rc = 0;
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
	
	l14a_2p8v = regulator_get(NULL, "8058_l14");
	if (IS_ERR(l14a_2p8v)) {
		SKYCERR("regulator_get l14a_2p8v : 0x%x fail\n", (unsigned int)l14a_2p8v);
		return -ENODEV;
	}
#if (BOARD_REV >= WS10)
	rc = regulator_set_voltage(l14a_2p8v, 2800000, 2800000);
#else // for SD card
	rc = regulator_set_voltage(l14a_2p8v, 2850000, 2850000);
#endif
	if (rc) {
		SKYCERR("%s: unable to set l14a_2p8v voltage to 2.8V\n", __func__);
		goto fail_l14;
	}

	//VDD_AF2.8
	rc = regulator_enable(l14a_2p8v);
	if (rc) {
		SKYCERR("%s: Enable regulator l14a_2p8v failed\n", __func__);
		goto fail_l14;
	}
	mdelay(1);	

	//VDD_AF2.8
	rc = regulator_disable(l14a_2p8v);
	if (rc){
		SKYCERR("%s: Disable regulator l14a_2p8v failed\n", __func__);
		goto fail_l14;
	}
	regulator_put(l14a_2p8v);	
	
	SKYCDBG("%s %s Success!:%d\n", __FILE__, __func__, __LINE__);
	return rc;
	
fail_l14:
	SKYCDBG("%s %s Failed!:%d\n", __FILE__, __func__, __LINE__);
	if(l14a_2p8v) {
		regulator_put(l14a_2p8v);
	}
	return rc;	
}
#endif

static int ce1612_power(int on)
{
	int rc = 0;
		SKYCDBG("%s %s:%d power = %d\n", __FILE__, __func__, __LINE__,on);

	f_stop_capture = 0;	// test cts
	
	if(on) {		
		//8M_CAM_1.1V_EN : VDD_CORE1.1V
		rc = gpio_tlmm_config(GPIO_CFG(SENSOR_1P1V_LOWQ, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);
		if (!rc) {
			SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
			gpio_set_value(SENSOR_1P1V_LOWQ,1);					
		}
		mdelay(1);
		rc = gpio_tlmm_config(GPIO_CFG(VDD_CORE_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);
		if (!rc) {
			SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
			gpio_set_value(VDD_CORE_EN,1);					
		}
		mdelay(1);
		//VDD_HOSTIO1.8V
		rc = regulator_enable(mvs0b_1p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator mvs0b_1p8v failed\n", __func__);
			goto fail;
		}
		//mdelay(1);
#ifdef C_PANTECH_CAMERA_EF40_MT9D113_STANDBY
		mdelay(5);
		rc = regulator_enable(lvs3b_1p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator lvs3b_1p8v failed\n", __func__);
			goto fail;
		}

		rc = gpio_tlmm_config(GPIO_CFG(MT9D113_RESET, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);
		if (!rc) {
			SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
			gpio_set_value(MT9D113_RESET,0);					
		}
		mdelay(5);
		rc = gpio_tlmm_config(GPIO_CFG(MT9D113_RESET, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);
		if (!rc) {
			SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
			gpio_set_value(MT9D113_RESET,1);					
		}
		mdelay(1);
		
		rc = regulator_enable(l19a_2p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator l19a_2p8v failed\n", __func__);
			goto fail;
		}
		mdelay(5);

		rc = gpio_tlmm_config(GPIO_CFG(MT9D113_STANDBY, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);
		if (!rc) {
			SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
			gpio_set_value(MT9D113_STANDBY,1);					
		}
		mdelay(1);
#endif		
		//CAM_SYSIO2.8V
		rc = regulator_enable(l3a_2p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator l3a_2p8v failed\n", __func__);
			goto fail;
		}
		mdelay(1);
		//CAM_IOVDD1.8
		rc = regulator_enable(lvs1b_1p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator lvs1b_1p8v failed\n", __func__);
			goto fail;
		}
		//mdelay(1);
		//CAM_AVDD2.8V
		rc = regulator_enable(l20a_2p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator l20a_2p8v failed\n", __func__);
			goto fail;
		}
		//mdelay(1);
		//VDD_AF2.8
		rc = regulator_enable(l14a_2p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator l14a_2p8v failed\n", __func__);
			goto fail;
		}
		mdelay(1);

		/* Input MCLK = 24MHz */
		SKYCDBG(" msm_camio_clk_rate_set E\n");
		msm_camio_clk_rate_set(24000000);
		SKYCDBG(" msm_camio_clk_rate_set X\n");
		mdelay(5);
	
		//standby control
		rc = gpio_tlmm_config(GPIO_CFG(SENSOR_STANDBY, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);
		if (!rc) {
			SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
			gpio_set_value(SENSOR_STANDBY,1);					
		}
		mdelay(1);
		
		rc = ce1612_reset(ON);
		if (rc < 0) {
			SKYCERR("reset failed!\n");
			goto fail;
		}
		mdelay(10);
		
		SKYCDBG("%s %s ON Success:%d\n", __FILE__, __func__, __LINE__);
	}
	else {
		ce1612_reset(OFF);
		mdelay(1);
		
		//standby control
		rc = gpio_tlmm_config(GPIO_CFG(SENSOR_STANDBY, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);
		if (!rc) {
			SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
			gpio_set_value(SENSOR_STANDBY,0);					
		}
		mdelay(1);
		//VDD_AF2.8
		rc = regulator_disable(l14a_2p8v);
		if (rc){
			SKYCERR("%s: Disable regulator l14a_2p8v failed\n", __func__);
			goto fail;
		}
		regulator_put(l14a_2p8v);	
		//CAM_AVDD2.8V
		rc = regulator_disable(l20a_2p8v);
		if (rc){
			SKYCERR("%s: Disable regulator l20a_2p8v failed\n", __func__);
			goto fail;
		}
		regulator_put(l20a_2p8v);
		//CAM_IOVDD1.8
		rc = regulator_disable(lvs1b_1p8v);
		if (rc){
			SKYCERR("%s: Disable regulator lvs1b_1p8v failed\n", __func__);
			goto fail;
		}
		//CAM_SYSIO2.8V
		rc = regulator_disable(l3a_2p8v);
		if (rc){
			SKYCERR("%s: Disable regulator l3a_2p8v failed\n", __func__);
			goto fail;
		}
		regulator_put(l3a_2p8v);

#ifdef C_PANTECH_CAMERA_EF40_MT9D113_STANDBY
		mdelay(1);
		rc = gpio_tlmm_config(GPIO_CFG(MT9D113_STANDBY, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);
		if (!rc) {
			SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
			gpio_set_value(MT9D113_STANDBY,0);					
		}
		rc = regulator_disable(l19a_2p8v);
		if (rc) {
			SKYCERR("%s: Disable regulator l19a_2p8v failed\n", __func__);
			goto fail;
		}		
		mdelay(5);

		rc = gpio_tlmm_config(GPIO_CFG(MT9D113_RESET, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);
		if (!rc) {
			SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
			gpio_set_value(MT9D113_RESET,0);					
		}
		mdelay(1);
		
		rc = regulator_disable(lvs3b_1p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator lvs3b_1p8v failed\n", __func__);
			goto fail;
		}
#endif		
		
		//VDD_HOSTIO1.8V
		rc = regulator_disable(mvs0b_1p8v);
		if (rc){
			SKYCERR("%s: Disable regulator mvs0b_1p8v failed\n", __func__);
			goto fail;
		}
		mdelay(1);
		//8M_CAM_1.1V_EN : VDD_CORE1.1V
		rc = gpio_tlmm_config(GPIO_CFG(VDD_CORE_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);
		if (!rc) {
			SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
			gpio_set_value(VDD_CORE_EN,0);					
		}
		rc = gpio_tlmm_config(GPIO_CFG(SENSOR_1P1V_LOWQ, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);
		if (!rc) {
			SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
			gpio_set_value(SENSOR_1P1V_LOWQ,0);					
		}

		SKYCDBG("%s %s OFF Success:%d\n", __FILE__, __func__, __LINE__);
	}

	return rc;
fail:
	SKYCERR("%s %s Failed!:%d\n", __FILE__, __func__, __LINE__);
#ifdef C_PANTECH_CAMERA_EF40_MT9D113_STANDBY
	if(l19a_2p8v) {
		regulator_put(l19a_2p8v);
	}
#endif
	if(l20a_2p8v){
		regulator_put(l20a_2p8v);
	}
	if(l3a_2p8v){
		regulator_put(l3a_2p8v);
	}
	if(l14a_2p8v){
		regulator_put(l14a_2p8v);
	}
	return rc;			
}
#endif

static int32_t ce1612_i2c_txdata(unsigned short saddr,
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

#ifdef F_PANTECH_CAMERA_I2C_LOG
	SKYCDBG("%s: tx. \n", __func__);
	for(i = 0; i < length; i++)
		SKYCDBG(" %02x.", *(txdata+i));
	SKYCDBG("\n");
#endif


#if SENSOR_DEBUG
	if (length == 2)
		SKYCDBG("msm_io_i2c_w: 0x%04x 0x%04x\n",
			*(u16 *) txdata, *(u16 *) (txdata + 2));
	else if (length == 4)
		SKYCDBG("msm_io_i2c_w: 0x%04x\n", *(u16 *) txdata);
	else
		SKYCDBG("msm_io_i2c_w: length = %d\n", length);
#endif
	for (i = 0; i < CE1612_I2C_RETRY; i++) {
		rc = i2c_transfer(ce1612_client->adapter, msg, 1); 
		if (rc >= 0) {			
			return 0;
		}
		SKYCDBG("%s: tx retry. [%02x.%02x.%02x] len=%d rc=%d\n", __func__,saddr, *txdata, *(txdata + 1), length, rc);
		msleep(CE1612_I2C_MPERIOD);
	}
	return -EIO;
}

static int ce1612_i2c_rxdata(uint16_t saddr, uint8_t *addr, 
				unsigned char *rdata, uint16_t len)
{
	uint32_t i = 0;
	int32_t rc = 0;
	
	struct i2c_msg msgs[] = {
	{
		.addr   = saddr,
		.flags = 0,
		.len   = 1,
		.buf   = addr,
	},
	{
		.addr   = saddr,
		.flags = I2C_M_RD,
		.len   = len,
		.buf   = rdata,
	},
	};
	
	for (i = 0; i < CE1612_I2C_RETRY; i++) {
		rc = i2c_transfer(ce1612_client->adapter, msgs, 2); 
		if (rc >= 0) {		
			
#ifdef F_PANTECH_CAMERA_I2C_LOG
{
	int j;
	SKYCDBG("%s: tx. \n", __func__);
	SKYCDBG(" %02x.", *(addr));
	SKYCDBG("\n");
	
	SKYCDBG("%s: rx. \n", __func__);
	for(j = 0; j < len; j++)
		SKYCDBG(" %02x.", *(rdata+j));
	SKYCDBG("\n");
}
#endif
			return 0;
		}
		SKYCERR("%s: rx retry. [%02x.%02x.xx] len=%d, rc=%d\n", 
			__func__, saddr, *addr, len, rc);
		msleep(CE1612_I2C_MPERIOD);
	}
	SKYCERR("%s: error. [%02x.%02x.xx] len=%d rc=%d\n", 
		__func__, saddr, *addr, len, rc);
	
	return -EIO;
}

static int ce1612_i2c_rxdata_2(uint16_t saddr, 
				unsigned char *rdata, uint16_t len)
{
	uint32_t i = 0;
	int32_t rc = 0;
	
	struct i2c_msg msgs[] = {
	{
		.addr   = saddr,
		.flags = I2C_M_RD,
		.len   = len,
		.buf   = rdata,
	},
	};
	
	for (i = 0; i < CE1612_I2C_RETRY; i++) {
		rc = i2c_transfer(ce1612_client->adapter, msgs, 1); 
		if (rc >= 0) {		
#ifdef F_PANTECH_CAMERA_I2C_LOG
{
	int j;
	SKYCDBG("%s: rx. \n", __func__);
	for(j = 0; j < len; j++)
		SKYCDBG(" %02x.", *(rdata+j));
	SKYCDBG("\n");
}
#endif
			return 0;
		}
		SKYCERR("%s: rx retry. [%02x.] len=%d, rc=%d\n", 
			__func__, saddr, len, rc);
		msleep(CE1612_I2C_MPERIOD);
	}
	SKYCERR("%s: error. [%02x.] len=%d rc=%d\n", 
		__func__, saddr, len, rc);
	
	return -EIO;
}

static int32_t ce1612_i2c_read(uint16_t saddr, uint8_t cmd, unsigned char *rdata)
{
	int32_t rc = 0;
	unsigned char tmp_raddr = 0;

	if (!rdata)
		return -EIO;

	tmp_raddr = cmd;

	rc = ce1612_i2c_rxdata(saddr, &tmp_raddr, rdata, 1);

	return rc;	
}

static int32_t ce1612_cmd_read(uint8_t cmd, uint8_t *rdata, int32_t len)
{
	int32_t rc = 0;
	unsigned char tmp_raddr = 0;

	if (!rdata)
		return -EIO;

	tmp_raddr = cmd;

      rc = ce1612_i2c_rxdata(ce1612_saddr, &tmp_raddr, rdata, (uint16_t)len);

      if (rc < 0)
      {
		SKYCDBG("ce1612_cmd_read I2C FAIL_2!!! return~~\n");
      } 		
	  
	//SKYCDBG("%s end : 0x%x register read data = 0x%x\n", __func__, raddr, rdata);

	return rc;
}

static int32_t ce1612_read(uint8_t *rdata, int32_t len)
{
	int32_t rc = 0;	

	rc = ce1612_i2c_rxdata_2(ce1612_saddr, rdata, (uint16_t)len);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
	}
	//SKYCDBG("%s end : 0x%x register read data = 0x%x\n", __func__, raddr, rdata);
	return rc; 
}

static int32_t ce1612_poll_bit(uint16_t saddr, uint8_t addr,
			   uint8_t mperiod, uint32_t retry)
{
	uint8_t rdata = 0;
	uint32_t i = 0;
	int32_t rc = 0;

	for (i = 0; i < retry; i++) {
		rc = ce1612_i2c_read(saddr, addr, &rdata);
		if (rc < 0)
			break;
		if (!(rdata & 0x01))
			break;

		msleep(mperiod);
	}

	if (i == retry) {
		SKYCERR("%s: -ETIMEDOUT, mperiod=%d, retry=%d\n", 
			__func__, mperiod, retry);
		rc = -ETIMEDOUT;
	}

	return rc;
}

#ifdef F_FW_UPDATE
static int32_t ce1612_poll(uint16_t saddr, uint8_t addr, uint8_t pdata, 
			   uint8_t mperiod, uint32_t retry)
{
	uint8_t rdata = 0;
	uint32_t i = 0;
	int32_t rc = 0;

	for (i = 0; i < retry; i++) {
		rc = ce1612_i2c_read(saddr, addr, &rdata);
		if (rc < 0)
			break;
		if (rdata == pdata)
			break;

		msleep(mperiod);
	}

	if (i == retry) {
		SKYCERR("%s: -ETIMEDOUT, mperiod=%d, retry=%d\n", 
			__func__, mperiod, retry);
		rc = -ETIMEDOUT;
	}

	return rc;
}
#endif

static int32_t ce1612_cmd(uint16_t saddr, uint8_t cmd, uint8_t * ydata, int32_t num_data)
{
	int32_t rc = 0;
	unsigned char buf[10];
	int32_t i = 0;
	
	memset(buf, 0, sizeof(buf));
	buf[0] = cmd;

	if(ydata != NULL)
	{
		for(i = 0; i < num_data; i++)
		{
			buf[i+1] = *(ydata+i);
		}	
	}
		
	rc = ce1612_i2c_txdata(saddr, buf, num_data+1);
	
	if (rc < 0)
		SKYCERR("ERR:%s FAIL!!!cmd=%d , rc=%d return~~\n", __func__, cmd, rc);	
	
	return rc;
}

static int32_t ce1612_cmd_poll(uint16_t saddr, uint8_t cmd, uint8_t * ydata, int32_t num_data)
{
	int32_t rc = 0;
	unsigned char buf[10];
	int32_t i = 0;
	
	memset(buf, 0, sizeof(buf));
	buf[0] = cmd;

	if(ydata != NULL)
	{
		for(i = 0; i < num_data; i++)
		{
			buf[i+1] = *(ydata+i);
		}	
	}
		
	rc = ce1612_i2c_txdata(saddr, buf, num_data+1);
	if (rc < 0)
		return rc;

	rc = ce1612_poll_bit(saddr, 0x24, 10, 400);
	
	return rc;
}

#if 0
static int32_t ce1612_set_lens_roll_off(void)
{
	int32_t rc = 0;
#if 0
	rc = ce1612_i2c_write_table(&ce1612_regs.rftbl[0],
								 ce1612_regs.rftbl_size);
#endif
	return rc;
}
#endif

#if 1
static const uint8_t isp_uploader_info[]= {
		0xF2,
#include CE1612_UPLOADER_INFO
};
static const uint8_t isp_uploader_bin[] = {
		0xF4,
#include CE1612_UPLOADER_BIN
};
static const uint8_t isp_fw_info[] = {
		0xF2,
#include CE1612_FW_INFO
};
/*
static const uint8_t isp_fw_bin[] = {
		0xF4,
#include CE1612_FW_BIN
};
*/
static const uint8_t isp_fw_bin_0[] = {
		0xF4,
#include CE1612_FW_BIN_0
};
static const uint8_t isp_fw_bin_1[] = {
		0xF4,
#include CE1612_FW_BIN_1
};
static const uint8_t isp_fw_bin_2[] = {
		0xF4,
#include CE1612_FW_BIN_2
};
static const uint8_t isp_fw_bin_3[] = {
		0xF4,
#include CE1612_FW_BIN_3
};
static const uint8_t isp_fw_bin_4[] = {
		0xF4,
#include CE1612_FW_BIN_4
};
static const uint8_t isp_fw_bin_5[] = {
		0xF4,
#include CE1612_FW_BIN_5
};
static const uint8_t isp_fw_bin_6[] = {
		0xF4,
#include CE1612_FW_BIN_6
};
static const uint8_t isp_fw_bin_7[] = {
		0xF4,
#include CE1612_FW_BIN_7
};
static const uint8_t isp_fw_bin_8[] = {
		0xF4,
#include CE1612_FW_BIN_8
};
static const uint8_t isp_fw_bin_9[] = {
		0xF4,
#include CE1612_FW_BIN_9
};
#if 0
static const uint8_t isp_fw_bin_10[] = {
		0xF4,
#include CE1612_FW_BIN_10
};
uint8_t isp_fw_bin_11[] = {
		0xF4,
#include CE1612_FW_BIN_11
};
#endif
#else
#include CE1612_UPLOADER_INFO
#include CE1612_UPLOADER_BIN
#include CE1612_FW_INFO
#include CE1612_FW_BIN
#endif
#if 1
static int32_t ce1612_update_fw(void)
{
#define CE1612_NUM_CFG_CMD 5
#define CE1612_NUM_UPDATE_DATA 129
#define CE1612_PACKETS_IN_TABLE 508

	int32_t rc = 0;
	uint32_t numPacket = 0;
#ifdef F_FW_UPDATE
	uint8_t * pFW = NULL;
	uint32_t i = 0;
	uint8_t cmd;
	uint8_t rdata = 0xC0;
	uint8_t * pbuf;
#endif

	numPacket = (isp_uploader_info[2] & 0xFF) << 8;
	numPacket |= (isp_uploader_info[1] & 0xFF);

	SKYCDBG("%s start : number of uploader packets is 0x%x\n",__func__, numPacket);

#ifdef F_FW_UPDATE	
	pbuf = (uint8_t *)isp_uploader_info;
	SKYCDBG("%s : table address = 0x%p , pointer = 0x%p\n",__func__, isp_uploader_info, pbuf);
	rc = ce1612_i2c_txdata(ce1612_saddr, (uint8_t *)isp_uploader_info, CE1612_NUM_CFG_CMD);
	if (rc < 0)
	{
		SKYCERR("%s : uploader configs write ERROR 0x%x, 0x%x, 0x%x, 0x%x, 0x%x!!!\n",__func__, *pbuf, *(pbuf+1), *(pbuf+2), *(pbuf+3), *(pbuf+4));
		goto fw_update_fail;
	}

	pFW = (uint8_t *)isp_uploader_bin;
	cmd = *pFW;

	for(i = 0; i < numPacket; i++, pFW+= CE1612_NUM_UPDATE_DATA)
	{
		*pFW = cmd;
		rc = ce1612_i2c_txdata(ce1612_saddr, pFW, CE1612_NUM_UPDATE_DATA+1);
		if (rc < 0)
		{
			SKYCERR("%s : uploader packet %d write ERROR !!!\n",__func__, i);
			goto fw_update_fail;
		}
		if(cmd == 0xF3)
		{
			rc = ce1612_read(&rdata, 1);
			if(rdata != 0)
			{
				SKYCERR("%s : uploader packet %d write ERROR [0xF3 = 0x%x]!!!\n",__func__, i, rdata);
				goto fw_update_fail;
			}
		}
	}

	msleep(5);
	
	rc = ce1612_poll(ce1612_saddr, 0xF5, 0x05, 10, 500);
	if (rc < 0)
	{
		SKYCERR("%s : uploader polling ERROR !!!\n",__func__);
//		goto fw_update_fail;
	}

	numPacket = (isp_fw_info[2] & 0xFF) << 8;
	numPacket |= (isp_fw_info[1] & 0xFF);

	SKYCDBG("%s start : number of FW packets is 0x%x\n",__func__, numPacket);

	rc = ce1612_i2c_txdata(ce1612_saddr, (uint8_t *)isp_fw_info, CE1612_NUM_CFG_CMD);
	if (rc < 0)
	{
		goto fw_update_fail;
	}
#if 1
	pFW = (uint8_t *)isp_fw_bin_0;
	cmd = *pFW;
	for(i = 0; (i < numPacket) && (i <CE1612_PACKETS_IN_TABLE); i++, pFW+= CE1612_NUM_UPDATE_DATA)
	{
		*pFW = cmd;
		rc = ce1612_i2c_txdata(ce1612_saddr, pFW, CE1612_NUM_UPDATE_DATA+1);
		if (rc < 0)
		{
			SKYCERR("%s : fw packet0 %d write ERROR !!!\n",__func__, i);
			goto fw_update_fail;
		}
		if(cmd == 0xF3)
		{
			rc = ce1612_read(&rdata, 1);
			if(rdata != 0)
			{
				SKYCERR("%s : fw packet0 %d write ERROR [0xF3 = 0x%x]!!!\n",__func__, i, rdata);
				goto fw_update_fail;
			}
		}
	}

	pFW = (uint8_t *)isp_fw_bin_1;
	cmd = *pFW;
	for(; (i < numPacket) && (i <CE1612_PACKETS_IN_TABLE*2); i++, pFW+= CE1612_NUM_UPDATE_DATA)
	{
		*pFW = cmd;
		rc = ce1612_i2c_txdata(ce1612_saddr, pFW, CE1612_NUM_UPDATE_DATA+1);
		if (rc < 0)
		{
			SKYCERR("%s : fw packet1 %d write ERROR !!!\n",__func__, i);
			goto fw_update_fail;
		}
		if(cmd == 0xF3)
		{
			rc = ce1612_read(&rdata, 1);
			if(rdata != 0)
			{
				SKYCERR("%s : fw packet1 %d write ERROR [0xF3 = 0x%x]!!!\n",__func__, i, rdata);
				goto fw_update_fail;
			}
		}
	}
	pFW = (uint8_t *)isp_fw_bin_2;
	cmd = *pFW;
	for(; (i < numPacket) && (i <CE1612_PACKETS_IN_TABLE*3); i++, pFW+= CE1612_NUM_UPDATE_DATA)
	{
		*pFW = cmd;
		rc = ce1612_i2c_txdata(ce1612_saddr, pFW, CE1612_NUM_UPDATE_DATA+1);
		if (rc < 0)
		{
			SKYCERR("%s : fw packet2 %d write ERROR !!!\n",__func__, i);
			goto fw_update_fail;
		}
		if(cmd == 0xF3)
		{
			rc = ce1612_read(&rdata, 1);
			if(rdata != 0)
			{
				SKYCERR("%s : fw packet2 %d write ERROR [0xF3 = 0x%x]!!!\n",__func__, i, rdata);
				goto fw_update_fail;
			}
		}
	}
	pFW = (uint8_t *)isp_fw_bin_3;
	cmd = *pFW;
	for(; (i < numPacket) && (i <CE1612_PACKETS_IN_TABLE*4); i++, pFW+= CE1612_NUM_UPDATE_DATA)
	{
		*pFW = cmd;
		rc = ce1612_i2c_txdata(ce1612_saddr, pFW, CE1612_NUM_UPDATE_DATA+1);
		if (rc < 0)
		{
			SKYCERR("%s : fw packet2 %d write ERROR !!!\n",__func__, i);
			goto fw_update_fail;
		}
		if(cmd == 0xF3)
		{
			rc = ce1612_read(&rdata, 1);
			if(rdata != 0)
			{
				SKYCERR("%s : fw packet2 %d write ERROR [0xF3 = 0x%x]!!!\n",__func__, i, rdata);
				goto fw_update_fail;
			}
		}
	}
	pFW = (uint8_t *)isp_fw_bin_4;
	cmd = *pFW;
	for(; (i < numPacket) && (i <CE1612_PACKETS_IN_TABLE*5); i++, pFW+= CE1612_NUM_UPDATE_DATA)
	{
		*pFW = cmd;
		rc = ce1612_i2c_txdata(ce1612_saddr, pFW, CE1612_NUM_UPDATE_DATA+1);
		if (rc < 0)
		{
			SKYCERR("%s : fw packet2 %d write ERROR !!!\n",__func__, i);
			goto fw_update_fail;
		}
		if(cmd == 0xF3)
		{
			rc = ce1612_read(&rdata, 1);
			if(rdata != 0)
			{
				SKYCERR("%s : fw packet2 %d write ERROR [0xF3 = 0x%x]!!!\n",__func__, i, rdata);
				goto fw_update_fail;
			}
		}
	}
	SKYCDBG("%s : fw packet4 %d write OK !!!\n",__func__, i);
	pFW = (uint8_t *)isp_fw_bin_5;
	cmd = *pFW;
	for(; (i < numPacket) && (i <CE1612_PACKETS_IN_TABLE*6); i++, pFW+= CE1612_NUM_UPDATE_DATA)
	{
		*pFW = cmd;
		rc = ce1612_i2c_txdata(ce1612_saddr, pFW, CE1612_NUM_UPDATE_DATA+1);
		if (rc < 0)
		{
			SKYCERR("%s : fw packet2 %d write ERROR !!!\n",__func__, i);
			goto fw_update_fail;
		}
		if(cmd == 0xF3)
		{
			rc = ce1612_read(&rdata, 1);
			if(rdata != 0)
			{
				SKYCERR("%s : fw packet2 %d write ERROR [0xF3 = 0x%x]!!!\n",__func__, i, rdata);
				goto fw_update_fail;
			}
		}
	}
	SKYCDBG("%s : fw packet5 %d write OK !!!\n",__func__, i);
	pFW = (uint8_t *)isp_fw_bin_6;
	cmd = *pFW;
	for(; (i < numPacket) && (i <CE1612_PACKETS_IN_TABLE*7); i++, pFW+= CE1612_NUM_UPDATE_DATA)
	{
		*pFW = cmd;
		rc = ce1612_i2c_txdata(ce1612_saddr, pFW, CE1612_NUM_UPDATE_DATA+1);
		if (rc < 0)
		{
			SKYCERR("%s : fw packet2 %d write ERROR !!!\n",__func__, i);
			goto fw_update_fail;
		}
		if(cmd == 0xF3)
		{
			rc = ce1612_read(&rdata, 1);
			if(rdata != 0)
			{
				SKYCERR("%s : fw packet2 %d write ERROR [0xF3 = 0x%x]!!!\n",__func__, i, rdata);
				goto fw_update_fail;
			}
		}
	}
	SKYCDBG("%s : fw packet6 %d write OK !!!\n",__func__, i);
	pFW = (uint8_t *)isp_fw_bin_7;
	cmd = *pFW;
	for(; (i < numPacket) && (i <CE1612_PACKETS_IN_TABLE*8); i++, pFW+= CE1612_NUM_UPDATE_DATA)
	{
		*pFW = cmd;
		rc = ce1612_i2c_txdata(ce1612_saddr, pFW, CE1612_NUM_UPDATE_DATA+1);
		if (rc < 0)
		{
			SKYCERR("%s : fw packet2 %d write ERROR !!!\n",__func__, i);
			goto fw_update_fail;
		}
		if(cmd == 0xF3)
		{
			rc = ce1612_read(&rdata, 1);
			if(rdata != 0)
			{
				SKYCERR("%s : fw packet2 %d write ERROR [0xF3 = 0x%x]!!!\n",__func__, i, rdata);
				goto fw_update_fail;
			}
		}
	}
	SKYCDBG("%s : fw packet7 %d write OK !!!\n",__func__, i);
	pFW = (uint8_t *)isp_fw_bin_8;
	cmd = *pFW;
	for(; (i < numPacket) && (i <CE1612_PACKETS_IN_TABLE*9); i++, pFW+= CE1612_NUM_UPDATE_DATA)
	{
		*pFW = cmd;
		rc = ce1612_i2c_txdata(ce1612_saddr, pFW, CE1612_NUM_UPDATE_DATA+1);
		if (rc < 0)
		{
			SKYCERR("%s : fw packet2 %d write ERROR !!!\n",__func__, i);
			goto fw_update_fail;
		}
		if(cmd == 0xF3)
		{
			rc = ce1612_read(&rdata, 1);
			if(rdata != 0)
			{
				SKYCERR("%s : fw packet2 %d write ERROR [0xF3 = 0x%x]!!!\n",__func__, i, rdata);
				goto fw_update_fail;
			}
		}
	}
	SKYCDBG("%s : fw packet8 %d write OK !!!\n",__func__, i);

	pFW = (uint8_t *)isp_fw_bin_9;
	cmd = *pFW;
	for(; (i < numPacket) && (i <CE1612_PACKETS_IN_TABLE*10); i++, pFW+= CE1612_NUM_UPDATE_DATA)
	{
		*pFW = cmd;
		rc = ce1612_i2c_txdata(ce1612_saddr, pFW, CE1612_NUM_UPDATE_DATA+1);
		if (rc < 0)
		{
			SKYCERR("%s : fw packet2 %d write ERROR !!!\n",__func__, i);
			goto fw_update_fail;
		}
		if(cmd == 0xF3)
		{
			rc = ce1612_read(&rdata, 1);
			if(rdata != 0)
			{
				SKYCERR("%s : fw packet2 %d write ERROR [0xF3 = 0x%x]!!!\n",__func__, i, rdata);
				goto fw_update_fail;
			}
		}
	}
	SKYCDBG("%s : fw packet9 %d write OK !!!\n",__func__, i);
#if 0
	pFW = (uint8_t *)isp_fw_bin_10;
	cmd = *pFW;
	for(; (i < numPacket) && (i <CE1612_PACKETS_IN_TABLE*11); i++, pFW+= CE1612_NUM_UPDATE_DATA)
	{
		*pFW = cmd;
		rc = ce1612_i2c_txdata(ce1612_saddr, pFW, CE1612_NUM_UPDATE_DATA+1);
		if (rc < 0)
		{
			SKYCERR("%s : fw packet2 %d write ERROR !!!\n",__func__, i);
			goto fw_update_fail;
		}
		if(cmd == 0xF3)
		{
			rc = ce1612_read(&rdata, 1);
			if(rdata != 0)
			{
				SKYCERR("%s : fw packet2 %d write ERROR [0xF3 = 0x%x]!!!\n",__func__, i, rdata);
				goto fw_update_fail;
			}
		}
	}
	SKYCDBG("%s : fw packet10 %d write OK !!!\n",__func__, i);
	pFW = (uint8_t *)isp_fw_bin_11;
	cmd = *pFW;
	for(; (i < numPacket) && (i <CE1612_PACKETS_IN_TABLE*12); i++, pFW+= CE1612_NUM_UPDATE_DATA)
	{
		*pFW = cmd;
		rc = ce1612_i2c_txdata(ce1612_saddr, pFW, CE1612_NUM_UPDATE_DATA+1);
		if (rc < 0)
		{
			SKYCERR("%s : fw packet2 %d write ERROR !!!\n",__func__, i);
			goto fw_update_fail;
		}
		if(cmd == 0xF3)
		{
			rc = ce1612_read(&rdata, 1);
			if(rdata != 0)
			{
				SKYCERR("%s : fw packet2 %d write ERROR [0xF3 = 0x%x]!!!\n",__func__, i, rdata);
				goto fw_update_fail;
			}
		}
	}
	SKYCDBG("%s : fw packet11 %d write OK !!!\n",__func__, i);
#endif

#else
	pFW = (uint8_t *)isp_fw_bin;
	cmd = *pFW;
//	for(i = 0; i < numPacket; i++, pFW+= CE1612_NUM_UPDATE_DATA)
	{
		*pFW = cmd;
		rc = ce1612_i2c_txdata(ce1612_saddr, pFW, CE1612_NUM_UPDATE_DATA+1);
		if (rc < 0)
		{
			SKYCERR("%s : uploader packet %d write ERROR !!!\n",__func__, i);
			goto fw_update_fail;
		}
		if(cmd == 0xF3)
		{
			rc = ce1612_read(&rdata, 1);
			if(rdata != 0)
			{
				SKYCERR("%s : uploader packet %d write ERROR [0xF3 = 0x%x]!!!\n",__func__, i, rdata);
				goto fw_update_fail;
			}
		}
	}
#endif
#if 1
	rc = ce1612_poll(ce1612_saddr, 0xF5, 0x06, 10, 1000);
	if (rc < 0)
	{
		SKYCERR("%s : uploader polling ERROR !!!\n",__func__);
		goto fw_update_fail;
	}
#endif
	return rc;	

fw_update_fail:

	SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);	
#endif
	return rc;
}

static int32_t ce1612_update_fw_boot(void)
{
	int32_t rc = 0;
	unsigned char data;
	unsigned char rdata[4];
	
#ifdef F_CE1612_POWER	
	ce1612_vreg_init();
	rc = ce1612_power(ON);
	if (rc)
	{		
		SKYCERR(" ce1612_power failed rc=%d\n",rc);
		goto update_fw_boot_done; 
	}
#endif

	rc = ce1612_cmd(ce1612_saddr, 0xF0, NULL, 0);
	if (rc < 0)
	{
#if 1
		goto update_fw_boot_done; 
#else
		if(ce1612_saddr == 0x3C)
			ce1612_saddr = 0x3E;
		else
			ce1612_saddr = 0x3C;
		
		rc = ce1612_cmd(ce1612_saddr, 0xF0, NULL, 0);
		if (rc < 0)
		{
			SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
			goto update_fw_boot_done; 
		}
#endif
	}
	msleep(70);

	data = 0x00;
	rc = ce1612_cmd(ce1612_saddr, 0x00, &data, 1);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		goto update_fw_boot_done; 
	}

	rc = ce1612_read(rdata, 4);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		goto update_fw_boot; 
	}
	ce1612_ver.fw_minor_ver = rdata[0] & 0xFF;	  
	ce1612_ver.fw_major_ver = rdata[1] & 0xFF;
	ce1612_ver.pf_ver = rdata[2] & 0xFF;	  
	data = rdata[3] & 0xFF;	  
	SKYCERR("%s : FW minor version=0x%x, FW major viersion=0x%x(0x%x), data = 0x%x\n",__func__, ce1612_ver.fw_minor_ver, ce1612_ver.fw_major_ver, ce1612_ver.pf_ver, data);

	if ((ce1612_ver.fw_major_ver == CE1612_FW_MAJOR_VER) &&
		(ce1612_ver.fw_minor_ver == CE1612_FW_MINOR_VER)) {

		data = 0x01;
		rc = ce1612_cmd(ce1612_saddr, 0x00, &data, 1);
		if (rc < 0)
		{
			SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
			goto update_fw_boot; 
		}

		rc = ce1612_read(rdata, 4);
		if (rc < 0)
		{
			SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
			goto update_fw_boot_done; 
		}
		ce1612_ver.prm_minor_ver = rdata[0] & 0xFF;	  
		ce1612_ver.prm_major_ver = rdata[1] & 0xFF;
		ce1612_ver.pf_ver = rdata[2] & 0xFF;	  
		data = rdata[3] & 0xFF;	  
		SKYCERR("%s : PRM minor version=0x%x, PRM major viersion=0x%x(0x%x), data = 0x%x\n",__func__, ce1612_ver.prm_minor_ver, ce1612_ver.prm_major_ver, ce1612_ver.pf_ver, data);

		if ((ce1612_ver.prm_major_ver == CE1612_PRM_MAJOR_VER) &&
			(ce1612_ver.prm_minor_ver == CE1612_PRM_MINOR_VER)) {			
			goto update_fw_boot_done;
		}
	}

update_fw_boot:

#ifdef F_CE1612_POWER	
	rc = ce1612_power(OFF);
	if (rc) {
		SKYCERR(" ce1612_power failed rc=%d\n",rc);		
	}
	ce1612_vreg_init();
	rc = ce1612_power(ON);
	if (rc) {		
		SKYCERR(" ce1612_power failed rc=%d\n",rc);
		goto update_fw_boot_done; 
	}
#endif
	rc = ce1612_update_fw();

update_fw_boot_done:
#ifdef F_CE1612_POWER	
	SKYCERR(" ce1612_sensor_release E\n");	
	rc = ce1612_power(OFF);
	if (rc) {
		SKYCERR(" ce1612_power failed rc=%d\n",rc);		
	}
#endif
	
	return rc;
}
#endif


static int32_t ce1612_update_fw_user(const struct sensor_cfg_data *scfg)
{
#define CE1612_NUM_CFG_DATA 4
#define CE1612_NUM_UPDATE_DATA 129
	int32_t rc = 0;
	uint8_t *paddr = NULL;
	int32_t vaddr = 0;

	uint32_t bnType = 0;
	uint32_t numPacket = 0;
	uint32_t i = 0;
	uint8_t rdata = 0xC0;
	uint8_t cmd;

	SKYCDBG("%s is called.\n", __func__);

	vaddr = scfg->cfg.isp_bin_vaddr;
	
	/* don't wanna copy, wanna convert virt to phy only. */
	paddr = kmalloc(64 * 1024, GFP_KERNEL);
	if (!paddr) {
		SKYCDBG("failed to malloc.\n");
		rc = -ENOMEM;
		goto update_fw_user_fail;
	}

	if (copy_from_user(paddr, (void *)vaddr, 4)) {
		SKYCDBG("failed to copy from user.\n");
		rc = -EFAULT;
		goto update_fw_user_fail;
	}

	bnType = *paddr++;
	numPacket = *paddr++;
	numPacket |= ((*paddr++) &0xFF) <<8;

	switch(bnType)
	{
	case 0:
#ifdef F_CE1612_POWER	
		ce1612_power(OFF);
		mdelay(10);
		ce1612_vreg_init();
		rc = ce1612_power(ON);		
		if (rc) {		
			SKYCERR(" ce1612_power failed rc=%d\n",rc);
			goto update_fw_user_fail; 
		}
#endif
		
	case 2:
		if (copy_from_user(paddr+1, (void *)vaddr+4, CE1612_NUM_CFG_DATA)) {
			SKYCDBG("failed to copy from user.\n");
			rc = -EFAULT;
			goto update_fw_user_fail;
		}
		rc = ce1612_i2c_txdata(ce1612_saddr, paddr, CE1612_NUM_CFG_DATA+1);
		//SKYCERR("%s : bnType = %d , data = %x!!!\n",__func__, bnType, *(paddr+CE1612_NUM_CFG_DATA));
		break;
	case 1:
	case 3:
		if (copy_from_user(paddr+1, (void *)vaddr+4, CE1612_NUM_UPDATE_DATA*numPacket)) {
			SKYCDBG("failed to copy from user.\n");
			rc = -EFAULT;
			goto update_fw_user_fail;
		}

		cmd = *paddr;
		for(i = 0; i < numPacket; i++, paddr += CE1612_NUM_UPDATE_DATA)
		{
			*paddr = cmd;
			rc = ce1612_i2c_txdata(ce1612_saddr, paddr, CE1612_NUM_UPDATE_DATA+1);
			//SKYCERR("%s : bnType = %d , i = %d(%d), data = %x!!!\n",__func__, bnType, i, numPacket, *(paddr+CE1612_NUM_UPDATE_DATA));
			if (rc < 0)
			{
				SKYCERR("%s : uploader packet %d write ERROR !!!\n",__func__, i);
				goto update_fw_user_fail;
			}

			if(cmd == 0xF3)
			{
				rc = ce1612_read(&rdata, 1);
				if(rdata != 0)
				{
					SKYCERR("%s : uploader packet %d write ERROR [0xF3 = 0x%x]!!!\n",__func__, i, rdata);
					goto update_fw_user_fail;
				}
			}
		}
		
		break;
	default:	
		SKYCERR("%s : Unknown bnType ERROR !!!\n",__func__);
		rc = -EFAULT;
		goto update_fw_user_fail;
	}

	if (paddr)
		kfree(paddr);

	SKYCDBG("%s: done.\n", __func__);
	
	return rc;
	
update_fw_user_fail:
	if (paddr)
		kfree(paddr);
	SKYCERR("%s: error. rc=%d\n", __func__, rc);
	return rc;
}

static long ce1612_reg_init(void)
{
//	uint16_t revision_num = 0;
	int32_t rc = 0;
	uint8_t data_buf[4];

	SKYCDBG("%s start : ce1612_saddr = 0x%x\n",__func__, ce1612_saddr);

	rc = ce1612_cmd(ce1612_saddr, 0xF0, NULL, 0);
	if (rc < 0)
	{
#if 1
		return rc;
#else
		if(ce1612_saddr == 0x3C)
			ce1612_saddr = 0x3E;
		else
			ce1612_saddr = 0x3C;
		
		rc = ce1612_cmd(ce1612_saddr, 0xF0, NULL, 0);
		if (rc < 0)
		{
			SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
			return rc;
		}
#endif
	}
	msleep(70);

	data_buf[0] = 0x1C;	// SXGA
	data_buf[1] = 0x01;
	data_buf[2] = 0x00;		
	rc = ce1612_cmd(ce1612_saddr, 0x54, data_buf, 3);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}

	// Buffering setting (YUV)
	data_buf[0] = 0x21;
	data_buf[1] = 0x05;
	data_buf[2] = 0x00;	
	data_buf[3] = 0x00;	
	rc = ce1612_cmd(ce1612_saddr, 0x73, data_buf, 4);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}

#if 0
	// AE/AWB enable
	data_buf[0] = 0x00;
	rc = ce1612_cmd(ce1612_saddr, 0x11, data_buf, 1);

	// Preview start (PREVIEW)
	data_buf[0] = 0x01;
	rc = ce1612_cmd(ce1612_saddr, 0x6B, data_buf, 1);
#endif

#if 0		
//	if(config_csi == 0)
	{
		struct msm_camera_csi_params ce1612_csi_params;
		SKYCDBG("ce1612_set_sensor_mode config_csi E\n");
		ce1612_csi_params.lane_cnt = 2; // 4;
		ce1612_csi_params.data_format = CSI_8BIT; //CSI_10BIT;
		ce1612_csi_params.lane_assign = 0xe4;
		ce1612_csi_params.dpcm_scheme = 0;
		ce1612_csi_params.settle_cnt = 0x14;
		rc = msm_camio_csi_config(&ce1612_csi_params);
//		msleep(ce1612_delay_msecs_stream);
//		config_csi = 1;
		SKYCDBG("ce1612_set_sensor_mode config_csi X\n");
	}	
#endif
	
	return rc;
}

static long ce1612_set_effect(int mode, int effect)
{
	uint8_t data_buf[2];
	int rc = 0;

	SKYCDBG("%s start\n",__func__);

	if(effect < 0 || effect >= CAMERA_EFFECT_MAX){
		SKYCERR("%s error. effect=%d\n", __func__, effect);
		return -EINVAL;
	}

	if(effect == 5) //posterize
	{
		data_buf[0] = 0x05;
		data_buf[1] = *(ce1612_regs.effect_data);
		rc = ce1612_cmd(ce1612_saddr, 0x3D, data_buf, 2);	//effect off
	
		data_buf[0] = 0x07;
		data_buf[1] = 0x0E;
		rc = ce1612_cmd(ce1612_saddr, 0x3D, data_buf, 2);
		if (rc < 0)
		{
			SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
			return rc;
		}	
		rc = ce1612_cmd(ce1612_saddr, 0x01, NULL, 0);
		if (rc < 0)
		{
			SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
			return rc;
		}	

	}
	else
	{
		data_buf[0] = 0x05;
		data_buf[1] = *(ce1612_regs.effect_data+effect);
		rc = ce1612_cmd(ce1612_saddr, 0x3D, data_buf, 2);
		
		data_buf[0] = 0x07;
		data_buf[1] = 0x06;
		rc = ce1612_cmd(ce1612_saddr, 0x3D, data_buf, 2);	// gamma default		
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}	
		rc = ce1612_cmd(ce1612_saddr, 0x01, NULL, 0);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}	
	}

	SKYCDBG("%s end\n",__func__);
	
	return rc;
}

static int32_t ce1612_set_whitebalance (int mode, int32_t whitebalance)
{
	uint8_t data_buf[2];
	int rc = 0;

	SKYCDBG("%s start\n",__func__);

	if(whitebalance < 1 || whitebalance > CE1612_WHITEBALANCE_MAX){
		SKYCERR("%s error. whitebalance=%d\n", __func__, whitebalance);
		return -EINVAL;
	}


	if(whitebalance == 1 || whitebalance == 2)		// auto
	{
		data_buf[0] = 0x00;
		rc = ce1612_cmd(ce1612_saddr, 0x1A, data_buf, 1);
		if (rc < 0)
		{
			SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
			return rc;
		}	
		
		data_buf[0] = 0x11;
		data_buf[1] =  0x00;
	}	
	else
	{
		data_buf[0] = 0x01;
		rc = ce1612_cmd(ce1612_saddr, 0x1A, data_buf, 1);
		if (rc < 0)
		{
			SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
			return rc;
		}	

		data_buf[0] = 0x10;
		data_buf[1] =  *(ce1612_regs.wb_data+whitebalance-1);
	}
	
	rc = ce1612_cmd(ce1612_saddr, 0x04, data_buf, 2);	
	
	rc = ce1612_cmd(ce1612_saddr, 0x01, NULL, 0);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}	

	SKYCDBG("%s end\n",__func__);

	return rc;
}

static int32_t ce1612_set_brightness(int32_t brightness)
{
	uint8_t data_buf[2];
	int rc = 0;

	SKYCDBG("%s start\n",__func__);

	if(brightness < 0 || brightness >= CE1612_BRIGHTNESS_MAX){
		SKYCERR("%s error. brightness=%d\n", __func__, brightness);
		return -EINVAL;
	}

	data_buf[0] = 0x02;
	data_buf[1] = *(ce1612_regs.bright_data+brightness);
	rc = ce1612_cmd(ce1612_saddr, 0x04, data_buf, 2);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}	
	rc = ce1612_cmd(ce1612_saddr, 0x01, NULL, 0);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}	

	SKYCDBG("%s end\n",__func__);
	
	return rc;
}

static int32_t ce1612_set_led_gpio_set(int32_t led_mode)
{
	int rc;
	int enable_flash_main_gpio = 0;	
	
	if(led_mode != CE1612_CFG_LED_MODE_OFF)
		enable_flash_main_gpio = ON;
	else
		enable_flash_main_gpio = OFF;

	//control ce1612 led flash main gpio
	rc = gpio_tlmm_config(GPIO_CFG(CE1612_LED_FLASH_ENABLE_GPIO, 0,
		GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);
	if (!rc)
	{
		SKYCDBG("%s enable_flash_main_gpio = %d\n", __func__, enable_flash_main_gpio);
		gpio_set_value(CE1612_LED_FLASH_ENABLE_GPIO,enable_flash_main_gpio);					
	}
	else
	{
		SKYCERR("ERR:%s ENABLE FLASH GPIO FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}

	return rc;
}


static int32_t ce1612_set_led_mode(int32_t led_mode)
{
	/* off, auto, on, movie */	

	int rc;
	uint8_t data_buf[4];
	uint8_t read_data =0;
	
	SKYCDBG("%s: led_mode=%d\n", __func__, led_mode);
	if ((led_mode < CE1612_CFG_LED_MODE_OFF) || (led_mode >= CE1612_CFG_LED_MODE_MAX)) {
		SKYCERR("%s: -EINVAL, led_mode=%d\n", __func__, led_mode);
		return -EINVAL;
	}

	rc = ce1612_cmd_read(0x24, &read_data, 1);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}

#if 1 // AF-T state check
	rc = ce1612_cmd_read(0x2D, data_buf, 2);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}

	if(data_buf[0] == 0x01)
		read_data = 0;
#endif

	if(read_data & 0x01)
	{
		rc = ce1612_cmd_poll(ce1612_saddr, 0x35, 0, 0);	// Lens Stop
		if (rc < 0)
		{
			SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
			return rc;
		}	
	}

//	if((led_mode != CE1612_CFG_LED_MODE_OFF)&&(led_mode != CE1612_CFG_LED_MODE_AUTO))
	rc = ce1612_set_led_gpio_set(led_mode);


	//control ce1612 isp gpio
	switch(led_mode)
	{
		case CE1612_CFG_LED_MODE_OFF:
			SKYCDBG("CE1612_CFG_LED_MODE_OFF SET\n");
			data_buf[0] = 0x00;
			data_buf[1] = 0x00;
			rc = ce1612_cmd(ce1612_saddr, 0x06, data_buf, 2);
			
			data_buf[0] = 0x01;
			data_buf[1] = 0x00;
			rc = ce1612_cmd(ce1612_saddr, 0xB2, data_buf, 2);

			data_buf[0] = 0x03;
			data_buf[1] = 0x00;
			rc = ce1612_cmd(ce1612_saddr, 0xB2, data_buf, 2);
			break;
			
		case CE1612_CFG_LED_MODE_AUTO:
			SKYCDBG("CE1612_CFG_LED_MODE_AUTO SET\n");
			
			data_buf[0] = 0x00;
			data_buf[1] = 0x00;
			rc = ce1612_cmd(ce1612_saddr, 0x06, data_buf, 2);			
			
/*			data_buf[0] = 0x01;
			data_buf[1] = 0x00;
			data_buf[2] = 0x00;
			data_buf[3] = 0x00;
			rc = ce1612_cmd(ce1612_saddr, 0xB3, data_buf, 4);

			data_buf[0] = 0x01;
			data_buf[1] = 0x00; //0x02;  //AF flash off
			rc = ce1612_cmd(ce1612_saddr, 0xB2, data_buf, 2);
*/
#if 0	// 발광량 설정 안함. (auto)
			data_buf[0] = 0x03;
			data_buf[1] = 0x00;
			data_buf[2] = 0x15;
			data_buf[3] = 0x00;
			rc = ce1612_cmd(ce1612_saddr, 0xB3, data_buf, 4);
#endif
			data_buf[0] = 0x03;
			data_buf[1] = 0x02;
			rc = ce1612_cmd(ce1612_saddr, 0xB2, data_buf, 2);
			break;	
			
		case CE1612_CFG_LED_MODE_ON:
			SKYCDBG("CE1612_CFG_LED_MODE_ON SET\n");

			data_buf[0] = 0x00;
			data_buf[1] = 0x00;
			rc = ce1612_cmd(ce1612_saddr, 0x06, data_buf, 2);
			
/*			data_buf[0] = 0x01;
			data_buf[1] = 0x00;
			data_buf[2] = 0x00;
			data_buf[3] = 0x00;
			rc = ce1612_cmd(ce1612_saddr, 0xB3, data_buf, 4);

			data_buf[0] = 0x01;
			data_buf[1] = 0x00; //0x01;  //AF flash off
			rc = ce1612_cmd(ce1612_saddr, 0xB2, data_buf, 2);
*/
#if 0	// 발광량 설정 안함. (auto)
			data_buf[0] = 0x03;
			data_buf[1] = 0x00;
			data_buf[2] = 0x15;
			data_buf[3] = 0x00;
			rc = ce1612_cmd(ce1612_saddr, 0xB3, data_buf, 4);
#endif
			data_buf[0] = 0x03;
			data_buf[1] = 0x01;
			rc = ce1612_cmd(ce1612_saddr, 0xB2, data_buf, 2);
			break;
			
		case CE1612_CFG_LED_MODE_MOVIE:
			SKYCDBG("CE1612_CFG_LED_MODE_MOVIE SET\n");
			data_buf[0] = 0x01;
			data_buf[1] = 0x00;
			rc = ce1612_cmd(ce1612_saddr, 0xB2, data_buf, 2);

			data_buf[0] = 0x03;
			data_buf[1] = 0x00;
			rc = ce1612_cmd(ce1612_saddr, 0xB2, data_buf, 2);
			
			data_buf[0] = 0x00;
			data_buf[1] = 0x00;
			data_buf[2] = 0x00;
			data_buf[3] = 0x00;
			rc = ce1612_cmd(ce1612_saddr, 0xB3, data_buf, 4);


			data_buf[0] = 0x00;
			data_buf[1] = 0x01;
			rc = ce1612_cmd(ce1612_saddr, 0x06, data_buf, 2);
			break;

		case CE1612_CFG_LED_MODE_FLASH:
			SKYCDBG("CE1612_CFG_LED_MODE_MOVIE SET\n");
			data_buf[0] = 0x01;
			data_buf[1] = 0x00;
			rc = ce1612_cmd(ce1612_saddr, 0xB2, data_buf, 2);

			data_buf[0] = 0x03;
			data_buf[1] = 0x00;
			rc = ce1612_cmd(ce1612_saddr, 0xB2, data_buf, 2);
			
			data_buf[0] = 0x00;
			data_buf[1] = 0x00;
			data_buf[2] = 0x15;
			data_buf[3] = 0x00;
			rc = ce1612_cmd(ce1612_saddr, 0xB3, data_buf, 4);


			data_buf[0] = 0x00;
			data_buf[1] = 0x01;
			rc = ce1612_cmd(ce1612_saddr, 0x06, data_buf, 2);
			break;
		default:		
			break;			
	}

	if(read_data & 0x01)
	{
		rc = ce1612_cmd(ce1612_saddr, 0x23, 0, 0);	// AF Start
		if (rc < 0)
		{
			SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
			return rc;
		}	
	}

/*
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
	}			
*/
	return rc;
}

static int32_t ce1612_set_auto_focus(int8_t autofocus)
{
	int32_t rc = 0;
	uint8_t read_data =0;
	uint8_t data_buf[4];

	SKYCDBG("%s  auto_focus = %d\n",__func__, autofocus);
	if ((autofocus < 0) || (autofocus > 4))
	{
		SKYCERR("%s FAIL!!! return~~  autofocus = %d\n",__func__,autofocus);
		return 0;//-EINVAL;
	}

#if 1 // AF-T state check
	rc = ce1612_cmd_read(0x2D, data_buf, 2);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}

	if(data_buf[0] == 0x01)
		return 0;
#endif

	SKYCDBG("%s START~ autofocus mode = %d\n",__func__, autofocus);

	rc = ce1612_cmd_read(0x24, &read_data, 1);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}

	if(read_data & 0x01)
	{
		rc = ce1612_cmd_poll(ce1612_saddr, 0x35, 0, 0);	// Lens Stop
		if (rc < 0)
		{
			SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
			return rc;
		}	
	}
	
	switch(autofocus)
	{
		case 4:	// SOPT			

			break;
		case 1:	// MACRO
			data_buf[0] = 0x00;
			rc = ce1612_cmd(ce1612_saddr, 0x42, data_buf, 1);
		
			data_buf[0] = 0x01;
			rc = ce1612_cmd_poll(ce1612_saddr, 0x20, data_buf, 1);
			if (rc < 0)
			{
				SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
				return rc;
			}		
		      

		default:	// NORMAL
			data_buf[0] = 0x00;
			rc = ce1612_cmd(ce1612_saddr, 0x42, data_buf, 1);
			
			data_buf[0] = 0x00;
			rc = ce1612_cmd_poll(ce1612_saddr, 0x20, data_buf, 1);
			if (rc < 0)
			{
				SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
				return rc;
			}			
	}

	rc = ce1612_cmd_poll(ce1612_saddr, 0x23, 0, 0);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}
	SKYCDBG("%s END~ autofocus mode = %d\n",__func__, autofocus);

	return rc;
}

static int32_t ce1612_set_iso(int32_t iso)
{
	uint8_t data_buf[2];
	int rc = 0;

	SKYCDBG("%s start\n",__func__);

	if(iso < 0 || iso >= CE1612_ISO_MAX){
		SKYCERR("%s error. iso=%d\n", __func__, iso);
		return 0; //-EINVAL;
	}

	data_buf[0] = 0x01;
	data_buf[1] = *(ce1612_regs.iso_data+iso);
	rc = ce1612_cmd(ce1612_saddr, 0x04, data_buf, 2);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}	
	rc = ce1612_cmd(ce1612_saddr, 0x01, NULL, 0);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}	

	SKYCDBG("%s end\n",__func__);	
	
	return rc;
}

static int32_t ce1612_set_scene_mode(int mode, int32_t scene_mode)
{
	uint8_t data_buf[2];
	int rc = 0;

	SKYCDBG("%s start\n",__func__);

	if(scene_mode < 0 || scene_mode >= CE1612_SCENE_MAX+1){
		SKYCERR("%s error. scene_mode=%d\n", __func__, scene_mode);
		return 0; //-EINVAL;
	}

	if(scene_mode == 0)	//OFF
	{
		data_buf[0] = 0x00;
		data_buf[1] = 0x00;
		rc = ce1612_cmd(ce1612_saddr, 0x82, data_buf, 2);

		data_buf[0] = 0x00;
		rc = ce1612_cmd(ce1612_saddr, 0x1A, data_buf, 1);
	}
	else if(scene_mode == CE1612_SCENE_MAX) //AUTO
	{
		data_buf[0] = 0x01;
		data_buf[1] = 0x00;
		rc = ce1612_cmd(ce1612_saddr, 0x82, data_buf, 2);

		data_buf[0] = 0x00;
		rc = ce1612_cmd(ce1612_saddr, 0x1A, data_buf, 1);

	}
	else	//MANUAL
	{
		data_buf[0] = 0x02;
		data_buf[1] = *(ce1612_regs.scene_data+scene_mode);
		rc = ce1612_cmd(ce1612_saddr, 0x82, data_buf, 2);
		if (rc < 0)
		{
			SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
			return rc;
		}	
		switch(scene_mode)
		{
		case 8:	//sunset
			data_buf[0] = 0x01;
			rc = ce1612_cmd(ce1612_saddr, 0x1A, data_buf, 1);
			data_buf[0] = 0x10;
			data_buf[1] = 0x03;
			rc = ce1612_cmd(ce1612_saddr, 0x04, data_buf, 2);
			break;
#if 0
		case 1:	//portrait
			data_buf[0] = 0x01;	//face detection assist
			rc = ce1612_cmd(ce1612_saddr, 0x42, data_buf, 1);
#endif			
		default:
			data_buf[0] = 0x00;
			rc = ce1612_cmd(ce1612_saddr, 0x1A, data_buf, 1);
		}	
	}

	rc = ce1612_cmd(ce1612_saddr, 0x01, NULL, 0);

	SKYCDBG("%s end\n",__func__);
	
	return rc;
}

static int32_t ce1612_set_focus_step(int mode, int32_t focus_step)
{
	int32_t rc = 0;

	if ((focus_step < 0) || (focus_step >= CE1612_CFG_FOCUS_STEP_MAX))
	{
		SKYCERR("%s FAIL!!! return~~  focus_step = %d\n",__func__,focus_step);
		return 0;//-EINVAL;
	}

#if 0 //jjhwang
	{
		rc = ce1612_i2c_write_table(ce1612_regs.focus_step_cfg_settings[focus_step],	
						ce1612_regs.focus_step_cfg_settings_size);
	}

	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}		
#endif
	SKYCDBG("%s end\n",__func__);

	return rc;
}

static int32_t ce1612_set_antishake(int32_t antishake)
{
	uint8_t data_buf[2];
	int32_t rc = 0;

	if ((antishake < 0) || (antishake >= CE1612_CFG_ANTISHAKE_MAX))
	{
		SKYCERR("%s FAIL!!! return~~  antishake = %d\n",__func__,antishake);
		return 0;//-EINVAL;
	}

	data_buf[0] = antishake;
	rc = ce1612_cmd(ce1612_saddr, 0x5B, data_buf, 1);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}	

	SKYCDBG("%s end\n",__func__);
	
	return rc;
}

static int32_t ce1612_set_exposure_mode(int32_t exposure)
{
	uint8_t data_buf[2];
	int32_t rc = 0;

	SKYCDBG("%s  exposure = %d\n",__func__, exposure);

	if ((exposure < 0) || (exposure >= CE1612_EXPOSURE_MAX))
	{
		SKYCERR("%s FAIL!!! return~~  exposure = %d\n",__func__,exposure);
		return 0;//-EINVAL;
	}

	data_buf[0] = 0x00;
	data_buf[1] = *(ce1612_regs.exposure_data+exposure);
	rc = ce1612_cmd(ce1612_saddr, 0x04, data_buf, 2);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}	
	rc = ce1612_cmd(ce1612_saddr, 0x01, NULL, 0);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}	

	SKYCDBG("%s end\n",__func__);
	
	return rc;
}

static int32_t ce1612_set_reflect(int32_t reflect)
{
	int32_t rc = 0;
#if 0 //jjhwang
	int32_t i = 0;
	//int8_t npolling = -1;
	uint16_t npolling = -1;

	SKYCDBG("%s  reflect = %d\n",__func__, reflect);

	if ((reflect < 0) || (reflect >= CE1612_CFG_REFLECT_MAX))
	{
		SKYCERR("%s FAIL!!! return~~  reflect = %d\n",__func__,reflect);
		return 0;//-EINVAL;
	}

	{
		rc = ce1612_i2c_write_table(ce1612_regs.reflect_cfg_settings[reflect],
					ce1612_regs.reflect_cfg_settings_size);
	}

	for (i = 0; i < CE1612_POLLING_RETRY; i++) {
		rc = ce1612_i2c_read(ce1612_saddr,
		0x8404, &npolling,TRIPLE_LEN);
		if (rc < 0)
		{
			SKYCERR("ERR:%s POLLING FAIL!!!rc=%d return~~\n", __func__, rc);
			//return rc;
		}
		SKYCDBG("%s: retry npolling = %x, count = %d\n", __func__,npolling, i);
		if (npolling == 0) {				
			return 0;
		}		
		msleep(20);
	}	
 
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}		
#endif
	SKYCDBG("%s end\n",__func__);

	return rc;
}

static int32_t ce1612_set_focus_rect(uint32_t focus_rect)
{
#define FOCUS_RECT_SIZE_1 63
	uint8_t data_buf[10];
	int32_t xleft, xright, ytop, ybottom;
	int32_t rc = 0;
	uint8_t read_data =0;

	SKYCDBG("%s  focus_rect = %x\n",__func__, focus_rect);

	if (focus_rect == 0) {
		data_buf[0] = 0x00;
		rc = ce1612_cmd(ce1612_saddr, 0x42, data_buf, 1);

		return rc;
	}

	rc = ce1612_cmd_read(0x24, &read_data, 1);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}

	if(read_data & 0x01)
	{
#if 1 // AF-T state check
		rc = ce1612_cmd_read(0x2D, data_buf, 2);
		if (rc < 0)
		{
			SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
			return rc;
		}

		if(data_buf[0] == 0x01)
		{
			data_buf[2] = 0x02;
			rc = ce1612_cmd(ce1612_saddr, 0x2C, &data_buf[2], 1);
			if (rc < 0)
			{
				SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
				return rc;
			}	
		}		
#endif
		rc = ce1612_cmd_poll(ce1612_saddr, 0x35, 0, 0);	// Lens Stop
		if (rc < 0)
		{
			SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
			return rc;
		}
#if 1 // AF-T state check
		if(data_buf[0] == 0x01)
		{
			data_buf[2] = 0x01;
			rc = ce1612_cmd(ce1612_saddr, 0x2C, &data_buf[2], 1);
			if (rc < 0)
			{
				SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
				return rc;
			}	
		}		
#endif	
	}

	xleft= (int32_t)((focus_rect & 0xffff0000) >> 16);
	ytop = (int32_t)(focus_rect & 0xffff);
	
	xright = xleft + FOCUS_RECT_SIZE_1;
	ybottom = ytop + FOCUS_RECT_SIZE_1;
	
	SKYCDBG("%s  xleft = %d, ytop = %d, xright = %d, ybottom = %d\n",__func__, xleft, ytop, xright, ybottom);
		
	data_buf[0] = 0x05;
	data_buf[1] = 0x03;
	data_buf[2] = xleft & 0xff;
	data_buf[3] = (xleft >> 8) & 0xff;
	data_buf[4] = ytop & 0xff;
	data_buf[5] = (ytop >> 8) & 0xff;
	data_buf[6] = xright & 0xff;
	data_buf[7] = (xright >> 8) & 0xff;
	data_buf[8] = ybottom & 0xff;
	data_buf[9] = (ybottom >> 8) & 0xff;
	
	rc = ce1612_cmd(ce1612_saddr, 0x41, data_buf, 10);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}	

	data_buf[0] = 0x05;
	rc = ce1612_cmd(ce1612_saddr, 0x42, data_buf, 1);

	rc = ce1612_poll(ce1612_saddr, 0x43, 0x05, 10, 1000);
	if (rc < 0)
	{
		SKYCERR("%s : uploader polling ERROR !!!\n",__func__);
		return rc;
	}
	
	SKYCDBG("%s end\n",__func__);
	
	return rc;	
}

static long ce1612_set_continuous_af(int caf)
{
	uint8_t data_buf[2];
	uint8_t read_data =0;
	
	int rc = 0;

	SKYCDBG("%s start : caf = %d\n",__func__, caf);

	rc = ce1612_cmd_read(0x2D, &read_data, 1);	// check AF-T
	if(read_data == 1)
	{
		return 0;
	}

	if(caf)
	{
		rc = ce1612_cmd_read(0x24, &read_data, 1);
		if (rc < 0)
		{
			SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
			return rc;
		}

		if(read_data & 0x01)
		{
			rc = ce1612_cmd_poll(ce1612_saddr, 0x35, 0, 0);	// Lens Stop
			if (rc < 0)
			{
				SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
				return rc;
			}	
		}
	
		data_buf[0] = 0x02;
		rc = ce1612_cmd_poll(ce1612_saddr, 0x20, data_buf, 1);
		if (rc < 0)
		{
			SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
			return rc;
		}		
	      
		SKYCDBG("AF-C start\n");
		rc = ce1612_cmd(ce1612_saddr, 0x23, 0, 0);
		if (rc < 0)
		{
			SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
			return rc;
		}
	}
	else
	{
#if 0
		rc = ce1612_cmd(ce1612_saddr, 0x35, 0, 0);
#else
		rc = ce1612_cmd_poll(ce1612_saddr, 0x35, 0, 0);
#endif
		if (rc < 0)
		{
			SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
			return rc;
		}
		
		data_buf[0] = 0x00;
		rc = ce1612_cmd_poll(ce1612_saddr, 0x20, data_buf, 1);
		if (rc < 0)
		{
			SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
			return rc;
		}		
		
	}

	SKYCDBG("%s end\n",__func__);
	
	return rc;
}


#if 1 //def F_PANTECH_CAMERA_ADD_CFG_DIMENSION
static int32_t ce1612_set_dimension(struct dimension_cfg *dimension)
{
	int32_t rc = 0;

#if 1 //def F_PANTECH_CAMERA_1080P_PREVIEW
	uint8_t read_data =0;
	uint8_t data_buf[4];

	rc = ce1612_cmd_read(0x24, &read_data, 1);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}

#if 1 // AF-T state check
	rc = ce1612_cmd_read(0x2D, data_buf, 2);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}

	if(data_buf[0] == 0x01)
		read_data = 0;
#endif

	if(read_data & 0x01)
	{
		rc = ce1612_cmd_poll(ce1612_saddr, 0x35, 0, 0);	// Lens Stop
		if (rc < 0)
		{
			SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
			return rc;
		}	
	}

	memcpy(&ce1612_dimension, dimension, sizeof(struct dimension_cfg));	

	SKYCDBG("%s: preview=%dx%d, snapshot=%dx%d, video=%dx%d\n", __func__,
		dimension->prev_dx, dimension->prev_dy,
		dimension->pict_dx, dimension->pict_dy,
		dimension->video_dx, dimension->video_dy);

	if((ce1612_dimension.video_dx == 1920)&&(ce1612_dimension.video_dy == 1080))
	{		
       	ce1612_config_1080P_preview = true;

		// Preview stop
		data_buf[0] = 0x00;
		rc = ce1612_cmd(ce1612_saddr, 0x6B, data_buf, 1);

		rc = ce1612_poll(ce1612_saddr, 0x6C, 0x00, 10, 1000);
		if (rc < 0)
		{
			SKYCERR("%s : Preview Stop polling ERROR !!!\n",__func__);
			return rc;
		}	
		
		data_buf[0] = 0x1E;	// Full HD
		data_buf[1] = 0x03;
		data_buf[2] = 0x00;		
		rc = ce1612_cmd(ce1612_saddr, 0x54, data_buf, 3);
		if (rc < 0)
		{
			SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
			return rc;
		}		
		
		// Preview start (PREVIEW)
		data_buf[0] = 0x01;
		rc = ce1612_cmd(ce1612_saddr, 0x6B, data_buf, 1);

		rc = ce1612_poll(ce1612_saddr, 0x6C, 0x08, 10, 1000);
		if (rc < 0)
		{
			SKYCERR("%s : Preview Start polling ERROR !!!\n",__func__);
			return rc;
		}	
		
	}
	else
	{
		if(ce1612_config_1080P_preview == true)
		{

			// Preview stop
			data_buf[0] = 0x00;
			rc = ce1612_cmd(ce1612_saddr, 0x6B, data_buf, 1);

			rc = ce1612_poll(ce1612_saddr, 0x6C, 0x00, 10, 1000);
			if (rc < 0)
			{
				SKYCERR("%s : Preview Stop polling ERROR !!!\n",__func__);
				return rc;
			}	

			data_buf[0] = 0x1C;	// SXGA
			data_buf[1] = 0x01;
			data_buf[2] = 0x00;		
			rc = ce1612_cmd(ce1612_saddr, 0x54, data_buf, 3);
			if (rc < 0)
			{
				SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
				return rc;
			}	

			// Preview start (PREVIEW)
			data_buf[0] = 0x01;
			rc = ce1612_cmd(ce1612_saddr, 0x6B, data_buf, 1);

			rc = ce1612_poll(ce1612_saddr, 0x6C, 0x08, 10, 1000);
			if (rc < 0)
			{
				SKYCERR("%s : Preview Start polling ERROR !!!\n",__func__);
				return rc;
			}	

			ce1612_config_1080P_preview = false;
		}			
	}
#endif


	if(read_data & 0x01)
	{
		rc = ce1612_cmd(ce1612_saddr, 0x23, 0, 0);	// AF Start
		if (rc < 0)
		{
			SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
			return rc;
		}	
	}
	
	SKYCDBG("%s end rc = %d\n",__func__, rc);

	return rc;
}
#endif

static int32_t ce1612_set_antibanding(int32_t antibanding)
{
	uint8_t data_buf[2];
	int32_t rc = 0;

	if ((antibanding < 0) || (antibanding >= CE1612_CFG_FLICKER_MAX))
	{
		SKYCERR("%s FAIL!!! return~~  antibanding = %d\n",__func__,antibanding);
		return 0;//-EINVAL;
	}

	data_buf[0] = *(ce1612_regs.exposure_data+antibanding);
	rc = ce1612_cmd(ce1612_saddr, 0x14, data_buf, 1);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}
	
	SKYCDBG("%s end\n",__func__);
	
	return rc;
}

static int32_t ce1612_set_preview_fps(int32_t preview_fps)
{
	/* 0 : variable 30fps, 1 ~ 30 : fixed fps */
	/* default: variable 8 ~ 30fps */
	uint8_t data_buf[4];
	int32_t rc = 0;

	if ((preview_fps < C_PANTECH_CAMERA_MIN_PREVIEW_FPS) || 
		(preview_fps > C_PANTECH_CAMERA_MAX_PREVIEW_FPS)) {
		SKYCERR("%s: -EINVAL, preview_fps=%d\n", 
			__func__, preview_fps);
		return 0; //-EINVAL;
	}

	SKYCDBG("%s: preview_fps=%d\n", __func__, preview_fps);

	if(preview_fps == C_PANTECH_CAMERA_MAX_PREVIEW_FPS)
	{
		data_buf[0] = 0x01;
		data_buf[1] = 0xFF;
		data_buf[2] = 0xFF;
		data_buf[3] = 0x00;
	}
	else
	{
		data_buf[0] = 0x01;
		data_buf[1] = preview_fps & 0xFF;
		data_buf[2] = (preview_fps >> 8) & 0xFF;
		data_buf[3] = 0x00;
	}
	
	rc = ce1612_cmd(ce1612_saddr, 0x5A, data_buf, 4);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}	

	SKYCDBG("%s end rc = %d\n",__func__, rc);

	return rc;
}

static int32_t ce1612_set_wdr(int32_t wdr)
{
	uint8_t data_buf[2];
	int32_t rc = 0;

	if ((wdr != 0) && (wdr != 1))
	{
		SKYCERR("%s FAIL!!! return~~  wdr = %d\n",__func__,wdr);
		return 0;//-EINVAL;
	}

	data_buf[0] = wdr;
	rc = ce1612_cmd(ce1612_saddr, 0x88, data_buf, 1);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}	

	SKYCDBG("%s end\n",__func__);
	
	return rc;
}

#ifdef CONFIG_PANTECH_CAMERA//IRQ
#ifndef AF_STATE_POLLING
static void ce1612_work_func(struct work_struct *work)
{
	int8_t d[4];
	uint8_t data_buf[4];	
	
	int rc = 0;
	SKYCDBG("%s\n", __func__);
	
	rc = ce1612_cmd_read(0xd0, d, 4);
	if (rc < 0) {
		SKYCDBG("%s read err\n", __func__);
		return;
	}
	SKYCDBG("%s status = %02x.%02x.%02x.%02x", __func__, d[0], d[1], d[2], d[3]);

	/* need lock */
	if (d[1] & 0x20) // bit 5, AF done
	{
		ce1612_irq_stat = 2;
	}
	else if (d[1] & 0x04) // bit 2, AF trigger
	{
		ce1612_irq_stat = 1;
		data_buf[0] = 0x00;
		rc = ce1612_cmd(ce1612_saddr, 0x42, data_buf, 1);
	}
}

static irqreturn_t ce1612_interrupt(int irq, void *dev_id)
{
//	disable_irq_nosync(gpio_to_irq(57));
	schedule_work(&(ce1612_sensorw->work));
	return IRQ_HANDLED;
}
#endif

uint32_t ce1612_readirq(void)
{
#ifdef AF_STATE_POLLING
	int8_t rdata;
	int rc = 0;
	
	rc = ce1612_cmd_read(0x24, &rdata, 1);
	if((rdata & 0x01)&& current_af_state==0)
		ce1612_irq_stat = 1;
	else if(!(rdata & 0x01)&& current_af_state==1)
		ce1612_irq_stat = 2;
	else
		ce1612_irq_stat = 0;
	
	current_af_state == 	(rdata & 0x01);	
#endif

	/* need lock */
	return ce1612_irq_stat;
}

#endif

static int32_t ce1612_set_caf_camera(int32_t caf)
{
	uint8_t data_buf[2];	
	int rc = 0;

	SKYCDBG("%s start : caf = %d\n",__func__, caf);

	if ((caf != 0) && (caf != 1))
	{
		SKYCERR("%s FAIL!!! return~~  caf = %d\n",__func__,caf);
		return 0;//-EINVAL;
	}

	data_buf[0] = 0x00;
	rc = ce1612_cmd_poll(ce1612_saddr, 0x20, data_buf, 1); //set focus mode normal

	data_buf[0] = caf;
	rc = ce1612_cmd(ce1612_saddr, 0x2C, data_buf, 1);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}	

	SKYCDBG("%s end\n",__func__);
	
	return rc;
}

static int32_t ce1612_lens_stability(void)
{
	int32_t rc = 0;
	uint8_t data_buf[4];
	uint8_t rdata = 0;

	SKYCDBG("%s  start\n",__func__);

	rc = ce1612_cmd_read(0x6C, &rdata, 1);

	if((rdata > 0) && (rdata < 8))
	{
		return 0;
	}
	
	rc = ce1612_cmd_poll(ce1612_saddr, 0x35, 0, 0);	// Lens Stop
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}	

	caf_camera_flag = 0;
	ce1612_set_caf_camera(caf_camera_flag); // AF-T stop	

#if 0
	data_buf[0] = 0x01;
	data_buf[0] = 0x00;
	data_buf[0] = 0x00;
	rc = ce1612_cmd(ce1612_saddr, 0x33, data_buf, 3);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}
#else
	data_buf[0] = 0x00;
	rc = ce1612_cmd_poll(ce1612_saddr, 0x20, data_buf, 1);
#endif

	SKYCDBG("%s END~\n",__func__);

	return rc;
}


static int32_t ce1612_video_config(void)
{
	int32_t rc = 0;
	uint8_t data_buf[4];
	uint8_t rdata = 0;

#if 1 // test cts
	if(f_stop_capture == 1)
	{
		f_stop_capture = 0;
		
		rc = ce1612_poll(ce1612_saddr, 0x6C, 0x08, 10, 1000);
		if (rc < 0)
		{
			SKYCERR("%s : Preview Start polling ERROR !!!\n",__func__);
			return rc;
		}	
		goto preview_ok;
	}
#endif	


	rc = ce1612_cmd_read(0x6C, &rdata, 1);

	if((rdata > 0) && (rdata < 8))
	{
#if 1
		// stop Capture	
		rc = ce1612_cmd(ce1612_saddr, 0x75, 0, 0);
		if (rc < 0)
		{
			SKYCERR("ERR:%s Capture Stop command FAIL!!!rc=%d return~~\n", __func__, rc);
			return rc;
		}	
#endif
		rc = ce1612_poll(ce1612_saddr, 0x6C, 0x00, 10, 1000);
		if (rc < 0)
		{
			SKYCERR("%s : Capture Stop polling ERROR !!!\n",__func__);
			return rc;
		}		
	}
	else if(rdata == 8)
	{
		goto preview_ok;
	}
	else if(rdata == 9)
	{
		rc = ce1612_poll(ce1612_saddr, 0x6C, 0x08, 10, 1000);
		if (rc < 0)
		{
			SKYCERR("%s : Preview Start polling ERROR !!!\n",__func__);
			return rc;
		}	
		goto preview_ok;
	}
	
#if 0
	// Preview setting (Sensor)
	data_buf[0] = 0x1C;	// SXGA
	data_buf[1] = 0x01;
	data_buf[2] = 0x00;	
	rc = ce1612_cmd(ce1612_saddr, 0x54, data_buf, 3);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}

	// Buffering setting
#if 0	//jpeg
	data_buf[0] = 0x00;
	data_buf[1] = 0x00;
	data_buf[2] = 0x01;	
	data_buf[3] = 0x01;	
	rc = ce1612_cmd(ce1612_saddr, 0x73, data_buf, 4);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}
#else	// yuv
	data_buf[0] = 0x21;
	data_buf[1] = 0x05;
	data_buf[2] = 0x00;	
	data_buf[3] = 0x00;	
	rc = ce1612_cmd(ce1612_saddr, 0x73, data_buf, 4);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}
#endif
#endif
	SKYCERR("%s : Preview Start CMD !!!\n",__func__);

	// AE/AWB enable
	data_buf[0] = 0x00;
	rc = ce1612_cmd(ce1612_saddr, 0x11, data_buf, 1);

	// Preview start (PREVIEW)
	data_buf[0] = 0x01;
	rc = ce1612_cmd(ce1612_saddr, 0x6B, data_buf, 1);

	rc = ce1612_poll(ce1612_saddr, 0x6C, 0x08, 10, 1000);
	if (rc < 0)
	{
		SKYCERR("%s : Preview Start polling ERROR !!!\n",__func__);
		return rc;
	}	

preview_ok:

	if(caf_camera_flag)
		ce1612_set_caf_camera(caf_camera_flag);

	f_stop_capture = 0;	// test cts

	SKYCDBG("%s end rc = %d\n",__func__, rc);
	return rc;
}

static int32_t ce1612_snapshot_config(void)
{
	int32_t rc = 0;
//	int i=0;
//	uint16_t read_data =0;
	uint8_t data_buf[10];

	/* set snapshot resolution to 3264x2448 */
	SKYCDBG("%s start\n",__func__);

	// Buffering setting
#if 0	//jpeg
	data_buf[0] = 0x00;
	data_buf[1] = 0x01;
	data_buf[2] = 0x00;		
	rc = ce1612_cmd(ce1612_saddr, 0x95, data_buf, 3);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}
#endif
	if(caf_camera_flag)
		ce1612_set_caf_camera(0);

	// Data output setting
	data_buf[0] = 0x00;
	rc = ce1612_cmd(ce1612_saddr, 0x65, data_buf, 1);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}	

	// Buffering Capture start
	rc = ce1612_cmd(ce1612_saddr, 0x74, 0, 0);

	SKYCDBG("%s end rc = %d\n",__func__, rc);
	
	return rc;
}

static int32_t ce1612_stop_capture(void)
{
	int rc = 0;
	uint8_t rdata = 0;

	SKYCDBG("%s start\n",__func__);

	rc = ce1612_cmd_read(0x6C, &rdata, 1);
	
	if((rdata > 0) && (rdata < 8))
	{
#if 1	//start Preview
		uint8_t data_buf[4];
		// AE/AWB enable
		data_buf[0] = 0x00;
		rc = ce1612_cmd(ce1612_saddr, 0x11, data_buf, 1);

		// Preview start (PREVIEW)
		data_buf[0] = 0x01;
		rc = ce1612_cmd(ce1612_saddr, 0x6B, data_buf, 1);
		if (rc < 0)
		{
			SKYCERR("%s : Preview Start CMD ERROR !!!\n",__func__);
			return rc;
		}	
		
		f_stop_capture = 1;	// test cts
#else
		// stop Capture	
		rc = ce1612_cmd(ce1612_saddr, 0x75, 0, 0);
		if (rc < 0)
		{
			SKYCERR("ERR:%s Capture Stop command FAIL!!!rc=%d return~~\n", __func__, rc);
			return rc;
		}	
#endif
	}

	SKYCDBG("%s end\n",__func__);
	
	return rc;
}


static long ce1612_set_sensor_mode(int mode)
{
//	uint16_t clock;
	int32_t rc = 0;
       struct msm_camera_csi_params ce1612_csi_params;

	SKYCDBG("%s start\n",__func__);
	SKYCDBG("ce1612_set_sensor_mode mode = %d!\n",mode);

	switch (mode) {
	case SENSOR_PREVIEW_MODE:
		b_snapshot_flag = false;	
#if 1		
		if(config_csi == 0)
		{			
			SKYCDBG("ce1612_set_sensor_mode config_csi E\n");
			ce1612_csi_params.lane_cnt = 2; // 4;
			ce1612_csi_params.data_format = CSI_8BIT; //CSI_10BIT;
			ce1612_csi_params.lane_assign = 0xe4;
			ce1612_csi_params.dpcm_scheme = 0;
			ce1612_csi_params.settle_cnt = 0x14;
			rc = msm_camio_csi_config(&ce1612_csi_params);			
			//msleep(ce1612_delay_msecs_stream);
			config_csi = 1;
			SKYCDBG("ce1612_set_sensor_mode config_csi X\n");
#ifdef CONFIG_PANTECH_CAMERA//IRQ
{
	int8_t d[4];
#if 1	// 
	SKYCDBG("%s\n", __func__);
	
	rc = ce1612_cmd_read(0xd0, d, 4);
	if (rc < 0) {
		SKYCDBG("%s read err\n", __func__);
		return rc;
	}
#endif
	d[0] = 0xff;
#ifdef AF_STATE_POLLING
	d[1] = 0xff;
#else
	d[1] = 0xdb;
#endif
	d[2] = 0xff;
	d[3] = 0xff;
	rc = ce1612_cmd(ce1612_saddr, 0xd1, d, 4);
	if (rc < 0) {
		SKYCDBG("%s bryan err", __func__);
	}
}

#ifndef AF_STATE_POLLING
	enable_irq(gpio_to_irq(57));
#endif
	ce1612_irq_stat = 0;
#endif
		}	
#endif		
		rc = ce1612_video_config();
		SKYCDBG("ce1612_video_config, rc = %d \n", rc);
		if (rc < 0)
              {
			SKYCERR("ERR:SENSOR_PREVIEW_MODE FAIL!!!rc=%d return~~\n", rc);
			return rc;
		}

		break;
		
	case SENSOR_SNAPSHOT_MODE:
		if(!b_snapshot_flag) {
			rc = ce1612_snapshot_config();
			b_snapshot_flag = true;
			SKYCDBG("ce1612_snapshot_config, rc = %d \n", rc);
			if (rc < 0)
	              {
				SKYCERR("ERR:SENSOR_SNAPSHOT_MODE FAIL!!! rc=%d return~~\n", rc);
				return rc;
			}			
		}
		break;

	case SENSOR_RAW_SNAPSHOT_MODE:		
		break;

	default:
		return -EINVAL;
	}
	SKYCDBG("%s end rc = %d\n",__func__, rc);
	
	return rc;
}

static int ce1612_sensor_init_probe(const struct msm_camera_sensor_info *data)
{
//	uint16_t model_id = 0;
	int rc = 0;

	SKYCDBG("init entry \n");
#if 0 //jjhwang
	rc = ce1612_reset(ON);
	if (rc < 0) {
		SKYCERR("reset failed!\n");
		goto init_probe_fail;
	}
	msleep(5);
#endif

	SKYCDBG("ce1612_reg_init E \n");
	rc = ce1612_reg_init();
	SKYCDBG("ce1612_reg_init X \n");
	if (rc < 0)
		goto init_probe_fail;

	return rc;

init_probe_fail:
	SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
	return rc;

}

int ce1612_sensor_init(const struct msm_camera_sensor_info *data)
{
	int rc = 0;

	config_csi = 0;
#if 1 //def F_PANTECH_CAMERA_1080P_PREVIEW
       ce1612_config_1080P_preview = false;
#endif

	b_snapshot_flag = false;
	SKYCDBG(" ce1612_sensor_init E\n");

#ifdef F_CE1612_POWER	
	ce1612_vreg_init();
	rc = ce1612_power(ON);
	if (rc) {		
		SKYCERR(" ce1612_power failed rc=%d\n",rc);
		goto init_fail; 
	}
#endif
	ce1612_ctrl = kzalloc(sizeof(struct ce1612_ctrl_t), GFP_KERNEL);
	if (!ce1612_ctrl) {
		SKYCERR("ce1612_init failed!\n");
		rc = -ENOMEM;
		goto init_done;
	}

	if (data)
		ce1612_ctrl->sensordata = data;
#if 0 //jjhwang
	/* Input MCLK = 24MHz */
	SKYCDBG(" msm_camio_clk_rate_set E\n");
	msm_camio_clk_rate_set(24000000);
	SKYCDBG(" msm_camio_clk_rate_set X\n");
	msleep(5);
#endif
	rc = ce1612_sensor_init_probe(data);
	if (rc < 0) {
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		goto init_fail;
	}

#if 0
	rc = gpio_tlmm_config(GPIO_CFG(SENSOR_STANDBY, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);

	if (!rc) {
		SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
		gpio_set_value(139,1);
		msleep(20);
        }
#endif
init_done:
	return rc;

init_fail:
	kfree(ce1612_ctrl);
	return rc;
}

static int ce1612_init_client(struct i2c_client *client)
{
	/* Initialize the MSM_CAMI2C Chip */
	init_waitqueue_head(&ce1612_wait_queue);
	return 0;
}

int ce1612_sensor_config(void __user *argp)
{
	struct sensor_cfg_data cfg_data;
	int rc = 0;

	if (copy_from_user(&cfg_data,
			(void *)argp,
			sizeof(struct sensor_cfg_data)))
		return -EFAULT;

	/* down(&ce1612_sem); */

	SKYCDBG("ce1612_sensor_config, cfgtype = %d, mode = %d\n",
		cfg_data.cfgtype, cfg_data.mode);

		switch (cfg_data.cfgtype) {
		case CFG_SET_MODE:
			rc = ce1612_set_sensor_mode(
						cfg_data.mode);
			break;

		case CFG_SET_EFFECT:
			rc = ce1612_set_effect(cfg_data.mode,
						cfg_data.cfg.effect);
			break;

		case CFG_GET_AF_MAX_STEPS:
			break;
		case CFG_SET_WB:			
			rc = ce1612_set_whitebalance(cfg_data.mode, cfg_data.cfg.whitebalance);
			SKYCDBG("ce1612_set_whitebalance OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.whitebalance =[%d]\n", rc, cfg_data.mode, cfg_data.cfg.whitebalance);
			break;
		case CFG_SET_BRIGHTNESS:
			rc = ce1612_set_brightness(cfg_data.cfg.brightness);
			SKYCDBG("ce1612_set_brightness OK! rc = [%d], cfg_data.cfg.brightness =[%d]\n", rc, cfg_data.cfg.brightness);
			break;
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_LED_MODE
		case CFG_SET_LED_MODE:			
/*			if(cfg_data.cfg.led_mode == 1)
				led_auto = CE1612_CFG_LED_MODE_AUTO;
			else
				led_auto = CE1612_CFG_LED_MODE_OFF;
*/
			rc = ce1612_set_led_mode(cfg_data.cfg.led_mode);
			SKYCDBG("ce1612_set_led_mode OK! rc = [%d], cfg_data.cfg.led_mode =[%d]\n", rc, cfg_data.cfg.led_mode);
			break;
#endif			
		case CFG_AUTO_FOCUS:		
			rc = ce1612_set_auto_focus(cfg_data.cfg.focus.dir);
			SKYCDBG("ce1612_auto_focus OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.focus.dir =[%d]\n", rc, cfg_data.mode, cfg_data.cfg.focus.dir);
			break;
		case CFG_SET_ISO:
			rc = ce1612_set_iso(cfg_data.cfg.iso);
			SKYCDBG("%s(CFG_SET_ISO), rc=%d\n", __func__, rc);
			break;	
		case CFG_SET_SCENE_MODE:
			rc = ce1612_set_scene_mode(cfg_data.mode, cfg_data.cfg.scene_mode);
			SKYCDBG("%s(CFG_SET_SCENE_MODE), rc=%d\n", __func__, rc);
			break;	
		case CFG_SET_FOCUS_STEP:
			rc = ce1612_set_focus_step(cfg_data.mode, cfg_data.cfg.focus_step);
			SKYCDBG("%s(CFG_SET_FOCUS_STEP), rc=%d\n", __func__, rc);
			break;
		case CFG_SET_ANTISHAKE:
			rc = ce1612_set_antishake(cfg_data.cfg.antishake);
			SKYCDBG("%s(CFG_SET_ANTISHAKE), rc=%d\n", __func__, rc);
			break;
		case CFG_SET_EXPOSURE_MODE:
			rc = ce1612_set_exposure_mode(cfg_data.cfg.exposure);
			SKYCDBG("ce1612_set_exposure_mode OK! rc = [%d], cfg_data.cfg.exposure =[%d]\n", rc, cfg_data.cfg.exposure);
			break;
		case CFG_SET_FOCUS_RECT:
			rc = ce1612_set_focus_rect(cfg_data.cfg.focus_rect);
			SKYCDBG("%s(CFG_SET_FOCUS_RECT), rc=%d\n", __func__, rc);
			break;
		case CFG_SET_PREVIEW_FPS:
			rc = ce1612_set_preview_fps(cfg_data.cfg.preview_fps);
			SKYCDBG("ce1612_set_exposure_mode OK! rc = [%d], cfg_data.cfg.preview_fps =[%d]\n", rc, cfg_data.cfg.preview_fps);
			break;
		case CFG_SET_ANTIBANDING:
			rc = ce1612_set_antibanding(cfg_data.cfg.antibanding);
			SKYCDBG("%s: CFG_SET_ANTIBANDING, rc=%d\n", __func__, rc);
			break;
		case CFG_SET_REFLECT:
			rc = ce1612_set_reflect(cfg_data.cfg.reflect);
			SKYCDBG("ce1612_set_reflect OK! rc = [%d], cfg_data.cfg.reflect =[%d]\n", rc, cfg_data.cfg.reflect);
			break;
#if 1 //def F_PANTECH_CAMERA_ADD_CFG_DIMENSION
		case CFG_SET_DIMENSION:
			rc = ce1612_set_dimension(&cfg_data.cfg.dimension);
			SKYCDBG("%s: CFG_SET_DIMENSION, rc=%d\n", __func__, rc);
			break;
#endif
		case CFG_SET_CONTINUOUS_AF:
			rc = ce1612_set_continuous_af(cfg_data.cfg.continuous_af);
			SKYCDBG("%s: CFG_UPDATE_ISP, rc=%d\n", __func__, rc);
			break;		
		case CFG_SET_WDR:
			rc = ce1612_set_wdr(cfg_data.cfg.wdr);
			SKYCDBG("%s(CFG_SET_WDR), rc=%d\n", __func__, rc);
			break;
		case CFG_SET_CAF_CAMERA:
			caf_camera_flag = cfg_data.cfg.continuous_af;
			rc = ce1612_set_caf_camera(cfg_data.cfg.continuous_af);
			SKYCDBG("%s(CFG_SET_CAF_CAMERA), rc=%d\n", __func__, rc);
			break;
		case CFG_UPDATE_ISP:
			rc = ce1612_update_fw_user(&cfg_data);
			SKYCDBG("%s: CFG_UPDATE_ISP, rc=%d\n", __func__, rc);
			break;
		case CFG_GET_REG:		
		{
			uint8_t rdata = 0xC0;
			rc = ce1612_cmd_read((uint8_t)(cfg_data.cfg.reg & 0xff), &rdata, 1);
			cfg_data.cfg.reg = rdata & 0xff;
			
			if(copy_to_user((void *)argp,
					&cfg_data,
					sizeof(struct sensor_cfg_data)))
				return -EFAULT;

				
			SKYCDBG("%s(CFG_GET_REG), read data =0x%x\n", __func__, cfg_data.cfg.reg);
			break;		
		}
		case CFG_STOP_CAPTURE:
			rc = ce1612_stop_capture();
			SKYCDBG("%s: CFG_STOP_CAPTURE, rc=%d\n", __func__, rc);
			break;		
		
	
		default:
			rc = -EINVAL;
			SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
			break;
		}

	/* up(&ce1612_sem); */

	return rc;
}

int ce1612_sensor_release(void)
{
	int rc = 0;
	
	SKYCDBG("%s start\n",__func__);

#ifdef CONFIG_PANTECH_CAMERA//IRQ
#ifndef AF_STATE_POLLING
	disable_irq(gpio_to_irq(57));
#endif
#endif

	/* down(&ce1612_sem); */
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_LED_MODE	
//	nbrightness = 0;
	//ce1612_set_led_mode(CE1612_CFG_LED_MODE_OFF);
	ce1612_set_led_gpio_set(CE1612_CFG_LED_MODE_OFF);
#endif	
	ce1612_lens_stability();
#if 0	//jjhwang
	ce1612_reset(OFF);
#endif
#ifdef F_CE1612_POWER	
	SKYCERR(" ce1612_sensor_release E\n");	
	rc = ce1612_power(OFF);
	if (rc) {
		SKYCERR(" ce1612_power failed rc=%d\n",rc);		
	}
#endif

	kfree(ce1612_ctrl);
	ce1612_ctrl = NULL;
	/* up(&ce1612_sem); */

	SKYCDBG("%s end rc = %d\n",__func__, rc);
	return rc;
}

static int ce1612_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int rc = 0;
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		rc = -ENOTSUPP;
		goto probe_failure;
	}

	ce1612_sensorw = kzalloc(sizeof(struct ce1612_work), GFP_KERNEL);
	if (!ce1612_sensorw) {
		rc = -ENOMEM;
		goto probe_failure;
	}

#ifdef CONFIG_PANTECH_CAMERA//IRQ
#ifndef AF_STATE_POLLING
	INIT_WORK(&ce1612_sensorw->work, ce1612_work_func);
#endif
#endif

	i2c_set_clientdata(client, ce1612_sensorw);
	ce1612_init_client(client);
	ce1612_client = client;

	SKYCDBG("ce1612_probe succeeded!\n");

	return rc;

probe_failure:
	kfree(ce1612_sensorw);
	ce1612_sensorw = NULL;
	SKYCERR("ce1612_probe failed!\n");
	return rc;
}

static const struct i2c_device_id ce1612_i2c_id[] = {
	{ "ce1612", 0},
	{ },
};

static struct i2c_driver ce1612_i2c_driver = {
	.id_table = ce1612_i2c_id,
	.probe  = ce1612_i2c_probe,
	.remove = __exit_p(ce1612_i2c_remove),
	.driver = {
		.name = "ce1612",
	},
};

static int32_t ce1612_init_i2c(void)
{
	int32_t rc = 0;

	SKYCDBG("%s start\n",__func__);

	rc = i2c_add_driver(&ce1612_i2c_driver);
	SKYCDBG("%s ce1612_i2c_driver rc = %d\n",__func__, rc);
	if (IS_ERR_VALUE(rc))
		goto init_i2c_fail;

	SKYCDBG("%s end\n",__func__);
	return 0;

init_i2c_fail:
	SKYCERR("%s failed! (%d)\n", __func__, rc);
	return rc;
}

static int ce1612_sensor_probe(const struct msm_camera_sensor_info *info,
				struct msm_sensor_ctrl *s)
{
	int rc = 0;
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);

#ifdef CONFIG_PANTECH_CAMERA//IRQ
#ifndef AF_STATE_POLLING
	rc = gpio_tlmm_config(GPIO_CFG(57, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc) {
		SKYCDBG("%s: tlmm err\n", __func__);
		kfree(ce1612_sensorw);
	}
	rc = request_irq(gpio_to_irq(57), ce1612_interrupt, IRQF_TRIGGER_RISING, "ce1612", 0);
	SKYCDBG("ce1612_sensorw = %p\n", ce1612_sensorw);
	if (rc < 0) {
		SKYCDBG("%s: request irq failed\n", __func__);
		kfree(ce1612_sensorw);
		return rc;
	}
	disable_irq(gpio_to_irq(57));
#endif
#endif

	rc = ce1612_init_i2c();	
	if (rc < 0 || ce1612_client == NULL)
	{
		SKYCERR("%s rc = %d, ce1612_client = %x\n",__func__, rc, (unsigned int)ce1612_client);
		goto probe_init_fail;
	}

#ifdef F_L14A_OFF_IN_PROBER
	ce1612_l14a_off();
#endif

#if 1
	//FW VERSION CHECK will be added
	rc = ce1612_update_fw_boot();
	if (rc < 0)
		goto probe_init_fail;
#endif

	s->s_init = ce1612_sensor_init;
	s->s_release = ce1612_sensor_release;
	s->s_config  = ce1612_sensor_config;	
#ifdef CONFIG_PANTECH_CAMERA//IRQ
	s->s_readirq = ce1612_readirq;
#endif
	s->s_camera_type = BACK_CAMERA_2D;
	s->s_mount_angle = 90;//0;

	SKYCDBG("%s %s Success!:%d\n", __FILE__, __func__, __LINE__);
	return rc;

probe_init_fail:
	SKYCERR("%s %s Failed!:%d\n", __FILE__, __func__, __LINE__);
	return rc;
}

static int __ce1612_probe(struct platform_device *pdev)
{
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
	return msm_camera_drv_start(pdev, ce1612_sensor_probe);
}

static struct platform_driver msm_camera_driver = {
	.probe = __ce1612_probe,
	.driver = {
		.name = "msm_camera_ce1612",
		.owner = THIS_MODULE,
	},
};

static int __init ce1612_init(void)
{
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
	return platform_driver_register(&msm_camera_driver);
}

module_init(ce1612_init);
