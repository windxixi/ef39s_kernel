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
#include "mt9v113.h"
#include <linux/regulator/consumer.h>
#include <linux/regulator/machine.h>


/* Micron S5K6AAFX13 Registers and their values */
#define SENSOR_DEBUG 0

#define ON  1
#define OFF 0


typedef struct {
	int32_t id;
	const char *label;
	uint32_t num;
} gpio_ctrl_t;


#define CAM_5MP_RST_N	0
#define CAM_5MP_STB	1
#define CAM_VGA_RST_N	2
#define CAM_VGA_STB_N	3
#define CAM_GPIO_MAX	4


static gpio_ctrl_t g_gpios[] = {
	{CAM_5MP_RST_N, "CAM_5MP_RST_N",  106},
	{CAM_5MP_STB,   "CAM_5MP_STB",    86},
	{CAM_VGA_RST_N, "CAM_VGA_RST_N", 137},
	{CAM_VGA_STB_N, "CAM_VGA_STB_N", 139}
};

#define mt9v113_delay_msecs_stream 500

struct mt9v113_work {
	struct work_struct work;
};

static struct  mt9v113_work *mt9v113_sensorw;
static struct  i2c_client *mt9v113_client;

struct mt9v113_ctrl_t {
	const struct msm_camera_sensor_info *sensordata;
};


static struct mt9v113_ctrl_t *mt9v113_ctrl;

static int32_t config_csi;

static DECLARE_WAIT_QUEUE_HEAD(mt9v113_wait_queue);
DECLARE_MUTEX(mt9v113_sem);


struct mt9v113_vreg_t {
	const char *name;
	unsigned short mvolt;
};

#if defined(CONFIG_PRESTO_CAM_PT10) || defined(CONFIG_PANTECH_CAMERA_S5K4ECGX)
static struct regulator *mvs0b_1p8v;
#else
static struct regulator *lvs3b_1p8v;
#endif
static struct regulator *lvs1b_1p8v;
static struct regulator *lvs2b_1p8v;
static struct regulator *l1b_2p8v;


static int32_t mt9v113_i2c_read(unsigned short   saddr,unsigned short raddr, unsigned short *rdata, enum mt9v113_width width);

/*=============================================================*/

static int32_t mt9v113_gpio_out(int32_t id, int32_t val)
{
	int32_t rc = 0;

	if ((id < CAM_5MP_RST_N) || (id >= CAM_GPIO_MAX)) {
		SKYCERR("%s#%d: -EINVAL\n", __func__, __LINE__);
		return -EINVAL;
	}

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

	gpio_free(g_gpios[id].num);

	SKYCDBG("%s: set %s to %d\n", __func__, g_gpios[id].label, val);
	return 0;

gpio_out_fail:
	return rc;
}


static int mt9v113_vreg_init(void)
{
	int rc;
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);

#if defined(CONFIG_PRESTO_CAM_PT10) || defined(CONFIG_PANTECH_CAMERA_S5K4ECGX)
// 5M_IO
	mvs0b_1p8v = regulator_get(NULL, "8901_mvs0" );
	if(IS_ERR(mvs0b_1p8v))
	{
		SKYCERR("regulator_get mvs0b_1p8v fail : 0x%x\n", (unsigned int)mvs0b_1p8v);
		return -ENODEV;
	}
#else
// 5M_IO
	lvs3b_1p8v = regulator_get(NULL, "8901_lvs3" );
	if(IS_ERR(lvs3b_1p8v))
	{
		SKYCERR("regulator_get lvs3b_1p8v fail : 0x%x\n", (unsigned int)lvs3b_1p8v);
		return -ENODEV;
	}
#endif
	lvs2b_1p8v = regulator_get(NULL, "8901_lvs2");
	if (IS_ERR(lvs2b_1p8v)) {
		SKYCERR("regulator_get lvs2b_1p8v fail\n" );;
		return -ENODEV;
	}

	lvs1b_1p8v = regulator_get(NULL, "8901_lvs1");
	if (IS_ERR(lvs1b_1p8v)) {
		SKYCERR("regulator_get lvs1b_1p8v fail\n" );;
		return -ENODEV;
	}

	l1b_2p8v = regulator_get(NULL, "8901_l1");
	if (IS_ERR(l1b_2p8v)) {
		SKYCERR("regulator_get l15a_2p8v fail\n" );;
		return -ENODEV;
	}
	rc = regulator_set_voltage(l1b_2p8v, 2800000, 2800000);
	if (rc) {
		SKYCERR("%s: unable to set l15a_2p8v voltage to 2.8V\n", __func__);
		goto fail;
	}

	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
	return rc;
