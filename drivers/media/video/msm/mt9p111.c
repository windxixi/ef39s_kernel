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
#include "mt9p111.h"
#include <linux/regulator/consumer.h>
#include <linux/regulator/machine.h>
#include "camsensor_mt9p111_tuneup.h"

static int auto_focus = 0;

/* F_PCAM_TODO, move to CUST_PANTECH_CAMERA.h */
/* The intermittent failure with no image after initialization sequence is
 * related to the sensor entering an invalid state during power up.
 * The in-rush current due to charge sharing and gate charge initialization
 * significantly reduce reset timing margin and cause some Aptina tested parts
 * to fail at voltages at or below 1.8V (VDD).
 * The in-rush current is believed to cause a significant internal glitch that
 * disrupts internal circuitry, results in unsuccessful reset during power up
 * and leads to no image issue.
 * Internal nodes not directly shorted to VDD are charged by initial reset and
 * are more robust by the second reset.
 * The suggested solution is to have the second reset and also forcing a reset
 * of the sensor core after initialization. */
#define F_PANTECH_CAMERA_MT9P111_NO_IMAGE_AFTER_INIT

/* Micron MT9P111 Registers and their values */
/* Sensor Core Registers */
#define MT9P111_REG_MCU_ADDR      0x098E
#define MT9P111_REG_MCU_DATA      0x0990
#define  REG_MT9P111_MODEL_ID 0x3000
#define  MT9P111_MODEL_ID     0x1580

/*  SOC Registers Page 1  */
#define  REG_MT9P111_SENSOR_RESET     0x301A
#define  REG_MT9P111_STANDBY_CONTROL  0x3202
#define  REG_MT9P111_MCU_BOOT         0x3386

#if 1 //def F_PANTECH_CAMERA_ADD_CFG_OTP_MODULE
/* OTP detecting register */
#define MT9P111_REG_OTP_DETECT_ZONE_ADDR	0x6026
#define MT9P111_REG_OTP_DETECT_ZONE_READ	0xE026

/* OTP setting register */
#define MT9P111_REG_OTP_SET_ZONE_ADDR 		0xD006
#define MT9P111_REG_OTP_SET_ZONE_CURRENT 	0xD005
#define MT9P111_REG_OTP_SET_ZONE_SOLUTION 	0xD004
#define MT9P111_REG_OTP_SET_ZONE_PGA_ALGO 	0xD002
#define MT9P111_REG_OTP_SET_ZONE_LENS_ENABLE 	0x3210

/* OTP zone read variable */
static uint16_t read_otp_zone = 0;
static uint16_t read_otp_zone_fail = 0;
#endif

#define mt9p111_delay_msecs_stream 500//200

#define SENSOR_DEBUG 0

#define MT9P111_I2C_RETRY	10
#define MT9P111_I2C_MPERIOD	200
#define MT9P111_SNAPSHOT_RETRY 	200//30
#define MT9P111_PREVIEW_RETRY 	50
#define MT9P111_POLLING_RETRY	 	30

struct mt9p111_work {
	struct work_struct work;
};

static struct  mt9p111_work *mt9p111_sensorw;
static struct  i2c_client *mt9p111_client;

struct mt9p111_ctrl_t {
	const struct msm_camera_sensor_info *sensordata;
};

static int multi_count = 0;
static struct mt9p111_ctrl_t *mt9p111_ctrl;
//static int LED_SET = 0;

static int32_t config_csi;

int led_auto;


static DECLARE_WAIT_QUEUE_HEAD(mt9p111_wait_queue);
DECLARE_MUTEX(mt9p111_sem);
static int16_t mt9p111_effect = CAMERA_EFFECT_OFF;

#define ON	1
#define OFF	0

#define LED_DATA1 31
#define LED_DATA2 62

struct mt9p111_vreg_t {
	const char *name;
	unsigned short mvolt;
};

#define CAM_5MP_RST_N	0
#define CAM_5MP_STB	1
#define CAM_VGA_RST_N	2
#define CAM_VGA_STB_N	3
#define CAM_FLASH_EN	4
#define CAM_FLASH_MODE	5
#define CAM_GPIO_MAX	6


typedef struct {
	int32_t id;
	const char *label;
	uint32_t num;
} gpio_ctrl_t;

static gpio_ctrl_t g_gpios[] = {
	{CAM_5MP_RST_N, "CAM_5MP_RST_N", 106},
	{CAM_5MP_STB,   "CAM_5MP_STB",    86},
	{CAM_VGA_RST_N, "CAM_VGA_RST_N", 137},
	{CAM_VGA_STB_N, "CAM_VGA_STB_N", 139},
	{CAM_FLASH_EN, "CAM_FLASH_EN",    31},
	{CAM_FLASH_MODE, "CAM_FLASH_MODE",62},
};

#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
static mt9p111_tune_state_type mt9p111_tup_state = MT9P111_TUNE_STATE_NONE;
static mt9p111_tune_mode_type mt9p111_tup_mode = MT9P111_TUNE_STATE_TUNNING_MODE_ON;
static mt9p111_params_tbl_t mt9p111_params_tbl;
#endif


static int32_t mt9p111_i2c_read(unsigned short   saddr,unsigned short raddr, unsigned short *rdata, enum mt9p111_width width);


/*=============================================================
	EXTERNAL DECLARATIONS
==============================================================*/
extern struct mt9p111_reg mt9p111_regs;

/*
================================================================================
LOCAL API DECLARATIONS
================================================================================
*/

static struct regulator *l2b_2p8v;
static struct regulator *l3b_2p8v;
static struct regulator *lvs3b_1p8v;
#ifdef CONFIG_PRESTO_CAM_PT10
static struct regulator *mvs0b_1p8v;
#endif

#if 0 //def F_PANTECH_CAMERA_TARGET_PRESTO // VGA
static struct regulator *lvs1b_1p8v;
static struct regulator *lvs2b_1p8v;
static struct regulator *l1b_2p8v;
#endif


static int mt9p111_vreg_init(void)
{
	int rc = 0;
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);

//5M_CORE
	lvs3b_1p8v = regulator_get(NULL, "8901_lvs3");
	if(IS_ERR(lvs3b_1p8v))
	{
		SKYCERR("regulator_get lvs3b_1p8v fail : 0x%x\n", (unsigned int)lvs3b_1p8v);
		return -ENODEV;
	}

#ifdef CONFIG_PRESTO_CAM_PT10
// 5M_IO
	mvs0b_1p8v = regulator_get(NULL, "8901_mvs0" );
	if(IS_ERR(mvs0b_1p8v))
	{
		SKYCERR("regulator_get mvs0b_1p8v fail : 0x%x\n", (unsigned int)mvs0b_1p8v);
		return -ENODEV;
	}
#endif

//5M Analog
	l2b_2p8v = regulator_get(NULL,  "8901_l2");
	if(IS_ERR(l2b_2p8v))
	{
		SKYCERR("regulator_get l2b_2p8v fail : 0x%x\n", (unsigned int)l2b_2p8v);
		return -ENODEV;
	}

	rc = regulator_set_voltage(l2b_2p8v, 2800000, 2800000);
	if(rc)
	{
		SKYCERR("%s: unable to set l2b_2p8v voltage to 2.8V\n", __func__);
		goto fail;
	}
//5M AF
	l3b_2p8v = regulator_get(NULL,  "8901_l3");
	if(IS_ERR(l3b_2p8v))
	{
		SKYCERR("regulator_get l3b_2p8v fail : 0x%x\n", (unsigned int)l3b_2p8v);
		return -ENODEV;
	}

	rc = regulator_set_voltage(l3b_2p8v, 2800000, 2800000);
	if(rc)
	{
		SKYCERR("%s: unable to set l3b_2p8v voltage to 2.8V\n", __func__);
		goto fail;
	}

	SKYCDBG("%s %s Success!:%d\n", __FILE__, __func__, __LINE__);
	return rc;

fail:
	SKYCDBG("%s %s Failed!:%d\n", __FILE__, __func__, __LINE__);
	if(lvs3b_1p8v) {
		regulator_put(lvs3b_1p8v);
	}
#ifdef CONFIG_PRESTO_CAM_PT10
	if(mvs0b_1p8v) {
		regulator_put(mvs0b_1p8v);
	}
#endif
	if(l2b_2p8v) {
		regulator_put(l2b_2p8v);
	}
	if(l3b_2p8v) {
		regulator_put(l3b_2p8v);
	}
	return rc;
}



static int32_t mt9p111_gpio_out(int32_t id, int32_t val)
{
	int32_t rc = 0;


	if ((id < CAM_5MP_RST_N) || (id >= CAM_GPIO_MAX)) {
		SKYCERR("%s#%d: -EINVAL\n", __func__, __LINE__);
		return -EINVAL;
	}

	gpio_free(g_gpios[id].num);

	rc = gpio_request(g_gpios[id].num, g_gpios[id].label);
	if (rc) {
		SKYCERR("%s#%d: error. rc=%d\n", __func__, __LINE__, rc);
		goto gpio_out_fail;
	}

	rc = gpio_direction_output(g_gpios[id].num, val);
	if (rc) {
		SKYCERR("%s#%d: error. rc=%d\n", __func__, __LINE__, rc);
		goto gpio_out_fail;
	}

//	gpio_free(g_gpios[id].num);

	SKYCDBG("%s: set %s to %d\n", __func__, g_gpios[id].label, val);
	return 0;
gpio_out_fail:
	return rc;
}

static int32_t mt9p111_i2c_read_word(unsigned short raddr, unsigned short *rdata);
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
static u_int32_t mt9p111_i2c_write_params
(
    mt9p111_cfg_param_t *params,
    u_int32_t num_params
);
#endif

//static int32_t mt9p111_sensor_mode = SENSOR_PREVIEW_MODE;
#if 0 //0 //def F_PANTECH_CAMERA_FIX_CFG_LED_MODE
static bool led_trigger = FALSE;
#endif

static int32_t mt9p111_i2c_txdata(unsigned short saddr,
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
#if 0
	if (i2c_transfer(mt9p111_client->adapter, msg, 1) < 0) {
		SKYCERR("mt9p111_i2c_txdata failed\n");
		return -EIO;
	}

	return 0;
#endif
	for (i = 0; i < MT9P111_I2C_RETRY; i++) {
		rc = i2c_transfer(mt9p111_client->adapter, msg, 1);
		if (rc >= 0) {
			return 0;
		}
		SKYCERR("%s: tx retry. [%02x.%02x.%02x] len=%d rc=%d\n", __func__,saddr, *txdata, *(txdata + 1), length, rc);
		msleep(MT9P111_I2C_MPERIOD);
	}
	return -EIO;
}

static int32_t mt9p111_i2c_write(unsigned short saddr,
	unsigned short waddr, unsigned short wdata, enum mt9p111_width width)
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

		rc = mt9p111_i2c_txdata(saddr, buf, 4);
	}
		break;

	case TRIPLE_LEN: {
		buf[0] = (waddr & 0xFF00)>>8;
		buf[1] = (waddr & 0x00FF);
		buf[2] = wdata;
		rc = mt9p111_i2c_txdata(saddr, buf, 3);
	}
		break;

	case BYTE_LEN: {
		buf[0] = waddr;
		buf[1] = wdata;
		rc = mt9p111_i2c_txdata(saddr, buf, 2);
	}
		break;

	default:
		break;
	}

	if (rc < 0)
		SKYCERR(
		"i2c_write failed, saddr= 0x%x, addr = 0x%x, val = 0x%x!\n",
		saddr, waddr, wdata);

	return rc;
}

static int32_t mt9p111_i2c_write_a2d1(unsigned short waddr, unsigned char wdata)
{
	int32_t rc = -EIO;
	unsigned char buf[3];

	memset(buf, 0, sizeof(buf));

	buf[0] = (waddr & 0xFF00)>>8;
	buf[1] = (waddr & 0x00FF);
	buf[2] = wdata;

	rc = mt9p111_i2c_txdata(mt9p111_client->addr, buf, 3);
#if 0 //def F_PANTECH_CAMERA_SENSOR_TUNEUP
	//SKYCDBG("<<[=WRITE A2D1!=] waddr=0x%x, bdata=0x%x\n", waddr, wdata);
#endif
	if (rc < 0)
		SKYCERR(
		"i2c_write failed, saddr= 0x%x, addr = 0x%x, val = 0x%x!\n",
		mt9p111_client->addr, waddr, wdata);

	return rc;
}

static int32_t mt9p111_i2c_write_word(unsigned short waddr, unsigned short wdata)
{
	int32_t rc = -EIO;
	unsigned char buf[4];

	memset(buf, 0, sizeof(buf));

	buf[0] = (waddr & 0xFF00)>>8;
	buf[1] = (waddr & 0x00FF);
	buf[2] = (wdata & 0xFF00)>>8;
	buf[3] = (wdata & 0x00FF);

	rc = mt9p111_i2c_txdata(mt9p111_client->addr, buf, 4);
#if 0 //def F_PANTECH_CAMERA_SENSOR_TUNEUP
	//SKYCDBG("<<[WRITE WORD!] waddr=0x%x, wdata=0x%x\n", waddr, wdata);
#endif
	if (rc < 0)
		SKYCERR(
		"i2c_write failed, saddr= 0x%x, addr = 0x%x, val = 0x%x!\n",
		mt9p111_client->addr, waddr, wdata);

	return rc;
}


