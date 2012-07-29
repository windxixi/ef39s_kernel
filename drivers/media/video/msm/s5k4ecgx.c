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
#include "s5k4ecgx.h"
#include <linux/regulator/consumer.h>
#include <linux/regulator/machine.h>

#define C_SKYCAM_MIN_BRIGHTNESS 0
#define C_SKYCAM_MAX_BRIGHTNESS 8

#define C_SKYCAM_MIN_PREVIEW_FPS	7
#define C_SKYCAM_MAX_PREVIEW_FPS	30

#define F_S5K4ECGX_POWER
#define S5K4ECGX_REG_POLLING_ERROR 3
/* Micron S5K4ECGX Registers and their values */

#define SENSOR_DEBUG 0

#define ON  1
#define OFF 0

#define LED_DATA1 31
#define LED_DATA2 62

#define S5K4ECGX_I2C_RETRY	10
#define S5K4ECGX_I2C_MPERIOD	200
#define S5K4ECGX_SNAPSHOT_RETRY 	200//30
#define S5K4ECGX_PREVIEW_RETRY 	50
#define S5K4ECGX_POLLING_RETRY	 	30


#define s5k4ecgx_delay_msecs_stream 200 //500 //200

#define CAM_5MP_RST_N	0
#define CAM_VGA_RST_N	1
#define CAM_VGA_STB_N	2
#define CAM_FLASH_EN	3
#define CAM_FLASH_MODE	4
#define CAM_GPIO_MAX	5


#define S5K4ECGX_CFG_FOCUS_STEP_MAX 9

typedef struct {
	int32_t id;
	const char *label;
	uint32_t num;
} gpio_ctrl_t;


static gpio_ctrl_t g_gpios[] = {
	{CAM_5MP_RST_N, "CAM_5MP_RST_N", 106},
	{CAM_VGA_RST_N, "CAM_VGA_RST_N", 137},
	{CAM_VGA_STB_N, "CAM_VGA_STB_N", 139},
	{CAM_FLASH_EN, "CAM_FLASH_EN",    31},
	{CAM_FLASH_MODE, "CAM_FLASH_MODE",62},
};

struct s5k4ecgx_work {
	struct work_struct work;
};

static struct  s5k4ecgx_work *s5k4ecgx_sensorw;
static struct  i2c_client *s5k4ecgx_client;

struct s5k4ecgx_ctrl_t {
	const struct msm_camera_sensor_info *sensordata;
};


static struct s5k4ecgx_ctrl_t *s5k4ecgx_ctrl;

static int32_t config_csi;
static bool b_snapshot_flag;

static DECLARE_WAIT_QUEUE_HEAD(s5k4ecgx_wait_queue);
DECLARE_MUTEX(s5k4ecgx_sem);


struct s5k4ecgx_vreg_t {
	const char *name;
	unsigned short mvolt;
};

static struct regulator *l2b_2p8v;
static struct regulator *l3b_2p8v;
static struct regulator *l0b_1p2v;
static struct regulator *mvs0b_1p8v;

/* static variable number */
static int auto_focus = 0;
static int led_auto = 0;


static int32_t s5k4ecgx_i2c_read(unsigned short saddr,
	unsigned short raddr, unsigned short *rdata, enum s5k4ecgx_width width);

/*=============================================================*/

static int s5k4ecgx_vreg_init(void)
{
	int rc = 0;
	SKYCDBG(" %s:%d\n", __func__, __LINE__);

//5M Core
	l0b_1p2v = regulator_get(NULL,  "8901_l0");
	if(IS_ERR(l0b_1p2v))
	{
		SKYCERR("regulator_get l0b_1p2v fail : 0x%x\n", (unsigned int)l0b_1p2v);
		return -ENODEV;
	}

	rc = regulator_set_voltage(l0b_1p2v, 1200000, 1200000);
	if(rc)
	{
		SKYCERR("%s: unable to set l0b_1p2v voltage to 1.2V\n", __func__);
		goto fail;
	}

// 5M_IO
	mvs0b_1p8v = regulator_get(NULL, "8901_mvs0" );
	if(IS_ERR(mvs0b_1p8v))
	{
		SKYCERR("regulator_get mvs0b_1p8v fail : 0x%x\n", (unsigned int)mvs0b_1p8v);
		return -ENODEV;
	}


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

	SKYCDBG("%s Success!:%d\n", __func__, __LINE__);
	return rc;

fail:
	SKYCDBG("%s Failed!:%d\n", __func__, __LINE__);
	if(l0b_1p2v) {
		regulator_put(l0b_1p2v);
	}

	if(mvs0b_1p8v) {
		regulator_put(mvs0b_1p8v);
	}

	if(l2b_2p8v) {
		regulator_put(l2b_2p8v);
	}
	if(l3b_2p8v) {
		regulator_put(l3b_2p8v);
	}
	return rc;
}



static int32_t s5k4ecgx_gpio_out(int32_t id, int32_t val)
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


static int32_t s5k4ecgx_power_on(void)
{

	int32_t rc = 0;
	if (regulator_enable(l2b_2p8v))	rc = -EIO;
	mdelay(1);
	if (regulator_enable(l3b_2p8v))	rc = -EIO;
	mdelay(1);
	if (regulator_enable(l0b_1p2v))	rc = -EIO;
	mdelay(1);
	if (regulator_enable(mvs0b_1p8v))	rc = -EIO;
	mdelay(1);
	msm_camio_clk_rate_set(24000000);
	mdelay(5);
	if (s5k4ecgx_gpio_out(CAM_5MP_RST_N, 1))		rc = -EIO;
	mdelay(2);
	return rc;
}