fail:
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
	if(l1b_2p8v) {
	regulator_put(l1b_2p8v);
	}
	return rc;
}

static int32_t mt9v113_power_on(void)
{

	int32_t rc = 0;
	SKYCDBG("%s %s:%d power = on\n", __FILE__, __func__, __LINE__);

/* VGA CAM GPIO RESET START*/
	SKYCDBG("VGA CAM RESET High Start \n");
	if (mt9v113_gpio_out(CAM_VGA_STB_N, 0))		rc = -EIO;
	msleep(40);
	//reset
	if (mt9v113_gpio_out(CAM_VGA_RST_N, 0))		rc = -EIO;
	msleep(40);
	if (mt9v113_gpio_out(CAM_VGA_RST_N, 1))		rc = -EIO;
	msleep(40);
	SKYCDBG("VGA CAM RESET End \n");


#if defined(CONFIG_PRESTO_CAM_PT10) || defined(CONFIG_PANTECH_CAMERA_S5K4ECGX)
		rc = regulator_enable(mvs0b_1p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator mvs0b_1p8v failed\n", __func__);
			goto fail;
		}
		msleep(1);
#else
		rc = regulator_enable(lvs3b_1p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator lvs3b_1p8v failed\n", __func__);
			goto fail;
		}
		msleep(1);
#endif
		rc = regulator_enable(lvs2b_1p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator lvs2b_1p8v failed\n", __func__);
			goto fail;
		}
		msleep(1);
		rc = regulator_enable(lvs1b_1p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator lvs1b_1p8v failed\n", __func__);
			goto fail;
		}
		msleep(1);
		rc = regulator_enable(l1b_2p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator l1b_2p8v failed\n", __func__);
			goto fail;
		}
		msleep(1);

	return rc;

fail:
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
	if(l1b_2p8v)
		regulator_put(l1b_2p8v);
	return rc;

}

static int32_t mt9v113_power_off(void)
{
	int32_t rc = 0;

	SKYCDBG("VGA CAM RESET Low\n");
	if (mt9v113_gpio_out(CAM_VGA_RST_N, 0))		rc = -EIO;
	msleep(40);
	SKYCDBG("VGA CAM RESET Low End\n");

	rc = regulator_disable(l1b_2p8v);
	if (rc)
		SKYCERR("%s: Disable regulator l1b_2p8v failed\n", __func__);
	regulator_put(l1b_2p8v);

	rc = regulator_disable(lvs1b_1p8v);
	if (rc)
		SKYCERR("%s: Disable regulator lvs1b_1p8v failed\n", __func__);
	rc = regulator_disable(lvs2b_1p8v);
	if (rc)
		SKYCERR("%s: Disable regulator lvs2b_1p8v failed\n", __func__);
#if defined(CONFIG_PRESTO_CAM_PT10) || defined(CONFIG_PANTECH_CAMERA_S5K4ECGX)
	rc = regulator_disable(mvs0b_1p8v);
	if (rc)
		SKYCERR("%s: Disable regulator mvs0b_1p8v failed\n", __func__);
#else
	rc = regulator_disable(lvs3b_1p8v);
	if (rc)
		SKYCERR("%s: Disable regulator lvs3b_1p8v failed\n", __func__);
#endif
	return rc;

}



static int32_t mt9v113_i2c_txdata(unsigned short saddr,
	unsigned char *txdata, int length)
{
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
	if (i2c_transfer(mt9v113_client->adapter, msg, 1) < 0) {
		SKYCERR("mt9v113_i2c_txdata failed\n");
		return -EIO;
	}

	return 0;
}