static int32_t mt9p111_i2c_write_table(
	struct mt9p111_i2c_reg_conf const *reg_conf_tbl,
	int num_of_items_in_table)
{
	int i;
	int32_t rc = -EIO;

	uint16_t poll_reg = 0;
	uint16_t poll_delay = 0;
	uint16_t poll_retry = 0;
	uint16_t poll_mcu_var = 0;
	uint16_t poll_data = 0;
	uint16_t poll_mask = 0;
	uint16_t retry_cnt = 0;
	uint16_t read_data = 0;
	//OTP 방어코드 추가
	uint16_t otp_retry_cnt = 0;
	uint16_t otp_poll_retry = 20;

	for (i = 0; i < num_of_items_in_table; i++)
	{
#if 0 //polling 완성
		if(reg_conf_tbl->width == ZERO_LEN)
		{
			SKYCDBG("ZERO_LEN continue ADDR = 0x%x, VALUE = 0x%x\n",reg_conf_tbl->waddr, reg_conf_tbl->wdata);
			continue;
		}

		if(reg_conf_tbl->width == POLL_MCU_VAR)
		{
#if 0
			poll_delay = (reg_conf_tbl[i+1].wdata & 0xff00) >> 8;
                     poll_retry = (reg_conf_tbl[i+1].wdata & 0x00ff);
	              poll_mcu_var = reg_conf_tbl[i].waddr;
	              poll_data = reg_conf_tbl[i+1].waddr;
	              poll_mask = reg_conf_tbl[i].wdata;
#else
			poll_mcu_var = reg_conf_tbl->waddr;
			poll_mask = reg_conf_tbl->wdata;
			poll_data = (reg_conf_tbl+1)->waddr;
			poll_delay = ((reg_conf_tbl+1)->wdata & 0xff00) >> 8;
			poll_retry = ((reg_conf_tbl+1)->wdata & 0x00ff);
#endif

		       SKYCDBG("POLLING!! poll_delay=%x, poll_retry=%x, poll_mcu_var=%x, poll_data=%x, poll_mask=%x\n",poll_delay, poll_retry, poll_mcu_var, poll_data, poll_mask);

			for (retry_cnt = 0; retry_cnt < poll_retry; retry_cnt++)
                	{
                		if (mt9p111_i2c_write_word(MT9P111_REG_MCU_ADDR, poll_mcu_var) < 0)
	                    {
	                        SKYCDBG("<<POLL_MCU_VAR mt9p111_i2c_write_word (-EIO)");
	                        return FALSE;
	                    }

	                    if (mt9p111_i2c_read_word(MT9P111_REG_MCU_DATA, &read_data) < 0)
	                    {
	                        SKYCDBG("<<POLL_MCU_VAR mt9p111_i2c_read_word (FALSE)");
	                        return FALSE;
	                    }

	                    if ((read_data & poll_mask) != poll_data)
	                    {
	                        SKYCDBG("retry polling MCU variable... after sleeping %d ms", poll_delay, 0, 0);
	                        msleep(poll_delay);
	                    }
	                    else
	                    {
	                        SKYCDBG("stop polling MCU variable... retried %d/%d time(s) (delay = %d ms)", retry_cnt, poll_retry, poll_delay);
	                        break;
	                    }
			}

			if (retry_cnt == poll_retry)
	              {
	                    SKYCDBG("<<RETRY FAIL!! read_data = %x (FALSE)", read_data, 0, 0);
	                    return FALSE;
	              }

			//  2개의 값을 이용하므로 +2를 해준다
			reg_conf_tbl++;
			reg_conf_tbl++;
		}
		else
		{
			rc = mt9p111_i2c_write(mt9p111_client->addr,
								reg_conf_tbl->waddr, reg_conf_tbl->wdata,
								reg_conf_tbl->width);

			if (rc < 0)
			{
				SKYCDBG("mt9p111_i2c_read failed!\n");
				break;
			}
#if 0
			if((reg_conf_tbl->waddr == 0x337E)||
			((reg_conf_tbl->waddr == 0x098E)&&(reg_conf_tbl->wdata == 0xA401))||
			(reg_conf_tbl->waddr == 0xA401)||
			(reg_conf_tbl->waddr == 0xA805)||
			(reg_conf_tbl->waddr == 0xA409)||
			(reg_conf_tbl->waddr == 0x8404))
			SKYCDBG("brightness WRITE!!! ADDR = 0x%x, VALUE = 0x%x, width = %d\n",reg_conf_tbl->waddr, reg_conf_tbl->wdata, reg_conf_tbl->width);
#endif
			if (reg_conf_tbl->msleep_time != 0)
				msleep(reg_conf_tbl->msleep_time);

			reg_conf_tbl++;
		}
#else
	switch(reg_conf_tbl->width )
	{
		case ZERO_LEN:
		{
			SKYCDBG("ZERO_LEN continue ADDR = 0x%x, VALUE = 0x%x\n",reg_conf_tbl->waddr, reg_conf_tbl->wdata);
			reg_conf_tbl++;
			continue;
		}
//ztlee
#if 1
		case POLL_REG:
        {
			poll_reg = reg_conf_tbl->waddr;
			poll_mask = reg_conf_tbl->wdata;
			poll_data = (reg_conf_tbl+1)->waddr;
			poll_delay = ((reg_conf_tbl+1)->wdata & 0xff00) >> 8;
			poll_retry = ((reg_conf_tbl+1)->wdata & 0x00ff);

			SKYCDBG("POLLING!! poll_delay=%x, poll_retry=%x, poll_mcu_var=%x, poll_data=%x, poll_mask=%x\n",poll_delay, poll_retry, poll_mcu_var, poll_data, poll_mask);

            //poll_retry=poll_retry*4;
			//poll_delay=poll_delay*4;
			for (retry_cnt = 0; retry_cnt < poll_retry; retry_cnt++)
			{
				if (mt9p111_i2c_read(mt9p111_client->addr,poll_reg, &read_data,WORD_LEN) < 0)
					//mt9v113_i2c_read_word(poll_reg, &read_data) == FALSE)
				{
					SKYCDBG("<<POLL_MCU_VAR mt9p111_i2c_read (FALSE)\n");
					return -1;
				}
				if ((read_data & poll_mask) != poll_data)
				{
					//hhs test CAM_INFO("retry polling MCU variable... after sleeping %d ms, read_data=%2x\n", poll_delay, read_data);
					//CAM_INFO("(read_data & poll_mask)=%4x , poll_data=%4x\n", (read_data & poll_mask), poll_data);
					msleep(poll_delay);
				}
				else
				{
					SKYCDBG("stop polling MCU variable... retried %d/%d time(s) (delay = %d ms), read_data=%2x\n", retry_cnt, poll_retry, poll_delay, read_data);
				    break;
				}
			}

			if (retry_cnt == poll_retry)
			{
				SKYCDBG("<<RETRY FAIL!! POLL_REG read_data = %x (FALSE)\n", read_data);
			    return -1;
			}

			//  2개의 값을 이용하므로 +2를 해준다
			reg_conf_tbl++;
			reg_conf_tbl++;
			break;
		}
#endif


		case POLL_MCU_VAR:
		{
			poll_mcu_var = reg_conf_tbl->waddr;
   		       poll_mask = reg_conf_tbl->wdata;
	              poll_data = (reg_conf_tbl+1)->waddr;
			poll_delay = ((reg_conf_tbl+1)->wdata & 0xff00) >> 8;
                     poll_retry = ((reg_conf_tbl+1)->wdata & 0x00ff);

		       SKYCDBG("POLLING!! poll_delay=%x, poll_retry=%x, poll_mcu_var=%x, poll_data=%x, poll_mask=%x\n",poll_delay, poll_retry, poll_mcu_var, poll_data, poll_mask);

			for (retry_cnt = 0; retry_cnt < poll_retry; retry_cnt++)
                	{
#ifdef MT9P111_MODULE_ABNORMAL_OPERATION_DEFENCE
			      if (mt9p111_i2c_read_word(poll_mcu_var, &read_data) < 0)
	                    {
	                        SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_read_word (FALSE)\n");
	                        return FALSE;
	                    }

#else
			      if (mt9p111_i2c_write_word(MT9P111_REG_MCU_ADDR, poll_mcu_var) < 0)
	                    {
	                        SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_write_word (FALSE)\n");
	                        return FALSE;
	                    }

	                    if (mt9p111_i2c_read_word(MT9P111_REG_MCU_DATA, &read_data) < 0)
	                    {
	                        SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_read_word (FALSE)\n");
	                        return FALSE;
	                    }
#endif

	                    if ((read_data & poll_mask) != poll_data)
	                    {
	                        SKYCDBG("retry polling MCU variable... after sleeping %d ms, read_data=%2x\n", poll_delay, read_data);
	                        msleep(poll_delay);
	                    }
	                    else
	                    {
	                        SKYCDBG("stop polling MCU variable... retried %d/%d time(s) (delay = %d ms), read_data=%2x\n", retry_cnt, poll_retry, poll_delay, read_data);
	                        break;
	                    }
			}

			if (retry_cnt == poll_retry)
	              {
	                    SKYCDBG("<<RETRY FAIL!! read_data = %x (FALSE)\n", read_data);
#ifdef MT9P111_MODULE_ABNORMAL_OPERATION_DEFENCE
			      return MT9P111_REG_POLLING_ERROR;
#endif
	                    //return FALSE;
	              }

			//  2개의 값을 이용하므로 +2를 해준다
			reg_conf_tbl++;
			reg_conf_tbl++;
			i++;

			break;
		}
		case POLL_MCU_VAR_NOT:
		{
			poll_mcu_var = reg_conf_tbl->waddr;
   		       poll_mask = reg_conf_tbl->wdata;
	              poll_data = (reg_conf_tbl+1)->waddr;
			poll_delay = ((reg_conf_tbl+1)->wdata & 0xff00) >> 8;
                     poll_retry = ((reg_conf_tbl+1)->wdata & 0x00ff);

		       SKYCDBG("POLLING!! poll_delay=%x, poll_retry=%x, poll_mcu_var=%x, poll_data=%x, poll_mask=%x\n",poll_delay, poll_retry, poll_mcu_var, poll_data, poll_mask);

			for (retry_cnt = 0; retry_cnt < poll_retry; retry_cnt++)
                	{
#ifdef MT9P111_MODULE_ABNORMAL_OPERATION_DEFENCE
			      if (mt9p111_i2c_read_word(poll_mcu_var, &read_data) < 0)
	                    {
	                        SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_read_word (FALSE)\n");
	                        return FALSE;
	                    }

#else
			      if (mt9p111_i2c_write_word(MT9P111_REG_MCU_ADDR, poll_mcu_var) < 0)
	                    {
	                        SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_write_word (FALSE)\n");
	                        return FALSE;
	                    }

	                    if (mt9p111_i2c_read_word(MT9P111_REG_MCU_DATA, &read_data) < 0)
	                    {
	                        SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_read_word (FALSE)\n");
	                        return FALSE;
	                    }
#endif
	                    if ((read_data & poll_mask) == poll_data)
	                    {
	                        SKYCDBG("retry polling MCU variable... after sleeping %d ms, read_data=%2x\n", poll_delay, read_data);
	                        msleep(poll_delay);
	                    }
	                    else
	                    {
	                        SKYCDBG("stop polling MCU variable... retried %d/%d time(s) (delay = %d ms), read_data=%2x\n", retry_cnt, poll_retry, poll_delay, read_data);
	                        break;
	                    }
			}

			if (retry_cnt == poll_retry)
	              {
	                    SKYCDBG("<<RETRY FAIL!! read_data = %x (FALSE)\n", read_data);
#ifdef MT9P111_MODULE_ABNORMAL_OPERATION_DEFENCE
			      return MT9P111_REG_POLLING_ERROR;
#endif
	                    //return FALSE;
	              }

			//  2개의 값을 이용하므로 +2를 해준다
			reg_conf_tbl++;
			reg_conf_tbl++;
			i++;

			break;
		}
		case BURST_WORD_START:
#if 0
			rc = mt9p111_i2c_txdata(mt9p111_client->addr, mt9p111_regs.init_patch_burst_settings, mt9p111_regs.init_patch_burst_settings_size);
			if (rc < 0)
			{
				SKYCERR("mt9p111_i2c_read failed!\n");
				break;
			}
#else
			{
uint16_t laddr = 0x886c;

uint8_t *base = mt9p111_regs.init_patch_burst_settings;
uint16_t len = mt9p111_regs.init_patch_burst_settings_size;
uint16_t pos = 0;

uint8_t  blk_dat[2 + 128]; // address 2 bytes + data 128 bytes
uint16_t blk_len = 0;

while (pos < len) {
	blk_len = ((len - pos) >= 128) ? 128 : (len - pos);
	blk_dat[0] = (uint8_t)((laddr & 0xff00) >> 8);
	blk_dat[1] = (uint8_t)(laddr & 0x00ff);
	memcpy(&blk_dat[2], &base[pos], blk_len);
	rc = mt9p111_i2c_txdata(mt9p111_client->addr, blk_dat, 2 + blk_len);
	if (rc < 0) {
		SKYCERR("burst write failed\n");
		break;
	}
	laddr += blk_len;
	pos += blk_len;
}
			}
#endif
			if((++reg_conf_tbl)->width == BURST_WORD_END)
			{
				reg_conf_tbl++;
				SKYCDBG("<<BURST_WORD_END SUCCESS!!reg_conf_tbl.waddr=%x, reg_conf_tbl.wdata=%x\n", reg_conf_tbl->waddr, reg_conf_tbl->wdata);
			}
			else
			{
				reg_conf_tbl++;
				reg_conf_tbl++;
				SKYCDBG("<<BURST_WORD_END FAIL!!reg_conf_tbl.waddr=%x, reg_conf_tbl.wdata=%x\n", reg_conf_tbl->waddr, reg_conf_tbl->wdata);
			}
			i++;
			break;
#if 1 //def F_PANTECH_CAMERA_ADD_CFG_OTP_MODULE
		case OTP_MODULE_ZONE_DETECT_START:

			msleep(100); //안정화 delay
		     	//열화 현상 방지를 위한 코드
#if 1 //def F_PANTECH_CAMERA_SENSOR_BUG
		     if (mt9p111_i2c_write_word(0x3812, 0x2124) < 0)
                    {
                        SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_read_word (-EIO)\n");
                        return -EIO;
                    }
#endif

            	      if (mt9p111_i2c_read_word(0xE023, &read_otp_zone_fail) < 0)
                    {
                        SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_read_word (FALSE)\n");
                        return FALSE;
                    }

                    if ((read_otp_zone_fail & 0xff00) != 0xC100)
                    {
                            SKYCDBG("DEFALT_LENS_SHADING_MODE!!!, read_data=%2x\n", read_otp_zone_fail);
#if 1
				mt9p111_i2c_write_word(0x3640, 0x02B0); 	// P_G1_P0Q0
				mt9p111_i2c_write_word(0x3642, 0x07EE); 	// P_G1_P0Q1
				mt9p111_i2c_write_word(0x3644, 0x3D71); 	// P_G1_P0Q2
				mt9p111_i2c_write_word(0x3646, 0xCAEB); 	// P_G1_P0Q3
				mt9p111_i2c_write_word(0x3648, 0xFE50); 	// P_G1_P0Q4
				mt9p111_i2c_write_word(0x364A, 0x02F0); 	// P_R_P0Q0
				mt9p111_i2c_write_word(0x364C, 0xDE8C); 	// P_R_P0Q1
				mt9p111_i2c_write_word(0x364E, 0x0211); 	// P_R_P0Q2
				mt9p111_i2c_write_word(0x3650, 0x6F4D); 	// P_R_P0Q3
				mt9p111_i2c_write_word(0x3652, 0xF070); 	// P_R_P0Q4
				mt9p111_i2c_write_word(0x3654, 0x03B0); 	// P_B_P0Q0
				mt9p111_i2c_write_word(0x3656, 0x6E8D); 	// P_B_P0Q1
				mt9p111_i2c_write_word(0x3658, 0x56D0); 	// P_B_P0Q2
				mt9p111_i2c_write_word(0x365A, 0xF36D); 	// P_B_P0Q3
				mt9p111_i2c_write_word(0x365C, 0x98F0); 	// P_B_P0Q4
				mt9p111_i2c_write_word(0x365E, 0x0150); 	// P_G2_P0Q0
				mt9p111_i2c_write_word(0x3660, 0xAD4D); 	// P_G2_P0Q1
				mt9p111_i2c_write_word(0x3662, 0x3F71); 	// P_G2_P0Q2
				mt9p111_i2c_write_word(0x3664, 0x016D); 	// P_G2_P0Q3
				mt9p111_i2c_write_word(0x3666, 0xFA30); 	// P_G2_P0Q4
				mt9p111_i2c_write_word(0x3680, 0x1FEA); 	// P_G1_P1Q0
				mt9p111_i2c_write_word(0x3682, 0xE0CD); 	// P_G1_P1Q1
				mt9p111_i2c_write_word(0x3684, 0x20AC); 	// P_G1_P1Q2
				mt9p111_i2c_write_word(0x3686, 0x192E); 	// P_G1_P1Q3
				mt9p111_i2c_write_word(0x3688, 0x912E); 	// P_G1_P1Q4
				mt9p111_i2c_write_word(0x368A, 0x4EAD); 	// P_R_P1Q0
				mt9p111_i2c_write_word(0x368C, 0x316D); 	// P_R_P1Q1
				mt9p111_i2c_write_word(0x368E, 0xFA09); 	// P_R_P1Q2
				mt9p111_i2c_write_word(0x3690, 0xCB0E); 	// P_R_P1Q3
				mt9p111_i2c_write_word(0x3692, 0xB90E); 	// P_R_P1Q4
				mt9p111_i2c_write_word(0x3694, 0xA4CE); 	// P_B_P1Q0
				mt9p111_i2c_write_word(0x3696, 0x598D); 	// P_B_P1Q1
				mt9p111_i2c_write_word(0x3698, 0x60AF); 	// P_B_P1Q2
				mt9p111_i2c_write_word(0x369A, 0xF2CE); 	// P_B_P1Q3
				mt9p111_i2c_write_word(0x369C, 0x9330); 	// P_B_P1Q4
				mt9p111_i2c_write_word(0x369E, 0xB82D); 	// P_G2_P1Q0
				mt9p111_i2c_write_word(0x36A0, 0xA86E); 	// P_G2_P1Q1
				mt9p111_i2c_write_word(0x36A2, 0x362F); 	// P_G2_P1Q2
				mt9p111_i2c_write_word(0x36A4, 0x3D0D); 	// P_G2_P1Q3
				mt9p111_i2c_write_word(0x36A6, 0xF6EF); 	// P_G2_P1Q4
				mt9p111_i2c_write_word(0x36C0, 0x5F11); 	// P_G1_P2Q0
				mt9p111_i2c_write_word(0x36C2, 0x0A4F); 	// P_G1_P2Q1
				mt9p111_i2c_write_word(0x36C4, 0xB572); 	// P_G1_P2Q2
				mt9p111_i2c_write_word(0x36C6, 0xA16B); 	// P_G1_P2Q3
				mt9p111_i2c_write_word(0x36C8, 0x0ED3); 	// P_G1_P2Q4
				mt9p111_i2c_write_word(0x36CA, 0x25B1); 	// P_R_P2Q0
				mt9p111_i2c_write_word(0x36CC, 0xF18E); 	// P_R_P2Q1
				mt9p111_i2c_write_word(0x36CE, 0x90D2); 	// P_R_P2Q2
				mt9p111_i2c_write_word(0x36D0, 0x276F); 	// P_R_P2Q3
				mt9p111_i2c_write_word(0x36D2, 0x2313); 	// P_R_P2Q4
				mt9p111_i2c_write_word(0x36D4, 0x2311); 	// P_B_P2Q0
				mt9p111_i2c_write_word(0x36D6, 0x126F); 	// P_B_P2Q1
				mt9p111_i2c_write_word(0x36D8, 0xF8D1); 	// P_B_P2Q2
				mt9p111_i2c_write_word(0x36DA, 0x870E); 	// P_B_P2Q3
				mt9p111_i2c_write_word(0x36DC, 0x12F3); 	// P_B_P2Q4
				mt9p111_i2c_write_word(0x36DE, 0x61F1); 	// P_G2_P2Q0
				mt9p111_i2c_write_word(0x36E0, 0x92AF); 	// P_G2_P2Q1
				mt9p111_i2c_write_word(0x36E2, 0xA1F2); 	// P_G2_P2Q2
				mt9p111_i2c_write_word(0x36E4, 0x1310); 	// P_G2_P2Q3
				mt9p111_i2c_write_word(0x36E6, 0x6872); 	// P_G2_P2Q4
				mt9p111_i2c_write_word(0x3700, 0x364F); 	// P_G1_P3Q0
				mt9p111_i2c_write_word(0x3702, 0xA04B); 	// P_G1_P3Q1
				mt9p111_i2c_write_word(0x3704, 0xBA6D); 	// P_G1_P3Q2
				mt9p111_i2c_write_word(0x3706, 0x6BF0); 	// P_G1_P3Q3
				mt9p111_i2c_write_word(0x3708, 0x91F2); 	// P_G1_P3Q4
				mt9p111_i2c_write_word(0x370A, 0x1E0F); 	// P_R_P3Q0
				mt9p111_i2c_write_word(0x370C, 0x954E); 	// P_R_P3Q1
				mt9p111_i2c_write_word(0x370E, 0xDBAF); 	// P_R_P3Q2
				mt9p111_i2c_write_word(0x3710, 0x1EF1); 	// P_R_P3Q3
				mt9p111_i2c_write_word(0x3712, 0x8A51); 	// P_R_P3Q4
				mt9p111_i2c_write_word(0x3714, 0xF66C); 	// P_B_P3Q0
				mt9p111_i2c_write_word(0x3716, 0xC10E); 	// P_B_P3Q1
				mt9p111_i2c_write_word(0x3718, 0x1BD0); 	// P_B_P3Q2
				mt9p111_i2c_write_word(0x371A, 0x0251); 	// P_B_P3Q3
				mt9p111_i2c_write_word(0x371C, 0xB5F2); 	// P_B_P3Q4
				mt9p111_i2c_write_word(0x371E, 0x736E); 	// P_G2_P3Q0
				mt9p111_i2c_write_word(0x3720, 0xA4CE); 	// P_G2_P3Q1
				mt9p111_i2c_write_word(0x3722, 0x430F); 	// P_G2_P3Q2
				mt9p111_i2c_write_word(0x3724, 0x45B1); 	// P_G2_P3Q3
				mt9p111_i2c_write_word(0x3726, 0xAED2); 	// P_G2_P3Q4
				mt9p111_i2c_write_word(0x3740, 0x9A11); 	// P_G1_P4Q0
				mt9p111_i2c_write_word(0x3742, 0x9350); 	// P_G1_P4Q1
				mt9p111_i2c_write_word(0x3744, 0x8D31); 	// P_G1_P4Q2
				mt9p111_i2c_write_word(0x3746, 0xD04E); 	// P_G1_P4Q3
				mt9p111_i2c_write_word(0x3748, 0x1174); 	// P_G1_P4Q4
				mt9p111_i2c_write_word(0x374A, 0x8811); 	// P_R_P4Q0
				mt9p111_i2c_write_word(0x374C, 0x292F); 	// P_R_P4Q1
				mt9p111_i2c_write_word(0x374E, 0x776F); 	// P_R_P4Q2
				mt9p111_i2c_write_word(0x3750, 0x9E71); 	// P_R_P4Q3
				mt9p111_i2c_write_word(0x3752, 0x21F3); 	// P_R_P4Q4
				mt9p111_i2c_write_word(0x3754, 0xB8D0); 	// P_B_P4Q0
				mt9p111_i2c_write_word(0x3756, 0xA010); 	// P_B_P4Q1
				mt9p111_i2c_write_word(0x3758, 0x9F0B); 	// P_B_P4Q2
				mt9p111_i2c_write_word(0x375A, 0x7190); 	// P_B_P4Q3
				mt9p111_i2c_write_word(0x375C, 0x47F3); 	// P_B_P4Q4
				mt9p111_i2c_write_word(0x375E, 0x89F1); 	// P_G2_P4Q0
				mt9p111_i2c_write_word(0x3760, 0x5D0E); 	// P_G2_P4Q1
				mt9p111_i2c_write_word(0x3762, 0x8C12); 	// P_G2_P4Q2
				mt9p111_i2c_write_word(0x3764, 0xA891); 	// P_G2_P4Q3
				mt9p111_i2c_write_word(0x3766, 0x3D54); 	// P_G2_P4Q4
				mt9p111_i2c_write_word(0x3782, 0x03B4); 	// CENTER_ROW
				mt9p111_i2c_write_word(0x3784, 0x04F4); 	// CENTER_COLUMN
				mt9p111_i2c_write_word(0x3210, 0x49B8); 	// COLOR_PIPELINE_CONTROL
#else
				mt9p111_i2c_write_word(0x3640, 0x0170); 	// P_G1_P0Q0
				mt9p111_i2c_write_word(0x3642, 0x18AD); 	// P_G1_P0Q1
				mt9p111_i2c_write_word(0x3644, 0x3A31); 	// P_G1_P0Q2
				mt9p111_i2c_write_word(0x3646, 0xC5CD); 	// P_G1_P0Q3
				mt9p111_i2c_write_word(0x3648, 0x9331); 	// P_G1_P0Q4
				mt9p111_i2c_write_word(0x364A, 0x0350); 	// P_R_P0Q0
				mt9p111_i2c_write_word(0x364C, 0xADCD); 	// P_R_P0Q1
				mt9p111_i2c_write_word(0x364E, 0x5A70); 	// P_R_P0Q2
				mt9p111_i2c_write_word(0x3650, 0x710D); 	// P_R_P0Q3
				mt9p111_i2c_write_word(0x3652, 0xD5D0); 	// P_R_P0Q4
				mt9p111_i2c_write_word(0x3654, 0x0350); 	// P_B_P0Q0
				mt9p111_i2c_write_word(0x3656, 0x12AD); 	// P_B_P0Q1
				mt9p111_i2c_write_word(0x3658, 0x4970); 	// P_B_P0Q2
				mt9p111_i2c_write_word(0x365A, 0x920E); 	// P_B_P0Q3
				mt9p111_i2c_write_word(0x365C, 0xBC90); 	// P_B_P0Q4
				mt9p111_i2c_write_word(0x365E, 0x0150); 	// P_G2_P0Q0
				mt9p111_i2c_write_word(0x3660, 0xF58D); 	// P_G2_P0Q1
				mt9p111_i2c_write_word(0x3662, 0x4231); 	// P_G2_P0Q2
				mt9p111_i2c_write_word(0x3664, 0x026B); 	// P_G2_P0Q3
				mt9p111_i2c_write_word(0x3666, 0x9751); 	// P_G2_P0Q4
				mt9p111_i2c_write_word(0x3680, 0xA547); 	// P_G1_P1Q0
				mt9p111_i2c_write_word(0x3682, 0xEAED); 	// P_G1_P1Q1
				mt9p111_i2c_write_word(0x3684, 0x1A0E); 	// P_G1_P1Q2
				mt9p111_i2c_write_word(0x3686, 0x17AE); 	// P_G1_P1Q3
				mt9p111_i2c_write_word(0x3688, 0xEBEF); 	// P_G1_P1Q4
				mt9p111_i2c_write_word(0x368A, 0x3DED); 	// P_R_P1Q0
				mt9p111_i2c_write_word(0x368C, 0x286D); 	// P_R_P1Q1
				mt9p111_i2c_write_word(0x368E, 0x070E); 	// P_R_P1Q2
				mt9p111_i2c_write_word(0x3690, 0x9CCE); 	// P_R_P1Q3
				mt9p111_i2c_write_word(0x3692, 0x88D0); 	// P_R_P1Q4
				mt9p111_i2c_write_word(0x3694, 0x8F0E); 	// P_B_P1Q0
				mt9p111_i2c_write_word(0x3696, 0x108E); 	// P_B_P1Q1
				mt9p111_i2c_write_word(0x3698, 0x2E30); 	// P_B_P1Q2
				mt9p111_i2c_write_word(0x369A, 0x814F); 	// P_B_P1Q3
				mt9p111_i2c_write_word(0x369C, 0x9B51); 	// P_B_P1Q4
				mt9p111_i2c_write_word(0x369E, 0xBD2C); 	// P_G2_P1Q0
				mt9p111_i2c_write_word(0x36A0, 0x99AE); 	// P_G2_P1Q1
				mt9p111_i2c_write_word(0x36A2, 0x01CF); 	// P_G2_P1Q2
				mt9p111_i2c_write_word(0x36A4, 0x2FAE); 	// P_G2_P1Q3
				mt9p111_i2c_write_word(0x36A6, 0xB34F); 	// P_G2_P1Q4
				mt9p111_i2c_write_word(0x36C0, 0x5AB1); 	// P_G1_P2Q0
				mt9p111_i2c_write_word(0x36C2, 0x660E); 	// P_G1_P2Q1
				mt9p111_i2c_write_word(0x36C4, 0xA132); 	// P_G1_P2Q2
				mt9p111_i2c_write_word(0x36C6, 0xF90E); 	// P_G1_P2Q3
				mt9p111_i2c_write_word(0x36C8, 0x08D3); 	// P_G1_P2Q4
				mt9p111_i2c_write_word(0x36CA, 0x1511); 	// P_R_P2Q0
				mt9p111_i2c_write_word(0x36CC, 0xD96E); 	// P_R_P2Q1
				mt9p111_i2c_write_word(0x36CE, 0x8392); 	// P_R_P2Q2
				mt9p111_i2c_write_word(0x36D0, 0x728D); 	// P_R_P2Q3
				mt9p111_i2c_write_word(0x36D2, 0x35B3); 	// P_R_P2Q4
				mt9p111_i2c_write_word(0x36D4, 0x20F1); 	// P_B_P2Q0
				mt9p111_i2c_write_word(0x36D6, 0x71CE); 	// P_B_P2Q1
				mt9p111_i2c_write_word(0x36D8, 0xA472); 	// P_B_P2Q2
				mt9p111_i2c_write_word(0x36DA, 0x830F); 	// P_B_P2Q3
				mt9p111_i2c_write_word(0x36DC, 0x50F3); 	// P_B_P2Q4
				mt9p111_i2c_write_word(0x36DE, 0x6511); 	// P_G2_P2Q0
				mt9p111_i2c_write_word(0x36E0, 0x90CF); 	// P_G2_P2Q1
				mt9p111_i2c_write_word(0x36E2, 0x9052); 	// P_G2_P2Q2
				mt9p111_i2c_write_word(0x36E4, 0x346F); 	// P_G2_P2Q3
				mt9p111_i2c_write_word(0x36E6, 0x6E12); 	// P_G2_P2Q4
				mt9p111_i2c_write_word(0x3700, 0x532B); 	// P_G1_P3Q0
				mt9p111_i2c_write_word(0x3702, 0x288D); 	// P_G1_P3Q1
				mt9p111_i2c_write_word(0x3704, 0xE8B1); 	// P_G1_P3Q2
				mt9p111_i2c_write_word(0x3706, 0x9590); 	// P_G1_P3Q3
				mt9p111_i2c_write_word(0x3708, 0x2352); 	// P_G1_P3Q4
				mt9p111_i2c_write_word(0x370A, 0x400C); 	// P_R_P3Q0
				mt9p111_i2c_write_word(0x370C, 0x3FCA); 	// P_R_P3Q1
				mt9p111_i2c_write_word(0x370E, 0xD071); 	// P_R_P3Q2
				mt9p111_i2c_write_word(0x3710, 0x836F); 	// P_R_P3Q3
				mt9p111_i2c_write_word(0x3712, 0x2572); 	// P_R_P3Q4
				mt9p111_i2c_write_word(0x3714, 0x940C); 	// P_B_P3Q0
				mt9p111_i2c_write_word(0x3716, 0xEE8E); 	// P_B_P3Q1
				mt9p111_i2c_write_word(0x3718, 0xBA31); 	// P_B_P3Q2
				mt9p111_i2c_write_word(0x371A, 0x052F); 	// P_B_P3Q3
				mt9p111_i2c_write_word(0x371C, 0x0C92); 	// P_B_P3Q4
				mt9p111_i2c_write_word(0x371E, 0x82AE); 	// P_G2_P3Q0
				mt9p111_i2c_write_word(0x3720, 0x1B0C); 	// P_G2_P3Q1
				mt9p111_i2c_write_word(0x3722, 0xAF10); 	// P_G2_P3Q2
				mt9p111_i2c_write_word(0x3724, 0x8270); 	// P_G2_P3Q3
				mt9p111_i2c_write_word(0x3726, 0x3EB0); 	// P_G2_P3Q4
				mt9p111_i2c_write_word(0x3740, 0xF750); 	// P_G1_P4Q0
				mt9p111_i2c_write_word(0x3742, 0x8E50); 	// P_G1_P4Q1
				mt9p111_i2c_write_word(0x3744, 0x96D3); 	// P_G1_P4Q2
				mt9p111_i2c_write_word(0x3746, 0x3392); 	// P_G1_P4Q3
				mt9p111_i2c_write_word(0x3748, 0x2F95); 	// P_G1_P4Q4
				mt9p111_i2c_write_word(0x374A, 0xDAD0); 	// P_R_P4Q0
				mt9p111_i2c_write_word(0x374C, 0x354F); 	// P_R_P4Q1
				mt9p111_i2c_write_word(0x374E, 0x0051); 	// P_R_P4Q2
				mt9p111_i2c_write_word(0x3750, 0x584E); 	// P_R_P4Q3
				mt9p111_i2c_write_word(0x3752, 0x2CB3); 	// P_R_P4Q4
				mt9p111_i2c_write_word(0x3754, 0xA770); 	// P_B_P4Q0
				mt9p111_i2c_write_word(0x3756, 0x9330); 	// P_B_P4Q1
				mt9p111_i2c_write_word(0x3758, 0x3AD1); 	// P_B_P4Q2
				mt9p111_i2c_write_word(0x375A, 0x37D2); 	// P_B_P4Q3
				mt9p111_i2c_write_word(0x375C, 0x2913); 	// P_B_P4Q4
				mt9p111_i2c_write_word(0x375E, 0xCA90); 	// P_G2_P4Q0
				mt9p111_i2c_write_word(0x3760, 0x1A4F); 	// P_G2_P4Q1
				mt9p111_i2c_write_word(0x3762, 0xBD73); 	// P_G2_P4Q2
				mt9p111_i2c_write_word(0x3764, 0x714F); 	// P_G2_P4Q3
				mt9p111_i2c_write_word(0x3766, 0x40F5); 	// P_G2_P4Q4
				mt9p111_i2c_write_word(0x3782, 0x03DC); 	// CENTER_ROW
				mt9p111_i2c_write_word(0x3784, 0x051C); 	// CENTER_COLUMN
				mt9p111_i2c_write_word(0x3210, 0x49B8); 	// COLOR_PIPELINE_CONTROL
#endif
                    }
                    else
                    {
				SKYCDBG("OTP_SELECT_MODE!!!, read_data=%2x\n", read_data);
				mt9p111_i2c_write_word(0xE024, 0x0100);
				mt9p111_i2c_write_word(0xE02A, 0xA010);
				mt9p111_i2c_write_a2d1(0x8404, 0x05);

			   	for (otp_retry_cnt = 0; otp_retry_cnt < otp_poll_retry; otp_retry_cnt++)
	                	{
	                	      if (mt9p111_i2c_read_word(0x8404, &read_data) < 0)
		                    {
		                        SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_read_word (FALSE)\n");
		                        return FALSE;
		                    }

		                    if ((read_data & 0xff00) != 0)
		                    {
		                        SKYCDBG("retry polling MCU variable... after sleeping %d ms, read_data=%2x\n", poll_delay, read_data);
		                        msleep(50);
		                    }
		                    else
		                    {
		                        SKYCDBG("stop polling MCU variable... retried %d/%d time(s) (delay = %d ms), read_data=%2x\n", retry_cnt, poll_retry, poll_delay, read_data);
		                        break;
		                    }
				}

				if (retry_cnt == poll_retry)
		              {
		                    SKYCDBG("<<RETRY FAIL!! read_data = %x (FALSE)\n", read_data);
#ifdef MT9P111_MODULE_ABNORMAL_OPERATION_DEFENCE
				      return MT9P111_REG_POLLING_ERROR;
#endif
		                    //return FALSE;
		              }
                    }

//===================================================================================
			if ((read_otp_zone_fail & 0xff00) == 0xC100)
		       {
		       	SKYCERR("<<OTP_ZONE_SELECT!!! read_otp_zone_fail=%x\n", read_otp_zone_fail);
				if (mt9p111_i2c_read_word(MT9P111_REG_OTP_DETECT_ZONE_READ, &read_otp_zone) < 0)
				{
					SKYCERR("<<MT9P111_REG_OTP_DETECT_ZONE_READ FAIL!! mt9p111_i2c_read_word (FALSE)\n");
					return FALSE;
				}

				if(read_otp_zone == 0)
				{
					SKYCDBG("<<OTP_MODULE_ZONE_SET : ZONE 0 [read_otp_zone]=%x\n", read_otp_zone);
	                            if (mt9p111_i2c_write_a2d1(MT9P111_REG_OTP_SET_ZONE_SOLUTION, 0x04) < 0)
					{
						SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_SOLUTION FAIL!! mt9p111_i2c_write_word (FALSE)\n");
						return FALSE;
					}
					if (mt9p111_i2c_write_word(MT9P111_REG_OTP_SET_ZONE_ADDR, 0) < 0)
					{
						SKYCERR("<<MT9P111_REG_OTP_DETECT_ZONE_ADDR FAIL!! mt9p111_i2c_write_word (FALSE)\n");
						return FALSE;
					}
					if (mt9p111_i2c_write_a2d1(MT9P111_REG_OTP_SET_ZONE_CURRENT, 0) < 0)
					{
						SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_CURRENT FAIL!! mt9p111_i2c_write_word (FALSE)\n");
						return FALSE;
					}
					if (mt9p111_i2c_write_word(MT9P111_REG_OTP_SET_ZONE_PGA_ALGO, 0x8002) < 0)
					{
						SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_PGA_ALGO FAIL!! mt9p111_i2c_write_word (FALSE)\n");
						return FALSE;
					}
					if (mt9p111_i2c_write_word(MT9P111_REG_OTP_SET_ZONE_LENS_ENABLE, 0x49B8) < 0)
					{
						SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_LENS_ENABLE FAIL!! mt9p111_i2c_write_word (FALSE)\n");
						return FALSE;
					}
				}
				else
				{
					SKYCDBG("<<OTP_MODULE_ZONE_SET : ZONE 1 [read_otp_zone]=%x\n", read_otp_zone);
	                                if (mt9p111_i2c_write_a2d1(MT9P111_REG_OTP_SET_ZONE_SOLUTION, 0x04) < 0)
					{
						SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_SOLUTION FAIL!! mt9p111_i2c_write_word (FALSE)\n");
						return FALSE;
					}
					if (mt9p111_i2c_write_word(MT9P111_REG_OTP_SET_ZONE_ADDR, 0x0100) < 0)
					{
						SKYCERR("<<MT9P111_REG_OTP_DETECT_ZONE_ADDR FAIL!! mt9p111_i2c_write_word (FALSE)\n");
						return FALSE;
					}
					if (mt9p111_i2c_write_a2d1(MT9P111_REG_OTP_SET_ZONE_CURRENT, 0x01) < 0)
					{
						SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_CURRENT FAIL!! mt9p111_i2c_write_word (FALSE)\n");
						return FALSE;
					}
					if (mt9p111_i2c_write_word(MT9P111_REG_OTP_SET_ZONE_PGA_ALGO, 0x8002) < 0)
					{
						SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_PGA_ALGO FAIL!! mt9p111_i2c_write_word (FALSE)\n");
						return FALSE;
					}
					if (mt9p111_i2c_write_word(MT9P111_REG_OTP_SET_ZONE_LENS_ENABLE, 0x49B8) < 0)
					{
						SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_LENS_ENABLE FAIL!! mt9p111_i2c_write_word (FALSE)\n");
						return FALSE;
					}
				}
			}
			if((++reg_conf_tbl)->width == OTP_MODULE_ZONE_DETECT_END)
			{
				reg_conf_tbl++;
				SKYCDBG("<<OTP_MODULE_ZONE_DETECT_END SUCCESS!!read_otp_zone_waddr=%x, read_otp_zone_wdata=%x\n", MT9P111_REG_OTP_DETECT_ZONE_READ, read_otp_zone);
			}
			else
			{
				reg_conf_tbl++;
				reg_conf_tbl++;
				SKYCDBG("<<OTP_MODULE_ZONE_DETECT_END FAIL!!read_otp_zone_waddr=%x, read_otp_zone_wdata=%x\n", MT9P111_REG_OTP_DETECT_ZONE_READ, read_otp_zone);
			}
			i++;
			break;
#else
		case OTP_MODULE_ZONE_DETECT_START:
			if (mt9p111_i2c_write_word(MT9P111_REG_MCU_ADDR, MT9P111_REG_OTP_DETECT_ZONE_ADDR) < 0)
			{
				SKYCERR("<<MT9P111_REG_OTP_DETECT_ZONE_ADDR FAIL!! mt9p111_i2c_write_word (FALSE)\n");
				return FALSE;
			}
			if (mt9p111_i2c_read_word(MT9P111_REG_OTP_DETECT_ZONE_READ, &read_otp_zone) < 0)
			{
				SKYCERR("<<MT9P111_REG_OTP_DETECT_ZONE_READ FAIL!! mt9p111_i2c_read_word (FALSE)\n");
				return FALSE;
			}

			if(read_otp_zone == 0)
			{
				SKYCDBG("<<OTP_MODULE_ZONE_SET : ZONE 0 [read_otp_zone]=%x\n", read_otp_zone);
                                if (mt9p111_i2c_write_a2d1(MT9P111_REG_OTP_SET_ZONE_SOLUTION, 0x04) < 0)
				{
					SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_SOLUTION FAIL!! mt9p111_i2c_write_word (FALSE)\n");
					return FALSE;
				}
				if (mt9p111_i2c_write_word(MT9P111_REG_OTP_SET_ZONE_ADDR, 0) < 0)
				{
					SKYCERR("<<MT9P111_REG_OTP_DETECT_ZONE_ADDR FAIL!! mt9p111_i2c_write_word (FALSE)\n");
					return FALSE;
				}
				if (mt9p111_i2c_write_a2d1(MT9P111_REG_OTP_SET_ZONE_CURRENT, 0) < 0)
				{
					SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_CURRENT FAIL!! mt9p111_i2c_write_word (FALSE)\n");
					return FALSE;
				}
			}
			else
			{
				SKYCDBG("<<OTP_MODULE_ZONE_SET : ZONE 1 [read_otp_zone]=%x\n", read_otp_zone);
                                if (mt9p111_i2c_write_a2d1(MT9P111_REG_OTP_SET_ZONE_SOLUTION, 0x04) < 0)
				{
					SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_SOLUTION FAIL!! mt9p111_i2c_write_word (FALSE)\n");
					return FALSE;
				}
				if (mt9p111_i2c_write_word(MT9P111_REG_OTP_SET_ZONE_ADDR, 0x0100) < 0)
				{
					SKYCERR("<<MT9P111_REG_OTP_DETECT_ZONE_ADDR FAIL!! mt9p111_i2c_write_word (FALSE)\n");
					return FALSE;
				}
				if (mt9p111_i2c_write_a2d1(MT9P111_REG_OTP_SET_ZONE_CURRENT, 0x01) < 0)
				{
					SKYCERR("<<MT9P111_REG_OTP_SET_ZONE_CURRENT FAIL!! mt9p111_i2c_write_word (FALSE)\n");
					return FALSE;
				}
			}

			if((++reg_conf_tbl)->width == OTP_MODULE_ZONE_DETECT_END)
			{
				reg_conf_tbl++;
				SKYCDBG("<<OTP_MODULE_ZONE_DETECT_END SUCCESS!!read_otp_zone_waddr=%x, read_otp_zone_wdata=%x\n", MT9P111_REG_OTP_DETECT_ZONE_READ, read_otp_zone);
			}
			else
			{
				reg_conf_tbl++;
				reg_conf_tbl++;
				SKYCDBG("<<OTP_MODULE_ZONE_DETECT_END FAIL!!read_otp_zone_waddr=%x, read_otp_zone_wdata=%x\n", MT9P111_REG_OTP_DETECT_ZONE_READ, read_otp_zone);
			}
			i++;
			break;
#endif
		default:
		{
			rc = mt9p111_i2c_write(mt9p111_client->addr,
								reg_conf_tbl->waddr, reg_conf_tbl->wdata,
								reg_conf_tbl->width);
			//SKYCDBG("I2C WRITE!!! ADDR = 0x%x, VALUE = 0x%x, width = %d, num_of_items_in_table=%d, i=%d\n",
			//	reg_conf_tbl->waddr, reg_conf_tbl->wdata, reg_conf_tbl->width, num_of_items_in_table, i);

			if (rc < 0)
			{
				SKYCERR("mt9p111_i2c_read failed!\n");
				return rc;
			}

			if (reg_conf_tbl->mdelay_time != 0)
				mdelay(reg_conf_tbl->mdelay_time);

			reg_conf_tbl++;

			break;
		}
	}
#endif
	}

	return rc;
}