static int32_t s5k4ecgx_power_off(void)
{
	int32_t rc = 0;

	
	/* VDD_CAM_1800_IO is opened and merged with VDD_CAM_1800 in ET15
	 * to correct 'no image' and 'burnt image' issues.
	 * We should not remove VDD_CAM_1800_IO for backward compatibility. */
	if (s5k4ecgx_gpio_out(CAM_5MP_RST_N, 0))		rc = -EIO;

	mdelay(3);
/*
	rc = regulator_disable(mvs0b_1p8v);
	if (rc)
		SKYCERR("%s: Disable regulator mvs0b_1p8v failed\n", __func__);
	return rc;
*/
	rc = regulator_disable(l3b_2p8v);
	if (rc)
		SKYCERR("%s: Disable regulator l3b_2p8v failed\n", __func__);
	regulator_put(l3b_2p8v);

	rc = regulator_disable(l2b_2p8v);
	if (rc)
		SKYCERR("%s: Disable regulator l2b_2p8v failed\n", __func__);
	regulator_put(l2b_2p8v);

	rc = regulator_disable(l0b_1p2v);
	if (rc)
		SKYCERR("%s: Disable regulator l0b_1p2v failed\n", __func__);
	regulator_put(l0b_1p2v);

	rc = regulator_disable(mvs0b_1p8v);
	if (rc)
		SKYCERR("%s: Disable regulator mvs0b_1p8v failed\n", __func__);

	return rc;

}


static int32_t s5k4ecgx_i2c_txdata(unsigned short saddr,
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

	for (i = 0; i < S5K4ECGX_I2C_RETRY; i++) {
		rc = i2c_transfer(s5k4ecgx_client->adapter, msg, 1);
		if (rc >= 0) {
			return 0;
		}
		SKYCDBG("%s: tx retry. [%02x.%02x.%02x] len=%d rc=%d\n", __func__,saddr, *txdata, *(txdata + 1), length, rc);
		msleep(S5K4ECGX_I2C_MPERIOD);
	}
	return -EIO;
}

static int32_t s5k4ecgx_i2c_write(unsigned short saddr,
	unsigned short waddr, unsigned short wdata, enum s5k4ecgx_width width)
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

		rc = s5k4ecgx_i2c_txdata(saddr, buf, 4);
	}
		break;

	case TRIPLE_LEN: {
		buf[0] = (waddr & 0xFF00)>>8;
		buf[1] = (waddr & 0x00FF);
		buf[2] = wdata;
		rc = s5k4ecgx_i2c_txdata(saddr, buf, 3);
	}
		break;

	case BYTE_LEN: {
		buf[0] = waddr;
		buf[1] = wdata;
		rc = s5k4ecgx_i2c_txdata(saddr, buf, 2);
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
static int32_t s5k4ecgx_i2c_write_a2d1(unsigned short waddr, unsigned char wdata)
{
	int32_t rc = -EIO;
	unsigned char buf[3];

	memset(buf, 0, sizeof(buf));

	buf[0] = (waddr & 0xFF00)>>8;
	buf[1] = (waddr & 0x00FF);
	buf[2] = wdata;

	rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr, buf, 3);

	if (rc < 0)
		SKYCERR(
		"i2c_write failed, saddr= 0x%x, addr = 0x%x, val = 0x%x!\n",
		s5k4ecgx_client->addr, waddr, wdata);

	return rc;
}
#endif