static int32_t mt9v113_i2c_write(unsigned short saddr,
	unsigned short waddr, unsigned short wdata, enum mt9v113_width width)
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

		rc = mt9v113_i2c_txdata(saddr, buf, 4);
	}
		break;

	case TRIPLE_LEN: {
		buf[0] = (waddr & 0xFF00)>>8;
		buf[1] = (waddr & 0x00FF);
		buf[2] = wdata;
		rc = mt9v113_i2c_txdata(saddr, buf, 3);
	}
		break;

	case BYTE_LEN: {
		buf[0] = waddr;
		buf[1] = wdata;
		rc = mt9v113_i2c_txdata(saddr, buf, 2);
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


#define MT9V113_REG_MCU_VAR_ADDR      0x098c
#define MT9V113_REG_MCU_VAR_DATA      0x0990
#define MT9V113_REG_MODEL_ID          0x3000
#define MT9V113_REG_FRAME_CNT         0x303a
#define MT9V113_REG_MCU_DATA_SEQ_CMD  0xa103

#define MT9V113_I2C_RETRY_CNT   3
#define MT9V113_MAX_WAIT_CNT    20
#define MT9V113_POLL_PERIOD_MS  10

static int32_t mt9v113_i2c_write_table(
	struct mt9v113_i2c_reg_conf const *reg_conf_tbl,
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
//	uint32_t otp_retry_cnt = 0;
//	uint32_t otp_poll_retry = 20;

	for (i = 0; i < num_of_items_in_table; i++)
	{
		switch(reg_conf_tbl->width )
		{
		    case DELAY_T:
			{
				if (reg_conf_tbl->mdelay_time != 0)
					msleep(reg_conf_tbl->mdelay_time);

				reg_conf_tbl++;
				break;
			}
			case ZERO_LEN:
			{
				SKYCDBG("ZERO_LEN continue ADDR = 0x%x, VALUE = 0x%x\n",reg_conf_tbl->waddr, reg_conf_tbl->wdata);
				reg_conf_tbl++;
				break;
			}
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
					if (mt9v113_i2c_read(mt9v113_client->addr, poll_reg, &read_data, WORD_LEN) < 0)
						//mt9v113_i2c_read_word(poll_reg, &read_data) == FALSE)
					{
						SKYCDBG("<<POLL_MCU_VAR mt9v113_i2c_read (FALSE)\n");
						return -1;
					}
					if ((read_data & poll_mask) != poll_data)
					{
						//hhs test SKYCDBG("retry polling MCU variable... after sleeping %d ms, read_data=%2x\n", poll_delay, read_data);
						//SKYCDBG("(read_data & poll_mask)=%4x , poll_data=%4x\n", (read_data & poll_mask), poll_data);
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
			case POLL_MCU_VAR: //polling을 빠져나오는 조건이 정해준 값과 같을 경우
			{
				poll_mcu_var = reg_conf_tbl->waddr;
				poll_mask = reg_conf_tbl->wdata;
				poll_data = (reg_conf_tbl+1)->waddr;
				poll_delay = ((reg_conf_tbl+1)->wdata & 0xff00) >> 8;
				poll_retry = ((reg_conf_tbl+1)->wdata & 0x00ff);

				SKYCDBG("POLLING!! poll_delay=%x, poll_retry=%x, poll_mcu_var=%x, poll_data=%x, poll_mask=%x\n",poll_delay, poll_retry, poll_mcu_var, poll_data, poll_mask);

				for (retry_cnt = 0; retry_cnt < poll_retry; retry_cnt++)
				{
					if (mt9v113_i2c_write(mt9v113_client->addr,MT9V113_REG_MCU_VAR_ADDR, poll_mcu_var,WORD_LEN) < 0)
						//mt9p111_i2c_write_word(MT9P111_REG_MCU_ADDR, poll_mcu_var) < 0)
					{
						SKYCDBG("<<POLL_MCU_VAR mt9p111_i2c_write_word (FALSE)\n");
						return -1;
					}
					if (mt9v113_i2c_read(mt9v113_client->addr,MT9V113_REG_MCU_VAR_DATA, &read_data,WORD_LEN) < 0)
						//mt9p111_i2c_read_word(MT9P111_REG_MCU_DATA, &read_data) < 0)
					{
						SKYCDBG("<<POLL_MCU_VAR mt9p111_i2c_read_word (FALSE)\n");
						return -1;
					}

					if ((read_data & poll_mask) != poll_data)
					{
						//hhs test SKYCDBG("retry polling MCU variable... after sleeping %d ms, read_data=%2x\n", poll_delay, read_data);
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
					SKYCDBG("<<RETRY FAIL!! POLL_MCU_VAR read_data = %x (FALSE)\n", read_data);
					return -1;
				}

				//  2개의 값을 이용하므로 +2를 해준다
				reg_conf_tbl++;
				reg_conf_tbl++;
				break;
			}
			default:
			{
				rc = mt9v113_i2c_write(mt9v113_client->addr,
									reg_conf_tbl->waddr, reg_conf_tbl->wdata,
									reg_conf_tbl->width);
				//SKYCDBG("I2C WRITE!!! ADDR = 0x%x, VALUE = 0x%x, width = %d, num_of_items_in_table=%d, i=%d\n",
				//	reg_conf_tbl->waddr, reg_conf_tbl->wdata, reg_conf_tbl->width, num_of_items_in_table, i);

				if (rc < 0)
				{
					SKYCERR("mt9v113_i2c_read failed!\n");
					return rc;
				}

				if (reg_conf_tbl->mdelay_time != 0)
					msleep(reg_conf_tbl->mdelay_time);

				reg_conf_tbl++;

				break;
			}
		}
	}

	return rc;
}


static int mt9v113_i2c_rxdata(unsigned short saddr,
	unsigned char *rxdata, int length)
{
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

	if (i2c_transfer(mt9v113_client->adapter, msgs, 2) < 0) {
		SKYCERR("mt9v113_i2c_rxdata failed!\n");
		return -EIO;
	}

	return 0;
}

static int32_t mt9v113_i2c_read(unsigned short   saddr,
	unsigned short raddr, unsigned short *rdata, enum mt9v113_width width)
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

		rc = mt9v113_i2c_rxdata(saddr, buf, 2);
		if (rc < 0)
			return rc;

		*rdata = buf[0] << 8 | buf[1];
	}
		break;

	default:
		break;
	}

	if (rc < 0)
		SKYCERR("mt9v113_i2c_read failed!\n");

	return rc;
}