#if 0
static int32_t mt9p111_i2c_write_table(
	struct mt9p111_i2c_reg_conf const *reg_conf_tbl,
	int num_of_items_in_table)
{
	int i;
	int32_t rc = -EIO;

	uint16_t poll_delay = 0;
       uint16_t poll_retry = 0;
       uint16_t poll_mcu_var = 0;
       uint16_t poll_data = 0;
       uint16_t poll_mask = 0;
       uint16_t retry_cnt = 0;
	uint16_t read_data = 0;

	for (i = 0; i < num_of_items_in_table; i++)
	{
#if 0 //polling 완성
		if(reg_conf_tbl->width == ZERO_LEN)
		{
			SKYCDBG("ZERO_LEN continue ADDR = 0x%x, VALUE = 0x%x\n",reg_conf_tbl->waddr, reg_conf_tbl->wdata);
			continue;
		}

		if(reg_conf_tbl->width == POLL_MCU_VAR)
		{
#if 0
			poll_delay = (reg_conf_tbl[i+1].wdata & 0xff00) >> 8;
                     poll_retry = (reg_conf_tbl[i+1].wdata & 0x00ff);
	              poll_mcu_var = reg_conf_tbl[i].waddr;
	              poll_data = reg_conf_tbl[i+1].waddr;
	              poll_mask = reg_conf_tbl[i].wdata;
#else
			poll_mcu_var = reg_conf_tbl->waddr;
   		       poll_mask = reg_conf_tbl->wdata;
	              poll_data = (reg_conf_tbl+1)->waddr;
			poll_delay = ((reg_conf_tbl+1)->wdata & 0xff00) >> 8;
                     poll_retry = ((reg_conf_tbl+1)->wdata & 0x00ff);
#endif

		       SKYCDBG("POLLING!! poll_delay=%x, poll_retry=%x, poll_mcu_var=%x, poll_data=%x, poll_mask=%x\n",poll_delay, poll_retry, poll_mcu_var, poll_data, poll_mask);

			for (retry_cnt = 0; retry_cnt < poll_retry; retry_cnt++)
                	{
                		if (mt9p111_i2c_write_word(MT9P111_REG_MCU_ADDR, poll_mcu_var) < 0)
	                    {
	                        SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_write_word (-EIO)");
	                        return FALSE;
	                    }

	                    if (mt9p111_i2c_read_word(MT9P111_REG_MCU_DATA, &read_data) < 0)
	                    {
	                        SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_read_word (FALSE)");
	                        return FALSE;
	                    }

	                    if ((read_data & poll_mask) != poll_data)
	                    {
	                        SKYCDBG("retry polling MCU variable... after sleeping %d ms", poll_delay, 0, 0);
	                        msleep(poll_delay);
	                    }
	                    else
	                    {
	                        SKYCDBG("stop polling MCU variable... retried %d/%d time(s) (delay = %d ms)", retry_cnt, poll_retry, poll_delay);
	                        break;
	                    }
			}

			if (retry_cnt == poll_retry)
	              {
	                    SKYCERR("<<RETRY FAIL!! read_data = %x (FALSE)", read_data, 0, 0);
	                    return FALSE;
	              }

			//  2개의 값을 이용하므로 +2를 해준다
			reg_conf_tbl++;
			reg_conf_tbl++;
		}
		else
		{
			rc = mt9p111_i2c_write(mt9p111_client->addr,
								reg_conf_tbl->waddr, reg_conf_tbl->wdata,
								reg_conf_tbl->width);

			if (rc < 0)
			{
				SKYCERR("mt9p111_i2c_read failed!\n");
				break;
			}
#if 0
			if((reg_conf_tbl->waddr == 0x337E)||
			((reg_conf_tbl->waddr == 0x098E)&&(reg_conf_tbl->wdata == 0xA401))||
			(reg_conf_tbl->waddr == 0xA401)||
			(reg_conf_tbl->waddr == 0xA805)||
			(reg_conf_tbl->waddr == 0xA409)||
			(reg_conf_tbl->waddr == 0x8404))
			SKYCDBG("brightness WRITE!!! ADDR = 0x%x, VALUE = 0x%x, width = %d\n",reg_conf_tbl->waddr, reg_conf_tbl->wdata, reg_conf_tbl->width);
#endif
			if (reg_conf_tbl->msleep_time != 0)
				msleep(reg_conf_tbl->msleep_time);

			reg_conf_tbl++;
		}
#else
	switch(reg_conf_tbl->width )
	{
		case ZERO_LEN:
		{
			SKYCDBG("ZERO_LEN continue ADDR = 0x%x, VALUE = 0x%x\n",reg_conf_tbl->waddr, reg_conf_tbl->wdata);
			reg_conf_tbl++;
			continue;
		}
		case POLL_MCU_VAR:
		{
			poll_mcu_var = reg_conf_tbl->waddr;
   		       poll_mask = reg_conf_tbl->wdata;
	              poll_data = (reg_conf_tbl+1)->waddr;
			poll_delay = ((reg_conf_tbl+1)->wdata & 0xff00) >> 8;
                     poll_retry = ((reg_conf_tbl+1)->wdata & 0x00ff);

		       SKYCDBG("POLLING!! poll_delay=%x, poll_retry=%x, poll_mcu_var=%x, poll_data=%x, poll_mask=%x\n",poll_delay, poll_retry, poll_mcu_var, poll_data, poll_mask);

			for (retry_cnt = 0; retry_cnt < poll_retry; retry_cnt++)
                	{
                		if (mt9p111_i2c_write_word(MT9P111_REG_MCU_ADDR, poll_mcu_var) < 0)
	                    {
	                        SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_write_word (FALSE)\n");
	                        return FALSE;
	                    }

	                    if (mt9p111_i2c_read_word(MT9P111_REG_MCU_DATA, &read_data) < 0)
	                    {
	                        SKYCERR("<<POLL_MCU_VAR mt9p111_i2c_read_word (FALSE)\n");
	                        return FALSE;
	                    }

	                    if ((read_data & poll_mask) != poll_data)
	                    {
	                        SKYCERR("retry polling MCU variable... after sleeping %d ms\n", poll_delay);
	                        msleep(poll_delay);
	                    }
	                    else
	                    {
	                        SKYCERR("stop polling MCU variable... retried %d/%d time(s) (delay = %d ms)\n", retry_cnt, poll_retry, poll_delay);
	                        break;
	                    }
			}

			if (retry_cnt == poll_retry)
	        {
	        	SKYCERR("<<RETRY FAIL!! read_data = %x (FALSE)\n", read_data);
	        	return FALSE;
	        }

			//  2개의 값을 이용하므로 +2를 해준다
			reg_conf_tbl++;
			reg_conf_tbl++;
			i++;

			break;
		}
		case BURST_WORD_START:
			rc = mt9p111_i2c_txdata(mt9p111_client->addr, mt9p111_regs.init_patch_burst_settings, mt9p111_regs.init_patch_burst_settings_size);
			if (rc < 0)
			{
				SKYCERR("mt9p111_i2c_read failed!\n");
				break;
			}

			if((++reg_conf_tbl)->width == BURST_WORD_END)
			{
				reg_conf_tbl++;
				SKYCDBG("<<BURST_WORD_END SUCCESS!!reg_conf_tbl.waddr=%x, reg_conf_tbl.wdata=%x\n", reg_conf_tbl->waddr, reg_conf_tbl->wdata);
			}
			else
			{
				reg_conf_tbl++;
				reg_conf_tbl++;
				SKYCDBG("<<BURST_WORD_END FAIL!!reg_conf_tbl.waddr=%x, reg_conf_tbl.wdata=%x\n", reg_conf_tbl->waddr, reg_conf_tbl->wdata);
			}
			i++;
			break;
		default:
		{
			rc = mt9p111_i2c_write(mt9p111_client->addr,
								reg_conf_tbl->waddr, reg_conf_tbl->wdata,
								reg_conf_tbl->width);
			//SKYCDBG("I2C WRITE!!! ADDR = 0x%x, VALUE = 0x%x, width = %d, num_of_items_in_table=%d, i=%d\n",
			//	reg_conf_tbl->waddr, reg_conf_tbl->wdata, reg_conf_tbl->width, num_of_items_in_table, i);

			if (rc < 0)
			{
				SKYCERR("mt9p111_i2c_read failed!\n");
				return rc;
			}

			if (reg_conf_tbl->msleep_time != 0)
				msleep(reg_conf_tbl->msleep_time);

			reg_conf_tbl++;

			break;
		}
	}
#endif
	}

	return rc;
}
#endif