static int32_t s5k4ecgx_i2c_write_table(
	struct s5k4ecgx_i2c_reg_conf const *reg_conf_tbl,
	int num_of_items_in_table)
{
#if 1
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

	for (i = 0; i < num_of_items_in_table; i++)
	{
		switch(reg_conf_tbl->width)
		{
			case ZERO_LEN:
				SKYCDBG("ZERO_LEN continue ADDR = 0x%x, VALUE = 0x%x\n",reg_conf_tbl->waddr, reg_conf_tbl->wdata);
				reg_conf_tbl++;
				rc = 0;
				continue;

			case POLLING_LEN:
				poll_reg = reg_conf_tbl->waddr;
				poll_mask = reg_conf_tbl->wdata;
				poll_data = (reg_conf_tbl+1)->waddr;
				poll_delay = ((reg_conf_tbl+1)->wdata & 0xff00) >> 8;
				poll_retry = ((reg_conf_tbl+1)->wdata & 0x00ff);

				SKYCDBG("POLLING!! poll_delay=%x, poll_retry=%x, poll_mcu_var=%x, poll_data=%x, poll_mask=%x\n",poll_delay, poll_retry, poll_mcu_var, poll_data, poll_mask);
				SKYCDBG("POLLING!! poll_reg = 0x%x\n" , poll_reg);
				for (retry_cnt = 0; retry_cnt < poll_retry; retry_cnt++)
				{
					if (s5k4ecgx_i2c_write(s5k4ecgx_client->addr, 0x002E, poll_reg, WORD_LEN) < 0)
					{
						SKYCDBG("<< s5k4ecgx_i2c_write_word (FALSE)\n");
						return -1;
					}
					if (s5k4ecgx_i2c_read(s5k4ecgx_client->addr, 0x0F12, &read_data,WORD_LEN) < 0)
					{
						SKYCDBG("<<POLL_MCU_VAR s5k4ecgx_i2c_read_word (FALSE)\n");
						return -1;
					}
					if ((read_data & poll_mask) != poll_data)
					{
						if( ((read_data & poll_mask) == 0x0003) && (poll_reg == 0x2EEE))
						{
							SKYCDBG("<<AF FAIL!! POLL_MCU_VAR read_data = %x (FALSE)\n", read_data);
							return -1;
						}
						SKYCDBG("retry polling MCU variable... after sleeping %d ms, read_data=%2x\n", poll_delay, read_data);
						SKYCDBG("(read_data & poll_mask)=%4x , poll_data=%4x\n", (read_data & poll_mask), poll_data);
						SKYCDBG("POLLING!! poll_reg = 0x%x\n" , poll_reg);
						mdelay(poll_delay);
					}
					else
					{
						SKYCDBG("stop polling MCU variable... retried %d/%d time(s) (delay = %d ms), read_data=%2x\n", retry_cnt, poll_retry, poll_delay, read_data);
						break;
					}
				}

				if (retry_cnt == poll_retry)
				{
					SKYCDBG("<<RETRY FAIL!! POLL_MCU_VAR read_data = %x (FALSE)\n", read_data);
					return -1;
				}

				//  2개의 값을 이용하므로 +2를 해준다
				reg_conf_tbl++;
				reg_conf_tbl++;
				i++;
				break;


#ifdef BURST_MODE_INIT
		case BURST_1:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_1, s5k4ecgx_regs.init_burst_settings_size_1);
			if (rc < 0) {
				SKYCERR("BURST_1 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_2:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_2, s5k4ecgx_regs.init_burst_settings_size_2);
			if (rc < 0) {
				SKYCERR("BURST_2 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_3:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_3, s5k4ecgx_regs.init_burst_settings_size_3);
			if (rc < 0) {
				SKYCERR("BURST_3 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_4:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_4, s5k4ecgx_regs.init_burst_settings_size_4);
			if (rc < 0) {
				SKYCERR("BURST_4 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_5:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_5, s5k4ecgx_regs.init_burst_settings_size_5);
			if (rc < 0) {
				SKYCERR("BURST_5 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_6:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_6, s5k4ecgx_regs.init_burst_settings_size_6);
			if (rc < 0) {
				SKYCERR("BURST_6 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_7:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_7, s5k4ecgx_regs.init_burst_settings_size_7);
			if (rc < 0) {
				SKYCERR("BURST_7 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_8:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_8, s5k4ecgx_regs.init_burst_settings_size_8);
			if (rc < 0) {
				SKYCERR("BURST_8 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_9:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_9, s5k4ecgx_regs.init_burst_settings_size_9);
			if (rc < 0) {
				SKYCERR("BURST_9 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_10:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_10, s5k4ecgx_regs.init_burst_settings_size_10);
			if (rc < 0) {
				SKYCERR("BURST_10 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;

		case BURST_11:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_11, s5k4ecgx_regs.init_burst_settings_size_11);
			if (rc < 0) {
				SKYCERR("BURST_11 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_12:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_12, s5k4ecgx_regs.init_burst_settings_size_12);
			if (rc < 0) {
				SKYCERR("BURST_12 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_13:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_13, s5k4ecgx_regs.init_burst_settings_size_13);
			if (rc < 0) {
				SKYCERR("BURST_13 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_14:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_14, s5k4ecgx_regs.init_burst_settings_size_14);
			if (rc < 0) {
				SKYCERR("BURST_14 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_15:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_15, s5k4ecgx_regs.init_burst_settings_size_15);
			if (rc < 0) {
				SKYCERR("BURST_15 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_16:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_16, s5k4ecgx_regs.init_burst_settings_size_16);
			if (rc < 0) {
				SKYCERR("BURST_16 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_17:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_17, s5k4ecgx_regs.init_burst_settings_size_17);
			if (rc < 0) {
				SKYCERR("BURST_17 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_18:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_18, s5k4ecgx_regs.init_burst_settings_size_18);
			if (rc < 0) {
				SKYCERR("BURST_18 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_19:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_19, s5k4ecgx_regs.init_burst_settings_size_19);
			if (rc < 0) {
				SKYCERR("BURST_19 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_20:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_20, s5k4ecgx_regs.init_burst_settings_size_20);
			if (rc < 0) {
				SKYCERR("BURST_20 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;

		case BURST_21:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_21, s5k4ecgx_regs.init_burst_settings_size_21);
			if (rc < 0) {
				SKYCERR("BURST_21 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_22:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_22, s5k4ecgx_regs.init_burst_settings_size_22);
			if (rc < 0) {
				SKYCERR("BURST_22 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_23:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_23, s5k4ecgx_regs.init_burst_settings_size_23);
			if (rc < 0) {
				SKYCERR("BURST_23 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_24:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_24, s5k4ecgx_regs.init_burst_settings_size_24);
			if (rc < 0) {
				SKYCERR("BURST_24 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_25:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_25, s5k4ecgx_regs.init_burst_settings_size_25);
			if (rc < 0) {
				SKYCERR("BURST_25 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_26:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_26, s5k4ecgx_regs.init_burst_settings_size_26);
			if (rc < 0) {
				SKYCERR("BURST_26 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_27:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_27, s5k4ecgx_regs.init_burst_settings_size_27);
			if (rc < 0) {
				SKYCERR("BURST_27 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_28:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_28, s5k4ecgx_regs.init_burst_settings_size_28);
			if (rc < 0) {
				SKYCERR("BURST_28 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_29:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_29, s5k4ecgx_regs.init_burst_settings_size_29);
			if (rc < 0) {
				SKYCERR("BURST_29 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_30:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_30, s5k4ecgx_regs.init_burst_settings_size_30);
			if (rc < 0) {
				SKYCERR("BURST_30 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;

		case BURST_31:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_31, s5k4ecgx_regs.init_burst_settings_size_31);
			if (rc < 0) {
				SKYCERR("BURST_31 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_32:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_32, s5k4ecgx_regs.init_burst_settings_size_32);
			if (rc < 0) {
				SKYCERR("BURST_32 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_33:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_33, s5k4ecgx_regs.init_burst_settings_size_33);
			if (rc < 0) {
				SKYCERR("BURST_33 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_34:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_34, s5k4ecgx_regs.init_burst_settings_size_34);
			if (rc < 0) {
				SKYCERR("BURST_34 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_35:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_35, s5k4ecgx_regs.init_burst_settings_size_35);
			if (rc < 0) {
				SKYCERR("BURST_35 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_36:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_36, s5k4ecgx_regs.init_burst_settings_size_36);
			if (rc < 0) {
				SKYCERR("BURST_36 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_37:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_37, s5k4ecgx_regs.init_burst_settings_size_37);
			if (rc < 0) {
				SKYCERR("BURST_37 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_38:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
				s5k4ecgx_regs.init_burst_settings_38, s5k4ecgx_regs.init_burst_settings_size_38);
			if (rc < 0) {
				SKYCERR("BURST_38 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
		case BURST_39:
			rc = s5k4ecgx_i2c_txdata(s5k4ecgx_client->addr,
			s5k4ecgx_regs.init_burst_settings_39, s5k4ecgx_regs.init_burst_settings_size_39);
			if (rc < 0) {
				SKYCERR("BURST_39 failed!\n");
				break;
			}
			reg_conf_tbl++;
			break;
#endif
		default:
			rc = s5k4ecgx_i2c_write(s5k4ecgx_client->addr,
								reg_conf_tbl->waddr, reg_conf_tbl->wdata,
								reg_conf_tbl->width);
			if (rc < 0)
			{
				SKYCERR("s5k4ecgx_i2c_read failed!\n");
				return rc;
			}

			if (reg_conf_tbl->mdelay_time != 0)
				mdelay(reg_conf_tbl->mdelay_time);

			reg_conf_tbl++;

			break;
	}
}

	return rc;
#else
	int i;
	int32_t rc = -EIO;

	for (i = 0; i < num_of_items_in_table; i++) {
		rc = s5k4ecgx_i2c_write(s5k4ecgx_client->addr,
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

#if 1
static int s5k4ecgx_i2c_rxdata(unsigned short saddr,
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

	for (i = 0; i < S5K4ECGX_I2C_RETRY; i++) {
		rc = i2c_transfer(s5k4ecgx_client->adapter, msgs, 2);
		if (rc >= 0) {
			return 0;
		}
		SKYCDBG("%s: tx retry. [%02x.%02x.%02x] len=%d rc=%d\n", __func__,saddr, *rxdata, *(rxdata + 1), length, rc);
		msleep(S5K4ECGX_I2C_MPERIOD);
	}
	return -EIO;
}
#endif

#if 1
static int32_t s5k4ecgx_i2c_read(unsigned short   saddr,
	unsigned short raddr, unsigned short *rdata, enum s5k4ecgx_width width)
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

		rc = s5k4ecgx_i2c_rxdata(saddr, buf, 2);
		if (rc < 0)
			return rc;

		*rdata = buf[0] << 8 | buf[1];
	}
		break;

	default:
		break;
	}

	if (rc < 0)
		SKYCERR("s5k4ecgx_i2c_read failed!\n");

	return rc;
}
#endif


static long s5k4ecgx_reg_init(void)
{
//	int32_t array_length;
//	int32_t i;
	int32_t rc;

	/* PLL Setup Start */
	rc = s5k4ecgx_i2c_write_table(&s5k4ecgx_regs.init_parm[0],
					s5k4ecgx_regs.init_parm_size);

	if (rc < 0)
		return rc;
	/* PLL Setup End   */

	return 0;
}

#if 1 //def F_SKYCAM_FIX_CFG_EFFECT
static long s5k4ecgx_set_effect(int mode, int8_t effect)
{
//	uint16_t reg_addr;
//	uint16_t reg_val;
	int32_t rc = 0;

	SKYCDBG("%s start\n",__func__);

	if(effect < CAMERA_EFFECT_OFF || effect >= CAMERA_EFFECT_MAX){
		SKYCERR("%s error. effect=%d\n", __func__, effect);
		return 0;//-EINVAL;
	}

	rc = s5k4ecgx_i2c_write_table(s5k4ecgx_regs.effect_cfg_settings[effect],
					s5k4ecgx_regs.effect_cfg_settings_size);
	if (rc < 0)
	{
		SKYCDBG("CAMERA_WB I2C FAIL!!! return~~\n");
		return rc;
	}
	SKYCDBG("%s end\n",__func__);
	return rc;
}
#endif


static int32_t s5k4ecgx_set_brightness(int8_t brightness)
{
	int32_t rc = 0;
	SKYCDBG("%s start~ receive brightness = %d\n",__func__, brightness);

	if ((brightness < C_SKYCAM_MIN_BRIGHTNESS) || (brightness > C_SKYCAM_MAX_BRIGHTNESS)) {
		SKYCERR("%s error. brightness=%d\n", __func__, brightness);
		return 0;//-EINVAL;
	}

	rc = s5k4ecgx_i2c_write_table(s5k4ecgx_regs.bright_cfg_settings[brightness],
					s5k4ecgx_regs.bright_cfg_settings_size);
	if (rc < 0)
	{
		SKYCDBG("CAMERA_BRIGHTNESS I2C FAIL!!! return~~\n");
		return rc;
	}
	SKYCDBG("%s end\n",__func__);
	return rc;
}

static int32_t s5k4ecgx_set_focus_step(int mode, int32_t focus_step)
{
	int32_t rc = 0;

	if ((focus_step != 0) && (focus_step != S5K4ECGX_CFG_FOCUS_STEP_MAX))
	{
		SKYCERR("%s FAIL!!! return~~  focus_step = %d\n",__func__,focus_step);
		return 0;//-EINVAL;
	}

	if( focus_step == S5K4ECGX_CFG_FOCUS_STEP_MAX)
	{
		rc = s5k4ecgx_i2c_write_table(s5k4ecgx_regs.focus_step_cfg_settings[1],
				s5k4ecgx_regs.focus_step_cfg_settings_size);

	}
	else
	{
		rc = s5k4ecgx_i2c_write_table(s5k4ecgx_regs.focus_step_cfg_settings[focus_step],
					s5k4ecgx_regs.focus_step_cfg_settings_size);
	}

	if (rc < 0)
	{
		SKYCERR("%s FAIL!!! return~~\n",__func__);
		return rc;
	}

	SKYCDBG("%s end\n",__func__);

	return rc;
}


static int32_t s5k4ecgx_set_whitebalance (int32_t whitebalance)
{

	int32_t rc = 0;

	SKYCDBG("%s start  whitebalance=%d\n",__func__, whitebalance);

	rc = s5k4ecgx_i2c_write_table(s5k4ecgx_regs.wb_cfg_settings[whitebalance-1],
					s5k4ecgx_regs.wb_cfg_settings_size);
	if (rc < 0)
	{
		SKYCDBG("CAMERA_WB I2C FAIL!!! return~~\n");
		return rc;
	}

	SKYCDBG("%s end\n",__func__);
	return rc;
}

static int32_t s5k4ecgx_set_exposure_mode(int32_t exposure)
{
	int32_t rc = 0;

	SKYCDBG("%s  exposure = %d\n",__func__, exposure);

	if ((exposure < 0) || (exposure >= 4))
	{
		SKYCERR("%s FAIL!!! return~~  exposure = %d\n",__func__,exposure);
		return 0;//-EINVAL;
	}

	rc = s5k4ecgx_i2c_write_table(s5k4ecgx_regs.exposure_cfg_settings[exposure],
					s5k4ecgx_regs.exposure_cfg_settings_size);
	if (rc < 0)
	{
		SKYCDBG("CAMERA_EFFECT_SEPIA I2C FAIL!!! return~~\n");
		return rc;
	}

	SKYCDBG("%s end\n",__func__);

	return rc;
}

static int32_t s5k4ecgx_set_preview_fps(int32_t preview_fps)
{
	/* 0 : variable 30fps, 1 ~ 30 : fixed fps */
	/* default: variable 7 ~ 30fps */
	int32_t rc = 0;

	if ((preview_fps < C_SKYCAM_MIN_PREVIEW_FPS) ||
		(preview_fps > C_SKYCAM_MAX_PREVIEW_FPS)) {
		SKYCDBG("%s: -EINVAL, preview_fps=%d\n",
			__func__, preview_fps);
		return 0;//-EINVAL;
	}

	SKYCDBG("%s: preview_fps=%d\n", __func__, preview_fps);

	rc = s5k4ecgx_i2c_write_table(s5k4ecgx_regs.preview_fps_cfg_settings[preview_fps - C_SKYCAM_MIN_PREVIEW_FPS],
					s5k4ecgx_regs.preview_fps_cfg_settings_size);

	SKYCDBG("%s end rc = %d\n",__func__, rc);

	return rc;
}

static int32_t s5k4ecgx_set_reflect(int32_t reflect)
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
	rc = s5k4ecgx_i2c_write_table(s5k4ecgx_regs.reflect_cfg_settings[reflect],
				s5k4ecgx_regs.reflect_cfg_settings_size);
	if (rc < 0)
	{
		SKYCERR("CAMERA_SET_REFLECT I2C FAIL!!! return~~\n");
		return rc;
	}

	SKYCDBG("%s end\n",__func__);

	return rc;
}

static int32_t s5k4ecgx_video_config(void)
{
	int32_t rc = 0;

	/* set preview resolution to 1280x960 */
	SKYCDBG("%s start\n",__func__);

	rc = s5k4ecgx_i2c_write_table(&s5k4ecgx_regs.preview_cfg_settings[0],
						s5k4ecgx_regs.preview_cfg_settings_size);

	if (rc < 0)
	{
		SKYCERR("s5k4ecgx_i2c_write_table FAIL!!! return~~\n");
		return rc;
	}
	SKYCDBG("%s end rc = %d\n",__func__, rc);

	return rc;
}

static int32_t s5k4ecgx_snapshot_config(void)
{
	int32_t rc = 0;

	/* set snapshot resolution to 1280x960 */
	SKYCDBG("%s start\n",__func__);
	rc = s5k4ecgx_i2c_write_table(&s5k4ecgx_regs.snapshot_cfg_settings[0],
					s5k4ecgx_regs.snapshot_cfg_settings_size);
	if (rc < 0)
	{
		SKYCERR("s5k4ecgx_i2c_write_table FAIL!!! return~~\n");
		return rc;
	}
	SKYCDBG("%s end rc = %d\n",__func__, rc);

	return rc;
}

static int32_t s5k4ecgx_set_led_mode(int32_t led_mode)
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
			s5k4ecgx_gpio_out(CAM_FLASH_EN, 0);
			s5k4ecgx_gpio_out(CAM_FLASH_MODE, 0);
			break;
		case 1:
			s5k4ecgx_gpio_out(CAM_FLASH_EN, 0);
			s5k4ecgx_gpio_out(CAM_FLASH_MODE, 0);
			break;
		case 2:
		case 3:
			s5k4ecgx_gpio_out(CAM_FLASH_EN, 1);
			s5k4ecgx_gpio_out(CAM_FLASH_MODE, 1);
			break;
		case 4:
			s5k4ecgx_gpio_out(CAM_FLASH_EN, 1);
//			s5k4ecgx_gpio_out(CAM_FLASH_MODE, 1);
			break;
		default:
			s5k4ecgx_gpio_out(CAM_FLASH_EN, 0);
			s5k4ecgx_gpio_out(CAM_FLASH_MODE, 0);
			break;
	}
	return 0;
}


static int32_t s5k4ecgx_set_auto_focus(int mode, int8_t autofocus)
{

	int32_t rc = 0;
	int32_t nbrightness = 0;
	int16_t read_data;

	if( autofocus == 0 || autofocus ==2)
		auto_focus = 1;
	else
		auto_focus = 0;


	if ((autofocus < 0) || (autofocus > 3))
	{
		SKYCERR("%s FAIL!!! return~~  autofocus = %d\n",__func__,autofocus);
		return 0;//-EINVAL;
	}


/*********************************************************************************/
	if(led_auto == 1)
	{

		rc = s5k4ecgx_i2c_write(s5k4ecgx_client->addr, 0xFCFC, 0xD000,WORD_LEN);
		rc = s5k4ecgx_i2c_write(s5k4ecgx_client->addr, 0x002C, 0x7000,WORD_LEN);
		rc = s5k4ecgx_i2c_write(s5k4ecgx_client->addr, 0x002E, 0x2C1A,WORD_LEN);
		rc = s5k4ecgx_i2c_read(s5k4ecgx_client->addr, 0x0F12, &read_data,WORD_LEN);
		nbrightness = read_data << 16;

		rc = s5k4ecgx_i2c_write(s5k4ecgx_client->addr, 0xFCFC, 0xD000,WORD_LEN);
		rc = s5k4ecgx_i2c_write(s5k4ecgx_client->addr, 0x002C, 0x7000,WORD_LEN);
		rc = s5k4ecgx_i2c_write(s5k4ecgx_client->addr, 0x002E, 0x2C18,WORD_LEN);
		rc = s5k4ecgx_i2c_read(s5k4ecgx_client->addr, 0x0F12, &read_data,WORD_LEN);
		nbrightness = nbrightness + read_data;

		if( nbrightness < 0x00000032 )
		{
			s5k4ecgx_set_led_mode(2);
		}
	}
/*********************************************************************************/

	SKYCDBG("%s start~  mode = %d, autofocus = %d\n",__func__,mode ,autofocus);

	rc = s5k4ecgx_i2c_write_table(&s5k4ecgx_regs.autofocus_cfg_settings[0],
					s5k4ecgx_regs.autofocus_cfg_settings_size);
	if (rc < 0)
	{
		SKYCERR("CAMERA_AUTO_FOCUS I2C FAIL_1!!! return~~\n");
/***************************************************************************/
		if(led_auto == 1 && nbrightness <= 0x00000032)
		s5k4ecgx_set_led_mode(0);
/***************************************************************************/
		return rc;
	}

	/* Last preview frame is not focused for some reason,
	 * so add delay here to view several frames after AF is done. */
	msleep(150);

	SKYCDBG("%s end\n",__func__);
/***************************************************************************/
		if(led_auto == 1 && nbrightness <= 0x00000032){
		s5k4ecgx_set_led_mode(0);
	}
/***************************************************************************/
	return rc;
}


static long s5k4ecgx_set_sensor_mode(int mode)
{
//	uint16_t clock;
	int32_t rc = 0;
	struct msm_camera_csi_params s5k4ecgx_csi_params;

	SKYCDBG("s5k4ecgx_set_sensor_mode mode = %d!\n",mode);

	switch (mode) {
	case SENSOR_PREVIEW_MODE:
		b_snapshot_flag = false;

		if(config_csi == 0)
		{
			SKYCDBG("s5k4ecgx_set_sensor_mode config_csi E\n");
			s5k4ecgx_csi_params.lane_cnt = 2; // 4;
			s5k4ecgx_csi_params.data_format = CSI_8BIT; //CSI_10BIT;
			s5k4ecgx_csi_params.lane_assign = 0xe4;
			s5k4ecgx_csi_params.dpcm_scheme = 0;
			s5k4ecgx_csi_params.settle_cnt = 0x14;
			rc = msm_camio_csi_config(&s5k4ecgx_csi_params);
			msleep(s5k4ecgx_delay_msecs_stream);
			config_csi = 1;
			SKYCDBG("s5k4ecgx_set_sensor_mode config_csi X\n");
		}
		else
		{
			rc = s5k4ecgx_video_config();
			SKYCDBG("s5k4ecgx_video_config, rc = %d \n", rc);
			if (rc < 0)
			{
				SKYCERR("s5k4ecgx_video_config FAIL!!! return~~\n");
				return rc;
			}
		}
		break;

	case SENSOR_SNAPSHOT_MODE:
		if(!b_snapshot_flag) {
			rc = s5k4ecgx_snapshot_config();
			b_snapshot_flag = true;
			SKYCDBG("s5k4ecgx_snapshot_config, rc = %d \n", rc);
			if (rc < 0)
	              {
				SKYCERR("s5k4ecgx_snapshot_config FAIL!!! return~~\n");
				return rc;
			}
		}
		break;

	case SENSOR_RAW_SNAPSHOT_MODE:

		break;

	default:
		return -EINVAL;
	}
	SKYCDBG("s5k4ecgx_set_sensor_mode(%d) X!\n",mode);

	return rc;
}

static int s5k4ecgx_sensor_init_probe(const struct msm_camera_sensor_info *data)
{
	int32_t rc;
	SKYCDBG("%s start\n", __func__);

	rc = s5k4ecgx_reg_init();
	if(rc < 0)
		goto init_probe_fail;

	return rc;

init_probe_fail:
	SKYCERR("%s init_probe_fail\n",__func__);
	return rc;
}

int s5k4ecgx_sensor_init(const struct msm_camera_sensor_info *data)
{
	int rc = 0;

	config_csi = 0;
	SKYCDBG("%s:	Line:%d \n", __func__,__LINE__);


	SKYCDBG("%s start\n", __func__);


	s5k4ecgx_vreg_init();

	rc = s5k4ecgx_power_on();
	if(rc)
	{
		rc = -ENOENT;
		SKYCERR(" s5k4ecgx_power failed rc = %d\n", rc);
		goto init_fail;
	}

	s5k4ecgx_ctrl = kzalloc(sizeof(struct s5k4ecgx_ctrl_t), GFP_KERNEL);
	if( !s5k4ecgx_ctrl)
	{
		SKYCERR("s5k4ecgx_init failed!\n");
		rc = -ENOMEM;
		goto init_fail;
	}


	if(data)
		s5k4ecgx_ctrl->sensordata = data;

	SKYCDBG("%s:	Line:%d \n", __func__, __LINE__);
	rc = s5k4ecgx_sensor_init_probe(data);
	if( rc < 0)
		goto init_fail;
	/* if need other initialize, I should add the initialize */

	goto init_done;
	/******************************/
	/*********************************************************/

init_done:
	SKYCDBG("%s end\n", __func__);
	return rc;

init_fail:
	SKYCERR("%s init_fail\n", __func__);
	(void)s5k4ecgx_power_off();
	kfree(s5k4ecgx_ctrl);
	return rc;
}

static int s5k4ecgx_init_client(struct i2c_client *client)
{
	/* Initialize the MSM_CAMI2C Chip */
	init_waitqueue_head(&s5k4ecgx_wait_queue);
	return 0;
}

int s5k4ecgx_sensor_config(void __user *argp)
{
	struct sensor_cfg_data cfg_data;
	int rc = 0;

	if (copy_from_user(&cfg_data,
			(void *)argp,
			sizeof(struct sensor_cfg_data)))
		return -EFAULT;


	SKYCDBG("s5k4ecgx_sensor_config, cfgtype = %d, mode = %d\n",
		cfg_data.cfgtype, cfg_data.mode);

		switch (cfg_data.cfgtype) {
		case CFG_SET_MODE:
			rc = s5k4ecgx_set_sensor_mode(cfg_data.mode);
			SKYCDBG("s5k4ecgx_set_sensor_mode OK rc =[%d]\n", rc);
			break;
		case CFG_SET_EFFECT:
			rc = s5k4ecgx_set_effect(cfg_data.mode,
						cfg_data.cfg.effect);
			SKYCDBG("s5k4ecgx_set_effect OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.effect =[%d]\n", rc, cfg_data.mode, cfg_data.cfg.effect);
			break;
		case CFG_SET_BRIGHTNESS:
			rc = s5k4ecgx_set_brightness(cfg_data.cfg.brightness);
			SKYCDBG("s5k4ecgx_set_brightness OK! rc = [%d], cfg_data.cfg.brightness =[%d]\n", rc, cfg_data.cfg.brightness);
			break;
		case CFG_SET_WB:
			rc = s5k4ecgx_set_whitebalance(cfg_data.cfg.whitebalance);
			SKYCDBG("s5k4ecgx_set_whitebalance OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.whitebalance =[%d]\n", rc, cfg_data.mode, cfg_data.cfg.whitebalance);
			break;
		case CFG_SET_EXPOSURE_MODE:
			rc = s5k4ecgx_set_exposure_mode(cfg_data.cfg.exposure);
			SKYCDBG("s5k4ecgx_set_exposure_mode OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.exposure =[%d]\n", rc, cfg_data.mode, cfg_data.cfg.exposure);
			break;
		case CFG_SET_PREVIEW_FPS:
			rc = s5k4ecgx_set_preview_fps(cfg_data.cfg.preview_fps);
			SKYCDBG("s5k4ecgx_set_frame_rate OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.preview_fps =[%d]\n", rc, cfg_data.mode, cfg_data.cfg.preview_fps);
			break;
		case CFG_SET_REFLECT:
			rc = s5k4ecgx_set_reflect(cfg_data.cfg.reflect);
			SKYCDBG("s5k4ecgx_set_reflect OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.reflect =[%d]\n", rc, cfg_data.mode, cfg_data.cfg.reflect);
			break;
		case CFG_AUTO_FOCUS:
			rc = s5k4ecgx_set_auto_focus(cfg_data.mode, cfg_data.cfg.focus.dir);
			SKYCDBG("s5k4ecgx_auto_focus OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.focus.dir =[%d]\n", rc, cfg_data.mode, cfg_data.cfg.focus.dir);
			break;
		case CFG_SET_LED_MODE:
			if(cfg_data.cfg.led_mode == 1)
				led_auto = 1;
			else
				led_auto = 0;
			rc = s5k4ecgx_set_led_mode(cfg_data.cfg.led_mode);
			SKYCDBG("s5k4ecgx_set_led_mode OK! rc = [%d], cfg_data.cfg.led_mode =[%d]\n", rc, cfg_data.cfg.led_mode);
			break;
		case CFG_SET_FOCUS_STEP:
			rc = s5k4ecgx_set_focus_step(cfg_data.mode, cfg_data.cfg.focus_step);
			SKYCDBG("%s(CFG_SET_FOCUS_STEP), rc=%d\n", __func__, rc);
			break;
		case CFG_GET_AF_MAX_STEPS:
		default:
			rc = -EINVAL;
			break;
		}

	return rc;
}

int s5k4ecgx_sensor_release(void)
{
	int rc = 0;
/****************************************************************/
	SKYCDBG("%s start	led_auto = %d \n",__func__, led_auto);
	s5k4ecgx_set_led_mode(0);
/****************************************************************/
	SKYCDBG(" s5k4ecgx_sensor_release E\n");
	rc = s5k4ecgx_power_off();
	if (rc) {
		SKYCERR(" s5k4ecgx_power failed rc=%d\n",rc);
	}
	kfree(s5k4ecgx_ctrl);
	s5k4ecgx_ctrl = NULL;
	return rc;
}

static int s5k4ecgx_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int rc = 0;
	SKYCDBG("%s:%d\n", __func__, __LINE__);

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		rc = -ENOTSUPP;
		goto probe_failure;
	}
	SKYCDBG("%s:%d\n", __func__, __LINE__);
	s5k4ecgx_sensorw =
		kzalloc(sizeof(struct s5k4ecgx_work), GFP_KERNEL);
	SKYCDBG("%s:%d\n", __func__, __LINE__);
	if (!s5k4ecgx_sensorw) {
		rc = -ENOMEM;
		goto probe_failure;
	}
	SKYCDBG("%s:%d\n", __func__, __LINE__);
	i2c_set_clientdata(client, s5k4ecgx_sensorw);
	s5k4ecgx_init_client(client);
	s5k4ecgx_client = client;

	SKYCDBG("s5k4ecgx_probe succeeded!\n");

	return 0;

probe_failure:
	kfree(s5k4ecgx_sensorw);
	s5k4ecgx_sensorw = NULL;
	SKYCERR("s5k4ecgx_probe failed!\n");
	return rc;
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

static int32_t s5k4ecgx_init_i2c(void)
{
	int32_t rc = 0;

	SKYCDBG("%s start\n",__func__);

	rc = i2c_add_driver(&s5k4ecgx_i2c_driver);
	SKYCDBG("%s s5k4ecgx_i2c_driver rc = %d\n",__func__, rc);
	if (IS_ERR_VALUE(rc))
		goto init_i2c_fail;

	SKYCDBG("%s end\n",__func__);
	return 0;

init_i2c_fail:
	SKYCERR("%s failed! (%d)\n", __func__, rc);
	return rc;
}

static int s5k4ecgx_sensor_probe(const struct msm_camera_sensor_info *info,
				struct msm_sensor_ctrl *s)
{
	int rc = 0;
	SKYCDBG("%s:%d\n", __func__, __LINE__);

	rc = s5k4ecgx_init_i2c();
	if (rc < 0 || s5k4ecgx_client == NULL)
	{
		SKYCERR("%s rc = %d, s5k4ecgx_client = %x\n",__func__, rc, (unsigned int)s5k4ecgx_client);
		goto probe_done;
	}

	s->s_init = s5k4ecgx_sensor_init;
	s->s_release = s5k4ecgx_sensor_release;
	s->s_config  = s5k4ecgx_sensor_config;

	s->s_camera_type = BACK_CAMERA_2D;
	s->s_mount_angle = 90;

probe_done:
	SKYCDBG("%s:%d\n", __func__, __LINE__);
	return rc;
}

static int __s5k4ecgx_probe(struct platform_device *pdev)
{
	SKYCDBG("%s:%d\n", __func__, __LINE__);
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
	SKYCDBG("%s:%d\n", __func__, __LINE__);
	return platform_driver_register(&msm_camera_driver);
}

module_init(s5k4ecgx_init);