static long mt9v113_reg_init(void)
{

	long rc;

	/* PLL Setup Start */
	rc = mt9v113_i2c_write_table(&mt9v113_regs.init_parm[0],
					mt9v113_regs.init_parm_size);
	if (rc < 0)
	{
		SKYCERR("mt9v113_i2c_write failed!\n");
		return rc;
	}
	else
	{
		SKYCDBG("--hhs mt9v113_i2c_write sucessed \n");
	}
	if (rc < 0)
		return rc;
	/* PLL Setup End   */

	return 0;
}


static long mt9v113_set_effect(int mode, int effect)
{
	long rc = 0;

	SKYCDBG("%s start\n",__func__);

	if(effect < CAMERA_EFFECT_OFF || effect >= CAMERA_EFFECT_MAX){
		SKYCERR("%s error. effect=%d\n", __func__, effect);
		return 0;//-EINVAL;
	}

	rc = mt9v113_i2c_write_table(mt9v113_regs.effect_cfg_settings[effect],
					mt9v113_regs.effect_cfg_settings_size);
	if (rc < 0)
	{
		SKYCDBG("CAMERA_WB I2C FAIL!!! return~~\n");
		return rc;
	}
	SKYCDBG("%s end\n",__func__);
	return rc;
}

static int32_t mt9v113_set_brightness(int8_t brightness)
{
	int32_t rc = 0;
	SKYCDBG("%s start~ receive brightness = %d\n",__func__, brightness);

	if ((brightness < C_PANTECH_CAMERA_MIN_BRIGHTNESS) || (brightness > C_PANTECH_CAMERA_MAX_BRIGHTNESS)) {
		SKYCERR("%s error. brightness=%d\n", __func__, brightness);
		return 0;//-EINVAL;
	}

	rc = mt9v113_i2c_write_table(mt9v113_regs.bright_cfg_settings[brightness],
					mt9v113_regs.bright_cfg_settings_size);
	if (rc < 0)
	{
		SKYCDBG("CAMERA_BRIGHTNESS I2C FAIL!!! return~~\n");
		return rc;
	}
	SKYCDBG("%s end\n",__func__);
	return rc;
}

static int32_t mt9v113_set_whitebalance (int32_t whitebalance)
{

	int32_t rc = 0;

	SKYCDBG("%s start  whitebalance=%d\n",__func__, whitebalance);

	rc = mt9v113_i2c_write_table(mt9v113_regs.wb_cfg_settings[whitebalance-1],
					mt9v113_regs.wb_cfg_settings_size);
	if (rc < 0)
	{
		SKYCDBG("CAMERA_WB I2C FAIL!!! return~~\n");
		return rc;
	}

	SKYCDBG("%s end\n",__func__);
	return rc;
}