static int mt9p111_i2c_rxdata(unsigned short saddr, int slength,
	unsigned char *rxdata, int length)
{
	uint32_t i = 0;
	int32_t rc = 0;

	struct i2c_msg msgs[] = {
	{
		.addr   = saddr,
		.flags = 0,
		.len   = slength,//   2, //읽고 싶은 레지스터의 주소 길이
		.buf   = rxdata,// 읽고 싶은 레지스터의 데이터 길이
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
#if 0
	if (i2c_transfer(mt9p111_client->adapter, msgs, 2) < 0) {
		SKYCERR("mt9p111_i2c_rxdata failed!\n");
		return -EIO;
	}

	return 0;
#endif
	for (i = 0; i < MT9P111_I2C_RETRY; i++) {
		rc = i2c_transfer(mt9p111_client->adapter, msgs, 2);
		if (rc >= 0) {
			return 0;
		}
		SKYCERR("%s: tx retry. [%02x.%02x] len=%d rc=%d\n", __func__,saddr, *rxdata, length, rc);
		msleep(MT9P111_I2C_MPERIOD);
	}
	//SKYCDBG("%s end\n",__func__);
	return -EIO;//0;
}

static int32_t mt9p111_i2c_read(unsigned short   saddr,
	unsigned short raddr, unsigned short *rdata, enum mt9p111_width width)
{
	int32_t rc = 0;
	unsigned char buf[4];

	if (!rdata)
		return -EIO;

	memset(buf, 0, sizeof(buf));

	switch (width) {
	case WORD_LEN: { //주소와 데이터가 모두 2바이트 일 경우
		buf[0] = (raddr & 0xFF00)>>8;
		buf[1] = (raddr & 0x00FF);

		rc = mt9p111_i2c_rxdata(saddr, 2, buf, 2);
		if (rc < 0)
			return rc;

		*rdata = buf[0] << 8 | buf[1];
	}
		break;

	case TRIPLE_LEN: { //주소는 2바이트 데이터는 1바이트 일 경우
		buf[0] = (raddr & 0xFF00)>>8;
		buf[1] = (raddr & 0x00FF);

		rc = mt9p111_i2c_rxdata(saddr, 2, buf, 1);
		if (rc < 0)
			return rc;

		*rdata = buf[0];
	}
		break;

	default:
		break;
	}

	if (rc < 0)
		SKYCERR("mt9p111_i2c_read failed!\n");

	return rc;
}

//#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
static int32_t mt9p111_i2c_read_word(unsigned short raddr, unsigned short *rdata)
{
	int32_t rc = 0;
	unsigned char buf[4];

	if (!rdata)
		return -EIO;

	memset(buf, 0, sizeof(buf));

	buf[0] = (raddr & 0xFF00)>>8;
	buf[1] = (raddr & 0x00FF);

	rc = mt9p111_i2c_rxdata(mt9p111_client->addr, 2, buf, 2);
	if (rc < 0)
		return rc;

	*rdata = buf[0] << 8 | buf[1];

	if (rc < 0)
		SKYCERR("mt9p111_i2c_read_word failed!\n");

	return rc;
}
//#endif

//사용하지 않음
#if 0
static int32_t mt9p111_i2c_read_table(
	struct mt9p111_i2c_reg_conf const *reg_conf_tbl,
	int num_of_items_in_table)
{
	int i;
	int32_t rc = -EIO;
	unsigned short read_buf = 0;

	for (i = 0; i < num_of_items_in_table; i++) {
		rc = mt9p111_i2c_read(mt9p111_client->addr,
			reg_conf_tbl->waddr, &read_buf,
			reg_conf_tbl->width);
		if (rc < 0)
			break;

		if(reg_conf_tbl->width == TRIPLE_LEN)
			SKYCDBG("READ!!! ADDR = 0x%x, VALUE = 0x%x, width = %d\n",reg_conf_tbl->waddr, (unsigned char)read_buf, reg_conf_tbl->width);
		else
			SKYCDBG("READ!!! ADDR = 0x%x, VALUE = 0x%x, width = %d\n",reg_conf_tbl->waddr, read_buf, reg_conf_tbl->width);
		reg_conf_tbl++;
	}

	return rc;
}
#endif



static int32_t mt9p111_power_on(void)
{

	int32_t rc = 0;

	if (regulator_enable(lvs3b_1p8v))		rc = -EIO;
#ifdef CONFIG_PRESTO_CAM_PT10
	msleep(1);
	if (regulator_enable(mvs0b_1p8v))		rc = -EIO;
#endif
	msleep(5);
	if (mt9p111_gpio_out(CAM_5MP_STB, 0))		rc = -EIO;
	msleep(5);
	/* prevent 5MP/VGA RESET signals from becoming HIGH state just after
	 * supplying CORE/IO voltages. (sensor HW issue) */
	if (mt9p111_gpio_out(CAM_5MP_RST_N, 0))		rc = -EIO;
	msleep(5);
	if (regulator_enable(l2b_2p8v))		rc = -EIO;
	msleep(5);
	if (regulator_enable(l3b_2p8v))	rc = -EIO;
	msleep(15);
	if (mt9p111_gpio_out(CAM_5MP_RST_N, 1))		rc = -EIO;
	msleep(10);
#if 1 //def F_PANTECH_CAMERA_MT9P111_NO_IMAGE_AFTER_INIT
	if (mt9p111_i2c_write_word(0x0020, 0x0000) < 0) rc = -EIO;
	msleep(1);
	if (mt9p111_gpio_out(CAM_5MP_RST_N, 0))		rc = -EIO;
	udelay(3);
	if (mt9p111_gpio_out(CAM_5MP_RST_N, 1))		rc = -EIO;
	udelay(40);
	if (mt9p111_i2c_write_word(0x0020, 0x0000) < 0) rc = -EIO;
#endif

	return rc;
}

static int32_t mt9p111_power_off(void)
{
	int32_t rc = 0;

	/* VDD_CAM_1800_IO is opened and merged with VDD_CAM_1800 in ET15
	 * to correct 'no image' and 'burnt image' issues.
	 * We should not remove VDD_CAM_1800_IO for backward compatibility. */
	if (mt9p111_gpio_out(CAM_5MP_RST_N, 0))		rc = -EIO;
	msleep(3);
	rc = regulator_disable(l3b_2p8v);
	if (rc)
		SKYCERR("%s: Disable regulator l3b_2p8v failed\n", __func__);
	regulator_put(l3b_2p8v);

	rc = regulator_disable(l2b_2p8v);
	if (rc)
		SKYCERR("%s: Disable regulator l2b_2p8v failed\n", __func__);
	regulator_put(l2b_2p8v);

	msleep(4);
	rc = regulator_disable(lvs3b_1p8v);
	if (rc)
		SKYCERR("%s: Disable regulator lvs3b_1p8v failed\n", __func__);
	msleep(1);
#ifdef CONFIG_PRESTO_CAM_PT10
	rc = regulator_disable(mvs0b_1p8v);
	if (rc)
		SKYCERR("%s: Disable regulator mvs0b_1p8v failed\n", __func__);
	msleep(1);
#endif
	return rc;
}



static long mt9p111_reg_init(void)
{
	int32_t rc = 0;
	int32_t i =0;
	SKYCDBG("%s start\n",__func__);

#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	// 튜닝 파일값
	if((mt9p111_tup_state == MT9P111_TUNE_STATE_LOAD_VALUE) && (mt9p111_params_tbl.init.num_params > 0))
	{
		mt9p111_i2c_write_params(mt9p111_params_tbl.init.params,
								mt9p111_params_tbl.init.num_params);
	}
	// static 값
	else
#endif
	{
		rc = mt9p111_i2c_write_table(&mt9p111_regs.init_settings[0],
					mt9p111_regs.init_settings_size);
	}
#ifdef MT9P111_MODULE_ABNORMAL_OPERATION_DEFENCE
	if(rc == MT9P111_REG_POLLING_ERROR)
	{
		for( i =0; i <5; i++ )
		{

		SKYCERR("POLLING FAIL!!!RETRY INIT SEQUENCE~~~\n");
		gpio_request(CAM_5MP_RST_N, "mt9p111");
		gpio_direction_output(CAM_5MP_RST_N, 0);
		msleep(5);
		gpio_direction_output(CAM_5MP_RST_N, 1);
		gpio_free(CAM_5MP_RST_N);
		msleep(5);

#if 0//def F_PANTECH_CAMERA_MT9P111_NO_IMAGE_AFTER_INIT
		if (mt9p111_i2c_write_word(0x0020, 0x0000) < 0) rc = -EIO;
		msleep(1);
		if (mt9p111_gpio_out(CAM_5MP_RST_N, 0))		rc = -EIO;
		udelay(3);
		if (mt9p111_gpio_out(CAM_5MP_RST_N, 1))		rc = -EIO;
		udelay(40);
		if (mt9p111_i2c_write_word(0x0020, 0x0000) < 0) rc = -EIO;
#endif

		rc = mt9p111_i2c_write_table(&mt9p111_regs.init_settings[0],
					mt9p111_regs.init_settings_size);

		if((i==4) && (rc == MT9P111_REG_POLLING_ERROR))
		{
			SKYCERR("5 times RETRY POLLING FAIL!!!OUT OF INIT~~~\n");
			return 0;
		}
		if(rc == MT9P111_REG_POLLING_ERROR)
		{
			SKYCERR("RETRY POLLING_ERROR!!!retry time : %d\n", i+1);
			continue;
		}
		else
		{
			SKYCERR("RETRY INIT SUCCESS!!!retry time : %d\n", i+1);
			return 0;
		}
#if 0
		if(rc == MT9P111_REG_POLLING_ERROR)
		{
			SKYCERR("SECOND POLLING FAIL!!!OUT OF INIT~~~\n");
			return 0;
		}
#endif
		}
	}
#endif

	if (rc < 0)
	{
		SKYCERR("mt9p111_i2c_write_table FAIL!!! return~~\n");
		return rc;
	}

#if 0
	mt9p111_i2c_read_table(&mt9p111_regs.init_settings[0],
					mt9p111_regs.init_settings_size);
#endif
	SKYCDBG("%s END\n",__func__);
	return 0;
}

static long mt9p111_set_effect(int mode, int effect)
{
	int32_t rc = 0;

	SKYCDBG("%s start\n",__func__);

	if(effect < CAMERA_EFFECT_OFF || effect >= CAMERA_EFFECT_MAX){
		SKYCERR("%s error. effect=%d\n", __func__, effect);
		return -EINVAL;
	}
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	// 튜닝 파일값
	if((mt9p111_tup_state == MT9P111_TUNE_STATE_LOAD_VALUE) && (mt9p111_params_tbl.effect[effect].num_params > 0))
	{
		mt9p111_i2c_write_params(mt9p111_params_tbl.effect[effect].params,
								mt9p111_params_tbl.effect[effect].num_params);
	}
	// static 값
	else
#endif
	{
		rc = mt9p111_i2c_write_table(mt9p111_regs.effect_cfg_settings[effect],
					mt9p111_regs.effect_cfg_settings_size);
	}

	if (rc < 0)
	{
		SKYCERR("CAMERA_WB I2C FAIL!!! return~~\n");
		return rc;
	}

	mt9p111_effect = effect;
	/* Refresh Sequencer */

	SKYCDBG("%s end\n",__func__);

	return rc;
}

#if 1 //def F_PANTECH_CAMERA_FIX_CFG_WB
static int32_t mt9p111_set_whitebalance (int mode, int32_t whitebalance)
{

	int32_t rc = 0;

	SKYCDBG("%s start  mode=%d, whitebalance=%d\n",__func__, mode, whitebalance);
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	// 튜닝 파일값
	if((mt9p111_tup_state == MT9P111_TUNE_STATE_LOAD_VALUE) && (mt9p111_params_tbl.wb[whitebalance-1].num_params > 0))
	{
		mt9p111_i2c_write_params(mt9p111_params_tbl.wb[whitebalance-1].params,
								mt9p111_params_tbl.wb[whitebalance-1].num_params);
	}
	// static 값
	else
#endif
	{
		rc = mt9p111_i2c_write_table(mt9p111_regs.wb_cfg_settings[whitebalance],
						mt9p111_regs.wb_cfg_settings_size);
	}

	if (rc < 0)
	{
		SKYCERR("CAMERA_WB I2C FAIL!!! return~~\n");
		return rc;
	}

	SKYCDBG("%s end\n",__func__);

	return rc;
}
#endif


#if 1 //def F_PANTECH_CAMERA_FIX_CFG_BRIGHTNESS
static int32_t mt9p111_set_brightness(int32_t brightness)
{
	int32_t rc = 0;
	SKYCDBG("%s start~ receive brightness = %d\n",__func__, brightness);

	if ((brightness < 0) || (brightness >= 9)) {
		SKYCERR("%s error. brightness=%d\n", __func__, brightness);
		return -EINVAL;
	}
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	// 튜닝 파일값
	if((mt9p111_tup_state == MT9P111_TUNE_STATE_LOAD_VALUE) && (mt9p111_params_tbl.bright[brightness].num_params > 0))
	{
		SKYCDBG("mt9p111_tup_state = %d, mt9p111_params_tbl.bright[0].num_params=%d\n",
			mt9p111_tup_state, brightness);
		mt9p111_i2c_write_params(mt9p111_params_tbl.bright[brightness].params,
								mt9p111_params_tbl.bright[brightness].num_params);
	}
	// static 값
	else
#endif
	{
		rc = mt9p111_i2c_write_table(mt9p111_regs.bright_cfg_settings[brightness],
					mt9p111_regs.bright_cfg_settings_size);
	}

	if (rc < 0)
	{
		SKYCERR("CAMERA_BRIGHTNESS I2C FAIL!!! return~~\n");
		return rc;
	}

	SKYCDBG("%s end\n",__func__);

	return rc;
}
#endif

///////////////////////////////////


static int32_t mt9p111_set_led_mode(int32_t led_mode)
{
	/* off, auto, on */
	/* 0x00, 0x10, 0x01 */
//	int rc = 0;
	SKYCDBG("%s: led_mode=%d\n", __func__, led_mode);
	if ((led_mode < 0) || (led_mode >= 5)) {
		SKYCERR("%s: -EINVAL, led_mode=%d\n", __func__, led_mode);
		return -EINVAL;
	}


	switch(led_mode)
	{
		case 0:
			mt9p111_gpio_out(CAM_FLASH_EN, 0);
			mt9p111_gpio_out(CAM_FLASH_MODE, 0);
			break;
		case 1:
			mt9p111_gpio_out(CAM_FLASH_EN, 0);
			mt9p111_gpio_out(CAM_FLASH_MODE, 0);
			break;
		case 2:
		case 3:
			mt9p111_gpio_out(CAM_FLASH_EN, 1);
//			mt9p111_gpio_out(CAM_FLASH_MODE, 1);
			break;
		default:
			mt9p111_gpio_out(CAM_FLASH_EN, 0);
			mt9p111_gpio_out(CAM_FLASH_MODE, 0);
			break;
	}
	return 0;
}


#if 1 //def F_PANTECH_CAMERA_FIX_CFG_AF
static int32_t mt9p111_set_auto_focus(int mode, int8_t autofocus)
{
#define AF_POLL_PERIOD	50
#define AF_POLL_RETRY	30

	int32_t rc = 0;
	int32_t i = 0;
	unsigned short read_buf = 0;
#if 1 //0 //def F_PANTECH_CAMERA_FIX_CFG_LED_MODE
	unsigned short nbrightness = 0;
//    led_trigger = FALSE;
#endif

	if( autofocus == 0 || autofocus ==2)
		auto_focus = 1;
	else
		auto_focus = 0;


	if ((autofocus < 0) || (autofocus > 3))
	{
		SKYCERR("%s FAIL!!! return~~  autofocus = %d\n",__func__,autofocus);
		return 0;//-EINVAL;
	}
#if 1 //0 //def F_PANTECH_CAMERA_FIX_CFG_LED_MODE
	if(led_auto == 1)
	{
		rc = mt9p111_i2c_write(mt9p111_client->addr, 0x098E, 0xB804,WORD_LEN);
		rc = mt9p111_i2c_read(mt9p111_client->addr, 0xB804, &nbrightness,TRIPLE_LEN);
		SKYCDBG("%s SENSOR_SNAPSHOT_MODE nbrightness =0x%x\n",__func__, nbrightness);
		if(nbrightness <= 0x38) {
			mt9p111_set_led_mode(2); // LED_MODE 01 ON
//			led_trigger = TRUE;
		}
//		msleep(50); // Need to delay 50ms before af trigger
		msleep(200); // Need to delay 200ms before af trigger becase of defocusing
	}
#endif

	SKYCDBG("%s start~  mode = %d, autofocust = %d\n",__func__,mode ,autofocus);

	rc = mt9p111_i2c_write_table(&mt9p111_regs.autofocus_trigger_cfg_settings[0],
					mt9p111_regs.autofocus_trigger_cfg_settings_size);
	if (rc < 0)
	{
		SKYCERR("CAMERA_AUTO_FOCUS I2C FAIL_1!!! return~~\n");
		return rc;
	}

       rc =	mt9p111_i2c_write(mt9p111_client->addr,
			0x098E, 0xB006,
			WORD_LEN);
	if (rc < 0)
	{
		SKYCERR("CAMERA_AUTO_FOCUS I2C FAIL_2!!! return~~\n");
		return rc;
	}

	for (i = 0; i < AF_POLL_RETRY; i++)
	{
		msleep(AF_POLL_PERIOD);

		rc = mt9p111_i2c_read(mt9p111_client->addr, 0x0990, &read_buf, WORD_LEN);
		if (rc < 0)
		{
			SKYCERR("AUTO FOCUS READ I2C FAIL!!! return~~\n");
#if 1 //0 //def F_PANTECH_CAMERA_FIX_CFG_LED_MODE
			if(led_auto == 1 && nbrightness <= 0x38)
				mt9p111_set_led_mode(0);
#endif
			return rc;
		}

		SKYCDBG("AF polling num = %d,  read_buf = %x\n", i, read_buf);

		if(read_buf == 0)
			break;
	}

	/* Last preview frame is not focused for some reason,
	 * so add delay here to view several frames after AF is done. */
	msleep(150);

	SKYCDBG("%s end\n",__func__);
#if 1 //0 //def F_PANTECH_CAMERA_FIX_CFG_LED_MODE
	if(led_auto == 1 && nbrightness <= 0x38){
		mt9p111_set_led_mode(0);
		/* There's enough time between AF and snapshot, so don't need
		 * fixed delay here. */
//		msleep(300);
	}
#endif
	return rc;
}
#endif

#if 0 //def F_PANTECH_CAMERA_FIX_CFG_ISO
static int32_t mt9p111_set_iso(int32_t iso)
{
	/* auto, N/A, ISO100, ISO200, ISO400, ISO800, ISO1600, ISO3200 */
	uint8_t data[] = {0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
	int32_t rc = 0;

#if 0
//	if(mode == SENSOR_SNAPSHOT_MODE) return 0;

	if ((iso < 0) || (iso >= sizeof(data)) || (iso == 1))
		return -EINVAL;

	SKYCDBG("%s  iso = %d\n",__func__, iso);

	rc = mt9p111_i2c_write(mt9p111_client->addr, 0x17, data[iso],WORD_LEN);
#endif

	return rc;
}
#endif

#if 1 //def F_PANTECH_CAMERA_FIX_CFG_SCENE_MODE
static int32_t mt9p111_set_scene_mode(int mode, int32_t scene_mode)
{
	/* +off, +auto, +portrait, +landscape, indoor, +night, +sports, dawn,
	 * snow, autumn, text, +sunset, backlight, beach, party */
	//uint8_t data[] = {0x00, 0x03, 0x05, 0x09, 0x0b, 0x0d, 0x0f, 0x11,
	//	0x13, 0x15, 0x1b, 0x1d, 0x1f, 0x21, 0x23};

	int32_t rc = 0;

	if ((scene_mode < 0) || (scene_mode > MT9P111_CFG_SCENE_MODE_MAX))
	{
		SKYCERR("%s not support scene mode  = %d\n",__func__,scene_mode);
		return 0;//-EINVAL;
	}
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	if((mt9p111_tup_state == MT9P111_TUNE_STATE_LOAD_VALUE) && (mt9p111_params_tbl.scene[scene_mode].num_params > 0))
	{
		mt9p111_i2c_write_params(mt9p111_params_tbl.scene[scene_mode].params,
								mt9p111_params_tbl.scene[scene_mode].num_params);
	}
	// static 값
	else
#endif
	{
		rc = mt9p111_i2c_write_table(mt9p111_regs.scene_mode_cfg_settings[scene_mode],
					mt9p111_regs.scene_mode_cfg_settings_size);
	}

	if (rc < 0)
	{
		SKYCERR("%s FAIL!!! return~~\n",__func__);
		return rc;
	}

	SKYCDBG("%s end\n",__func__);

	return rc;
}
#endif

#if 1 //def F_PANTECH_CAMERA_FIX_CFG_FOCUS_STEP
static int32_t mt9p111_set_focus_step(int mode, int32_t focus_step)
{
	int32_t rc = 0;

	if ((focus_step < 0) || (focus_step >= MT9P111_CFG_FOCUS_STEP_MAX))
	{
		SKYCERR("%s FAIL!!! return~~  focus_step = %d\n",__func__,focus_step);
		return 0;//-EINVAL;
	}
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	if((mt9p111_tup_state == MT9P111_TUNE_STATE_LOAD_VALUE) && (mt9p111_params_tbl.focus_step[0].num_params > 0))
	{
		mt9p111_i2c_write_params(mt9p111_params_tbl.focus_step[focus_step].params,
								mt9p111_params_tbl.focus_step[focus_step].num_params);
	}
	// static 값
	else
#endif
	{
		rc = mt9p111_i2c_write_table(mt9p111_regs.focus_step_cfg_settings[focus_step],
						mt9p111_regs.focus_step_cfg_settings_size);
	}

	if (rc < 0)
	{
		SKYCERR("%s FAIL!!! return~~\n",__func__);
		return rc;
	}

	SKYCDBG("%s end\n",__func__);

	return rc;
}
#endif

#if 1 //def F_PANTECH_CAMERA_ADD_CFG_ANTISHAKE
static int32_t mt9p111_set_antishake(int32_t antishake)
{
	int32_t rc = 0;

	if ((antishake < 0) || (antishake >= MT9P111_CFG_ANTISHAKE_MAX))
	{
		SKYCERR("%s FAIL!!! return~~  antishake = %d\n",__func__,antishake);
		return 0;//-EINVAL;
	}
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	if((mt9p111_tup_state == MT9P111_TUNE_STATE_LOAD_VALUE) && (mt9p111_params_tbl.antishake[antishake].num_params > 0))
	{
		mt9p111_i2c_write_params(mt9p111_params_tbl.antishake[antishake].params,
								mt9p111_params_tbl.antishake[antishake].num_params);
	}
	// static 값
	else
#endif
	{
		rc = mt9p111_i2c_write_table(mt9p111_regs.antishake_cfg_settings[antishake],
					mt9p111_regs.antishake_cfg_settings_size);
	}

	if (rc < 0)
	{
		SKYCERR("%s FAIL!!! return~~\n",__func__);
		return rc;
	}

	SKYCDBG("%s end\n",__func__);

	return rc;
}
#endif

#if 1 //def F_PANTECH_CAMERA_FIX_CFG_EXPOSURE
static int32_t mt9p111_set_exposure_mode(int32_t exposure)
{
	int32_t rc = 0;

	SKYCDBG("%s  exposure = %d\n",__func__, exposure);

	if(auto_focus == 1)
		return 0;

	if ((exposure < 0) || (exposure >= MT9P111_CFG_EXPOSURE_MAX))
	{
		SKYCERR("%s FAIL!!! return~~  exposure = %d\n",__func__,exposure);
		return 0;//-EINVAL;
	}
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	if((mt9p111_tup_state == MT9P111_TUNE_STATE_LOAD_VALUE) && (mt9p111_params_tbl.exposure[exposure].num_params > 0))
	{
		mt9p111_i2c_write_params(mt9p111_params_tbl.exposure[exposure].params,
								mt9p111_params_tbl.exposure[exposure].num_params);
	}
	// static 값
	else
#endif
	{
		rc = mt9p111_i2c_write_table(mt9p111_regs.exposure_cfg_settings[exposure],
					mt9p111_regs.exposure_cfg_settings_size);
	}

	if (rc < 0)
	{
		SKYCERR("CAMERA_EFFECT_SEPIA I2C FAIL!!! return~~\n");
		return rc;
	}

	SKYCDBG("%s end\n",__func__);

	return rc;
}
#endif

#if 1 //def F_PANTECH_CAMERA_FIX_CFG_REFLECT
static int32_t mt9p111_set_reflect(int32_t reflect)
{
	int32_t rc = 0;
	int32_t i = 0;
	uint16_t npolling = 0;

	SKYCDBG("%s  reflect = %d\n",__func__, reflect);

	if ((reflect < 0) || (reflect >= MT9P111_CFG_REFLECT_MAX))
	{
		SKYCERR("%s FAIL!!! return~~  reflect = %d\n",__func__,reflect);
		return 0;//-EINVAL;
	}
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	if((mt9p111_tup_state == MT9P111_TUNE_STATE_LOAD_VALUE) && (mt9p111_params_tbl.reflect[reflect].num_params > 0))
	{
		mt9p111_i2c_write_params(mt9p111_params_tbl.reflect[reflect].params,
								mt9p111_params_tbl.reflect[reflect].num_params);
	}
	// static 값
	else
#endif
	{
		rc = mt9p111_i2c_write_table(mt9p111_regs.reflect_cfg_settings[reflect],
					mt9p111_regs.reflect_cfg_settings_size);
	}

	for (i = 0; i < MT9P111_POLLING_RETRY; i++) {
		rc = mt9p111_i2c_read(mt9p111_client->addr,
		0x8404, &npolling,TRIPLE_LEN);
		if (rc < 0)
		{
			SKYCERR("mt9p111_snapshot_config read  FAIL!!! return~~\n");
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
		SKYCERR("CAMERA_SET_REFLECT I2C FAIL!!! return~~\n");
		return rc;
	}

	SKYCDBG("%s end\n",__func__);

	return rc;
}
#endif

#if 0 //def F_PANTECH_CAMERA_FIX_CFG_CAF
static int mt9p111_set_caf(int32_t caf)
{
	int32_t rc = 0;

	SKYCDBG("%s  continous_af = %d\n",__func__, caf);

	if(caf == 1)
	{
		rc = mt9p111_i2c_write_table(mt9p111_regs.caf_cfg_settings,
					mt9p111_regs.caf_cfg_setting_size);
	}
	else
	{
		rc = mt9p111_i2c_write_table(mt9p111_regs.fastmode_cfg_settings,
					mt9p111_regs.fastmode_cfg_setting_size);
	}

	SKYCDBG("%s end\n", __func__);
	return rc;
}
#endif




#if 1 //def F_PANTECH_CAMERA_FIX_CFG_FOCUS_RECT
static int32_t mt9p111_set_focus_rect(uint32_t focus_rect)
{
	uint8_t xleft, xright, ytop, ybottom;
	uint8_t nwidth, nheight;
	int32_t rc = 0;

	SKYCDBG("%s  focus_rect = %d\n",__func__, focus_rect);

	xleft   = (uint8_t)((focus_rect & 0x0f000000) >> 24);
	xright  = (uint8_t)((focus_rect & 0x000f0000) >> 16);
	ytop    = (uint8_t)((focus_rect & 0x00000f00) >> 8);
	ybottom = (uint8_t)((focus_rect & 0x0000000f));

	if ((xleft == 0) && (xright == 0) && (ytop == 0) && (ybottom == 0)) {
		//mv9337_focus_rect_mode = MV9337_FOCUS_RECT_AUTO;
		//return 0;
		xleft = 0x20;
		ytop = 0x20;
		nwidth = 0xBF;
		nheight = 0xBF;
	}
	else
	{
		xleft   = xleft*16;
		xright  = ((xright+1)*16 )- 1;
		ytop    = ytop*16;
		ybottom = ((ybottom+1)*16 )- 1;

		nwidth = xright - xleft;
		nheight = ybottom - ytop;
	}

	SKYCDBG("%s  xleft = %d, ytop = %d, xright = %d, ybottom = %d\n",__func__, xleft, ytop, xright, ybottom);
	SKYCDBG("%s  nwidth = %d, nheight = %d\n",__func__, nwidth, nheight);

	rc =	mt9p111_i2c_write(mt9p111_client->addr,	0xB854, xleft, TRIPLE_LEN);
	if (rc < 0) {
		SKYCERR("CAMERA_AUTO_FOCUS I2C FAIL_1!!! return~~\n");
		return rc;
	}
	rc =	mt9p111_i2c_write(mt9p111_client->addr,	0xB855, ytop, TRIPLE_LEN);
	if (rc < 0) {
		SKYCERR("CAMERA_AUTO_FOCUS I2C FAIL_2!!! return~~\n");
		return rc;
	}
	rc =	mt9p111_i2c_write(mt9p111_client->addr,	0xB856, nwidth, TRIPLE_LEN);
	if (rc < 0) {
		SKYCERR("CAMERA_AUTO_FOCUS I2C FAIL_3!!! return~~\n");
		return rc;
	}
	rc =	mt9p111_i2c_write(mt9p111_client->addr,	0xB857, nheight, TRIPLE_LEN);
	if (rc < 0) {
		SKYCERR("CAMERA_AUTO_FOCUS I2C FAIL_4!!! return~~\n");
		return rc;
	}


	rc = mt9p111_i2c_write_table(&mt9p111_regs.autofocus_rect_cfg_settings[0],
					mt9p111_regs.autofocus_rect_cfg_settings_size);
	if (rc < 0)
	{
		SKYCERR("CAMERA_AUTO_FOCUS I2C FAIL_1!!! return~~\n");
		return rc;
	}

//	mt9p111_set_auto_focus(0, 1);


/*
	//xc = xleft * 16 + xright;
	//yc = ytop * 16 + ybottom;

	rc = mt9p111_cmd(mt9p111_client->addr, 0x21, xc);
	if (rc < 0)
		return rc;

	rc = mt9p111_cmd(mt9p111_client->addr, 0x22, yc);
	if (rc < 0)

*/		return rc;

	return 0;
}
#endif

#if 0 //def F_PANTECH_CAMERA_ADD_CFG_DIMENSION
static int32_t mt9p111_set_dimension(struct dimension_cfg *dimension)
{
	memcpy(&mt9p111_dimension, dimension, sizeof(struct dimension_cfg));
	SKYCDBG("%s: preview=%dx%d, snapshot=%dx%d\n", __func__,
		dimension->prev_dx, dimension->prev_dy,
		dimension->pict_dx, dimension->pict_dy);
	return 0;
}
#endif

#if 1 //def F_PANTECH_CAMERA_FIX_CFG_ANTIBANDING
static int32_t mt9p111_set_antibanding(int32_t antibanding)
{
	int32_t rc = 0;

	if ((antibanding < 0) || (antibanding >= MT9P111_CFG_FLICKER_MAX))
	{
		SKYCERR("%s FAIL!!! return~~  antibanding = %d\n",__func__,antibanding);
		return 0;//-EINVAL;
	}
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	if((mt9p111_tup_state == MT9P111_TUNE_STATE_LOAD_VALUE) && (mt9p111_params_tbl.flicker[antibanding].num_params > 0))
	{
		mt9p111_i2c_write_params(mt9p111_params_tbl.flicker[antibanding].params,
								mt9p111_params_tbl.flicker[antibanding].num_params);
	}
	// static 값
	else
#endif
	{
		rc = mt9p111_i2c_write_table(mt9p111_regs.flicker_cfg_settings[antibanding],
					mt9p111_regs.flicker_cfg_settings_size);
	}

	if (rc < 0)
	{
		SKYCERR("%s FAIL!!! return~~\n",__func__);
		return rc;
	}

	SKYCDBG("%s end\n",__func__);
	return rc;
}
#endif


#if 1 //def F_PANTECH_CAMERA_FIX_CFG_PREVIEW_FPS
//static int32_t mt9p111_set_preview_fps(int32_t preview_fps)
static int32_t mt9p111_set_frame_rate(int16_t preview_fps)
{
	/* 0 : variable 30fps, 1 ~ 30 : fixed fps */
	/* default: variable 8 ~ 30fps */
	int32_t rc = 0;
	if( preview_fps > 27)
		preview_fps=27;

	if ((preview_fps < C_PANTECH_CAMERA_MIN_PREVIEW_FPS) ||
		(preview_fps > C_PANTECH_CAMERA_MAX_PREVIEW_FPS)) {
		SKYCERR("%s: -EINVAL, preview_fps=%d\n",
			__func__, preview_fps);
		return -EINVAL;
	}

	SKYCDBG("%s: preview_fps=%d\n", __func__, preview_fps);
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	if((mt9p111_tup_state == MT9P111_TUNE_STATE_LOAD_VALUE) && (mt9p111_params_tbl.ffps[preview_fps-5].num_params > 0))
	{
		mt9p111_i2c_write_params(mt9p111_params_tbl.ffps[preview_fps-5].params,
								mt9p111_params_tbl.ffps[preview_fps-5].num_params);
	}
	// static 값
	else
#endif
	{
		rc = mt9p111_i2c_write_table(mt9p111_regs.preview_fps_cfg_settings[preview_fps-5],
					mt9p111_regs.preview_fps_cfg_settings_size);
	}

	SKYCDBG("%s end rc = %d\n",__func__, rc);

	return 0;
}
#endif

#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
static u_int32_t mt9p111_i2c_write_params
(
    mt9p111_cfg_param_t *params,
    u_int32_t num_params
)
{
    u_int16_t read_data = 0;
    u_int16_t write_data = 0;
    u_int8_t write_data_byte = 0;
    u_int32_t i = 0;

    SKYCDBG(">>%s ", __func__);
    SKYCDBG("- params = %p, num_params = %d", params, num_params);

    if ((params == NULL) || (num_params == 0))
    {
        SKYCERR("Invalid param! [params = 0x%08x][num_params = %d]", (u_int32_t)params, num_params);
        return -EIO;
    }

    for (i = 0; i < num_params; i++)
    {
        switch(params[i].op)
        {
            case CAMOP_NOP:
                SKYCDBG("WRITE: NOP");
                break;

            case CAMOP_DELAY:
                SKYCDBG("WRITE: DELAY (%dms)", params[i].data);
                msleep(params[i].data);
                break;

            case CAMOP_WRITE:
                if (mt9p111_i2c_write_word(params[i].addr, params[i].data) < 0)
                {
                    SKYCERR("<<%s (-EIO)", __func__);
                    return -EIO;
                }
                break;
	    	 case CAMOP_WRITE_DATA1:
			 //배열의 2바이트 데이터를 1바이트로 캐스팅
 	 		 write_data_byte = (unsigned char)params[i].data;
             	if (mt9p111_i2c_write_a2d1(params[i].addr, write_data_byte) < 0)
            	 {
                    SKYCERR("<<%s (-EIO)", __func__);
                    return -EIO;
          	     }
                break;
            case CAMOP_WRITE_BIT:
            {
                u_int16_t bit_pos = 0;
                u_int16_t bit_val = 0;

                if (mt9p111_i2c_read_word(params[i].addr, &read_data) < 0)
                {
                    SKYCERR("<<%s (-EIO)", __func__);
                    return -EIO;
                }

                bit_pos = (params[i].data & 0xff00) >> 8;
                bit_val = (params[i].data & 0x00ff);

                if (bit_val != 0) // set to 1
                {
                    write_data = read_data | (0x0001 << bit_pos);
                    SKYCDBG("set 0x%04x[%d] to 1", params[i].addr, bit_pos);
                }
                else // set to 0
                {
                    write_data = read_data & (~(0x0001 << bit_pos));
                    SKYCDBG("set 0x%04x[%d] to 0", params[i].addr, bit_pos);
                }

                if (mt9p111_i2c_write_word(params[i].addr, write_data) < 0)
                {
                    SKYCERR("<<%s (-EIO)", __func__);
                    return -EIO;
                }
                break;
            }

            case CAMOP_POLL_REG:
            {
                u_int16_t poll_delay = 0;
                u_int16_t poll_retry = 0;
                u_int16_t poll_reg = 0;
                u_int16_t poll_data = 0;
                u_int16_t poll_mask = 0;
                u_int16_t retry_cnt = 0;

                if (params[i+1].op != CAMOP_POLL_REG)
                {
                    SKYCERR("<<%s (-EIO)", __func__);
                    return -EIO;
                }

                poll_delay = (params[i+1].data & 0xff00) >> 8;
                poll_retry = (params[i+1].data & 0x00ff);
                poll_reg = params[i].addr;
                poll_data = params[i+1].addr;
                poll_mask = params[i].data;;

                SKYCDBG("start polling register... if [0x%04x] AND 0x%04x equals 0x%04x, then stop polling", poll_reg, poll_mask, poll_data);


                for (retry_cnt = 0; retry_cnt < poll_retry; retry_cnt++)
                {
                    if (mt9p111_i2c_read_word(poll_reg, &read_data) < 0)
                    {
                        SKYCERR("<<%s (-EIO)", __func__);
                        return -EIO;
                    }

                    if ((read_data & poll_mask) != poll_data)
                    {
                        SKYCDBG("retry polling register... after sleeping %d ms", poll_delay);
                        msleep(poll_delay);
                    }
                    else
                    {
                        SKYCDBG("stop polling register... retried %d/%d time(s) (delay = %d ms)", retry_cnt, poll_retry, poll_delay);
                        break;
                    }
                }

                if (retry_cnt == poll_retry)
                {
                    SKYCERR("<<%s (-EIO)", __func__);
                    return -EIO;
                }

                ++i;
                break;
            }

            case CAMOP_POLL_MCU_VAR:
            {
                u_int16_t poll_delay = 0;
                u_int16_t poll_retry = 0;
                u_int16_t poll_mcu_var = 0;
                u_int16_t poll_data = 0;
                u_int16_t poll_mask = 0;
                u_int16_t retry_cnt = 0;

                if (params[i+1].op != CAMOP_POLL_MCU_VAR)
                {
                    SKYCERR("<<%s (-EIO)", __func__);
                    return -EIO;
                }

                poll_delay = (params[i+1].data & 0xff00) >> 8;
                poll_retry = (params[i+1].data & 0x00ff);
                poll_mcu_var = params[i].addr;
                poll_data = params[i+1].addr;
                poll_mask = params[i].data;;

                SKYCDBG("start polling MCU variable... if [0x%04x] AND 0x%04x equals 0x%04x, then stop polling", poll_mcu_var, poll_mask, poll_data);


                for (retry_cnt = 0; retry_cnt < poll_retry; retry_cnt++)
                {
                    if (mt9p111_i2c_write_word(MT9P111_REG_MCU_ADDR, poll_mcu_var) < 0)
                    {
                        SKYCERR("<<%s (-EIO)", __func__);
                        return -EIO;
                    }

                    if (mt9p111_i2c_read_word(MT9P111_REG_MCU_DATA, &read_data) < 0)
                    {
                        SKYCERR("<<%s (-EIO)", __func__);
                        return -EIO;
                    }

                    if ((read_data & poll_mask) != poll_data)
                    {
                        SKYCDBG("retry polling MCU variable... after sleeping %d ms", poll_delay);
                        msleep(poll_delay);
                    }
                    else
                    {
                        SKYCDBG("stop polling MCU variable... retried %d/%d time(s) (delay = %d ms)", retry_cnt, poll_retry, poll_delay);
                        break;
                    }
                }

                if (retry_cnt == poll_retry)
                {
                    SKYCERR("<<%s (-EIO)", __func__);
                    return -EIO;
                }

                ++i;
                break;
            }

            default:
                SKYCERR("Invalid op code! [op = %d]", params[i].op);
                return -EIO;
                break;
        }

    }

    SKYCDBG("<<%s (TRUE)", __func__);
    return TRUE;
}

static void mt9p111_init_params_tbl (int8_t *stream)
{
#if 0 //defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
    mt9p111_cfg_param_t *params = NULL;
    u_int32_t num_params = 0;
#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
    u_int32_t i = 0;

    SKYCDBG(">>%s ()", __func__);

#if 0 //defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
    camsensor_mt9p111_tup_init(stream);
    num_params = camsensor_mt9p111_tup_get_init_params(&params);
    if ((num_params > 0) && (params != NULL))
    {
        mt9p111_params_tbl.init.num_params = num_params;
        mt9p111_params_tbl.init.params = params;
 //       SKYCDBG("MT9P111 INIT params are loaded from TUNEUP file!", 0, 0, 0);
    }
    else
#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
    {
        mt9p111_params_tbl.init.num_params = num_params;
        mt9p111_params_tbl.init.params = params;
 //       SKYCDBG("MT9P111 INIT params are loaded from TUNEUP file!", 0, 0, 0);
#if 0
        mt9p111_params_tbl.init.num_params = sizeof(mt9p111_cfg_init_params) / sizeof(mt9p111_cfg_param_t);
        mt9p111_params_tbl.init.params = &mt9p111_cfg_init_params[0];
        SKYCDBG("MT9P111 INIT params are loaded from RO data!", 0, 0, 0);
#endif
    }

    for (i = 0; i < MT9P111_CFG_WB_MAX; i++)
    {
#if 0 //defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
        num_params = camsensor_mt9p111_tup_get_wb_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            mt9p111_params_tbl.wb[i].num_params = num_params;
            mt9p111_params_tbl.wb[i].params = params;
 //           SKYCDBG("MT9P111 WB params [%d] are loaded from TUNEUP file!", i, 0, 0);
        }
        else
#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
        {
            mt9p111_params_tbl.wb[i].num_params = num_params;
            mt9p111_params_tbl.wb[i].params = params;
 //           SKYCDBG("MT9P111 WB params [%d] are loaded from TUNEUP file!", i, 0, 0);
#if 0
            mt9p111_params_tbl.wb[i].num_params = MT9P111_CFG_WB_MAX_PARAMS;
            mt9p111_params_tbl.wb[i].params = &mt9p111_cfg_wb_params[i][0];
            SKYCDBG("MT9P111 WB params [%d] are loaded from RO data!", i, 0, 0);
#endif
        }
    }

    for (i = 0; i < MT9P111_CFG_BRIGHT_MAX; i++)
    {
#if 0 //defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
        num_params = camsensor_mt9p111_tup_get_bright_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            mt9p111_params_tbl.bright[i].num_params = num_params;
            mt9p111_params_tbl.bright[i].params = params;
//            SKYCDBG("MT9P111 BRIGHT params [%d] are loaded from TUNEUP file!", i, 0, 0);
        }
        else
#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
        {
      	     mt9p111_params_tbl.bright[i].num_params = num_params;
            mt9p111_params_tbl.bright[i].params = params;
//            SKYCDBG("MT9P111 BRIGHT params [%d] are loaded from TUNEUP file!", i, 0, 0);
#if 0
            mt9p111_params_tbl.bright[i].num_params = MT9P111_CFG_BRIGHT_MAX_PARAMS;
            mt9p111_params_tbl.bright[i].params = &mt9p111_cfg_bright_params[i][0];
            SKYCDBG("MT9P111 BRIGHT params [%d] are loaded from RO data!", i, 0, 0);
#endif
        }

    }

    for (i = 0; i < MT9P111_CFG_EXPOSURE_MAX; i++)
    {
#if 0 //defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
        num_params = camsensor_mt9p111_tup_get_exposure_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            mt9p111_params_tbl.exposure[i].num_params = num_params;
            mt9p111_params_tbl.exposure[i].params = params;
 //           SKYCDBG("MT9P111 EXPOSURE params [%d] are loaded from TUNEUP file!", i, 0, 0);
        }
        else
#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
        {
            mt9p111_params_tbl.exposure[i].num_params = num_params;
            mt9p111_params_tbl.exposure[i].params = params;
//            SKYCDBG("MT9P111 EXPOSURE params [%d] are loaded from TUNEUP file!", i, 0, 0);
#if 0
            mt9p111_params_tbl.exposure[i].num_params = MT9P111_CFG_EXPOSURE_MAX_PARAMS;
            mt9p111_params_tbl.exposure[i].params = &mt9p111_cfg_exposure_params[i][0];
            SKYCDBG("MT9P111 EXPOSURE params [%d] are loaded from RO data!", i, 0, 0);
#endif
        }

    }

    for (i = MT9P111_CFG_FFPS_MIN; i <= MT9P111_CFG_FFPS_MAX; i++)
    {
#if 0 //defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
        num_params = camsensor_mt9p111_tup_get_ffps_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            mt9p111_params_tbl.ffps[i - MT9P111_CFG_FFPS_MIN].num_params = num_params;
            mt9p111_params_tbl.ffps[i - MT9P111_CFG_FFPS_MIN].params = params;
 //           SKYCDBG("MT9P111 fixed FPS params [%d] are loaded from TUNEUP file!", i - MT9P111_CFG_FFPS_MIN, 0, 0);
        }
        else
#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
        {
            mt9p111_params_tbl.ffps[i - MT9P111_CFG_FFPS_MIN].num_params = num_params;
            mt9p111_params_tbl.ffps[i - MT9P111_CFG_FFPS_MIN].params = params;
//            SKYCDBG("MT9P111 fixed FPS params [%d] are loaded from TUNEUP file!", i - MT9P111_CFG_FFPS_MIN, 0, 0);
#if 0
            mt9p111_params_tbl.ffps[i - MT9P111_CFG_FFPS_MIN].num_params = MT9P111_CFG_FFPS_MAX_PARAMS;
            mt9p111_params_tbl.ffps[i - MT9P111_CFG_FFPS_MIN].params = &mt9p111_cfg_ffps_params[i - MT9P111_CFG_FFPS_MIN][0];
//            SKYCDBG("MT9P111 fixed FPS params [%d] are loaded from RO data!", i - MT9P111_CFG_FFPS_MIN, 0, 0);
#endif
        }
    }

    for (i = 0; i < MT9P111_CFG_REFLECT_MAX; i++)
    {
#if 0 //defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
        num_params = camsensor_mt9p111_tup_get_reflect_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            mt9p111_params_tbl.reflect[i].num_params = num_params;
            mt9p111_params_tbl.reflect[i].params = params;
//            SKYCDBG("MT9P111 REFLECT params [%d] are loaded from TUNEUP file!", i, 0, 0);
        }
        else
#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
        {
            mt9p111_params_tbl.reflect[i].num_params = num_params;
            mt9p111_params_tbl.reflect[i].params = params;
//            SKYCDBG("MT9P111 REFLECT params [%d] are loaded from TUNEUP file!", i, 0, 0);
#if 0
            mt9p111_params_tbl.reflect[i].num_params = MT9P111_CFG_FFPS_MAX_PARAMS;
            mt9p111_params_tbl.reflect[i].params = &mt9p111_cfg_reflect_params[i][0];
//            SKYCDBG("MT9P111 REFLECT params [%d] are loaded from RO data!", i, 0, 0);
#endif
        }
    }

    for (i = 0; i < MT9P111_CFG_EFFECT_MAX; i++)
    {
#if 0 //defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
        num_params = camsensor_mt9p111_tup_get_effect_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            mt9p111_params_tbl.effect[i].num_params = num_params;
            mt9p111_params_tbl.effect[i].params = params;
//            SKYCDBG("MT9P111 EFFECT params [%d] are loaded from TUNEUP file!", i, 0, 0);
        }
        else