static int32_t mt9v113_set_exposure_mode(int32_t exposure)
{
	int32_t rc = 0;

	SKYCDBG("%s  exposure = %d\n",__func__, exposure);

	if ((exposure < 0) || (exposure >= 4))
	{
		SKYCERR("%s FAIL!!! return~~  exposure = %d\n",__func__,exposure);
		return 0;//-EINVAL;
	}

	rc = mt9v113_i2c_write_table(mt9v113_regs.exposure_cfg_settings[exposure],
					mt9v113_regs.exposure_cfg_settings_size);
	if (rc < 0)
	{
		SKYCDBG("CAMERA_EFFECT_SEPIA I2C FAIL!!! return~~\n");
		return rc;
	}

	SKYCDBG("%s end\n",__func__);

	return rc;
}


static int32_t mt9v113_set_preview_fps(int32_t preview_fps)
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

	rc = mt9v113_i2c_write_table(mt9v113_regs.preview_fps_cfg_settings[preview_fps - C_PANTECH_CAMERA_MIN_PREVIEW_FPS],
					mt9v113_regs.preview_fps_cfg_settings_size);

	SKYCDBG("%s end rc = %d\n",__func__, rc);

	return rc;
}


static int32_t mt9v113_set_reflect(int32_t reflect)
{
	int32_t rc = 0;


	SKYCDBG("%s  reflect = %d\n",__func__, reflect);

	if ((reflect < 0) || (reflect >= 4))
	{
		SKYCDBG("%s FAIL!!! return~~  reflect = %d\n",__func__,reflect);
		return 0;//-EINVAL;
	}
	rc = mt9v113_i2c_write_table(mt9v113_regs.reflect_cfg_settings[reflect],
				mt9v113_regs.reflect_cfg_settings_size);
	if (rc < 0)
	{
		SKYCERR("CAMERA_SET_REFLECT I2C FAIL!!! return~~\n");
		return rc;
	}

	SKYCDBG("%s end\n",__func__);

	return rc;
}

static int32_t mt9v113_video_config(void)
{
	int32_t rc = 0;

	SKYCDBG("%s start\n",__func__);

	rc = mt9v113_i2c_write_table(&mt9v113_regs.preview_cfg_settings[0],
						mt9v113_regs.preview_cfg_settings_size);

	if (rc < 0)
	{
		SKYCERR("mt9v113_i2c_write_table FAIL!!! return~~\n");
		return rc;
	}
	SKYCDBG("%s end rc = %d\n",__func__, rc);

	return rc;
}

static int32_t mt9v113_snapshot_config(void)
{
	int32_t rc = 0;

	SKYCDBG("%s start\n",__func__);
	rc = mt9v113_i2c_write_table(&mt9v113_regs.snapshot_cfg_settings[0],
					mt9v113_regs.snapshot_cfg_settings_size);
	if (rc < 0)
	{
		SKYCERR("mt9v113_i2c_write_table FAIL!!! return~~\n");
		return rc;
	}
	SKYCDBG("%s end rc = %d\n",__func__, rc);

	return rc;
}


static long mt9v113_set_sensor_mode(int mode)
{
	long rc = 0;
	struct msm_camera_csi_params mt9v113_csi_params;

	SKYCDBG("mt9v113_set_sensor_mode mode = %d!\n",mode);

	switch (mode) {
	case SENSOR_PREVIEW_MODE:
		if(config_csi == 0)
		{
			SKYCDBG("mt9v113_set_sensor_mode config_csi E\n");
			mt9v113_csi_params.lane_cnt = 1; // 4;
			mt9v113_csi_params.data_format = CSI_8BIT; //CSI_10BIT;
			mt9v113_csi_params.lane_assign = 0xe4;
			mt9v113_csi_params.dpcm_scheme = 0;
			mt9v113_csi_params.settle_cnt = 0x14;
			rc = msm_camio_csi_config(&mt9v113_csi_params);
			msleep(mt9v113_delay_msecs_stream);
			config_csi = 1;
			SKYCDBG("mt9v113_set_sensor_mode config_csi X\n");
		}
		rc = mt9v113_video_config();
		SKYCDBG("mt9v113_video_config, rc = %d \n", (int)rc);
		if (rc < 0)
		{
			SKYCERR("mt9v113_video_config FAIL!!! return~~\n");
			return rc;
		}
		break;

	case SENSOR_SNAPSHOT_MODE:
		rc = mt9v113_snapshot_config();
		SKYCDBG("mt9v113_snapshot_config, rc = %d \n", (int)rc);
		if (rc < 0)
		{
			SKYCERR("mt9v113_snapshot_config FAIL!!! return~~\n");
			return rc;
		}
		break;

	case SENSOR_RAW_SNAPSHOT_MODE:

		break;

	default:
		return -EINVAL;
	}
	SKYCDBG("mt9v113_set_sensor_mode X!\n");

	return rc;
}

static int mt9v113_sensor_init_probe(const struct msm_camera_sensor_info *data)
{
	int32_t rc;
	SKYCDBG("%s start\n", __func__);

	rc = mt9v113_reg_init();
	if(rc < 0)
		goto init_probe_fail;
	return rc;

init_probe_fail:
	SKYCERR("%s init_probe_fail\n",__func__);
	return rc;
}

int mt9v113_sensor_init(const struct msm_camera_sensor_info *data)
{
	int rc = 0;

	config_csi = 0;
	SKYCDBG("%s:	Line:%d \n", __func__,__LINE__);


	SKYCDBG("%s start\n", __func__);

	mt9v113_vreg_init();

	rc = mt9v113_power_on();
	if(rc)
	{
		rc = -ENOENT;
		SKYCERR(" mt9v113_power failed rc = %d\n", rc);
		goto init_fail;
	}

	mt9v113_ctrl = kzalloc(sizeof(struct mt9v113_ctrl_t), GFP_KERNEL);
	if( !mt9v113_ctrl)
	{
		SKYCERR("mt9v113_init failed!\n");
		rc = -ENOMEM;
		goto init_fail;
	}


	if(data)
		mt9v113_ctrl->sensordata = data;

	/* enable mclk first */
	msm_camio_clk_rate_set(24000000);
	mdelay(10);


	SKYCDBG("%s:	Line:%d \n", __func__, __LINE__);
	rc = mt9v113_sensor_init_probe(data);
	if( rc < 0)
		goto init_fail;
	/* if need other initialize, I should add the initialize */

	goto init_done;

init_done:
	SKYCDBG("%s end\n", __func__);
	return rc;

init_fail:
	SKYCERR("%s init_fail\n", __func__);
	(void)mt9v113_power_off();
	kfree(mt9v113_ctrl);
	return rc;
}

static int mt9v113_init_client(struct i2c_client *client)
{
	/* Initialize the MSM_CAMI2C Chip */
	init_waitqueue_head(&mt9v113_wait_queue);
	return 0;
}

int mt9v113_sensor_config(void __user *argp)
{
	struct sensor_cfg_data cfg_data;
	int  rc = 0;

	if (copy_from_user(&cfg_data,
			(void *)argp,
			sizeof(struct sensor_cfg_data)))
		return -EFAULT;

	/* down(&s5k6aafx13_sem); */

	SKYCDBG("mt9v113_sensor_config, cfgtype = %d, mode = %d\n",
		cfg_data.cfgtype, cfg_data.mode);

		switch (cfg_data.cfgtype) {
		case CFG_SET_MODE:
			rc = mt9v113_set_sensor_mode(
						cfg_data.mode);
			break;

		case CFG_SET_EFFECT:
			rc = mt9v113_set_effect(cfg_data.mode,
						cfg_data.cfg.effect);
			SKYCDBG("mt9v113_set_effect OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.effect =[%d]\n", (int)rc, (int)cfg_data.mode, (int)cfg_data.cfg.effect);
			break;
		case CFG_SET_BRIGHTNESS:
			rc = mt9v113_set_brightness(cfg_data.cfg.brightness);
			SKYCDBG("mt9v113_set_brightness OK! rc = [%d], cfg_data.cfg.brightness =[%d]\n", (int)rc, cfg_data.cfg.brightness);
			break;
		case CFG_SET_WB:
			rc = mt9v113_set_whitebalance(cfg_data.cfg.whitebalance);
			SKYCDBG("mt9v113_set_whitebalance OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.whitebalance =[%d]\n", (int)rc, cfg_data.mode, cfg_data.cfg.whitebalance);
			break;
		case CFG_SET_EXPOSURE_MODE:
			rc = mt9v113_set_exposure_mode(cfg_data.cfg.exposure);
			SKYCDBG("mt9v113_set_exposure_mode OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.exposure =[%d]\n", (int)rc, cfg_data.mode, cfg_data.cfg.exposure);
			break;
		case CFG_SET_PREVIEW_FPS:
			rc = mt9v113_set_preview_fps(cfg_data.cfg.preview_fps);
			SKYCDBG("mt9v113_set_frame_rate OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.preview_fps =[%d]\n", (int)rc, cfg_data.mode, cfg_data.cfg.preview_fps);
			break;
		case CFG_SET_REFLECT:
			rc = mt9v113_set_reflect(cfg_data.cfg.reflect);
			SKYCDBG("mt9v113_set_reflect OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.reflect =[%d]\n", rc, cfg_data.mode, cfg_data.cfg.reflect);
			break;
		case CFG_GET_AF_MAX_STEPS:
		default:
			rc = -EINVAL;
			break;
		}

	return rc;
}