#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
        {
            mt9p111_params_tbl.effect[i].num_params = num_params;
            mt9p111_params_tbl.effect[i].params = params;
//            SKYCDBG("MT9P111 EFFECT params [%d] are loaded from TUNEUP file!", i, 0, 0);
#if 0
            mt9p111_params_tbl.effect[i].num_params = MT9P111_CFG_EFFECT_MAX_PARAMS;
            mt9p111_params_tbl.effect[i].params = &mt9p111_cfg_effect_params[i][0];
            SKYCDBG("MT9P111 EFFECT params [%d] are loaded from RO data!", i, 0, 0);
#endif
	}
    }

for (i = 0; i < MT9P111_CFG_FLICKER_MAX; i++)
    {
#if 0 //defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
        num_params = camsensor_mt9p111_tup_get_flicker_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            mt9p111_params_tbl.flicker[i].num_params = num_params;
            mt9p111_params_tbl.flicker[i].params = params;
//            SKYCDBG("MT9P111 FLICKER params [%d] are loaded from TUNEUP file!", i, 0, 0);
        }
        else
#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
        {
            mt9p111_params_tbl.flicker[i].num_params = num_params;
            mt9p111_params_tbl.flicker[i].params = params;
//	     SKYCDBG("MT9P111 FLICKER params [%d] are loaded from TUNEUP file!", i, 0, 0);
#if 0
            mt9p111_params_tbl.flicker[i].num_params = MT9P111_CFG_FLICKER_MAX_PARAMS;
            mt9p111_params_tbl.flicker[i].params = &mt9p111_cfg_flicker_params[i][0];
            SKYCDBG("MT9P111 FLICKER params [%d] are loaded from RO data!", i, 0, 0);
#endif
	}
    }

for (i = 0; i < MT9P111_CFG_ANTISHAKE_MAX; i++)
    {
#if 0 //defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
        num_params = camsensor_mt9p111_tup_get_antishake_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            mt9p111_params_tbl.antishake[i].num_params = num_params;
            mt9p111_params_tbl.antishake[i].params = params;
//            SKYCDBG("MT9P111 ANTISHAKE params [%d] are loaded from TUNEUP file!", i, 0, 0);
        }
        else
#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
        {
            mt9p111_params_tbl.antishake[i].num_params = num_params;
            mt9p111_params_tbl.antishake[i].params = params;
//            SKYCDBG("MT9P111 ANTISHAKE params [%d] are loaded from TUNEUP file!", i, 0, 0);
#if 0
            mt9p111_params_tbl.antishake[i].num_params = MT9P111_CFG_ANTISHAKE_MAX_PARAMS;
            mt9p111_params_tbl.antishake[i].params = &mt9p111_cfg_antishake_params[i][0];
//            SKYCDBG("MT9P111 ANTISHAKE params [%d] are loaded from RO data!", i, 0, 0);
#endif
	}
    }

for (i = 0; i < MT9P111_CFG_FOCUS_STEP_MAX; i++)
    {
#if 0 //defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
        num_params = camsensor_mt9p111_tup_get_focus_step_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            mt9p111_params_tbl.focus_step[i].num_params = num_params;
            mt9p111_params_tbl.focus_step[i].params = params;
//            SKYCDBG("MT9P111 FOCUS_STEP params [%d] are loaded from TUNEUP file!", i, 0, 0);
        }
        else