int mt9v113_sensor_release(void)
{
	int rc = 0;

	SKYCDBG(" mt9v113_sensor_release E\n");
	rc = mt9v113_power_off();

	if (rc) {
		SKYCERR(" mt9v113_power failed rc=%d\n",rc);
	}

	kfree(mt9v113_ctrl);

	return rc;
}

static int mt9v113_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int rc = 0;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		rc = -ENOTSUPP;
		goto probe_failure;
	}

	mt9v113_sensorw =
		kzalloc(sizeof(struct mt9v113_work), GFP_KERNEL);

	if (!mt9v113_sensorw) {
		rc = -ENOMEM;
		goto probe_failure;
	}

	i2c_set_clientdata(client, mt9v113_sensorw);
	mt9v113_init_client(client);
	mt9v113_client = client;

	SKYCDBG("mt9v113_probe succeeded!\n");

	return 0;

probe_failure:
	kfree(mt9v113_sensorw);
	mt9v113_sensorw = NULL;
	SKYCERR("mt9v113_probe failed!\n");
	return rc;
}

static const struct i2c_device_id mt9v113_i2c_id[] = {
	{ "mt9v113", 0},
	{ },
};

static struct i2c_driver mt9v113_i2c_driver = {
	.id_table = mt9v113_i2c_id,
	.probe  = mt9v113_i2c_probe,
	.remove = __exit_p(mt9v113_i2c_remove),
	.driver = {
		.name = "mt9v113",
	},
};

static int32_t mt9v113_init_i2c(void)
{
	int32_t rc = 0;

	SKYCDBG("%s start\n",__func__);

	rc = i2c_add_driver(&mt9v113_i2c_driver);
	SKYCDBG("%s mt9v113_i2c_driver rc = %d\n",__func__, rc);
	if (IS_ERR_VALUE(rc))
		goto init_i2c_fail;

	SKYCDBG("%s end\n",__func__);

	//hhs i2c test
	return 0;

init_i2c_fail:
	SKYCERR("%s failed! (%d)\n", __func__, rc);
	return rc;
}

static int mt9v113_sensor_probe(const struct msm_camera_sensor_info *info,
				struct msm_sensor_ctrl *s)
{
	int rc = 0;
	SKYCDBG("%s start\n",__func__);

	rc = mt9v113_init_i2c();
	if (rc < 0 || mt9v113_client == NULL)
	{
		SKYCERR("%s rc = %d, mt9v113_client = %x\n",__func__, rc,(unsigned int) mt9v113_client);
		goto probe_done;
	}

	s->s_init = mt9v113_sensor_init;
	s->s_release = mt9v113_sensor_release;
	s->s_config  = mt9v113_sensor_config;

	s->s_camera_type = FRONT_CAMERA_2D;
	s->s_mount_angle = 270;

probe_done:
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
	return rc;
}

static int __mt9v113_probe(struct platform_device *pdev)
{
	return msm_camera_drv_start(pdev, mt9v113_sensor_probe);
}

static struct platform_driver msm_camera_driver = {
	.probe = __mt9v113_probe,
	.driver = {
		.name = "msm_camera_mt9v113",
		.owner = THIS_MODULE,
	},
};

static int __init mt9v113_init(void)
{
	return platform_driver_register(&msm_camera_driver);
}

module_init(mt9v113_init);