#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
        {
            mt9p111_params_tbl.focus_step[i].num_params = num_params;
            mt9p111_params_tbl.focus_step[i].params = params;
//            SKYCDBG("MT9P111 FOCUS_STEP params [%d] are loaded from TUNEUP file!", i, 0, 0);
#if 0
            mt9p111_params_tbl.focus_step[i].num_params = MT9P111_CFG_FOCUS_STEP_MAX_PARAMS;
            mt9p111_params_tbl.focus_step[i].params = &mt9p111_cfg_focus_step_params[i][0];
            SKYCDBG("MT9P111 FOCUS_STEP params [%d] are loaded from RO data!", i, 0, 0);
#endif
	}
    }

for (i = 0; i < MT9P111_CFG_SCENE_MODE_MAX; i++)
    {
#if 0 //defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
        num_params = camsensor_mt9p111_tup_get_scene_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            mt9p111_params_tbl.scene[i].num_params = num_params;
            mt9p111_params_tbl.scene[i].params = params;
//            SKYCDBG("MT9P111 SCENE params [%d] are loaded from TUNEUP file!", i, 0, 0);
        }
        else
#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
        {
            mt9p111_params_tbl.scene[i].num_params = num_params;
            mt9p111_params_tbl.scene[i].params = params;
//            SKYCDBG("MT9P111 SCENE params [%d] are loaded from TUNEUP file!", i, 0, 0);
#if 0
            mt9p111_params_tbl.scene[i].num_params = MT9P111_CFG_SCENE_MAX_PARAMS;
            mt9p111_params_tbl.scene[i].params = &mt9p111_cfg_scene_params[i][0];
            SKYCDBG("MT9P111 SCENE params [%d] are loaded from RO data!", i, 0, 0);
#endif
	}
    }

    SKYCDBG("<<%s ()", __func__);
    return;
}
#endif

#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
static int mt9p111_set_tune_value(const struct sensor_cfg_data *scfg)
{
	int32_t rc = 0;
	int32_t loop_count = 0;
	int32_t i = 0;
	int32_t last_size = 0;
	int32_t copy_size = 0;
	uint8_t * pKTune_stream = NULL;
	uint8_t * pUTune_stream = NULL;

if(mt9p111_tup_state != MT9P111_TUNE_STATE_LOAD_VALUE)
{
	SKYCDBG("%s start\n",__func__);

	pKTune_stream = kmalloc(scfg->cfg.tune_value.file_size, GFP_KERNEL);
	SKYCDBG("pKTune_stream =%p\n", pKTune_stream);
	if (!pKTune_stream) {
		SKYCERR("failed to malloc.\n");
		rc = -ENOMEM;
		goto update_fw_user_fail;
	}

	//유저인터페이스에서 튜닝 파일 스트림을 얻어옴
	pUTune_stream = scfg->cfg.tune_value.pfile_stream;
	//페이지 단위 사이즈로 나누어 loop_count 저장
	loop_count = (scfg->cfg.tune_value.file_size / 4096) + 1;
	last_size = scfg->cfg.tune_value.file_size % 4096;
	copy_size = 4096;

	SKYCDBG("scfg->cfg.tune_value.pfile_stream=%p, scfg->cfg.tune_value.file_size=%d\n", scfg->cfg.tune_value.pfile_stream, scfg->cfg.tune_value.file_size);
	for(i = 0; i < loop_count; i++)
	{
		//마지막 루프일 경우 남은 사이즈로 변경
		if(i == (loop_count-1))
			copy_size = last_size;
		else
			copy_size = 4096;

		if (copy_from_user(pKTune_stream + i*copy_size, (void *)pUTune_stream +  i*copy_size, copy_size))
		{
			rc = -EFAULT;
			goto update_fw_user_fail;
        	}
		//SKYCDBG(" i =%d, loop_count=%d, copy_size=%d, pKTune_stream=%x\n", i, loop_count, copy_size, pKTune_stream);
	}

	//파일 스트림에서 개별 튜닝값을 배열에 로딩
	mt9p111_init_params_tbl(pKTune_stream);

	mt9p111_tup_state = MT9P111_TUNE_STATE_LOAD_VALUE;
}

#if 0
	if (pKTune_stream)
		kfree(pKTune_stream);
#endif
	SKYCDBG("%s done.\n", __func__);
	return 0;

update_fw_user_fail:
	if (pKTune_stream)
	{
		kfree(pKTune_stream);
		pKTune_stream = NULL;
	}
	SKYCERR("%s error. rc=%d\n", __func__, rc);
	return rc;
}
#endif

static int32_t mt9p111_video_config(void)
{
	int32_t rc = 0;
	uint16_t npolling = 0;
	int i;

	/* set preview resolution to 1280x960 */
	SKYCDBG("%s start\n",__func__);

#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	if(mt9p111_tup_state == MT9P111_TUNE_STATE_LOAD_VALUE)
	{
		rc = mt9p111_reg_init();
		if (rc < 0)
		{
			SKYCERR("mt9p111_i2c_write_table FAIL!!! return~~\n");
			return rc;
		}
	}
	else
#endif
	{
		rc = mt9p111_i2c_write_table(&mt9p111_regs.preview_cfg_settings[0],
						mt9p111_regs.preview_cfg_settings_size);

#if 0
	mt9p111_i2c_read_table(&mt9p111_regs.preview_cfg_settings[0],
					mt9p111_regs.preview_cfg_settings_size);
#endif

		for (i = 0; i < MT9P111_PREVIEW_RETRY; i++)
		{
			rc = mt9p111_i2c_read(mt9p111_client->addr,	0x8404, &npolling,TRIPLE_LEN);
			if (rc < 0)
			{
				SKYCERR("mt9p111_snapshot_config read  FAIL!!! return~~\n");
				//return rc;
			}
			SKYCDBG("%s: retry npolling = %x, count = %d\n", __func__,npolling, i);

			if (npolling == 0) {
				//msleep(300);
				return 0;
			}
			msleep(20);
		}
	}

	if (rc < 0)
	{
		SKYCERR("mt9p111_i2c_write_table FAIL!!! return~~\n");
		return rc;
	}
	SKYCDBG("%s end rc = %d\n",__func__, rc);

	return 0;
}

static int32_t mt9p111_snapshot_config(void)
{
	int32_t rc = 0;
	uint16_t npolling = 0;
	int i;

	/* set snapshot resolution to 2592x1944 */
	SKYCDBG("%s start\n",__func__);

#if 0 //def F_PANTECH_CAMERA_FIX_CFG_LED_MODE
	if (led_trigger == TRUE) {
		/* You MUST turn off LED in picture callback.
		 * See QualcommCameraHardware::receiveRawPicture() */
		mt9p111_set_led_mode(3);
		led_trigger = FALSE;
		/* MANDATORY: You should give settling time for MT9P111 to
		 * stabilize AE with flash light because MT9P111 doesn't run
		 * AE function in snapshot mode and uses AE-related values
		 * fixed in preview mode. You would see saturated snapshot
		 * image if there was no delay here. */
		msleep(300);
	}
#endif

	rc = mt9p111_i2c_write_table(&mt9p111_regs.snapshot_cfg_settings[0],
					mt9p111_regs.snapshot_cfg_settings_size);

	for (i = 0; i < MT9P111_SNAPSHOT_RETRY; i++) {
		rc = mt9p111_i2c_read(mt9p111_client->addr,
		0x8404, &npolling,TRIPLE_LEN);
		if (rc < 0)
		{
			SKYCERR("mt9p111_snapshot_config read  FAIL!!! return~~\n");
			//return rc;
		}
		SKYCDBG("%s: retry npolling = %x, count = %d\n", __func__,npolling, i);
		if (npolling == 0) {
			//msleep(300);
			return 0;
		}
		msleep(20);
	}
#if 0
	mt9p111_i2c_read_table(&mt9p111_regs.snapshot_cfg_settings[0],
					mt9p111_regs.snapshot_cfg_settings_size);
#endif
	if (rc < 0)
	{
		SKYCERR("mt9p111_i2c_write_table FAIL!!! return~~\n");
		return rc;
	}

	SKYCDBG("%s end rc = %d\n",__func__, rc);

	return 0;
}

static long mt9p111_set_sensor_mode(int mode)
{
	int32_t rc = 0;
	uint16_t nbrightness;
	struct msm_camera_csi_params mt9p111_csi_params;

 	SKYCDBG("mt9p111_set_sensor_mode mode = %d!\n",mode);

//	if (mode == mt9p111_sensor_mode) {
//		SKYCDBG("%s: skip, mode=%d\n", __func__, mode);
//		return 0;
//	}

	SKYCDBG("%s start\n",__func__);
	switch (mode) {
	case SENSOR_PREVIEW_MODE:

//PANTECH_CAM TO DO MIPI Setting
		if( config_csi == 0 )
		{
			SKYCDBG("mt9p111_set_sensor_mode config_csi E\n");
			mt9p111_csi_params.lane_cnt = 1; // 4;
			mt9p111_csi_params.data_format = CSI_8BIT; //CSI_10BIT;
			mt9p111_csi_params.lane_assign = 0xe4;
			mt9p111_csi_params.dpcm_scheme = 0;
			mt9p111_csi_params.settle_cnt = 0x14;
			rc = msm_camio_csi_config(&mt9p111_csi_params);
			msleep(mt9p111_delay_msecs_stream);
			config_csi = 1;
			SKYCDBG("mt9p111_set_sensor_mode config_csi X\n");
		}
		rc = mt9p111_video_config();
		multi_count = 0;
		SKYCDBG("mt9p111_video_config end multi count =%d, rc = %d\n",  multi_count, rc);
		break;

	case SENSOR_SNAPSHOT_MODE:

		if(led_auto == 1) {
			rc = mt9p111_i2c_write(mt9p111_client->addr,0x098E, 0xB804,WORD_LEN);
			rc = mt9p111_i2c_read(mt9p111_client->addr, 0xB804, &nbrightness,TRIPLE_LEN);
			SKYCDBG("%s SENSOR_SNAPSHOT_MODE nbrightness =0x%x\n",__func__, nbrightness);
			if(nbrightness <= 0x38)
				mt9p111_set_led_mode(2);
		}
		/* Switch to lower fps for Snapshot */
		if(multi_count ==0)
			rc = mt9p111_snapshot_config();

		SKYCDBG("mt9p111_snapshot_config end multi count =%d, rc = %d\n", multi_count, rc);
		multi_count++;

		break;

	case SENSOR_RAW_SNAPSHOT_MODE:
		/* Setting the effect to CAMERA_EFFECT_OFF */
		rc = mt9p111_snapshot_config();
		break;
	default:
		SKYCERR("mt9p111_set_sensor_mode DEFAULT FAIL[EINVAL] = %d\n", EINVAL);
		return -EINVAL;
	}

//	if (rc < 0)
//		return rc;

//	mt9p111_sensor_mode = mode;

	SKYCDBG("%s end rc = %d\n",__func__, rc);
	return rc;
}

#if 0
static int mt9p111_reset(const struct msm_camera_sensor_info *dev)
{
	int rc = 0;

	//standby

	if (mt9p111_gpio_out(CAM_5MP_STB, 0))		rc = -EIO;
	msleep(40);
	//reset
	if (mt9p111_gpio_out(CAM_5MP_RST_N, 0))		rc = -EIO;
	msleep(40);
	if (mt9p111_gpio_out(CAM_5MP_RST_N, 1))		rc = -EIO;
	msleep(40);

#if 1 //def F_PANTECH_CAMERA_MT9P111_NO_IMAGE_AFTER_INIT
	if (mt9p111_i2c_write_word(0x0020, 0x0000) < 0) rc = -EIO;
	msleep(1);
	if (mt9p111_gpio_out(CAM_5MP_RST_N, 0))		rc = -EIO;
	msleep(3);
	if (mt9p111_gpio_out(CAM_5MP_RST_N, 1))		rc = -EIO;
	msleep(1);
	if (mt9p111_i2c_write_word(0x0020, 0x0000) < 0) rc = -EIO;
#endif


	return rc;
}
#endif

static int mt9p111_sensor_init_probe(const struct msm_camera_sensor_info *data)
{
	int rc = 0;

	SKYCDBG("init entry \n");


#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	if(mt9p111_tup_mode == MT9P111_TUNE_STATE_TUNNING_MODE_ON)
		return rc;
#endif

#if 0
	rc = mt9p111_reset(data);
	if (rc < 0) {
		SKYCERR("reset failed!\n");
		goto init_probe_fail;
	}
#endif

	rc = mt9p111_reg_init();
	if (rc < 0)
		goto init_probe_fail;

	return rc;

init_probe_fail:
	SKYCERR("%s failed! (%d)\n", __func__, rc);
	return rc;
}

static int mt9p111_sensor_init(const struct msm_camera_sensor_info *data)
{
	int rc = 0;

	config_csi = 0;
#if 0 //def F_PANTECH_CAMERA_FIX_CFG_LED_MODE
	led_trigger = FALSE;
#endif

//	mt9v113_vreg_init();
	mt9p111_vreg_init();

	rc = mt9p111_power_on();
	if (rc) {
		rc = -ENOENT;
		SKYCERR(" mt9p111_power failed rc=%d\n",rc);
		goto init_fail;
	}

	mt9p111_ctrl = kzalloc(sizeof(struct mt9p111_ctrl_t), GFP_KERNEL);
//	SKYCDBG(" mt9p111_ctrl [ kzalloc ] mt9p111_ctrl=%x\n",mt9p111_ctrl);
	if (!mt9p111_ctrl) {
		SKYCERR("mt9p111_init failed!\n");
		rc = -ENOMEM;
		goto init_done;
	}

	if (data)
		mt9p111_ctrl->sensordata = data;

	/* Input MCLK = 24MHz */
	msm_camio_clk_rate_set(24000000);
	msleep(20);

	rc = mt9p111_sensor_init_probe(data);
	SKYCDBG(" mt9p111_sensor_init_probe(data); rc =%d\n",rc);
	if (rc < 0) {
		SKYCERR("mt9p111_sensor_init failed!\n");
		goto init_fail;
	}

	led_auto = 0;
	multi_count = 0;

init_done:
	SKYCDBG("%s init_done\n",__func__);
	return rc;

init_fail:
	SKYCERR("%s failed! (%d)\n", __func__, rc);
	(void)mt9p111_power_off();
	kfree(mt9p111_ctrl);
	return rc;
}

static int mt9p111_init_client(struct i2c_client *client)
{
	/* Initialize the MSM_CAMI2C Chip */
	init_waitqueue_head(&mt9p111_wait_queue);
	return 0;
}

static int mt9p111_sensor_config(void __user *argp)
{
	struct sensor_cfg_data cfg_data;
	int32_t   rc = 0;

	SKYCDBG("%s start\n",__func__);
	if (copy_from_user(&cfg_data,
			(void *)argp,
			sizeof(struct sensor_cfg_data)))
		return -EFAULT;

	/* down(&mt9p111_sem); */

	SKYCDBG("mt9p111_ioctl, cfgtype = %d, mode = %d\n",
		cfg_data.cfgtype, cfg_data.mode);

		switch (cfg_data.cfgtype) {
		case CFG_SET_MODE:
			rc = mt9p111_set_sensor_mode(cfg_data.mode);
			SKYCDBG("mt9p111_set_sensor_mode OK! rc = [%d], cfg_data.mode = [%d]\n", rc, cfg_data.mode);
			break;

		case CFG_SET_EFFECT:
			rc = mt9p111_set_effect(cfg_data.mode, cfg_data.cfg.effect);
			SKYCDBG("mt9p111_set_effect OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.effect =[%d]\n", rc, cfg_data.mode, cfg_data.cfg.effect);
			break;
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_WB
		case CFG_SET_WB:
			rc = mt9p111_set_whitebalance(cfg_data.mode, cfg_data.cfg.whitebalance);
			SKYCDBG("mt9p111_set_whitebalance OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.whitebalance =[%d]\n", rc, cfg_data.mode, cfg_data.cfg.whitebalance);
			break;
#endif
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_BRIGHTNESS
		case CFG_SET_BRIGHTNESS:
			rc = mt9p111_set_brightness(cfg_data.cfg.brightness);
			SKYCDBG("mt9p111_set_brightness OK! rc = [%d], cfg_data.cfg.brightness =[%d]\n", rc, cfg_data.cfg.brightness);
			break;
#endif
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_LED_MODE
		case CFG_SET_LED_MODE:
			if(cfg_data.cfg.led_mode == 1)
				led_auto = 1;
			else
				led_auto = 0;
			rc = mt9p111_set_led_mode(cfg_data.cfg.led_mode);
			SKYCDBG("mt9p111_set_led_mode OK! rc = [%d], cfg_data.cfg.led_mode =[%d]\n", rc, cfg_data.cfg.led_mode);
			break;
#endif
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_AF
		case CFG_AUTO_FOCUS:
			rc = mt9p111_set_auto_focus(cfg_data.mode, cfg_data.cfg.focus.dir);
			SKYCDBG("mt9p111_auto_focus OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.focus.dir =[%d]\n", rc, cfg_data.mode, cfg_data.cfg.focus.dir);
			break;
#endif
#if 0 //def F_PANTECH_CAMERA_FIX_CFG_ISO
	case CFG_SET_ISO:
			rc = mt9p111_set_iso(cfg_data.cfg.iso);
			SKYCDBG("%s(CFG_SET_ISO), rc=%d\n", __func__, rc);
			break;
#endif
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_SCENE_MODE
	case CFG_SET_SCENE_MODE:
			rc = mt9p111_set_scene_mode(cfg_data.mode, cfg_data.cfg.scene_mode);
			SKYCDBG("%s(CFG_SET_SCENE_MODE), rc=%d\n", __func__, rc);
			break;
#endif
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_FOCUS_STEP
	case CFG_SET_FOCUS_STEP:
			rc = mt9p111_set_focus_step(cfg_data.mode, cfg_data.cfg.focus_step);
			SKYCDBG("%s(CFG_SET_FOCUS_STEP), rc=%d\n", __func__, rc);
			break;
#endif
#if 1 //def F_PANTECH_CAMERA_ADD_CFG_ANTISHAKE
	case CFG_SET_ANTISHAKE:
			rc = mt9p111_set_antishake(cfg_data.cfg.antishake);
			SKYCDBG("%s(CFG_SET_ANTISHAKE), rc=%d\n", __func__, rc);
			break;
#endif
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_EXPOSURE
		case CFG_SET_EXPOSURE_MODE:
			rc = mt9p111_set_exposure_mode(cfg_data.cfg.exposure);
			SKYCDBG("mt9p111_set_exposure_mode OK! rc = [%d], cfg_data.cfg.exposure =[%d]\n", rc, cfg_data.cfg.exposure);
			break;
#endif
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_FOCUS_RECT
	case CFG_SET_FOCUS_RECT:
			rc = mt9p111_set_focus_rect(cfg_data.cfg.focus_rect);
			SKYCDBG("%s(CFG_SET_FOCUS_RECT), rc=%d\n", __func__, rc);
			break;
#endif
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_PREVIEW_FPS
	case CFG_SET_PREVIEW_FPS:
			rc = mt9p111_set_frame_rate(cfg_data.cfg.preview_fps); //mt9p111_set_preview_fps(cfg_data.cfg.preview_fps);
			SKYCDBG("mt9p111_set_exposure_mode OK! rc = [%d], cfg_data.cfg.preview_fps =[%d]\n", rc, cfg_data.cfg.preview_fps);
			break;
#endif
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_ANTIBANDING
	case CFG_SET_ANTIBANDING:
			rc = mt9p111_set_antibanding(cfg_data.cfg.antibanding);
			SKYCDBG("%s: CFG_SET_ANTIBANDING, rc=%d\n", __func__, rc);
			break;
#endif
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
		case CFG_SET_TUNE_VALUE:
			rc = mt9p111_set_tune_value(&cfg_data);
			break;
#endif
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_REFLECT
		case CFG_SET_REFLECT:
			rc = mt9p111_set_reflect(cfg_data.cfg.reflect);
			SKYCDBG("mt9p111_set_reflect OK! rc = [%d], cfg_data.cfg.reflect =[%d]\n", rc, cfg_data.cfg.reflect);
			break;
#endif
#if 0 //def F_PANTECH_CAMERA_FIX_CFG_CAF
		case CFG_SET_CAF:
			rc = mt9p111_set_caf(cfg_data.cfg.caf);
			SKYCDBG("mt9p111_set_caf OK! rc = [%d], cfg_data.cfg.caf =[%d]\n", rc, cfg_data.cfg.caf);
			break;
#endif
		default:
			//rc = -EINVAL;
			rc = 0;
			SKYCERR("mt9p111_video_config DEFAULT FAIL!!![EINVAL] rc = [%d]\n", EINVAL);
			break;
		}

	/* up(&mt9p111_sem); */
	SKYCDBG("%s end rc = %d\n",__func__, rc);
	return rc;
}

static int mt9p111_sensor_release(void)
{
	int32_t rc = 0;
	SKYCDBG("%s start	led_auto = %d \n",__func__, led_auto);
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_LED_MODE
	mt9p111_set_led_mode(0);
#endif

	rc = mt9p111_power_off();
	if (rc)
		goto sensor_release_fail;

	/* down(&mt9p111_sem); */
	kfree(mt9p111_ctrl);
	mt9p111_ctrl = NULL;
	/* up(&mt9p111_sem); */
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	//튜닝값 로딩 체크 flag
	mt9p111_tup_state = MT9P111_TUNE_STATE_NONE;
	//mt9p111_done_set_tune_load = FALSE;
	//mt9p111_done_set_tune_value = FALSE;
	//mt9p111_tup_mode = MT9P111_TUNE_STATE_TUNNING_MODE_OFF;
#endif

	SKYCDBG("%s end rc = %d\n",__func__, rc);
	return rc;

sensor_release_fail:
	SKYCERR("%s failed! (%d)\n", __func__, rc);
	return rc;
}

static int mt9p111_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int rc = 0;
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		rc = -ENOTSUPP;
		goto probe_failure;
	}

	mt9p111_sensorw = kzalloc(sizeof(struct mt9p111_work), GFP_KERNEL);
	if (!mt9p111_sensorw) {
		rc = -ENOMEM;
		goto probe_failure;
	}

	i2c_set_clientdata(client, mt9p111_sensorw);
	mt9p111_init_client(client);
	mt9p111_client = client;

	SKYCDBG("mt9p111_probe succeeded!\n");

	return rc;

probe_failure:
	kfree(mt9p111_sensorw);
	mt9p111_sensorw = NULL;
	SKYCERR("mt9p111_probe failed!\n");
	return rc;
}

static const struct i2c_device_id mt9p111_i2c_id[] = {
	{ "mt9p111", 0},
	{ },
};

static struct i2c_driver mt9p111_i2c_driver = {
	.id_table = mt9p111_i2c_id,
	.probe  = mt9p111_i2c_probe,
	.remove = __exit_p(mt9p111_i2c_remove),
	.driver = {
		.name = "mt9p111",
	},
};

#if 1 //def F_PANTECH_CAMERA
static int32_t mt9p111_init_i2c(void)
{
	int32_t rc = 0;

	SKYCDBG("%s start\n",__func__);

	rc = i2c_add_driver(&mt9p111_i2c_driver);
	SKYCDBG("%s mt9p111_i2c_driver rc = %d\n",__func__, rc);
	if (IS_ERR_VALUE(rc))
		goto init_i2c_fail;

	SKYCDBG("%s end\n",__func__);
	return 0;

init_i2c_fail:
	SKYCERR("%s failed! (%d)\n", __func__, rc);
	return rc;
}
#endif

static int mt9p111_sensor_probe(const struct msm_camera_sensor_info *info,
				struct msm_sensor_ctrl *s)
{
	int rc = 0;
	SKYCDBG("%s start\n",__func__);

	rc = mt9p111_init_i2c();
	if (rc < 0 || mt9p111_client == NULL)
	{
//		SKYCERR("%s rc = %d, mt9p111_client = %x\n",__func__, rc, mt9p111_client);
		goto probe_init_fail;
	}

	s->s_init = mt9p111_sensor_init;
	s->s_release = mt9p111_sensor_release;
	s->s_config  = mt9p111_sensor_config;
	//mt9p111_probe_init_done(info);


	s->s_camera_type = BACK_CAMERA_2D;
	s->s_mount_angle  = 90;


	return 0;

probe_init_fail:
	SKYCERR("%s Failed!\n", __func__);
	return 0;
}

static int __mt9p111_probe(struct platform_device *pdev)
{
	return msm_camera_drv_start(pdev, mt9p111_sensor_probe);
}

static struct platform_driver msm_camera_driver = {
	.probe = __mt9p111_probe,
	.driver = {
		.name = "msm_camera_mt9p111",
		.owner = THIS_MODULE,
	},
};

static int __init mt9p111_init(void)
{
	return platform_driver_register(&msm_camera_driver);
}

module_init(mt9p111_init);
