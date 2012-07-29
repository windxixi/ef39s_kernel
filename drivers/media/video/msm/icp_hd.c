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
#include "icp_hd.h"
#include <linux/regulator/consumer.h>
#include <linux/regulator/machine.h>

//#include <mach/vreg.h>
#include "camsensor_icp_hd_tuneup.h"

#define C_PANTECH_CAMERA_MIN_PREVIEW_FPS	5
#define C_PANTECH_CAMERA_MAX_PREVIEW_FPS	30

#define F_ICP_HD_POWER

/* Micron ICP_HD Registers and their values */
#define SENSOR_DEBUG 0

#define ON  1
#define OFF 0

#define SENSOR_RESET 106
#define SENSOR_STANDBY 57
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_LED_MODE
#define ICP_HD_LED_FLASH_ENABLE_GPIO 31
#endif

#define icp_hd_delay_msecs_stream 100//50//200//500
#define ICP_HD_I2C_RETRY	10
#define ICP_HD_I2C_MPERIOD	200
#define ICP_HD_SNAPSHOT_RETRY 	200//30
#define ICP_HD_PREVIEW_RETRY 	30
#define ICP_HD_POLLING_RETRY	 	30

struct icp_hd_work {
	struct work_struct work;
};

static struct  icp_hd_work *icp_hd_sensorw;
static struct  i2c_client *icp_hd_client;

struct icp_hd_ctrl_t {
	const struct msm_camera_sensor_info *sensordata;
};


static struct icp_hd_ctrl_t *icp_hd_ctrl = NULL;

#if 1 //def F_PANTECH_CAMERA_ADD_CFG_DIMENSION
#if 1 //def F_PANTECH_CAMERA_1080P_PREVIEW
static struct dimension_cfg icp_hd_dimension;
static int32_t icp_hd_config_1080P_preview;
#endif
#endif

static int32_t config_csi;
static bool b_snapshot_flag;

static DECLARE_WAIT_QUEUE_HEAD(icp_hd_wait_queue);
DECLARE_MUTEX(icp_hd_sem);
static int16_t icp_hd_effect = CAMERA_EFFECT_OFF;

#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
static icp_hd_tune_state_type icp_hd_tup_state = ICP_HD_TUNE_STATE_NONE;
static icp_hd_tune_mode_type icp_hd_tup_mode = ICP_HD_TUNE_STATE_TUNNING_MODE_ON;
static icp_hd_params_tbl_t icp_hd_params_tbl;
#endif

struct icp_hd_vreg_t {
	const char *name;
	unsigned short mvolt;
};

#if 1 //def F_PANTECH_CAMERA_FIX_CFG_LED_MODE
int led_auto;
uint16_t nbrightness = 0;
#endif
#ifdef F_ICP_HD_POWER
static struct regulator *l20a_2p8v;
static struct regulator *mvs0b_1p8v;
static struct regulator *s2b_1p2v;
static struct regulator *lvs3b_1p8v;
static struct regulator *l14a_2p8v;

/*
================================================================================
LOCAL API DECLARATIONS
================================================================================
*/
static int32_t icp_hd_i2c_read(unsigned short saddr, unsigned short raddr, unsigned short *rdata, enum icp_hd_width width);
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
static u_int32_t icp_hd_i2c_write_params
(
    icp_hd_cfg_param_t *params,
    u_int32_t num_params
);
#endif

/*
================================================================================
FUNCTION
================================================================================
*/
static int icp_hd_vreg_init(void)
{
	int rc = 0;
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);

	s2b_1p2v = regulator_get(NULL, "8901_s2");
	if (IS_ERR(s2b_1p2v)) {
		SKYCERR("regulator_get s2b_1p2v fail : 0x%x\n", (unsigned int)s2b_1p2v);
		return -ENODEV;
	}
	rc = regulator_set_voltage(s2b_1p2v, 1300000, 1300000);
	if (rc) {
		SKYCERR("%s: unable to set s2b_1p2v voltage to 1.2V\n", __func__);
		goto fail;
	}

	lvs3b_1p8v = regulator_get(NULL, "8901_lvs3");
	if (IS_ERR(lvs3b_1p8v)) {
		SKYCERR("regulator_get lvs3b_1p8v : 0x%x fail\n", (unsigned int)lvs3b_1p8v);
		return -ENODEV;
	}

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

	
	l14a_2p8v = regulator_get(NULL, "8058_l14");
	if (IS_ERR(l14a_2p8v)) {
		SKYCERR("regulator_get l14a_2p8v : 0x%x fail\n", (unsigned int)l14a_2p8v);
		return -ENODEV;
	}
#if 1 // for SD card
	rc = regulator_set_voltage(l14a_2p8v, 2850000, 2850000);
#else
	rc = regulator_set_voltage(l14a_2p8v, 2800000, 2800000);
#endif
	if (rc) {
		SKYCERR("%s: unable to set l14a_2p8v voltage to 2.8V\n", __func__);
		goto fail;
	}
	
	SKYCDBG("%s %s Success!:%d\n", __FILE__, __func__, __LINE__);
	return rc;
fail:
	SKYCDBG("%s %s Failed!:%d\n", __FILE__, __func__, __LINE__);
	if(l20a_2p8v) {
		regulator_put(l20a_2p8v);
	}
	if(s2b_1p2v) {
		regulator_put(s2b_1p2v);
	}
	if(l14a_2p8v) {
		regulator_put(l14a_2p8v);
	}
	return rc;	

}

static int icp_hd_power(int on)
{
	int rc = 0;
		SKYCDBG("%s %s:%d power = %d\n", __FILE__, __func__, __LINE__,on);
	if(on) {
		//standby control
		rc = gpio_tlmm_config(GPIO_CFG(SENSOR_STANDBY, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);
		if (!rc) {
			SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
			gpio_set_value(SENSOR_STANDBY,0);					
		}
		
		rc = regulator_enable(s2b_1p2v);
		if (rc) {
			SKYCERR("%s: Enable regulator s2b_1p2v failed\n", __func__);
			goto fail;
		}
		mdelay(1);
		
		rc = regulator_enable(mvs0b_1p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator mvs0b_1p8v failed\n", __func__);
			goto fail;
		}
		mdelay(1);
		
		rc = regulator_enable(lvs3b_1p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator lvs3b_1p8v failed\n", __func__);
			goto fail;
		}		
		mdelay(1);
		
		rc = regulator_enable(l20a_2p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator l20a_2p8v failed\n", __func__);
			goto fail;
		}
		mdelay(1);
		
		rc = regulator_enable(l14a_2p8v);
		if (rc) {
			SKYCERR("%s: Enable regulator l14a_2p8v failed\n", __func__);
			goto fail;
		}

		SKYCDBG("%s %s ON Success:%d\n", __FILE__, __func__, __LINE__);
	}
	else {
		rc = regulator_disable(s2b_1p2v);
		if (rc){
			SKYCERR("%s: Disable regulator s2b_1p2v failed\n", __func__);
			goto fail;
		}
		regulator_put(s2b_1p2v);

		rc = regulator_disable(lvs3b_1p8v);
		if (rc){
			SKYCERR("%s: Disable regulator lvs3b_1p8v failed\n", __func__);		
			goto fail;
		}

		rc = regulator_disable(mvs0b_1p8v);
		if (rc){
			SKYCERR("%s: Disable regulator mvs0b_1p8v failed\n", __func__);
			goto fail;
		}
		
		rc = regulator_disable(l20a_2p8v);
		if (rc){
			SKYCERR("%s: Disable regulator l20a_2p8v failed\n", __func__);
			goto fail;
		}
		regulator_put(l20a_2p8v);
		
		rc = regulator_disable(l14a_2p8v);
		if (rc){
			SKYCERR("%s: Disable regulator l14a_2p8v failed\n", __func__);
			goto fail;
		}
		regulator_put(l14a_2p8v);		
		SKYCDBG("%s %s OFF Success:%d\n", __FILE__, __func__, __LINE__);
	}
	
	return rc;
fail:
	SKYCERR("%s %s Failed!:%d\n", __FILE__, __func__, __LINE__);
	if(l20a_2p8v){
		regulator_put(l20a_2p8v);
	}
	if(s2b_1p2v){
		regulator_put(s2b_1p2v);
	}
	if(l14a_2p8v){
		regulator_put(l14a_2p8v);
	}
	return rc;			
}
#endif

static int icp_hd_reset(const struct msm_camera_sensor_info *dev, int set)
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
		SKYCERR("icp_hd_reset gpio_tlmm_config Failed!\n");
		goto reset_fail;
	}

	SKYCDBG("%s %s Success:%d\n", __FILE__, __func__, __LINE__);
	return rc;

reset_fail:
	SKYCERR("%s %s Failed!:%d\n", __FILE__, __func__, __LINE__);
	return rc;
}

static int32_t icp_hd_i2c_txdata(unsigned short saddr,
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
	for (i = 0; i < ICP_HD_I2C_RETRY; i++) {
		rc = i2c_transfer(icp_hd_client->adapter, msg, 1); 
		if (rc >= 0) {			
			return 0;
		}
		SKYCDBG("%s: tx retry. [%02x.%02x.%02x] len=%d rc=%d\n", __func__,saddr, *txdata, *(txdata + 1), length, rc);
		msleep(ICP_HD_I2C_MPERIOD);
	}
	return -EIO;
}

static int32_t icp_hd_i2c_write(unsigned short saddr,
	unsigned short waddr, unsigned int dwdata, enum icp_hd_width width)
{
	int32_t rc = -EIO;
	unsigned char buf[6];

	memset(buf, 0, sizeof(buf));
	switch (width) {
	case DWORD_LEN: {
		buf[0] = (waddr & 0xFF00)>>8;
		buf[1] = (waddr & 0x00FF);
		buf[2] = (dwdata & 0xFF000000)>>24;
		buf[3] = (dwdata & 0x00FF0000)>>16;
		buf[4] = (dwdata & 0x0000FF00)>>8;
		buf[5] = (dwdata & 0x000000FF);
#ifdef DEBUG_I2C_VALUE	
		SKYCDBG("DWORD_LEN10: dwdata  = 0x%x\n", (unsigned int)dwdata );
		SKYCDBG("DWORD_LEN11: buf[2] = 0x%x, buf[3] = 0x%x, buf[4] = 0x%x, buf[5] = 0x%x\n", (unsigned int)buf[2], (unsigned int)buf[3], (unsigned int)buf[4], (unsigned int)buf[5]);
#endif		
		rc = icp_hd_i2c_txdata(saddr, buf, 6);
	}
		break;
	case WORD_LEN: {
		buf[0] = (waddr & 0xFF00)>>8;
		buf[1] = (waddr & 0x00FF);
		buf[2] = (dwdata & 0x0000FF00)>>8;
		buf[3] = (dwdata & 0x000000FF);
		rc = icp_hd_i2c_txdata(saddr, buf, 4);
	}
		break;

	case TRIPLE_LEN: {
		buf[0] = (waddr & 0xFF00)>>8;
		buf[1] = (waddr & 0x00FF);
		buf[2] = (dwdata & 0x000000FF);
		rc = icp_hd_i2c_txdata(saddr, buf, 3);
	}
		break;

	case BYTE_LEN: {
		buf[0] = waddr;
		buf[1] = (dwdata & 0x000000FF);
		rc = icp_hd_i2c_txdata(saddr, buf, 2);
	}
		break;

	default:
		break;
	}

	if (rc < 0)
		SKYCERR(
		"i2c_write failed, addr = 0x%x, val = 0x%x!\n",
		waddr, dwdata);

	return rc;
}

#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
static int32_t icp_hd_i2c_write_a2d1(unsigned short waddr, unsigned char wdata)
{
	int32_t rc = -EIO;
	unsigned char buf[3];

	memset(buf, 0, sizeof(buf));

	buf[0] = (waddr & 0xFF00)>>8;
	buf[1] = (waddr & 0x00FF);
	buf[2] = wdata;
	
	rc = icp_hd_i2c_txdata(icp_hd_client->addr, buf, 3);

	if (rc < 0)
		SKYCERR(
		"i2c_write failed, saddr= 0x%x, addr = 0x%x, val = 0x%x!\n",
		icp_hd_client->addr, waddr, wdata);

	return rc;
}
#endif

#ifdef BURST_MODE_64_BYTE
static int32_t icp_hd_i2c_write_dw(unsigned short waddr, unsigned int dwdata32)
{
	int32_t rc = -EIO;
	unsigned char buf[6];

	memset(buf, 0, sizeof(buf));
	
	buf[0] = (waddr & 0xFF00)>>8;
	buf[1] = (waddr & 0x00FF);
	buf[2] = (dwdata32 & 0xFF000000)>>24;
	buf[3] = (dwdata32 & 0x00FF0000)>>16;
	buf[4] = (dwdata32 & 0x0000FF00)>>8;
	buf[5] = (dwdata32 & 0x000000FF);

	rc = icp_hd_i2c_txdata(icp_hd_client->addr, buf, 6);
#ifdef DEBUG_I2C_VALUE	
	SKYCDBG("icp_hd_i2c_write_dw10: dwdata32 = 0x%x\n", dwdata32);
	SKYCDBG("icp_hd_i2c_write_dw11: buf[2] = 0x%x, buf[3] = 0x%x, buf[4] = 0x%x, buf[5] = 0x%x\n", buf[2], buf[3], buf[4], buf[5]);
#endif	
	if (rc < 0)
		SKYCERR("i2c_write failed, addr = 0x%x, dwdata32 = 0x%x\n", waddr, dwdata32);

	return rc;
}
#endif

static int32_t icp_hd_i2c_write_w(unsigned short waddr, unsigned short wdata)
{
	int32_t rc = -EIO;
	unsigned char buf[4];

	memset(buf, 0, sizeof(buf));
	
	buf[0] = (waddr & 0xFF00)>>8;
	buf[1] = (waddr & 0x00FF);
	buf[2] = (wdata & 0xFF00)>>8;
	buf[3] = (wdata & 0x00FF);

	rc = icp_hd_i2c_txdata(icp_hd_client->addr, buf, 4);

	if (rc < 0)
		SKYCERR("i2c_write failed, addr = 0x%x, dwdata32 = 0x%x\n", waddr, wdata);

	return rc;
}

static int32_t icp_hd_i2c_write_table(
	struct icp_hd_i2c_reg_conf const *reg_conf_tbl,
	int num_of_items_in_table)
{
#if 1
	int i;
	int32_t rc = -EIO;
#if defined(BURST_MODE_64_BYTE)||defined(BURST_MODE_4096_BYTE)||defined(BURST_MODE_64_TO_4096_BYTE)
	int32_t burst_current_pos = 0;
	int32_t burst_current_size = 0;
	int32_t burst_reg_size = 0;
	int32_t icp_hd_start_addr = 0;
	uint8_t * icp_hd_reg_base_addr = 0;
#if defined(BURST_MODE_64_TO_4096_BYTE)
#define BURST_DATA_LEN 2046//4078//4094//60//2046//4094
	uint8_t icp_hd_burst_data[2+BURST_DATA_LEN];
#endif
#endif	

	uint16_t poll_delay = 0;
       uint16_t poll_retry = 0;
       uint16_t poll_mcu_var = 0;
       uint16_t poll_data = 0;
       uint16_t poll_mask = 0;
       uint16_t retry_cnt = 0;
	uint16_t read_data = 0;
	//OTP defense code add
//	uint16_t otp_retry_cnt = 0;
//	uint16_t otp_poll_retry = 20;

	for (i = 0; i < num_of_items_in_table; i++) 
	{		
	switch(reg_conf_tbl->width )
	{
		case ZERO_LEN:
		{
			SKYCDBG("ZERO_LEN continue ADDR = 0x%x, VALUE = 0x%x\n",reg_conf_tbl->waddr, reg_conf_tbl->dwdata);
			reg_conf_tbl++;	
			rc = 0;
			continue;
		}
		case POLL_MCU_VAR:
		{
			poll_mcu_var = reg_conf_tbl->waddr;
   		       poll_mask = reg_conf_tbl->dwdata;	              
	              poll_data = (reg_conf_tbl+1)->waddr;
			poll_delay = ((reg_conf_tbl+1)->dwdata & 0xff00) >> 8;
                     poll_retry = ((reg_conf_tbl+1)->dwdata & 0x00ff);              

		       SKYCDBG("POLLING!! poll_delay=%x, poll_retry=%x, poll_mcu_var=%x, poll_data=%x, poll_mask=%x\n",poll_delay, poll_retry, poll_mcu_var, poll_data, poll_mask);
				  
			for (retry_cnt = 0; retry_cnt < poll_retry; retry_cnt++)
                	{
			            rc = icp_hd_i2c_read(icp_hd_client->addr, poll_mcu_var, &read_data, WORD_LEN);
	                    if (rc < 0)
	                    {
	                        SKYCERR("<<POLL_MCU_VAR icp_hd_i2c_read_word (FALSE)\n");
	                        return FALSE;
	                    }
	                    
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
	                    SKYCERR("<<RETRY FAIL!! read_data = %x (FALSE)\n", read_data);
#ifdef ICP_HD_MODULE_ABNORMAL_OPERATION_DEFENCE			      
				return ICP_HD_REG_POLLING_ERROR;
#else
				return -EIO;
#endif
	              }

			//  2개의 값을 이용하므로 +2를 해준다
			reg_conf_tbl++;
			reg_conf_tbl++;
			i++;

			break;
		}
#ifdef BURST_MODE_64_BYTE
		case BURST_WORD_1:
			burst_current_pos = 0;
 			burst_current_size = 0;
			burst_reg_size = icp_hd_regs.init_burst_settings_size_1;
			icp_hd_reg_base_addr = icp_hd_regs.init_burst_settings_1;
			icp_hd_start_addr = 0x0000644C;		
			SKYCDBG("BURST_WORD_1 START~~~ icp_hd_start_addr= 0x%x, icp_hd_reg_base_addr = 0x%x, burst_reg_size = 0x%x\n", icp_hd_start_addr, icp_hd_reg_base_addr, burst_reg_size);
							
			while(burst_current_pos < burst_reg_size){
				burst_current_size = ((burst_reg_size - burst_current_pos)>=62 )? 62 : (burst_reg_size - burst_current_pos);
#ifdef DEBUG_I2C_VALUE		
#if 0
				SKYCDBG("BURST_WORD_10~~ icp_hd_start_addr= 0x%x, icp_hd_reg_base_addr = 0x%x, burst_current_size\n", icp_hd_start_addr, icp_hd_reg_base_addr);
				SKYCDBG("BURST_WORD_11~~ burst_current_size= 0x%x, burst_current_pos = 0x%x, burst_reg_size = 0x%x\n", burst_current_size, burst_current_pos, burst_reg_size);
#else
				SKYCDBG("BURST_1 ADDR~~ icp_hd_start_addr= 0x%x, icp_hd_reg_base_addr = 0x%x, burst_current_size = %d\n", icp_hd_start_addr, icp_hd_reg_base_addr, burst_current_size);
				SKYCDBG("BURST_1 DATA~~\n");
				SKYCDBG("%2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\n", *icp_hd_reg_base_addr, *(icp_hd_reg_base_addr+1), *(icp_hd_reg_base_addr+2),*(icp_hd_reg_base_addr+3),*(icp_hd_reg_base_addr+4),*(icp_hd_reg_base_addr+5),*(icp_hd_reg_base_addr+6),*(icp_hd_reg_base_addr+7),*(icp_hd_reg_base_addr+8),*(icp_hd_reg_base_addr+9),*(icp_hd_reg_base_addr+10),*(icp_hd_reg_base_addr+11),*(icp_hd_reg_base_addr+12),*(icp_hd_reg_base_addr+13));
				SKYCDBG("%2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\n", *(icp_hd_reg_base_addr+14), *(icp_hd_reg_base_addr+15), *(icp_hd_reg_base_addr+16),*(icp_hd_reg_base_addr+17),*(icp_hd_reg_base_addr+18),*(icp_hd_reg_base_addr+19),*(icp_hd_reg_base_addr+20),*(icp_hd_reg_base_addr+21),*(icp_hd_reg_base_addr+22),*(icp_hd_reg_base_addr+23),*(icp_hd_reg_base_addr+24),*(icp_hd_reg_base_addr+25),*(icp_hd_reg_base_addr+26),*(icp_hd_reg_base_addr+27),*(icp_hd_reg_base_addr+28),*(icp_hd_reg_base_addr+29));
				SKYCDBG("%2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\n", *(icp_hd_reg_base_addr+30), *(icp_hd_reg_base_addr+31), *(icp_hd_reg_base_addr+32),*(icp_hd_reg_base_addr+33),*(icp_hd_reg_base_addr+34),*(icp_hd_reg_base_addr+35),*(icp_hd_reg_base_addr+36),*(icp_hd_reg_base_addr+37),*(icp_hd_reg_base_addr+38),*(icp_hd_reg_base_addr+39),*(icp_hd_reg_base_addr+40),*(icp_hd_reg_base_addr+41),*(icp_hd_reg_base_addr+42),*(icp_hd_reg_base_addr+43),*(icp_hd_reg_base_addr+44),*(icp_hd_reg_base_addr+45));
				SKYCDBG("%2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\n", *(icp_hd_reg_base_addr+46), *(icp_hd_reg_base_addr+47), *(icp_hd_reg_base_addr+48),*(icp_hd_reg_base_addr+49),*(icp_hd_reg_base_addr+50),*(icp_hd_reg_base_addr+51),*(icp_hd_reg_base_addr+52),*(icp_hd_reg_base_addr+53),*(icp_hd_reg_base_addr+54),*(icp_hd_reg_base_addr+55),*(icp_hd_reg_base_addr+56),*(icp_hd_reg_base_addr+57),*(icp_hd_reg_base_addr+58),*(icp_hd_reg_base_addr+59),*(icp_hd_reg_base_addr+60),*(icp_hd_reg_base_addr+61));				
#endif				
#endif				
				rc = icp_hd_i2c_write_dw(0xF03C, icp_hd_start_addr);
				rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_reg_base_addr, burst_current_size);			
				if (rc < 0)
				{
					SKYCERR("icp_hd_i2c_bust_1 failed!\n");
					break;
				}
				burst_current_pos += burst_current_size;
				icp_hd_start_addr = icp_hd_start_addr + burst_current_size - 2;
				icp_hd_reg_base_addr += burst_current_size;				
			}
			
			SKYCDBG("BURST_WORD_1 END!!! icp_hd_start_addr= 0x%x, icp_hd_reg_base_addr = 0x%x, burst_reg_size = 0x%x, burst_current_pos = 0x%x\n", 
				icp_hd_start_addr, icp_hd_reg_base_addr, burst_reg_size, burst_current_pos);
			rc = icp_hd_i2c_write_w(0x601A, 0x0002);
			reg_conf_tbl++;
			//msleep(50);
			
			break;
		case BURST_WORD_2:
			burst_current_pos = 0;
 			burst_current_size = 0;
			burst_reg_size = icp_hd_regs.init_burst_settings_size_2;
			icp_hd_reg_base_addr = icp_hd_regs.init_burst_settings_2;
			icp_hd_start_addr = 0x0000DC40;
							
			while(burst_current_pos < burst_reg_size){
				burst_current_size = ((burst_reg_size - burst_current_pos)>=62 )? 62 : (burst_reg_size - burst_current_pos);
				rc = icp_hd_i2c_write_dw(0xF03C, icp_hd_start_addr);
				rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_reg_base_addr, burst_current_size);			
				if (rc < 0)
				{
					SKYCERR("icp_hd_i2c_bust_2 failed!\n");
					break;
				}

				burst_current_pos += burst_current_size;
				icp_hd_start_addr = icp_hd_start_addr + burst_current_size - 2;
				icp_hd_reg_base_addr += burst_current_size;				
			}
			SKYCDBG("BURST_WORD_2 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_2 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_2);
			rc = icp_hd_i2c_write_w(0x601A, 0x0010);			
			reg_conf_tbl++;
			//msleep(50);
			
			break;
		case BURST_WORD_3:
			burst_current_pos = 0;
 			burst_current_size = 0;
			burst_reg_size = icp_hd_regs.init_burst_settings_size_3;
			icp_hd_reg_base_addr = icp_hd_regs.init_burst_settings_3;
			icp_hd_start_addr = 0x00006BAC;
							
			while(burst_current_pos < burst_reg_size){
				burst_current_size = ((burst_reg_size - burst_current_pos)>=62 )? 62 : (burst_reg_size - burst_current_pos);
				rc = icp_hd_i2c_write_dw(0xF03C, icp_hd_start_addr);
				rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_reg_base_addr, burst_current_size);			
				if (rc < 0)
				{
					SKYCERR("icp_hd_i2c_bust_3 failed!\n");
					break;
				}

				burst_current_pos += burst_current_size;
				icp_hd_start_addr = icp_hd_start_addr + burst_current_size - 2;
				icp_hd_reg_base_addr += burst_current_size;				
			}
			SKYCDBG("BURST_WORD_3 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_3 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_3);
			rc = icp_hd_i2c_write_w(0x601A, 0x0002);
			reg_conf_tbl++;
			//msleep(50);
			
			break;
		case BURST_WORD_4:
			burst_current_pos = 0;
 			burst_current_size = 0;
			burst_reg_size = icp_hd_regs.init_burst_settings_size_4;
			icp_hd_reg_base_addr = icp_hd_regs.init_burst_settings_4;
			icp_hd_start_addr = 0x0000E0DC;
							
			while(burst_current_pos < burst_reg_size){
				burst_current_size = ((burst_reg_size - burst_current_pos)>=62 )? 62 : (burst_reg_size - burst_current_pos);
				rc = icp_hd_i2c_write_dw(0xF03C, icp_hd_start_addr);
				rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_reg_base_addr, burst_current_size);			
				if (rc < 0)
				{
					SKYCERR("icp_hd_i2c_bust_4 failed!\n");
					break;
				}

				burst_current_pos += burst_current_size;
				icp_hd_start_addr = icp_hd_start_addr + burst_current_size - 2;
				icp_hd_reg_base_addr += burst_current_size;				
			}
			SKYCDBG("BURST_WORD_4 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_4 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_4);			
			reg_conf_tbl++;			
			
			break;
		case BURST_WORD_5:
			burst_current_pos = 0;
 			burst_current_size = 0;
			burst_reg_size = icp_hd_regs.init_burst_settings_size_5;
			icp_hd_reg_base_addr = icp_hd_regs.init_burst_settings_5;
			icp_hd_start_addr = 0x0000E574;
							
			while(burst_current_pos < burst_reg_size){
				burst_current_size = ((burst_reg_size - burst_current_pos)>=62 )? 62 : (burst_reg_size - burst_current_pos);
				rc = icp_hd_i2c_write_dw(0xF03C, icp_hd_start_addr);
				rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_reg_base_addr, burst_current_size);			
				if (rc < 0)
				{
					SKYCERR("icp_hd_i2c_bust_5 failed!\n");
					break;
				}

				burst_current_pos += burst_current_size;
				icp_hd_start_addr = icp_hd_start_addr + burst_current_size - 2;
				icp_hd_reg_base_addr += burst_current_size;				
			}
			SKYCDBG("BURST_WORD_5 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_5 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_5);			
			reg_conf_tbl++;			
			
			break;
#elif defined(BURST_MODE_64_TO_4096_BYTE)
		case BURST_WORD_1:
			burst_current_pos = 0;
 			burst_current_size = 0;
			burst_reg_size = icp_hd_regs.init_burst_settings_size_1;
			icp_hd_reg_base_addr = icp_hd_regs.init_burst_settings_1;
			icp_hd_start_addr = 0x0000644C;		

			icp_hd_burst_data[0] = 0xE0;
			icp_hd_burst_data[1] = 0x00;			
			SKYCDBG("BURST_WORD_1 START~~~ icp_hd_start_addr= 0x%x, icp_hd_reg_base_addr = 0x%x, burst_reg_size = 0x%x\n", icp_hd_start_addr, icp_hd_reg_base_addr, burst_reg_size);
							
			while(burst_current_pos < burst_reg_size){
				burst_current_size = ((burst_reg_size - burst_current_pos)>=BURST_DATA_LEN )? BURST_DATA_LEN : (burst_reg_size - burst_current_pos);
				SKYCDBG("MEMCPY~ START~~~ &icp_hd_reg_base_addr[burst_current_pos]= 0x%x, burst_current_size = 0x%x\n", &icp_hd_reg_base_addr[burst_current_pos] , burst_current_size);				
				memcpy(&icp_hd_burst_data[2], &icp_hd_reg_base_addr[burst_current_pos] , burst_current_size);
				SKYCDBG("MEMCPY~ END~~~ &icp_hd_reg_base_addr[burst_current_pos]= 0x%x, burst_current_size = 0x%x\n", &icp_hd_reg_base_addr[burst_current_pos] , burst_current_size);
#ifdef DEBUG_I2C_VALUE		
#if 0
				SKYCDBG("BURST_WORD_10~~ icp_hd_start_addr= 0x%x, icp_hd_reg_base_addr = 0x%x, burst_current_size\n", icp_hd_start_addr, icp_hd_reg_base_addr);
				SKYCDBG("BURST_WORD_11~~ burst_current_size= 0x%x, burst_current_pos = 0x%x, burst_reg_size = 0x%x\n", burst_current_size, burst_current_pos, burst_reg_size);
#else
				SKYCDBG("BURST_1 ADDR~~ icp_hd_start_addr= 0x%x, icp_hd_reg_base_addr = 0x%x, burst_current_size = %d\n", icp_hd_start_addr, icp_hd_reg_base_addr, burst_current_size);
				SKYCDBG("BURST_1 DATA~~\n");
				SKYCDBG("%2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\n", *icp_hd_burst_data, *(icp_hd_burst_data+1), *(icp_hd_burst_data+2),*(icp_hd_burst_data+3),*(icp_hd_burst_data+4),*(icp_hd_burst_data+5),*(icp_hd_burst_data+6),*(icp_hd_burst_data+7),*(icp_hd_burst_data+8),*(icp_hd_burst_data+9),*(icp_hd_burst_data+10),*(icp_hd_burst_data+11),*(icp_hd_burst_data+12),*(icp_hd_burst_data+13));
				SKYCDBG("%2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\n", *(icp_hd_burst_data+14), *(icp_hd_burst_data+15), *(icp_hd_burst_data+16),*(icp_hd_burst_data+17),*(icp_hd_burst_data+18),*(icp_hd_burst_data+19),*(icp_hd_burst_data+20),*(icp_hd_burst_data+21),*(icp_hd_burst_data+22),*(icp_hd_burst_data+23),*(icp_hd_burst_data+24),*(icp_hd_burst_data+25),*(icp_hd_burst_data+26),*(icp_hd_burst_data+27),*(icp_hd_burst_data+28),*(icp_hd_burst_data+29));
				SKYCDBG("%2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\n", *(icp_hd_burst_data+30), *(icp_hd_burst_data+31), *(icp_hd_burst_data+32),*(icp_hd_burst_data+33),*(icp_hd_burst_data+34),*(icp_hd_burst_data+35),*(icp_hd_burst_data+36),*(icp_hd_burst_data+37),*(icp_hd_burst_data+38),*(icp_hd_burst_data+39),*(icp_hd_burst_data+40),*(icp_hd_burst_data+41),*(icp_hd_burst_data+42),*(icp_hd_burst_data+43),*(icp_hd_burst_data+44),*(icp_hd_burst_data+45));
				SKYCDBG("%2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\n", *(icp_hd_burst_data+46), *(icp_hd_burst_data+47), *(icp_hd_burst_data+48),*(icp_hd_burst_data+49),*(icp_hd_burst_data+50),*(icp_hd_burst_data+51),*(icp_hd_burst_data+52),*(icp_hd_burst_data+53),*(icp_hd_burst_data+54),*(icp_hd_burst_data+55),*(icp_hd_burst_data+56),*(icp_hd_burst_data+57),*(icp_hd_burst_data+58),*(icp_hd_burst_data+59),*(icp_hd_burst_data+60),*(icp_hd_burst_data+61));				
#endif				
#endif							
				rc = icp_hd_i2c_write_dw(0xF03C, icp_hd_start_addr);
				rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_burst_data, burst_current_size+2);			
				if (rc < 0)
				{
					SKYCERR("icp_hd_i2c_burst_1 failed!\n");
					break;
				}
				burst_current_pos += burst_current_size;
				icp_hd_start_addr += burst_current_size;							
			}
			
			SKYCDBG("BURST_WORD_1 END!!! icp_hd_start_addr= 0x%x, icp_hd_reg_base_addr = 0x%x, burst_reg_size = 0x%x, burst_current_pos = 0x%x\n", 
				icp_hd_start_addr, icp_hd_reg_base_addr, burst_reg_size, burst_current_pos);
			rc = icp_hd_i2c_write_w(0x601A, 0x0002);
			reg_conf_tbl++;
			//msleep(50);
			
			break;
		case BURST_WORD_2:
			burst_current_pos = 0;
 			burst_current_size = 0;
			burst_reg_size = icp_hd_regs.init_burst_settings_size_2;
			icp_hd_reg_base_addr = icp_hd_regs.init_burst_settings_2;
			icp_hd_start_addr = 0x0000DC40;

			icp_hd_burst_data[0] = 0xE0;
			icp_hd_burst_data[1] = 0x00;
			while(burst_current_pos < burst_reg_size){
				burst_current_size = ((burst_reg_size - burst_current_pos)>=BURST_DATA_LEN )? BURST_DATA_LEN : (burst_reg_size - burst_current_pos);
				memcpy(&icp_hd_burst_data[2], &icp_hd_reg_base_addr[burst_current_pos] , burst_current_size);
				rc = icp_hd_i2c_write_dw(0xF03C, icp_hd_start_addr);
				rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_burst_data, burst_current_size+2);			
				if (rc < 0)
				{
					SKYCERR("icp_hd_i2c_burst_2 failed!\n");
					break;
				}
				burst_current_pos += burst_current_size;
				icp_hd_start_addr += burst_current_size;		
			}
			SKYCDBG("BURST_WORD_2 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_2 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_2);
			rc = icp_hd_i2c_write_w(0x601A, 0x0010);			
			reg_conf_tbl++;
			//msleep(50);
			
			break;
		case BURST_WORD_3:
			burst_current_pos = 0;
 			burst_current_size = 0;
			burst_reg_size = icp_hd_regs.init_burst_settings_size_3;
			icp_hd_reg_base_addr = icp_hd_regs.init_burst_settings_3;
			icp_hd_start_addr = 0x00006BAC;

			icp_hd_burst_data[0] = 0xE0;
			icp_hd_burst_data[1] = 0x00;
			while(burst_current_pos < burst_reg_size){
				burst_current_size = ((burst_reg_size - burst_current_pos)>=BURST_DATA_LEN )? BURST_DATA_LEN : (burst_reg_size - burst_current_pos);
				memcpy(&icp_hd_burst_data[2], &icp_hd_reg_base_addr[burst_current_pos] , burst_current_size);
				rc = icp_hd_i2c_write_dw(0xF03C, icp_hd_start_addr);
				rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_burst_data, burst_current_size+2);			
				if (rc < 0)
				{
					SKYCERR("icp_hd_i2c_burst_3 failed!\n");
					break;
				}
				burst_current_pos += burst_current_size;
				icp_hd_start_addr += burst_current_size;			
			}
			SKYCDBG("BURST_WORD_3 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_3 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_3);
			rc = icp_hd_i2c_write_w(0x601A, 0x0002);
			reg_conf_tbl++;
			//msleep(50);
			
			break;
		case BURST_WORD_4:
			burst_current_pos = 0;
 			burst_current_size = 0;
			burst_reg_size = icp_hd_regs.init_burst_settings_size_4;
			icp_hd_reg_base_addr = icp_hd_regs.init_burst_settings_4;
			icp_hd_start_addr = 0x0000E0DC;

			icp_hd_burst_data[0] = 0xE0;
			icp_hd_burst_data[1] = 0x00;							
			while(burst_current_pos < burst_reg_size){
				burst_current_size = ((burst_reg_size - burst_current_pos)>=BURST_DATA_LEN )? BURST_DATA_LEN : (burst_reg_size - burst_current_pos);
				memcpy(&icp_hd_burst_data[2], &icp_hd_reg_base_addr[burst_current_pos] , burst_current_size);
				rc = icp_hd_i2c_write_dw(0xF03C, icp_hd_start_addr);
				rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_burst_data, burst_current_size+2);			
				if (rc < 0)
				{
					SKYCERR("icp_hd_i2c_burst_4 failed!\n");
					break;
				}
				burst_current_pos += burst_current_size;
				icp_hd_start_addr += burst_current_size;			
			}
			SKYCDBG("BURST_WORD_4 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_4 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_4);			
			reg_conf_tbl++;			
			
			break;
		case BURST_WORD_5:
			burst_current_pos = 0;
 			burst_current_size = 0;
			burst_reg_size = icp_hd_regs.init_burst_settings_size_5;
			icp_hd_reg_base_addr = icp_hd_regs.init_burst_settings_5;
			icp_hd_start_addr = 0x0000E574;
							
			icp_hd_burst_data[0] = 0xE0;
			icp_hd_burst_data[1] = 0x00;
			while(burst_current_pos < burst_reg_size){
				burst_current_size = ((burst_reg_size - burst_current_pos)>=BURST_DATA_LEN )? BURST_DATA_LEN : (burst_reg_size - burst_current_pos);
				memcpy(&icp_hd_burst_data[2], &icp_hd_reg_base_addr[burst_current_pos] , burst_current_size);
				rc = icp_hd_i2c_write_dw(0xF03C, icp_hd_start_addr);
				rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_burst_data, burst_current_size+2);			
				if (rc < 0)
				{
					SKYCERR("icp_hd_i2c_burst_5 failed!\n");
					break;
				}
				burst_current_pos += burst_current_size;
				icp_hd_start_addr += burst_current_size;				
			}
			SKYCDBG("BURST_WORD_5 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_5 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_5);			
			reg_conf_tbl++;			
			
			break;
#elif defined(BURST_MODE_4096_BYTE_CAPTURE_12FPS)
		case BURST_WORD_1:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x0000644C);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_1, icp_hd_regs.init_burst_settings_size_1);			
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_1 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_1 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_1 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_1);
			rc = icp_hd_i2c_write_w(0x601A, 0x0002);
			reg_conf_tbl++;	
			break;
		case BURST_WORD_2:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x0000E940);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_2, icp_hd_regs.init_burst_settings_size_2);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_2 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_2 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_2 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_2);
			rc = icp_hd_i2c_write_w(0x601A, 0x0010);			
			reg_conf_tbl++;			
			break;
		case BURST_WORD_3:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x00006B38);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_3, icp_hd_regs.init_burst_settings_size_3);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_3 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_3 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_3 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_3);
			reg_conf_tbl++;
			break;
		case BURST_WORD_4:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x00007B36);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_4, icp_hd_regs.init_burst_settings_size_4);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_4 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_4 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_4 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_4);
			reg_conf_tbl++;
			break;
		case BURST_WORD_5:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x00008B34);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_5, icp_hd_regs.init_burst_settings_size_5);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_5 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_5 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_5 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_5);
			reg_conf_tbl++;
			break;
		case BURST_WORD_6:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x00009B32);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_6, icp_hd_regs.init_burst_settings_size_6);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_6 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_6 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_6 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_6);
			reg_conf_tbl++;
			break;
		case BURST_WORD_7:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x0000AB30);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_7, icp_hd_regs.init_burst_settings_size_7);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_7 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_7 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_7 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_7);
			reg_conf_tbl++;
			break;
		case BURST_WORD_8:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x0000BB2E);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_8, icp_hd_regs.init_burst_settings_size_8);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_8 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_8 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_8 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_8);
			reg_conf_tbl++;
			break;
		case BURST_WORD_9:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x0000CB2C);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_9, icp_hd_regs.init_burst_settings_size_9);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_9 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_9 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_9 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_9);
			reg_conf_tbl++;
			break;
		case BURST_WORD_10:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x0000DB2A);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_10, icp_hd_regs.init_burst_settings_size_10);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_10 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_10 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_10 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_10);
			rc = icp_hd_i2c_write_w(0x601A, 0x0002);
			reg_conf_tbl++;
			break;
		case BURST_WORD_11:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x0000EE54);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_11, icp_hd_regs.init_burst_settings_size_11);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_11 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_11 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_11 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_11);
			reg_conf_tbl++;
			break;
		case BURST_WORD_12:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x0000F2EC);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_12, icp_hd_regs.init_burst_settings_size_12);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_12 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_12 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_12 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_12);
			reg_conf_tbl++;
			break;
#elif defined(BURST_MODE_4096_BYTE_CAPTURE_10FPS)
		case BURST_WORD_1:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x0000644C);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_1, icp_hd_regs.init_burst_settings_size_1);			
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_1 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_1 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_1 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_1);
			rc = icp_hd_i2c_write_w(0x601A, 0x0002);
			reg_conf_tbl++;	
			break;
		case BURST_WORD_2:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x0000ECC8);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_2, icp_hd_regs.init_burst_settings_size_2);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_2 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_2 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_2 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_2);
			rc = icp_hd_i2c_write_w(0x601A, 0x0010);			
			reg_conf_tbl++;			
			break;
		case BURST_WORD_3:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x00006B6C);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_3, icp_hd_regs.init_burst_settings_size_3);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_3 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_3 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_3 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_3);
			reg_conf_tbl++;
			break;
		case BURST_WORD_4:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x00007B6A);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_4, icp_hd_regs.init_burst_settings_size_4);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_4 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_4 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_4 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_4);
			reg_conf_tbl++;
			break;
		case BURST_WORD_5:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x00008B68);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_5, icp_hd_regs.init_burst_settings_size_5);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_5 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_5 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_5 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_5);
			reg_conf_tbl++;
			break;
		case BURST_WORD_6:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x00009B66);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_6, icp_hd_regs.init_burst_settings_size_6);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_6 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_6 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_6 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_6);
			reg_conf_tbl++;
			break;
		case BURST_WORD_7:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x0000AB64);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_7, icp_hd_regs.init_burst_settings_size_7);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_7 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_7 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_7 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_7);
			reg_conf_tbl++;
			break;
		case BURST_WORD_8:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x0000BB62);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_8, icp_hd_regs.init_burst_settings_size_8);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_8 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_8 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_8 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_8);
			reg_conf_tbl++;
			break;
		case BURST_WORD_9:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x0000CB60);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_9, icp_hd_regs.init_burst_settings_size_9);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_9 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_9 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_9 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_9);
			reg_conf_tbl++;
			break;
		case BURST_WORD_10:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x0000DB5E);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_10, icp_hd_regs.init_burst_settings_size_10);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_10 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_10 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_10 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_10);			
			reg_conf_tbl++;
			break;
		case BURST_WORD_11:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x0000EB5C);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_11, icp_hd_regs.init_burst_settings_size_11);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_11 failed!\n");
				break;
			}			
			SKYCDBG("BURST_WORD_11 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_11 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_11);
			rc = icp_hd_i2c_write_w(0x601A, 0x0002);
			reg_conf_tbl++;
			break;
		case BURST_WORD_12:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x0000F1DC);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_12, icp_hd_regs.init_burst_settings_size_12);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_12 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_12 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_12 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_12);
			reg_conf_tbl++;
			break;
		case BURST_WORD_13:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x0000F674);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_13, icp_hd_regs.init_burst_settings_size_13);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_13 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_13 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_13 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_13);
			reg_conf_tbl++;
			break;
#elif defined(BURST_MODE_4096_BYTE_CAPTURE_11FPS)
		case BURST_WORD_1:			
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_1, icp_hd_regs.init_burst_settings_size_1);			
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_1 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_1 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_1 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_1);
			reg_conf_tbl++;	
			break;
		case BURST_WORD_2:			
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_2, icp_hd_regs.init_burst_settings_size_2);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_2 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_2 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_2 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_2);
			rc = icp_hd_i2c_write_dw(0x9FFC, 0x19BE7D0E);
			reg_conf_tbl++;			
			break;
		case BURST_WORD_3:			
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_3, icp_hd_regs.init_burst_settings_size_3);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_3 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_3 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_3 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_3);
			reg_conf_tbl++;
			break;
		case BURST_WORD_4:			
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_4, icp_hd_regs.init_burst_settings_size_4);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_4 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_4 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_4 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_4);
			rc = icp_hd_i2c_write_dw(0x9FFC, 0x5F0B0F54);
			reg_conf_tbl++;
			break;
		case BURST_WORD_5:
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_5, icp_hd_regs.init_burst_settings_size_5);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_5 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_5 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_5 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_5);
			reg_conf_tbl++;
			break;
		case BURST_WORD_6:
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_6, icp_hd_regs.init_burst_settings_size_6);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_6 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_6 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_6 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_6);
			rc = icp_hd_i2c_write_dw(0x9FFC, 0x35B13B78);
			reg_conf_tbl++;
			break;
		case BURST_WORD_7:
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_7, icp_hd_regs.init_burst_settings_size_7);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_7 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_7 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_7 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_7);
			rc = icp_hd_i2c_write_w(0x6002, 0xFFFF);
			rc = icp_hd_i2c_write_dw(0x2028, 0x000186A0);
			reg_conf_tbl++;
			break;		
#elif defined(BURST_MODE_8192_BYTE_CAPTURE_11FPS)		
			case BURST_WORD_1:			
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_1, icp_hd_regs.init_burst_settings_size_1);			
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_1 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_1 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_1 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_1);
			rc = icp_hd_i2c_write_w(0x9FFE, 0x7D0E);
			reg_conf_tbl++;	
			break;
		case BURST_WORD_2:			
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_2, icp_hd_regs.init_burst_settings_size_2);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_2 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_2 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_2 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_2);
			rc = icp_hd_i2c_write_w(0x9FFE, 0x0F54);
			reg_conf_tbl++;			
			break;
		case BURST_WORD_3:			
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_3, icp_hd_regs.init_burst_settings_size_3);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_3 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_3 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_3 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_3);
			icp_hd_i2c_write_w(0x9FFE, 0x3B78);
			reg_conf_tbl++;
			break;
		case BURST_WORD_4:			
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_4, icp_hd_regs.init_burst_settings_size_4);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_4 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_4 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_4 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_4);
			rc = icp_hd_i2c_write_w(0x6002, 0xFFFF);
			reg_conf_tbl++;
			break;
#elif defined(BURST_MODE_8192_BYTE_COMPRESS_INIT)
		case BURST_WORD_1:
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_1, icp_hd_regs.init_burst_settings_size_1);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_1 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_1 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_1 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_1);
			reg_conf_tbl++;
			break;
		case BURST_WORD_2:
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_2, icp_hd_regs.init_burst_settings_size_2);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_2 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_2 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_2 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_2);
			reg_conf_tbl++;
			break;
		case BURST_WORD_3:
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_3, icp_hd_regs.init_burst_settings_size_3);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_3 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_3 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_3 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_3);
			reg_conf_tbl++;
			break;
		case BURST_WORD_4:
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_4, icp_hd_regs.init_burst_settings_size_4);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_4 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_4 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_4 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_4);
			reg_conf_tbl++;
			break;
		case BURST_WORD_5:
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_5, icp_hd_regs.init_burst_settings_size_5);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_5 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_5 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_5 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_5);
			reg_conf_tbl++;
			break;
		case BURST_WORD_6:			
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_6, icp_hd_regs.init_burst_settings_size_6);			
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_6 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_6 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_6 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_6);
			rc = icp_hd_i2c_write_w(0x9FFE, 0x4220);
			reg_conf_tbl++;	
			break;
		case BURST_WORD_7:			
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_7, icp_hd_regs.init_burst_settings_size_7);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_7 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_7 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_7 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_7);
			rc = icp_hd_i2c_write_w(0x9FFE, 0x62B8);
			reg_conf_tbl++;			
			break;
		case BURST_WORD_8:			
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_8, icp_hd_regs.init_burst_settings_size_8);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_8 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_8 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_8 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_8);
			rc = icp_hd_i2c_write_w(0x6002, 0xFFFF);
			rc = icp_hd_i2c_write_dw(0x2028, 0x000186A0);
			reg_conf_tbl++;
			break;
#elif defined(BURST_MODE_8192_BYTE_COMPRESS_INIT_OTP)
		case BURST_WORD_1:
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_1, icp_hd_regs.init_burst_settings_size_1);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_1 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_1 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_1 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_1);
			rc = icp_hd_i2c_write_w(0x9FFE, 0xF391);
			reg_conf_tbl++;
			break;
		case BURST_WORD_2:			
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_2, icp_hd_regs.init_burst_settings_size_2);			
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_2 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_2 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_2 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_2);
			rc = icp_hd_i2c_write_w(0x9FFE, 0x0004);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_3, icp_hd_regs.init_burst_settings_size_3);
			rc = icp_hd_i2c_write_w(0x6002, 0xFFFF);
			rc = icp_hd_i2c_write_dw(0x2028, 0x000186A0);
			reg_conf_tbl++;
			break;
#elif defined(BURST_MODE_8192_BYTE_COMPRESS_INIT_OTP_TUNE_UPDATE)
        case BURST_WORD_1:
            rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_1, icp_hd_regs.init_burst_settings_size_1);
            if (rc < 0)
            {
                SKYCERR("icp_hd_i2c_bust_1 failed!\n");
                break;
            }
            SKYCDBG("BURST_WORD_1 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_1 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_1);
            rc = icp_hd_i2c_write_w(0x9FFE, 0x00A0);
            reg_conf_tbl++;
            break;
        case BURST_WORD_2:          
            rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_2, icp_hd_regs.init_burst_settings_size_2);         
            if (rc < 0)
            {
                SKYCERR("icp_hd_i2c_bust_2 failed!\n");
                break;
            }
            SKYCDBG("BURST_WORD_2 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_2 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_2);
            rc = icp_hd_i2c_write_w(0x9FFE, 0x0808);
            rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_3, icp_hd_regs.init_burst_settings_size_3);
            rc = icp_hd_i2c_write_w(0x6002, 0xFFFF);
//            rc = icp_hd_i2c_write_dw(0x2028, 0x000186A0);
            reg_conf_tbl++;
            break;
#elif defined(BURST_MODE_8192_BYTE_COMPRESS_INIT_NODATA)
	  	case BURST_WORD_1:
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_1, icp_hd_regs.init_burst_settings_size_1);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_1 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_1 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_1 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_1);
			rc = icp_hd_i2c_write_w(0x9FFE, 0x4240);
			reg_conf_tbl++;
			break;
		case BURST_WORD_2:			
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_2, icp_hd_regs.init_burst_settings_size_2);			
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_2 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_2 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_2 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_2);
			rc = icp_hd_i2c_write_w(0x9FFE, 0x4021);
			reg_conf_tbl++;
			break;
		case BURST_WORD_3:
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_3, icp_hd_regs.init_burst_settings_size_3);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_3 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_3 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_3 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_3);
			rc = icp_hd_i2c_write_w(0x6002, 0xFFFF);
			reg_conf_tbl++;
			break;    	
#elif defined(BURST_MODE_8192_BYTE_COMPRESS_INIT_OTP_TUNE_UPDATE_2)
			case BURST_WORD_1:
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_1, icp_hd_regs.init_burst_settings_size_1);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_1 failed!\n");
				break;
			}
			SKYCDBG("OTP BURST_WORD_1 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_1 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_1);
			rc = icp_hd_i2c_write_w(0x9FFE, 0x4048);
			reg_conf_tbl++;
			break;
			case BURST_WORD_2:			
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_2, icp_hd_regs.init_burst_settings_size_2);			
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_2 failed!\n");
				break;
			}
			SKYCDBG("OTP BURST_WORD_2 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_2 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_2);
			rc = icp_hd_i2c_write_w(0x9FFE, 0x8181);
			reg_conf_tbl++;
			break;
		case BURST_WORD_3:
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_3, icp_hd_regs.init_burst_settings_size_3);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_3 failed!\n");
				break;
			}
			SKYCDBG("OTP BURST_WORD_3 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_3 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_3);
			rc = icp_hd_i2c_write_w(0x6002, 0xFFFF);
			reg_conf_tbl++;
			break;
#elif defined(BURST_MODE_8192_BYTE_COMPRESS_INIT_OTP_TUNE_UPDATE_3)
		case BURST_WORD_1:
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_1, icp_hd_regs.init_burst_settings_size_1);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_1 failed!\n");
				break;
			}
			SKYCDBG("OTP BURST_WORD_1 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_1 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_1);
			rc = icp_hd_i2c_write_w(0x9FFE, 0x5EDF);
			reg_conf_tbl++;
			break;
			case BURST_WORD_2:			
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_2, icp_hd_regs.init_burst_settings_size_2);			
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_2 failed!\n");
				break;
			}
			SKYCDBG("OTP BURST_WORD_2 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_2 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_2);
			rc = icp_hd_i2c_write_w(0x9FFE, 0x0073);
			reg_conf_tbl++;
			break;
		case BURST_WORD_3:
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_3, icp_hd_regs.init_burst_settings_size_3);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_3 failed!\n");
				break;
			}
			SKYCDBG("OTP BURST_WORD_3 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_3 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_3);
			rc = icp_hd_i2c_write_w(0x6002, 0xFFFF);
			reg_conf_tbl++;
			break;
#elif defined(BURST_MODE_4096_BYTE)
		case BURST_WORD_1:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x0000644C);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_1, icp_hd_regs.init_burst_settings_size_1);			
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_1 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_1 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_1 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_1);
			rc = icp_hd_i2c_write_w(0x601A, 0x0002);
			reg_conf_tbl++;
			msleep(50);
			
			break;
		case BURST_WORD_2:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x0000644C);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_2, icp_hd_regs.init_burst_settings_size_2);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_2 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_2 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_2 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_2);
			rc = icp_hd_i2c_write_w(0x601A, 0x0010);			
			reg_conf_tbl++;
			msleep(50);
			
			break;
		case BURST_WORD_3:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x00000000);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_3, icp_hd_regs.init_burst_settings_size_3);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_3 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_3 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_3 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_3);
			reg_conf_tbl++;
			break;
		case BURST_WORD_4:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x00000FFC);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_4, icp_hd_regs.init_burst_settings_size_4);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_4 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_4 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_4 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_4);
			reg_conf_tbl++;
			break;
		case BURST_WORD_5:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x00001FF8);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_5, icp_hd_regs.init_burst_settings_size_5);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_5 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_5 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_5 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_5);
			reg_conf_tbl++;
			break;
		case BURST_WORD_6:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x00002FF4);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_6, icp_hd_regs.init_burst_settings_size_6);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_6 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_6 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_6 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_6);
			reg_conf_tbl++;
			break;
		case BURST_WORD_7:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x00003FF0);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_7, icp_hd_regs.init_burst_settings_size_7);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_7 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_7 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_7 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_7);
			reg_conf_tbl++;
			break;
		case BURST_WORD_8:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x00004FEC);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_8, icp_hd_regs.init_burst_settings_size_8);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_8 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_8 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_8 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_8);
			reg_conf_tbl++;
			break;
		case BURST_WORD_9:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x00005FE8);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_9, icp_hd_regs.init_burst_settings_size_9);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_9 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_9 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_9 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_9);
			reg_conf_tbl++;
			break;
		case BURST_WORD_10:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x00006FE4);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_10, icp_hd_regs.init_burst_settings_size_10);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_10 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_10 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_10 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_10);
			rc = icp_hd_i2c_write_w(0x601A, 0x0002);
			reg_conf_tbl++;
			msleep(50);
			break;
		case BURST_WORD_11:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x000068E8);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_11, icp_hd_regs.init_burst_settings_size_11);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_11 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_11 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_11 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_11);
			reg_conf_tbl++;
			break;
		case BURST_WORD_12:
			rc = icp_hd_i2c_write_dw(0xF03C, 0x00006D80);
			rc = icp_hd_i2c_txdata(icp_hd_client->addr, icp_hd_regs.init_burst_settings_12, icp_hd_regs.init_burst_settings_size_12);
			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_bust_12 failed!\n");
				break;
			}
			SKYCDBG("BURST_WORD_12 SUCCESS!!! next_dwdata= 0x%x, icp_hd_regs.init_burst_settings_size_12 = %d\n", (reg_conf_tbl+1)->dwdata, icp_hd_regs.init_burst_settings_size_12);
			reg_conf_tbl++;
			break;		
#endif			
		default:
		{
			rc = icp_hd_i2c_write(icp_hd_client->addr,
								reg_conf_tbl->waddr, reg_conf_tbl->dwdata,
								reg_conf_tbl->width);
			//SKYCDBG("I2C WRITE!!! ADDR = 0x%x, VALUE = 0x%x, width = %d, num_of_items_in_table=%d, i=%d\n",
				//reg_conf_tbl->waddr, reg_conf_tbl->wdata, reg_conf_tbl->width, num_of_items_in_table, i);

			if (rc < 0)
			{
				SKYCERR("icp_hd_i2c_write failed!\n");
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
#else
	int i;
	int32_t rc = -EIO;

	for (i = 0; i < num_of_items_in_table; i++) {
		rc = icp_hd_i2c_write(icp_hd_client->addr,
			reg_conf_tbl->waddr, reg_conf_tbl->wdata,
			reg_conf_tbl->width);
		if (rc < 0)
			break;
		if (reg_conf_tbl->mdelay_time != 0)
			msleep(reg_conf_tbl->mdelay_time);
		reg_conf_tbl++;
	}
#endif
	return rc;
}

static int icp_hd_i2c_rxdata(unsigned short saddr,
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
	
	for (i = 0; i < ICP_HD_I2C_RETRY; i++) {
		rc = i2c_transfer(icp_hd_client->adapter, msgs, 2); 
		if (rc >= 0) {			
			return 0;
		}
		SKYCDBG("%s: tx retry. [%02x.%02x.%02x] len=%d rc=%d\n", __func__,saddr, *rxdata, *(rxdata + 1), length, rc);
		msleep(ICP_HD_I2C_MPERIOD);
	}
	return -EIO;
}

static int32_t icp_hd_i2c_read(unsigned short   saddr,
	unsigned short raddr, unsigned short *rdata, enum icp_hd_width width)
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

		rc = icp_hd_i2c_rxdata(saddr, buf, 2);
		if (rc < 0)
			return rc;

		*rdata = buf[0] << 8 | buf[1];
	}
		break;
	case DWORD_LEN: {
		buf[0] = (raddr & 0xFF00)>>8;
		buf[1] = (raddr & 0x00FF);

		rc = icp_hd_i2c_rxdata(saddr, buf, 4);
		if (rc < 0)
			return rc;

		*rdata = buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];
	}
		break;
	default:
		break;
	}

	if (rc < 0)
		SKYCERR("icp_hd_i2c_read failed!\n");

	return rc;
}

#if 0
static int32_t icp_hd_set_lens_roll_off(void)
{
	int32_t rc = 0;
#if 0
	rc = icp_hd_i2c_write_table(&icp_hd_regs.rftbl[0],
								 icp_hd_regs.rftbl_size);
#endif
	return rc;
}
#endif

static long icp_hd_reg_init(void)
{
	uint16_t revision_num = 0;
	int32_t rc = 0;

	SKYCDBG("%s start : icp_hd_client->addr = 0x%x\n",__func__, icp_hd_client->addr);

	icp_hd_i2c_read(icp_hd_client->addr, 0x0050, &revision_num,WORD_LEN);
	SKYCDBG("SENSOR REVISION NUM~~~ = 0x%x\n", revision_num);
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	// tunning value
	if((icp_hd_tup_state == ICP_HD_TUNE_STATE_LOAD_VALUE) && (icp_hd_params_tbl.init.num_params > 0))	
	{
		icp_hd_i2c_write_params(icp_hd_params_tbl.init.params,
								icp_hd_params_tbl.init.num_params);
	}
	// static value
	else
#endif
	/* PLL Setup Start */
	rc = icp_hd_i2c_write_table(&icp_hd_regs.init_parm[0],
					icp_hd_regs.init_parm_size);
#ifdef ICP_HD_MODULE_ABNORMAL_OPERATION_DEFENCE
	if(rc == ICP_HD_REG_POLLING_ERROR)
	{
		gpio_set_value(SENSOR_RESET,0);
		msleep(5);
		gpio_set_value(SENSOR_RESET,1);
		msleep(5);
		rc = icp_hd_i2c_write_table(&icp_hd_regs.init_parm[0],
					icp_hd_regs.init_parm_size);

		if(rc == ICP_HD_REG_POLLING_ERROR)
		{
			SKYCERR("SECOND POLLING FAIL!!!OUT OF INIT~~~\n");
			return -EIO;
		}
	}
#endif	
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}
	
	return rc;
}

static long icp_hd_set_effect(int mode, int effect)
{
//	uint16_t reg_addr;
//	uint16_t reg_val;
	int rc = 0;

	SKYCDBG("%s start\n",__func__);

	if(effect < CAMERA_EFFECT_OFF || effect >= CAMERA_EFFECT_MAX){
		SKYCERR("%s error. effect=%d\n", __func__, effect);
		return -EINVAL;
	}
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	// tunning value
	if((icp_hd_tup_state == ICP_HD_TUNE_STATE_LOAD_VALUE) && (icp_hd_params_tbl.effect[effect].num_params > 0))	
	{
		icp_hd_i2c_write_params(icp_hd_params_tbl.effect[effect].params,
								icp_hd_params_tbl.effect[effect].num_params);
	}
	// static value
	else
#endif		
	{
		rc = icp_hd_i2c_write_table(icp_hd_regs.effect_cfg_settings[effect],
					icp_hd_regs.effect_cfg_settings_size);
	}
	
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}		

	icp_hd_effect = effect;
	/* Refresh Sequencer */	

	SKYCDBG("%s end\n",__func__);
	
	return rc;
}

#if 1 //def F_PANTECH_CAMERA_FIX_CFG_WB
static int32_t icp_hd_set_whitebalance (int mode, int32_t whitebalance)
{
	
	int32_t rc = 0;
		
	SKYCDBG("%s start  mode=%d, whitebalance=%d\n",__func__, mode, whitebalance);
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	// tunning value
	if((icp_hd_tup_state == ICP_HD_TUNE_STATE_LOAD_VALUE) && (icp_hd_params_tbl.wb[whitebalance-1].num_params > 0))	
	{
		icp_hd_i2c_write_params(icp_hd_params_tbl.wb[whitebalance-1].params,
								icp_hd_params_tbl.wb[whitebalance-1].num_params);
	}
	// static value
	else
#endif		
	{
		rc = icp_hd_i2c_write_table(icp_hd_regs.wb_cfg_settings[whitebalance-1],
						icp_hd_regs.wb_cfg_settings_size);
	}

	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}		

	SKYCDBG("%s end\n",__func__);

	return rc;
}
#endif

#if 1 //def F_PANTECH_CAMERA_FIX_CFG_BRIGHTNESS
static int32_t icp_hd_set_brightness(int32_t brightness)
{
	int32_t rc = 0;
	SKYCDBG("%s start~ receive brightness = %d\n",__func__, brightness);
	
	if ((brightness < 0) || (brightness >= 9)) {
		SKYCERR("%s error. brightness=%d\n", __func__, brightness);
		return -EINVAL;
	}
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	// tunning value
	if((icp_hd_tup_state == ICP_HD_TUNE_STATE_LOAD_VALUE) && (icp_hd_params_tbl.bright[brightness].num_params > 0))	
	{
		SKYCDBG("icp_hd_tup_state = %d, icp_hd_params_tbl.bright[0].num_params=%d\n", icp_hd_tup_state, brightness);
		icp_hd_i2c_write_params(icp_hd_params_tbl.bright[brightness].params,
								icp_hd_params_tbl.bright[brightness].num_params);
	}
	// static value
	else
#endif		
	{
		rc = icp_hd_i2c_write_table(icp_hd_regs.bright_cfg_settings[brightness],	
					icp_hd_regs.bright_cfg_settings_size);
	}		

	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}		
	
	SKYCDBG("%s end\n",__func__);

	return rc;
}
#endif

#if 1 //def F_PANTECH_CAMERA_FIX_CFG_LED_MODE
static int32_t icp_hd_set_isp_gpio_0(bool enable)
{
	unsigned short isp_gpio_value = 0;
	int rc, final_rc = 0;
		
//	if(rc = icp_hd_i2c_write(icp_hd_client->addr, 0xf03c, 0x0009,WORD_LEN)) final_rc = rc;
//	if(rc = icp_hd_i2c_write(icp_hd_client->addr, 0xf03e, 0x5004,WORD_LEN)) final_rc = rc;
//	if(rc = icp_hd_i2c_read(icp_hd_client->addr,	0xE002, &isp_gpio_value,WORD_LEN)) final_rc = rc;
	if((rc = icp_hd_i2c_write(icp_hd_client->addr, 0xf03c, 0x0009,WORD_LEN)) != 0) final_rc = rc;
	if((rc = icp_hd_i2c_write(icp_hd_client->addr, 0xf03e, 0x5004,WORD_LEN)) != 0) final_rc = rc;
	if((rc = icp_hd_i2c_read(icp_hd_client->addr,	0xE002, &isp_gpio_value,WORD_LEN)) != 0) final_rc = rc;
	
	SKYCDBG("GPIO_0:read isp_gpio_value_11 = 0x%x\n",isp_gpio_value);
	isp_gpio_value = isp_gpio_value|0x0001;
	SKYCDBG("GPIO_0:read isp_gpio_value_12 = 0x%x\n",isp_gpio_value);
//	if(rc = icp_hd_i2c_write(icp_hd_client->addr, 0xE002, isp_gpio_value,WORD_LEN)) final_rc = rc;
	if((rc = icp_hd_i2c_write(icp_hd_client->addr, 0xE002, isp_gpio_value,WORD_LEN)) != 0) final_rc = rc;
	
//	if(rc = icp_hd_i2c_write(icp_hd_client->addr, 0xf03c, 0x0009,WORD_LEN)) final_rc = rc;
//	if(rc = icp_hd_i2c_write(icp_hd_client->addr, 0xf03e, 0x5000,WORD_LEN)) final_rc = rc;
//	if(rc = icp_hd_i2c_read(icp_hd_client->addr,	0xE002, &isp_gpio_value,WORD_LEN)) final_rc = rc;
	if((rc = icp_hd_i2c_write(icp_hd_client->addr, 0xf03c, 0x0009,WORD_LEN)) != 0) final_rc = rc;
	if((rc = icp_hd_i2c_write(icp_hd_client->addr, 0xf03e, 0x5000,WORD_LEN)) != 0) final_rc = rc;
	if((rc = icp_hd_i2c_read(icp_hd_client->addr,	0xE002, &isp_gpio_value,WORD_LEN)) != 0) final_rc = rc;
	SKYCDBG("GPIO_0:read isp_gpio_value_13 = 0x%x\n",isp_gpio_value);
	if(enable)
	{
		isp_gpio_value = isp_gpio_value|0x0001;
	}
	else
	{
		isp_gpio_value = isp_gpio_value&0xfffe;
	}
	SKYCDBG("GPIO_0:read isp_gpio_value_14 = 0x%x\n",isp_gpio_value);
//	if(rc = icp_hd_i2c_write(icp_hd_client->addr, 0xE002, isp_gpio_value,WORD_LEN)) final_rc = rc;
	if((rc = icp_hd_i2c_write(icp_hd_client->addr, 0xE002, isp_gpio_value,WORD_LEN)) != 0) final_rc = rc;
	if (final_rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return final_rc;
	}
	return final_rc;
}

static int32_t icp_hd_set_isp_gpio_2(bool enable)
{
	unsigned short isp_gpio_value = 0;
	int rc, final_rc = 0;
#if 0		
	if(rc = icp_hd_i2c_write(icp_hd_client->addr, 0xf03c, 0x0009,WORD_LEN)) final_rc = rc;
	if(rc = icp_hd_i2c_write(icp_hd_client->addr, 0xf03e, 0x5004,WORD_LEN)) final_rc = rc;
	if(rc = icp_hd_i2c_read(icp_hd_client->addr,	0xE002, &isp_gpio_value,WORD_LEN)) final_rc = rc;
#else
	if((rc = icp_hd_i2c_write(icp_hd_client->addr, 0xf03c, 0x0009,WORD_LEN)) != 0) final_rc = rc;
	if((rc = icp_hd_i2c_write(icp_hd_client->addr, 0xf03e, 0x5004,WORD_LEN)) != 0) final_rc = rc;
	if((rc = icp_hd_i2c_read(icp_hd_client->addr,	0xE002, &isp_gpio_value,WORD_LEN)) != 0) final_rc = rc;
#endif
	SKYCDBG("GPIO_2:read isp_gpio_value_21 = 0x%x\n",isp_gpio_value);
	isp_gpio_value = isp_gpio_value|0x0004;
	SKYCDBG("GPIO_2:read isp_gpio_value_22 = 0x%x\n",isp_gpio_value);
//	if(rc = icp_hd_i2c_write(icp_hd_client->addr, 0xE002, isp_gpio_value,WORD_LEN)) final_rc = rc;
	if((rc = icp_hd_i2c_write(icp_hd_client->addr, 0xE002, isp_gpio_value,WORD_LEN)) != 0) final_rc = rc;
#if 0
	if(rc = icp_hd_i2c_write(icp_hd_client->addr, 0xf03c, 0x0009,WORD_LEN)) final_rc = rc;
	if(rc = icp_hd_i2c_write(icp_hd_client->addr, 0xf03e, 0x5000,WORD_LEN)) final_rc = rc;
	if(rc = icp_hd_i2c_read(icp_hd_client->addr,	0xE002, &isp_gpio_value,WORD_LEN)) final_rc = rc;
#else
	if((rc = icp_hd_i2c_write(icp_hd_client->addr, 0xf03c, 0x0009,WORD_LEN)) != 0) final_rc = rc;
	if((rc = icp_hd_i2c_write(icp_hd_client->addr, 0xf03e, 0x5000,WORD_LEN)) != 0) final_rc = rc;
	if((rc = icp_hd_i2c_read(icp_hd_client->addr,	0xE002, &isp_gpio_value,WORD_LEN)) != 0) final_rc = rc;
#endif
	SKYCDBG("GPIO_2:read isp_gpio_value_23 = 0x%x\n",isp_gpio_value);
	if(enable)
	{
		isp_gpio_value = isp_gpio_value|0x0004;
	}
	else
	{
		isp_gpio_value = isp_gpio_value&0xfffb;
	}
	SKYCDBG("GPIO_2:read isp_gpio_value_24 = 0x%x\n",isp_gpio_value);
//	if(rc = icp_hd_i2c_write(icp_hd_client->addr, 0xE002, isp_gpio_value,WORD_LEN)) final_rc = rc;
	if((rc = icp_hd_i2c_write(icp_hd_client->addr, 0xE002, isp_gpio_value,WORD_LEN)) != 0) final_rc = rc;
	if (final_rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return final_rc;
	}
	return final_rc;
}

static int32_t icp_hd_set_led_mode(int32_t led_mode)
{
	/* off, auto, on, movie */	
	int enable_flash_main_gpio = 0;	
	int rc, final_rc = 0;
	
	SKYCDBG("%s: led_mode=%d\n", __func__, led_mode);
	if ((led_mode < ICP_HD_CFG_LED_MODE_OFF) || (led_mode >= ICP_HD_CFG_LED_MODE_MAX)) {
		SKYCERR("%s: -EINVAL, led_mode=%d\n", __func__, led_mode);
		return -EINVAL;
	}

	if((led_mode != ICP_HD_CFG_LED_MODE_OFF)&&(led_mode != ICP_HD_CFG_LED_MODE_AUTO))
		enable_flash_main_gpio = ON;
	else
		enable_flash_main_gpio = OFF;

	//control icp_hd led flash main gpio
	if((rc = gpio_tlmm_config(GPIO_CFG(ICP_HD_LED_FLASH_ENABLE_GPIO, 0,
		GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE)) != 0) final_rc = rc;
	if (!final_rc)
	{
		SKYCDBG("%s enable_flash_main_gpio = %d\n", __func__, enable_flash_main_gpio);
		gpio_set_value(ICP_HD_LED_FLASH_ENABLE_GPIO,enable_flash_main_gpio);					
	}
	else
	{
		SKYCERR("ERR:%s ENABLE FLASH GPIO FAIL!!!rc=%d return~~\n", __func__, rc);
		return final_rc;
	}

	//control icp_hd isp gpio
	switch(led_mode)
	{
		case ICP_HD_CFG_LED_MODE_OFF:
			if((rc = icp_hd_set_isp_gpio_0(OFF)) != 0) final_rc = rc;
			if((rc = icp_hd_set_isp_gpio_2(OFF)) != 0) final_rc = rc;
			break;
		case ICP_HD_CFG_LED_MODE_AUTO:
			if((rc = icp_hd_set_isp_gpio_0(OFF)) != 0) final_rc = rc;
			if((rc = icp_hd_set_isp_gpio_2(OFF)) != 0) final_rc = rc;
			SKYCDBG("PM_FLASH_LED_SET_CURRENT_PROC rc = %d\n", rc);
			break;
		case ICP_HD_CFG_LED_MODE_ON:
			if((rc = icp_hd_set_isp_gpio_0(OFF)) != 0) final_rc = rc;
			if((rc = icp_hd_set_isp_gpio_2(ON)) != 0) final_rc = rc;
			break;
		case ICP_HD_CFG_LED_MODE_MOVIE:
			if((rc = icp_hd_set_isp_gpio_0(OFF)) != 0) final_rc = rc;
			if((rc = icp_hd_set_isp_gpio_2(ON)) != 0) final_rc = rc;
			break;
		case ICP_HD_CFG_LED_MODE_SNAP:
			if((rc = icp_hd_set_isp_gpio_0(ON)) != 0) final_rc = rc;
			if((rc = icp_hd_set_isp_gpio_2(ON)) != 0) final_rc = rc;
			break;
		default:		
			if((rc = icp_hd_set_isp_gpio_0(OFF)) != 0) final_rc = rc;
			if((rc = icp_hd_set_isp_gpio_2(OFF)) != 0) final_rc = rc;
			break;			
	}

	if (final_rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return final_rc;
	}			
	return final_rc;
}
#endif

#if 1 //def F_PANTECH_CAMERA_FIX_CFG_AF
static int32_t icp_hd_set_auto_focus(int mode, int8_t autofocus)
{
#define AF_POLL_PERIOD	50
#define AF_POLL_RETRY	100
#define ICP_HD_FOCUS_AUTO_COMPLETE_REG 0x60
#define ICP_HD_FOCUS_RECT_START_REG 0x01F1
#define ICP_HD_FOCUS_RECT_COMPLETE_REG 0x01F0
	int32_t rc = 0;
	int32_t i = 0;
	unsigned short read_buf = 0;

	SKYCDBG("%s  auto_focus = %d\n",__func__, autofocus);
	if ((autofocus < 0) || (autofocus > 4))
	{
		SKYCERR("%s FAIL!!! return~~  autofocus = %d\n",__func__,autofocus);
		return 0;//-EINVAL;
	}

	switch(autofocus)
	{
		case 4:
			SKYCDBG("%s START~ SPOT autofocus = %d\n",__func__, autofocus);
			rc =	icp_hd_i2c_write(icp_hd_client->addr,	0x4004, ICP_HD_FOCUS_RECT_START_REG, WORD_LEN);
			if (rc < 0) {
				SKYCERR("ERR:%s SF SET END RECT FAIL!!!rc=%d return~~\n", __func__, rc);
				return rc;
			}

			for (i = 0; i < AF_POLL_RETRY; i++) 
			{
				msleep(AF_POLL_PERIOD);
				
				rc = icp_hd_i2c_read(icp_hd_client->addr,
						0x4004, &read_buf,
						WORD_LEN);
				if (rc < 0)
				{
					SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
					return rc;
				}
				SKYCDBG("AF polling num = %d,  read_buf = %x\n", i, read_buf);

				if(read_buf == ICP_HD_FOCUS_RECT_COMPLETE_REG)
					break;
			}
			SKYCDBG("%s END~ SPOT autofocus = %d\n",__func__, autofocus);
			break;
		default:
			SKYCDBG("%s START~ DEFAULT autofocus = %d\n",__func__, autofocus);
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_LED_MODE			
			if(led_auto == ICP_HD_CFG_LED_MODE_AUTO)
			{	
				rc = icp_hd_i2c_read(icp_hd_client->addr,	0x400A , &nbrightness,WORD_LEN);
				SKYCDBG("%s FLASH_AUTO_MODE nbrightness =0x%x\n",__func__, nbrightness);
				if((nbrightness <= 0x0200)||(nbrightness >= 0x8000))
					icp_hd_set_led_mode(ICP_HD_CFG_LED_MODE_MOVIE);
			}
#endif
			rc = icp_hd_i2c_write_table(&icp_hd_regs.autofocus_trigger_cfg_settings[0],
							icp_hd_regs.autofocus_trigger_cfg_settings_size);
			if (rc < 0)
			{
				SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
				return rc;
			}		
		      
			for (i = 0; i < AF_POLL_RETRY; i++) 
			{
				msleep(AF_POLL_PERIOD);
				
				rc = icp_hd_i2c_read(icp_hd_client->addr,
						0x4004, &read_buf,
						WORD_LEN);
				if (rc < 0)
				{
					SKYCDBG("AUTO FOCUS READ I2C FAIL!!! return~~\n");
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_LED_MODE
					if((led_auto == ICP_HD_CFG_LED_MODE_AUTO)&&((nbrightness <= 0x0200)||(nbrightness >= 0x8000)))
						icp_hd_set_led_mode(ICP_HD_CFG_LED_MODE_OFF);
#endif			
					return rc;
				}

				SKYCDBG("AF polling num = %d,  read_buf = %x\n", i, read_buf);

				if(read_buf == ICP_HD_FOCUS_AUTO_COMPLETE_REG)
					break;
			}

			SKYCDBG("%s end\n",__func__);
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_LED_MODE
			if((led_auto == ICP_HD_CFG_LED_MODE_AUTO)&&((nbrightness <= 0x0200)||(nbrightness >= 0x8000)))
				icp_hd_set_led_mode(ICP_HD_CFG_LED_MODE_OFF);
#endif	
			SKYCDBG("%s END~ DEFAULT autofocus = %d\n",__func__, autofocus);
			break;			
	}

	return rc;
}
#endif

#if 0 //def F_PANTECH_CAMERA_FIX_CFG_ISO
static int32_t icp_hd_set_iso(int32_t iso)
{
	/* auto, N/A, ISO100, ISO200, ISO400, ISO800, ISO1600, ISO3200 */
	uint8_t data[] = {0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
	int32_t rc = 0;

#if 0	
//	if(mode == SENSOR_SNAPSHOT_MODE) return 0;

	if ((iso < 0) || (iso >= sizeof(data)) || (iso == 1))
		return -EINVAL;

	SKYCDBG("%s  iso = %d\n",__func__, iso);

	rc = icp_hd_i2c_write(icp_hd_client->addr, 0x17, data[iso],WORD_LEN);
#endif
	
	return rc;
}
#endif

#if 1 //def F_PANTECH_CAMERA_FIX_CFG_SCENE_MODE
static int32_t old_scene_value = ICP_HD_CFG_SCENE_MODE_OFF;
static int32_t icp_hd_set_scene_mode(int mode, int32_t scene_mode)
{
	/* +off, +auto, +portrait, +landscape, indoor, +night, +sports, dawn,
	 * snow, autumn, text, +sunset, backlight, beach, party */
	//uint8_t data[] = {0x00, 0x03, 0x05, 0x09, 0x0b, 0x0d, 0x0f, 0x11, 
	//	0x13, 0x15, 0x1b, 0x1d, 0x1f, 0x21, 0x23};

	int32_t rc = 0;
	int32_t scene_mode_off_index = 0;

	SKYCDBG("%s start~  mode = %d, scene_mode = %d\n",__func__,mode ,scene_mode);
	if ((scene_mode < 0) || (scene_mode >= ICP_HD_CFG_SCENE_MODE_MAX))
	{
		SKYCERR("%s not support scene mode  = %d\n",__func__,scene_mode);
		return 0;//-EINVAL;
	}
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE	
	if((icp_hd_tup_state == ICP_HD_TUNE_STATE_LOAD_VALUE) && (icp_hd_params_tbl.scene[scene_mode].num_params > 0))	
	{
		icp_hd_i2c_write_params(icp_hd_params_tbl.scene[scene_mode].params,
								icp_hd_params_tbl.scene[scene_mode].num_params);
	}
	// static value
	else
#endif		
	{
		if((scene_mode == ICP_HD_CFG_SCENE_MODE_OFF) || (scene_mode == ICP_HD_CFG_SCENE_MODE_AUTO))
		{
		SKYCDBG("DBG~1 old_scene_value=%d, new_scene_mode=%d, scene_mode_off_index=%d",old_scene_value, scene_mode, scene_mode_off_index);
			if((old_scene_value == ICP_HD_CFG_SCENE_MODE_PORTRAIT)||
			   (old_scene_value == ICP_HD_CFG_SCENE_MODE_SPORTS)||
			   (old_scene_value == ICP_HD_CFG_SCENE_MODE_NIGHT))
			{
				switch(old_scene_value)
				{
					case ICP_HD_CFG_SCENE_MODE_PORTRAIT:
						scene_mode_off_index = 0;
						break;
					case ICP_HD_CFG_SCENE_MODE_SPORTS:
						scene_mode_off_index = 1;
						break;
					case ICP_HD_CFG_SCENE_MODE_NIGHT:
						scene_mode_off_index = 2;
						break;
					default:
						break;
				}
				SKYCDBG("DBG~2 old_scene_value=%d, new_scene_mode=%d, scene_mode_off_index=%d",old_scene_value, scene_mode, scene_mode_off_index);
				rc = icp_hd_i2c_write_table(icp_hd_regs.scene_mode_off_cfg_settings[scene_mode_off_index],	
					icp_hd_regs.scene_mode_off_cfg_settings_size);
				SKYCDBG("SCENE MODE OFF register setting!!! old_scene_value=%d, new_scene_mode=%d",old_scene_value, scene_mode);
			}
		}
		rc = icp_hd_i2c_write_table(icp_hd_regs.scene_mode_cfg_settings[scene_mode],	
					icp_hd_regs.scene_mode_cfg_settings_size);
	}

	//to turn off the portrait, sports, night scene
	old_scene_value = scene_mode;

	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}		

	SKYCDBG("%s end\n",__func__);

	return rc;
}
#endif

#if 1 //def F_PANTECH_CAMERA_FIX_CFG_FOCUS_STEP
static int32_t icp_hd_set_focus_step(int mode, int32_t focus_step)
{
	int32_t rc = 0;

	if ((focus_step < 0) || (focus_step >= ICP_HD_CFG_FOCUS_STEP_MAX))
	{
		SKYCERR("%s FAIL!!! return~~  focus_step = %d\n",__func__,focus_step);
		return 0;//-EINVAL;
	}
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	if((icp_hd_tup_state == ICP_HD_TUNE_STATE_LOAD_VALUE) && (icp_hd_params_tbl.focus_step[0].num_params > 0))	
	{
		icp_hd_i2c_write_params(icp_hd_params_tbl.focus_step[focus_step].params,
								icp_hd_params_tbl.focus_step[focus_step].num_params);
	}
	// static value
	else
#endif		
	{
		rc = icp_hd_i2c_write_table(icp_hd_regs.focus_step_cfg_settings[focus_step],	
						icp_hd_regs.focus_step_cfg_settings_size);
	}

	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}		

	SKYCDBG("%s end\n",__func__);

	return rc;
}
#endif

#if 1 //def F_PANTECH_CAMERA_ADD_CFG_ANTISHAKE
static int32_t is_antishake = ICP_HD_CFG_ANTISHAKE_OFF;
static int32_t icp_hd_set_antishake(int32_t antishake)
{
	int32_t rc = 0;

	if ((antishake < 0) || (antishake >= ICP_HD_CFG_ANTISHAKE_MAX))
	{
		SKYCERR("%s FAIL!!! return~~  antishake = %d\n",__func__,antishake);
		return 0;//-EINVAL;
	}
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	if((icp_hd_tup_state == ICP_HD_TUNE_STATE_LOAD_VALUE) && (icp_hd_params_tbl.antishake[antishake].num_params > 0))	
	{
		icp_hd_i2c_write_params(icp_hd_params_tbl.antishake[antishake].params,
								icp_hd_params_tbl.antishake[antishake].num_params);
	}
	// static value
	else
#endif		
	{
		rc = icp_hd_i2c_write_table(icp_hd_regs.antishake_cfg_settings[antishake],	
					icp_hd_regs.antishake_cfg_settings_size);
	}

	//check the antishake flag to do halfshutter
	is_antishake = antishake;

	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}		

	SKYCDBG("%s end\n",__func__);
	
	return rc;
}
#endif

#if 1 //def F_PANTECH_CAMERA_FIX_CFG_EXPOSURE
static int32_t icp_hd_set_exposure_mode(int32_t exposure)
{
	int32_t rc = 0;

	SKYCDBG("%s  exposure = %d\n",__func__, exposure);

	if ((exposure < 0) || (exposure >= ICP_HD_CFG_EXPOSURE_MAX))
	{
		SKYCERR("%s FAIL!!! return~~  exposure = %d\n",__func__,exposure);
		return 0;//-EINVAL;
	}
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	if((icp_hd_tup_state == ICP_HD_TUNE_STATE_LOAD_VALUE) && (icp_hd_params_tbl.exposure[exposure].num_params > 0))	
	{
		icp_hd_i2c_write_params(icp_hd_params_tbl.exposure[exposure].params,
								icp_hd_params_tbl.exposure[exposure].num_params);
	}
	// static value
	else
#endif		
	{
		rc = icp_hd_i2c_write_table(icp_hd_regs.exposure_cfg_settings[exposure],
					icp_hd_regs.exposure_cfg_settings_size);
	}
 
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}		
	
	SKYCDBG("%s end\n",__func__);

	return rc;
}
#endif

#if 1 //def F_PANTECH_CAMERA_FIX_CFG_REFLECT
static int32_t icp_hd_set_reflect(int32_t reflect)
{
	int32_t rc = 0;
	int32_t i = 0;
	//int8_t npolling = -1;
	uint16_t npolling = -1;

	SKYCDBG("%s  reflect = %d\n",__func__, reflect);

	if ((reflect < 0) || (reflect >= ICP_HD_CFG_REFLECT_MAX))
	{
		SKYCERR("%s FAIL!!! return~~  reflect = %d\n",__func__,reflect);
		return 0;//-EINVAL;
	}
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	if((icp_hd_tup_state == ICP_HD_TUNE_STATE_LOAD_VALUE) && (icp_hd_params_tbl.reflect[reflect].num_params > 0))	
	{
		icp_hd_i2c_write_params(icp_hd_params_tbl.reflect[reflect].params,
								icp_hd_params_tbl.reflect[reflect].num_params);
	}
	// static value
	else
#endif		
	{
		rc = icp_hd_i2c_write_table(icp_hd_regs.reflect_cfg_settings[reflect],
					icp_hd_regs.reflect_cfg_settings_size);
	}

	for (i = 0; i < ICP_HD_POLLING_RETRY; i++) {
		rc = icp_hd_i2c_read(icp_hd_client->addr,
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
	
	SKYCDBG("%s end\n",__func__);

	return rc;
}
#endif

#if 1 //def F_PANTECH_CAMERA_FIX_CFG_FOCUS_RECT
static int32_t icp_hd_set_focus_rect(uint32_t focus_rect)
{
#define ICP_HD_FOCUS_RECT_WINDOW_MAX 255
	uint8_t xleft, xright, ytop, ybottom;
	uint32_t sysx = 0;
	uint32_t eyex = 0;
	int32_t rc = 0;

	SKYCDBG("%s  focus_rect = %d\n",__func__, focus_rect);

	ytop = (uint8_t)((focus_rect & 0xff000000) >> 24);
	xleft = (uint8_t)((focus_rect & 0x00ff0000) >> 16);
	ybottom = (uint8_t)((focus_rect & 0x0000ff00) >> 8);
	xright = (uint8_t)((focus_rect & 0x000000ff));

	if (focus_rect == 0) {		
		xleft = 0x40;
		ytop = 0x40;
#if 0	//test
		xright = 0x12C;
		ybottom = 0x12C;
#else
		xright = 0x80;
		ybottom = 0x80;
#endif
	}

	sysx = (ytop << 8)|xleft;
	eyex = (ybottom << 8)|xright;
	
	SKYCDBG("%s  xleft = %d, ytop = %d, xright = %d, ybottom = %d, sysx = %d, eyex = %d,\n",__func__, xleft, ytop, xright, ybottom, sysx, eyex);
		
	rc =	icp_hd_i2c_write(icp_hd_client->addr,	0x404C, sysx, WORD_LEN);
	if (rc < 0) {
		SKYCERR("ERR:%s SF SET START RECT FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}
	rc =	icp_hd_i2c_write(icp_hd_client->addr,	0x404E, eyex, WORD_LEN);
	if (rc < 0) {
		SKYCERR("ERR:%s SF SET END RECT FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}

	SKYCDBG("%s end\n",__func__);
	
	return rc;	
}
#endif

#if 1 //def F_PANTECH_CAMERA_ADD_CFG_DIMENSION
static int32_t icp_hd_set_dimension(struct dimension_cfg *dimension)
{
	int32_t rc = 0;

#if 1 //def F_PANTECH_CAMERA_1080P_PREVIEW
	int32_t i=0;
	uint16_t read_data =0;
	
	memcpy(&icp_hd_dimension, dimension, sizeof(struct dimension_cfg));	

	SKYCDBG("%s: preview=%dx%d, snapshot=%dx%d, video=%dx%d\n", __func__,
		dimension->prev_dx, dimension->prev_dy,
		dimension->pict_dx, dimension->pict_dy,
		dimension->video_dx, dimension->video_dy);

	if((icp_hd_dimension.video_dx == 1920)&&(icp_hd_dimension.video_dy == 1080))
	{		
       	icp_hd_config_1080P_preview = TRUE;

		rc = icp_hd_i2c_write_table(&icp_hd_regs.preview_1920_1080_cfg_settings[0],
						icp_hd_regs.preview_1920_1080_cfg_settings_size);
		rc = icp_hd_i2c_write(icp_hd_client->addr, 0x1000, 0x0001, WORD_LEN);
		rc = icp_hd_i2c_write(icp_hd_client->addr, 0x1000, 0x2000, WORD_LEN);
		for(i = 0; i<100; i++) {
		   	rc = icp_hd_i2c_read(icp_hd_client->addr, 0x1000, &read_data, WORD_LEN);
			SKYCDBG("%s icp_hd_i2c_read cont = %d, read_data = %d\n",__func__,i, read_data);
			msleep(5);
			if (read_data == 0x0000) {	
				msleep(200);
				break;
			}
			//msleep(50);
		}
	}
	else
	{
		if(icp_hd_config_1080P_preview == TRUE)
		{
		       rc = icp_hd_i2c_write_table(&icp_hd_regs.preview_1280_960_cfg_settings[0],
							icp_hd_regs.preview_1280_960_cfg_settings_size);
			rc = icp_hd_i2c_write(icp_hd_client->addr, 0x1000, 0x0001, WORD_LEN);
			rc = icp_hd_i2c_write(icp_hd_client->addr, 0x1000, 0x2000, WORD_LEN);
			for(i = 0; i<100; i++) {
			   	rc = icp_hd_i2c_read(icp_hd_client->addr, 0x1000, &read_data, WORD_LEN);
				SKYCDBG("%s icp_hd_i2c_read cont = %d, read_data = %d\n",__func__,i, read_data);
				msleep(5);
				if (read_data == 0x0000) {	
					msleep(200);
					break;
				}
				//msleep(50);
			}
			icp_hd_config_1080P_preview = FALSE;
		}			
	}
#endif

	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}
	SKYCDBG("%s end rc = %d\n",__func__, rc);

	return rc;
}
#endif

#if 1 //def F_PANTECH_CAMERA_FIX_CFG_ANTIBANDING
static int32_t icp_hd_set_antibanding(int32_t antibanding)
{
	int32_t rc = 0;

	if ((antibanding < 0) || (antibanding >= ICP_HD_CFG_FLICKER_MAX))
	{
		SKYCERR("%s FAIL!!! return~~  antibanding = %d\n",__func__,antibanding);
		return 0;//-EINVAL;
	}
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	if((icp_hd_tup_state == ICP_HD_TUNE_STATE_LOAD_VALUE) && (icp_hd_params_tbl.flicker[antibanding].num_params > 0))	
	{
		icp_hd_i2c_write_params(icp_hd_params_tbl.flicker[antibanding].params,
								icp_hd_params_tbl.flicker[antibanding].num_params);
	}
	// static value
	else
#endif		
	{
		rc = icp_hd_i2c_write_table(icp_hd_regs.flicker_cfg_settings[antibanding],	
					icp_hd_regs.flicker_cfg_settings_size);
	}		

	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}		

	SKYCDBG("%s end\n",__func__);
	return rc;
}
#endif


#if 1 //def F_PANTECH_CAMERA_FIX_CFG_PREVIEW_FPS
static int32_t icp_hd_set_preview_fps(int32_t preview_fps)
{
	/* 0 : variable 10~30fps, 1 ~ 30 : fixed fps, 31 : variable 10~30fps */
	/* default 31 : variable 10 ~ 30fps */
	int32_t rc = 0;

	if ((preview_fps < C_PANTECH_CAMERA_MIN_PREVIEW_FPS) || (preview_fps > C_PANTECH_CAMERA_MAX_PREVIEW_FPS)) {
		SKYCERR("%s: -EINVAL, preview_fps=%d\n", 
			__func__, preview_fps);
		return -EINVAL;
	}

	SKYCDBG("%s: preview_fps=%d\n", __func__, preview_fps);
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	if((icp_hd_tup_state == ICP_HD_TUNE_STATE_LOAD_VALUE) && (icp_hd_params_tbl.ffps[preview_fps-5].num_params > 0))	
	{
		icp_hd_i2c_write_params(icp_hd_params_tbl.ffps[preview_fps-5].params,
								icp_hd_params_tbl.ffps[preview_fps-5].num_params);
	}
	// static value
	else
#endif		
	{
		rc = icp_hd_i2c_write_table(icp_hd_regs.preview_fps_cfg_settings[preview_fps-C_PANTECH_CAMERA_MIN_PREVIEW_FPS],
					icp_hd_regs.preview_fps_cfg_settings_size);
	}

	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}

	SKYCDBG("%s end rc = %d\n",__func__, rc);

	return 0;
}
#endif

#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
static u_int32_t icp_hd_i2c_write_params
(
    icp_hd_cfg_param_t *params,
    u_int32_t num_params
)
{
    u_int16_t read_data = 0;
    u_int16_t write_data = 0;
    u_int8_t write_data_byte = 0;	
    u_int32_t i = 0;

    SKYCDBG(">>%s ", __func__);
    SKYCDBG("- params = 0x%08x, num_params = %d\n", params, num_params, 0);

    if ((params == NULL) || (num_params == 0))
    {
        SKYCERR("Invalid param! [params = 0x%08x][num_params = %d]\n", (u_int32_t)params, num_params, 0);
        return -EIO;    
    }

    for (i = 0; i < num_params; i++)
    {
        switch(params[i].op)
        {
            case CAMOP_NOP:
                SKYCDBG("WRITE: NOP\n", 0, 0, 0);
                break;

            case CAMOP_DELAY:
                SKYCDBG("WRITE: DELAY (%dms)\n", params[i].data, 0, 0);
                msleep(params[i].data);
                break;

            case CAMOP_WRITE:
                if (icp_hd_i2c_write_word(params[i].addr, params[i].data) < 0)
                {
                    SKYCERR("<<%s (-EIO)\n", __func__, 0, 0);
                    return -EIO;
                }
                break;
	     case CAMOP_WRITE_DATA1:
		 //1 byte casting 2 bytes data in array
 	 	 write_data_byte = (unsigned char)params[i].data;
		 if (icp_hd_i2c_write_a2d1(params[i].addr, write_data_byte) < 0)
                {
                    SKYCERR("<<%s (-EIO)\n", __func__, 0, 0);
                    return -EIO;
                }
                break;
            case CAMOP_WRITE_BIT:
            {
                u_int16_t bit_pos = 0;
                u_int16_t bit_val = 0;

                if (icp_hd_i2c_read_word(params[i].addr, &read_data) < 0)
                {
                    SKYCERR("<<%s (-EIO)", __func__, 0, 0);
                    return -EIO;
                }

                bit_pos = (params[i].data & 0xff00) >> 8;
                bit_val = (params[i].data & 0x00ff);

                if (bit_val != 0) // set to 1
                {
                    write_data = read_data | (0x0001 << bit_pos);
                    SKYCDBG("set 0x%04x[%d] to 1\n", params[i].addr, bit_pos, 0);
                }
                else // set to 0
                {
                    write_data = read_data & (~(0x0001 << bit_pos));
                    SKYCDBG("set 0x%04x[%d] to 0\n", params[i].addr, bit_pos, 0);
                }

                if (icp_hd_i2c_write_word(params[i].addr, write_data) < 0)
                {
                    SKYCERR("<<%s (-EIO)\n", __func__, 0, 0);
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
                    SKYCERR("<<%s (-EIO)", __func__, 0, 0);
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
                    if (icp_hd_i2c_read_word(poll_reg, &read_data) < 0)
                    {
                        SKYCERR("<<%s (-EIO)", __func__, 0, 0);
                        return -EIO;
                    }
                    
                    if ((read_data & poll_mask) != poll_data)
                    {
                        SKYCDBG("retry polling register... after sleeping %d ms", poll_delay, 0, 0);
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
                    SKYCDBG("<<%s (-EIO)", __func__, 0, 0);
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
                    SKYCERR("<<%s (-EIO)", __func__, 0, 0);
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
                    if (icp_hd_i2c_write_word(ICP_HD_REG_MCU_ADDR, poll_mcu_var) < 0)
                    {
                        SKYCERR("<<%s (-EIO)", __func__, 0, 0);
                        return -EIO;
                    }

                    if (icp_hd_i2c_read_word(ICP_HD_REG_MCU_DATA, &read_data) < 0)
                    {
                        SKYCERR("<<%s (-EIO)", __func__, 0, 0);
                        return -EIO;
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
                    SKYCDBG("<<%s (-EIO)", __func__, 0, 0);
                    return -EIO;
                }

                ++i;
                break;
            }

            default:
                SKYCERR("Invalid op code! [op = %d]", params[i].op, 0, 0);
                return -EIO;
                break;
        }

    }

    SKYCDBG("<<%s (TRUE)", __func__, 0, 0);
    return TRUE;
}

static void icp_hd_init_params_tbl (int8_t *stream)
{
#if defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
    icp_hd_cfg_param_t *params = NULL;
    u_int32_t num_params = 0;
#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
    u_int32_t i = 0;

    SKYCDBG(">>%s ()", __func__, 0, 0);

#if defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
    camsensor_icp_hd_tup_init(stream);
    num_params = camsensor_icp_hd_tup_get_init_params(&params);
    if ((num_params > 0) && (params != NULL))
    {
        icp_hd_params_tbl.init.num_params = num_params;
        icp_hd_params_tbl.init.params = params;
        //SKYCDBG("ICP_HD INIT params are loaded from TUNEUP file!\n", 0, 0, 0);
    }
    else
#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
    {
        icp_hd_params_tbl.init.num_params = num_params;
        icp_hd_params_tbl.init.params = params;
        //SKYCDBG("ICP_HD INIT params are loaded from TUNEUP file!\n", 0, 0, 0);
#if 0		
        icp_hd_params_tbl.init.num_params = sizeof(icp_hd_cfg_init_params) / sizeof(icp_hd_cfg_param_t);
        icp_hd_params_tbl.init.params = &icp_hd_cfg_init_params[0];
        SKYCDBG("ICP_HD INIT params are loaded from RO data!", 0, 0, 0);
#endif		
    }

    for (i = 0; i < ICP_HD_CFG_WB_MAX; i++)
    {
#if defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
        num_params = camsensor_icp_hd_tup_get_wb_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            icp_hd_params_tbl.wb[i].num_params = num_params;
            icp_hd_params_tbl.wb[i].params = params;
            //SKYCDBG("ICP_HD WB params [%d] are loaded from TUNEUP file!\n", i, 0, 0);
        }
        else
#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
        {        
            icp_hd_params_tbl.wb[i].num_params = num_params;
            icp_hd_params_tbl.wb[i].params = params;
            //SKYCDBG("ICP_HD WB params [%d] are loaded from TUNEUP file!\n", i, 0, 0);        
#if 0        
            icp_hd_params_tbl.wb[i].num_params = ICP_HD_CFG_WB_MAX_PARAMS;
            icp_hd_params_tbl.wb[i].params = &icp_hd_cfg_wb_params[i][0];
            SKYCDBG("ICP_HD WB params [%d] are loaded from RO data!", i, 0, 0);
#endif			
        }
    }

    for (i = 0; i < ICP_HD_CFG_BRIGHT_MAX; i++)
    {
#if defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
        num_params = camsensor_icp_hd_tup_get_bright_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            icp_hd_params_tbl.bright[i].num_params = num_params;
            icp_hd_params_tbl.bright[i].params = params;
            //SKYCDBG("ICP_HD BRIGHT params [%d] are loaded from TUNEUP file!\n", i, 0, 0);
        }
        else
#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
        {
      	     icp_hd_params_tbl.bright[i].num_params = num_params;
            icp_hd_params_tbl.bright[i].params = params;
            //SKYCDBG("ICP_HD BRIGHT params [%d] are loaded from TUNEUP file!\n", i, 0, 0);
#if 0        
            icp_hd_params_tbl.bright[i].num_params = ICP_HD_CFG_BRIGHT_MAX_PARAMS;
            icp_hd_params_tbl.bright[i].params = &icp_hd_cfg_bright_params[i][0];
            SKYCDBG("ICP_HD BRIGHT params [%d] are loaded from RO data!", i, 0, 0);
#endif			
        }

    }

    for (i = 0; i < ICP_HD_CFG_EXPOSURE_MAX; i++)
    {
#if defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
        num_params = camsensor_icp_hd_tup_get_exposure_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            icp_hd_params_tbl.exposure[i].num_params = num_params;
            icp_hd_params_tbl.exposure[i].params = params;
            //SKYCDBG("ICP_HD EXPOSURE params [%d] are loaded from TUNEUP file!\n", i, 0, 0);
        }
        else
#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
        {
            icp_hd_params_tbl.exposure[i].num_params = num_params;
            icp_hd_params_tbl.exposure[i].params = params;
            //SKYCDBG("ICP_HD EXPOSURE params [%d] are loaded from TUNEUP file!\n", i, 0, 0);
#if 0        
            icp_hd_params_tbl.exposure[i].num_params = ICP_HD_CFG_EXPOSURE_MAX_PARAMS;
            icp_hd_params_tbl.exposure[i].params = &icp_hd_cfg_exposure_params[i][0];
            SKYCDBG("ICP_HD EXPOSURE params [%d] are loaded from RO data!", i, 0, 0);
#endif			
        }

    }

    for (i = ICP_HD_CFG_FFPS_MIN; i <= ICP_HD_CFG_FFPS_MAX; i++)
    {
#if defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
        num_params = camsensor_icp_hd_tup_get_ffps_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            icp_hd_params_tbl.ffps[i - ICP_HD_CFG_FFPS_MIN].num_params = num_params;
            icp_hd_params_tbl.ffps[i - ICP_HD_CFG_FFPS_MIN].params = params;
            //SKYCDBG("ICP_HD fixed FPS params [%d] are loaded from TUNEUP file!\n", i - ICP_HD_CFG_FFPS_MIN, 0, 0);
        }
        else
#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
        {
            icp_hd_params_tbl.ffps[i - ICP_HD_CFG_FFPS_MIN].num_params = num_params;
            icp_hd_params_tbl.ffps[i - ICP_HD_CFG_FFPS_MIN].params = params;
            //SKYCDBG("ICP_HD fixed FPS params [%d] are loaded from TUNEUP file!\n", i - ICP_HD_CFG_FFPS_MIN, 0, 0);
#if 0        
            icp_hd_params_tbl.ffps[i - ICP_HD_CFG_FFPS_MIN].num_params = ICP_HD_CFG_FFPS_MAX_PARAMS;
            icp_hd_params_tbl.ffps[i - ICP_HD_CFG_FFPS_MIN].params = &icp_hd_cfg_ffps_params[i - ICP_HD_CFG_FFPS_MIN][0];
            SKYCDBG("ICP_HD fixed FPS params [%d] are loaded from RO data!", i - ICP_HD_CFG_FFPS_MIN, 0, 0);
#endif			
        }
    }

    for (i = 0; i < ICP_HD_CFG_REFLECT_MAX; i++)
    {
#if defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
        num_params = camsensor_icp_hd_tup_get_reflect_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            icp_hd_params_tbl.reflect[i].num_params = num_params;
            icp_hd_params_tbl.reflect[i].params = params;
            //SKYCDBG("ICP_HD REFLECT params [%d] are loaded from TUNEUP file!\n", i, 0, 0);
        }
        else
#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
        {
            icp_hd_params_tbl.reflect[i].num_params = num_params;
            icp_hd_params_tbl.reflect[i].params = params;
            //SKYCDBG("ICP_HD REFLECT params [%d] are loaded from TUNEUP file!\n", i, 0, 0);
#if 0        
            icp_hd_params_tbl.reflect[i].num_params = ICP_HD_CFG_FFPS_MAX_PARAMS;
            icp_hd_params_tbl.reflect[i].params = &icp_hd_cfg_reflect_params[i][0];
            SKYCDBG("ICP_HD REFLECT params [%d] are loaded from RO data!", i, 0, 0);
#endif			
        }
    }

    for (i = 0; i < ICP_HD_CFG_EFFECT_MAX; i++)
    {
#if defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
        num_params = camsensor_icp_hd_tup_get_effect_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            icp_hd_params_tbl.effect[i].num_params = num_params;
            icp_hd_params_tbl.effect[i].params = params;
            //SKYCDBG("ICP_HD EFFECT params [%d] are loaded from TUNEUP file!\n", i, 0, 0);
        }
        else
#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
        {
            icp_hd_params_tbl.effect[i].num_params = num_params;
            icp_hd_params_tbl.effect[i].params = params;
            //SKYCDBG("ICP_HD EFFECT params [%d] are loaded from TUNEUP file!\n", i, 0, 0);
#if 0        
            icp_hd_params_tbl.effect[i].num_params = ICP_HD_CFG_EFFECT_MAX_PARAMS;
            icp_hd_params_tbl.effect[i].params = &icp_hd_cfg_effect_params[i][0];
            SKYCDBG("ICP_HD EFFECT params [%d] are loaded from RO data!", i, 0, 0);
#endif
	}
    }
	
for (i = 0; i < ICP_HD_CFG_FLICKER_MAX; i++)
    {
#if defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
        num_params = camsensor_icp_hd_tup_get_flicker_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            icp_hd_params_tbl.flicker[i].num_params = num_params;
            icp_hd_params_tbl.flicker[i].params = params;
            //SKYCDBG("ICP_HD FLICKER params [%d] are loaded from TUNEUP file!\n", i, 0, 0);
        }
        else
#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
        {
            icp_hd_params_tbl.flicker[i].num_params = num_params;
            icp_hd_params_tbl.flicker[i].params = params;
	     //SKYCDBG("ICP_HD FLICKER params [%d] are loaded from TUNEUP file!\n", i, 0, 0);
#if 0        
            icp_hd_params_tbl.flicker[i].num_params = ICP_HD_CFG_FLICKER_MAX_PARAMS;
            icp_hd_params_tbl.flicker[i].params = &icp_hd_cfg_flicker_params[i][0];
            SKYCDBG("ICP_HD FLICKER params [%d] are loaded from RO data!", i, 0, 0);
#endif
	}
    }

for (i = 0; i < ICP_HD_CFG_ANTISHAKE_MAX; i++)
    {
#if defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
        num_params = camsensor_icp_hd_tup_get_antishake_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            icp_hd_params_tbl.antishake[i].num_params = num_params;
            icp_hd_params_tbl.antishake[i].params = params;
            //SKYCDBG("ICP_HD ANTISHAKE params [%d] are loaded from TUNEUP file!\n", i, 0, 0);
        }
        else
#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
        {
            icp_hd_params_tbl.antishake[i].num_params = num_params;
            icp_hd_params_tbl.antishake[i].params = params;
            //SKYCDBG("ICP_HD ANTISHAKE params [%d] are loaded from TUNEUP file!\n", i, 0, 0);
#if 0        
            icp_hd_params_tbl.antishake[i].num_params = ICP_HD_CFG_ANTISHAKE_MAX_PARAMS;
            icp_hd_params_tbl.antishake[i].params = &icp_hd_cfg_antishake_params[i][0];
            SKYCDBG("ICP_HD ANTISHAKE params [%d] are loaded from RO data!", i, 0, 0);
#endif
	}
    }

for (i = 0; i < ICP_HD_CFG_FOCUS_STEP_MAX; i++)
    {
#if defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
        num_params = camsensor_icp_hd_tup_get_focus_step_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            icp_hd_params_tbl.focus_step[i].num_params = num_params;
            icp_hd_params_tbl.focus_step[i].params = params;
            //SKYCDBG("ICP_HD FOCUS_STEP params [%d] are loaded from TUNEUP file!\n", i, 0, 0);
        }
        else
#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
        {
            icp_hd_params_tbl.focus_step[i].num_params = num_params;
            icp_hd_params_tbl.focus_step[i].params = params;
            //SKYCDBG("ICP_HD FOCUS_STEP params [%d] are loaded from TUNEUP file!\n", i, 0, 0);
#if 0        
            icp_hd_params_tbl.focus_step[i].num_params = ICP_HD_CFG_FOCUS_STEP_MAX_PARAMS;
            icp_hd_params_tbl.focus_step[i].params = &icp_hd_cfg_focus_step_params[i][0];
            SKYCDBG("ICP_HD FOCUS_STEP params [%d] are loaded from RO data!", i, 0, 0);
#endif
	}
    }

for (i = 0; i < ICP_HD_CFG_SCENE_MODE_MAX; i++)
    {
#if defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
        num_params = camsensor_icp_hd_tup_get_scene_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            icp_hd_params_tbl.scene[i].num_params = num_params;
            icp_hd_params_tbl.scene[i].params = params;
            //SKYCDBG("ICP_HD SCENE params [%d] are loaded from TUNEUP file!\n", i, 0, 0);
        }
        else
#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
        {
            icp_hd_params_tbl.scene[i].num_params = num_params;
            icp_hd_params_tbl.scene[i].params = params;
            //SKYCDBG("ICP_HD SCENE params [%d] are loaded from TUNEUP file!\n", i, 0, 0);
#if 0        
            icp_hd_params_tbl.scene[i].num_params = ICP_HD_CFG_SCENE_MAX_PARAMS;
            icp_hd_params_tbl.scene[i].params = &icp_hd_cfg_scene_params[i][0];
            SKYCDBG("ICP_HD SCENE params [%d] are loaded from RO data!", i, 0, 0);
#endif
	}
    }    

    SKYCDBG("<<%s ()", __func__, 0, 0);
    return;
}
#endif

#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
static int icp_hd_set_tune_value(const struct sensor_cfg_data *scfg)
{
	int32_t rc = 0;
	int32_t loop_count = 0;
	int32_t i = 0;
	int32_t last_size = 0;
	int32_t copy_size = 0;
	uint8_t * pKTune_stream = NULL;
	uint8_t * pUTune_stream = NULL;
	
if(icp_hd_tup_state != ICP_HD_TUNE_STATE_LOAD_VALUE)
{
	SKYCDBG("%s start\n",__func__);
	
	pKTune_stream = kmalloc(scfg->cfg.tune_value.file_size, GFP_KERNEL);
	SKYCDBG("pKTune_stream =%x\n", pKTune_stream);
	if (!pKTune_stream) {
		SKYCERR("failed to malloc.\n");
		rc = -ENOMEM;
		goto update_fw_user_fail;
	}

	//tunning file stream get from user space
	pUTune_stream = scfg->cfg.tune_value.pfile_stream;
	//loop_count save
	loop_count = (scfg->cfg.tune_value.file_size / 4096) + 1; 
	last_size = scfg->cfg.tune_value.file_size % 4096;
	copy_size = 4096;

	SKYCDBG("scfg->cfg.tune_value.pfile_stream=%x, scfg->cfg.tune_value.file_size=%d\n", scfg->cfg.tune_value.pfile_stream, scfg->cfg.tune_value.file_size);
	for(i = 0; i < loop_count; i++)
	{
		//If last loop change remaind size.
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

	//Each tunning values loading in array from fime stream
	icp_hd_init_params_tbl(pKTune_stream);

	icp_hd_tup_state = ICP_HD_TUNE_STATE_LOAD_VALUE;
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
	SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
	return rc;
}
#endif

static int32_t icp_hd_video_config(void)
{
	int32_t rc = 0;
	int32_t i=0;
	uint16_t read_data =0;
	
	/* set preview resolution to 1280x960 */
	SKYCDBG("%s start\n",__func__);
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	if(icp_hd_tup_state == ICP_HD_TUNE_STATE_LOAD_VALUE)
	{		
		rc = icp_hd_reg_init();
		if (rc < 0)
		{
			SKYCERR("icp_hd_i2c_write_table FAIL!!! return~~\n");
			return rc;
		}		
	}
	else
#endif	
#if 0
	rc = icp_hd_i2c_write_table(&icp_hd_regs.preview_cfg_settings[0],
						icp_hd_regs.preview_cfg_settings_size);
	msleep(300);
#else
	rc = icp_hd_i2c_write(icp_hd_client->addr, 0x1000, 0x0001, WORD_LEN);
       rc = icp_hd_i2c_write(icp_hd_client->addr, 0x1000, 0x2000, WORD_LEN);
	for(i = 0; i<100; i++) {
       	rc = icp_hd_i2c_read(icp_hd_client->addr, 0x1000, &read_data, WORD_LEN);
		SKYCDBG("%s icp_hd_i2c_read cont = %d, read_data = %d\n",__func__,i, read_data);
		msleep(5);
		if (read_data == 0x0000) {	
			//msleep(200);
			break;
		}
		//msleep(50);
	}
#endif
		rc = icp_hd_i2c_read(icp_hd_client->addr, 0x000C, &read_data, WORD_LEN);
		SKYCDBG("PREVIEW_WITDH = %d\n",read_data);
		rc = icp_hd_i2c_read(icp_hd_client->addr, 0x000E, &read_data, WORD_LEN);
		SKYCDBG("PREVIEW_HEIGHT = %d\n",read_data);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}
	SKYCDBG("%s end rc = %d\n",__func__, rc);

	return rc;
}

static int32_t icp_hd_snapshot_config(void)
{
	int32_t rc = 0;
	int i=0;
	uint16_t read_data =0;

	/* set snapshot resolution to 3264x2448 */
	SKYCDBG("%s start\n",__func__);
#if 0
	rc = icp_hd_i2c_write_table(&icp_hd_regs.snapshot_cfg_settings[0],
					icp_hd_regs.snapshot_cfg_settings_size);
	msleep(300);
#else
	//rc = icp_hd_i2c_write(icp_hd_client->addr, 0x1004, 0x0028, WORD_LEN);
       rc = icp_hd_i2c_write(icp_hd_client->addr, 0x1000, 0x0008, WORD_LEN);
	for(i = 0; i<100; i++) {
       	rc = icp_hd_i2c_read(icp_hd_client->addr, 0x1000, &read_data, WORD_LEN);
		SKYCDBG("%s icp_hd_i2c_read cont = %d, read_data = %d\n",__func__,i, read_data);
		msleep(5);
		if (read_data == 0x0009) {
			//msleep(200);
			break;
		}
		//msleep(50);
	}
#endif
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}
	SKYCDBG("%s end rc = %d\n",__func__, rc);
	
	return rc;
}


static long icp_hd_set_sensor_mode(int mode)
{
//	uint16_t clock;
	int32_t rc = 0;
       struct msm_camera_csi_params icp_hd_csi_params;

	SKYCDBG("%s start\n",__func__);
	SKYCDBG("icp_hd_set_sensor_mode mode = %d!\n",mode);

	switch (mode) {
	case SENSOR_PREVIEW_MODE:
		b_snapshot_flag = false;
#if 0		
		icp_hd_i2c_write_table(&icp_hd_regs.arm_go_cfg_settings[0],
						icp_hd_regs.arm_go_cfg_settings_size);
		icp_hd_i2c_write_table(&icp_hd_regs.standby_enter_cfg_settings[0],
						icp_hd_regs.standby_enter_cfg_settings_size);
		gpio_set_value(SENSOR_STANDBY,0);
		msleep(20);		
#endif		
#if 1		
		if(config_csi == 0)
		{
			SKYCDBG("icp_hd_set_sensor_mode config_csi E\n");
			icp_hd_csi_params.lane_cnt = 2; // 4;
			icp_hd_csi_params.data_format = CSI_8BIT; //CSI_10BIT;
			icp_hd_csi_params.lane_assign = 0xe4;
			icp_hd_csi_params.dpcm_scheme = 0;
			icp_hd_csi_params.settle_cnt = 0x14;
			rc = msm_camio_csi_config(&icp_hd_csi_params);
			//msleep(icp_hd_delay_msecs_stream);
			msleep(icp_hd_delay_msecs_stream);
			config_csi = 1;
			SKYCDBG("icp_hd_set_sensor_mode config_csi X\n");
#if defined(BURST_MODE_8192_BYTE_COMPRESS_INIT_NODATA) || defined(BURST_MODE_8192_BYTE_COMPRESS_INIT_OTP_TUNE_UPDATE_2)	|| defined(BURST_MODE_8192_BYTE_COMPRESS_INIT_OTP_TUNE_UPDATE_3)
			if (icp_hd_i2c_write(icp_hd_client->addr, 0x2036, 0x0012, WORD_LEN) < 0)
				SKYCERR("BURST_MODE_8192_BYTE_COMPRESS_INIT_NODATA 1");
			if (icp_hd_i2c_write(icp_hd_client->addr, 0x302E, 0x0012, WORD_LEN) < 0)
				SKYCERR("BURST_MODE_8192_BYTE_COMPRESS_INIT_NODATA 2");
			if (icp_hd_i2c_write(icp_hd_client->addr, 0x1000, 0x2000, WORD_LEN) < 0)
				SKYCERR("BURST_MODE_8192_BYTE_COMPRESS_INIT_NODATA 3");
#endif	
		}	
#endif		
		else
		{
		    rc = icp_hd_video_config();
		    SKYCDBG("icp_hd_video_config, rc = %d \n", rc);
		    if (rc < 0)
		    {
			SKYCERR("ERR:SENSOR_PREVIEW_MODE FAIL!!!rc=%d return~~\n", rc);
			return rc;
		    }
		}
		break;
		
	case SENSOR_SNAPSHOT_MODE:
		if(!b_snapshot_flag) {
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_LED_MODE
			if(led_auto == ICP_HD_CFG_LED_MODE_AUTO)
			{	
				rc = icp_hd_i2c_read(icp_hd_client->addr,	0x400A , &nbrightness,WORD_LEN);
				SKYCDBG("%s FLASH_AUTO_MODE SNAP nbrightness =0x%x\n",__func__, nbrightness);
				if((nbrightness <= 0x0200)||(nbrightness >= 0x8000))
				{
				icp_hd_set_led_mode(ICP_HD_CFG_LED_MODE_SNAP);
					//AF, AWB stable time for FLASH auto mode
					rc = icp_hd_i2c_write_table(&icp_hd_regs.half_sutter_cfg_settings[0],
							icp_hd_regs.half_sutter_cfg_settings_size);
				}
			}
#endif
#if 1 //def F_PANTECH_CAMERA_ADD_CFG_ANTISHAKE
			if((is_antishake == ICP_HD_CFG_ANTISHAKE_ON) && (led_auto != ICP_HD_CFG_LED_MODE_AUTO))
			{
				//AF, AWB stable time for FLASH auto mode
					rc = icp_hd_i2c_write_table(&icp_hd_regs.half_sutter_cfg_settings[0],
							icp_hd_regs.half_sutter_cfg_settings_size);
			}
#endif
			rc = icp_hd_snapshot_config();
			b_snapshot_flag = true;
			SKYCDBG("icp_hd_snapshot_config, rc = %d \n", rc);
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

static int icp_hd_sensor_init_probe(const struct msm_camera_sensor_info *data)
{
//	uint16_t model_id = 0;
	int rc = 0;

	SKYCDBG("init entry \n");
	rc = icp_hd_reset(data, ON);
	if (rc < 0) {
		SKYCERR("reset failed!\n");
		goto init_probe_fail;
	}

	msleep(5);

#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	if(icp_hd_tup_mode == ICP_HD_TUNE_STATE_TUNNING_MODE_ON)
		return rc;
#endif	

	SKYCDBG("icp_hd_reg_init E \n");
	rc = icp_hd_reg_init();
	SKYCDBG("icp_hd_reg_init X \n");
	if (rc < 0)
		goto init_probe_fail;

	return rc;

init_probe_fail:
	SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
	return rc;
}

int icp_hd_sensor_init(const struct msm_camera_sensor_info *data)
{
	int rc = 0;

	config_csi = 0;
#if 1 //def F_PANTECH_CAMERA_1080P_PREVIEW
       icp_hd_config_1080P_preview = FALSE;
#endif

	b_snapshot_flag = false;
	SKYCDBG(" icp_hd_sensor_init E\n");

#ifdef F_ICP_HD_POWER	
	icp_hd_vreg_init();
	rc = icp_hd_power(ON);
	if (rc) {		
		SKYCERR(" icp_hd_power failed rc=%d\n",rc);
		goto init_fail; 
	}
#endif
	icp_hd_ctrl = kzalloc(sizeof(struct icp_hd_ctrl_t), GFP_KERNEL);
	if (!icp_hd_ctrl) {
		SKYCERR("icp_hd_init failed!\n");
		rc = -ENOMEM;
		goto init_done;
	}

	if (data)
		icp_hd_ctrl->sensordata = data;

	/* Input MCLK = 24MHz */
	SKYCDBG(" msm_camio_clk_rate_set E\n");
	msm_camio_clk_rate_set(24000000);
	SKYCDBG(" msm_camio_clk_rate_set X\n");
	msleep(5);

	rc = icp_hd_sensor_init_probe(data);
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
	kfree(icp_hd_ctrl);
	return rc;
}

static int icp_hd_init_client(struct i2c_client *client)
{
	/* Initialize the MSM_CAMI2C Chip */
	init_waitqueue_head(&icp_hd_wait_queue);
	return 0;
}

int icp_hd_sensor_config(void __user *argp)
{
	struct sensor_cfg_data cfg_data;
	int rc = 0;

	if (copy_from_user(&cfg_data,
			(void *)argp,
			sizeof(struct sensor_cfg_data)))
		return -EFAULT;

	/* down(&icp_hd_sem); */

	SKYCDBG("icp_hd_sensor_config, cfgtype = %d, mode = %d\n",
		cfg_data.cfgtype, cfg_data.mode);

		switch (cfg_data.cfgtype) {
		case CFG_SET_MODE:
			rc = icp_hd_set_sensor_mode(
						cfg_data.mode);
			break;

		case CFG_SET_EFFECT:
			rc = icp_hd_set_effect(cfg_data.mode,
						cfg_data.cfg.effect);
			break;

		case CFG_GET_AF_MAX_STEPS:
			break;
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_WB
		case CFG_SET_WB:			
			rc = icp_hd_set_whitebalance(cfg_data.mode, cfg_data.cfg.whitebalance);
			SKYCDBG("icp_hd_set_whitebalance OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.whitebalance =[%d]\n", rc, cfg_data.mode, cfg_data.cfg.whitebalance);
			break;
#endif					
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_BRIGHTNESS
		case CFG_SET_BRIGHTNESS:
			rc = icp_hd_set_brightness(cfg_data.cfg.brightness);
			SKYCDBG("icp_hd_set_brightness OK! rc = [%d], cfg_data.cfg.brightness =[%d]\n", rc, cfg_data.cfg.brightness);
			break;
#endif			
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_LED_MODE
		case CFG_SET_LED_MODE:			
			if(cfg_data.cfg.led_mode == 1)
				led_auto = ICP_HD_CFG_LED_MODE_AUTO;
			else
				led_auto = ICP_HD_CFG_LED_MODE_OFF;
			rc = icp_hd_set_led_mode(cfg_data.cfg.led_mode);
			SKYCDBG("icp_hd_set_led_mode OK! rc = [%d], cfg_data.cfg.led_mode =[%d]\n", rc, cfg_data.cfg.led_mode);
			break;
#endif			
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_AF
		case CFG_AUTO_FOCUS:		
			rc = icp_hd_set_auto_focus(cfg_data.mode, cfg_data.cfg.focus.dir);
			SKYCDBG("icp_hd_auto_focus OK! rc = [%d], cfg_data.mode = [%d], cfg_data.cfg.focus.dir =[%d]\n", rc, cfg_data.mode, cfg_data.cfg.focus.dir);
			break;
#endif		
#if 0 //def F_PANTECH_CAMERA_FIX_CFG_ISO
	case CFG_SET_ISO:
			rc = icp_hd_set_iso(cfg_data.cfg.iso);
			SKYCDBG("%s(CFG_SET_ISO), rc=%d\n", __func__, rc);
			break;	
#endif
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_SCENE_MODE
	case CFG_SET_SCENE_MODE:
			rc = icp_hd_set_scene_mode(cfg_data.mode, cfg_data.cfg.scene_mode);
			SKYCDBG("%s(CFG_SET_SCENE_MODE), rc=%d\n", __func__, rc);
			break;	
#endif
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_FOCUS_STEP
	case CFG_SET_FOCUS_STEP:
			rc = icp_hd_set_focus_step(cfg_data.mode, cfg_data.cfg.focus_step);
			SKYCDBG("%s(CFG_SET_FOCUS_STEP), rc=%d\n", __func__, rc);
			break;
#endif
#if 1 //def F_PANTECH_CAMERA_ADD_CFG_ANTISHAKE
	case CFG_SET_ANTISHAKE:
			rc = icp_hd_set_antishake(cfg_data.cfg.antishake);
			SKYCDBG("%s(CFG_SET_ANTISHAKE), rc=%d\n", __func__, rc);
			break;
#endif
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_EXPOSURE
		case CFG_SET_EXPOSURE_MODE:
			rc = icp_hd_set_exposure_mode(cfg_data.cfg.exposure);
			SKYCDBG("icp_hd_set_exposure_mode OK! rc = [%d], cfg_data.cfg.exposure =[%d]\n", rc, cfg_data.cfg.exposure);
			break;
#endif
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_FOCUS_RECT
	case CFG_SET_FOCUS_RECT:
			rc = icp_hd_set_focus_rect(cfg_data.cfg.focus_rect);
			SKYCDBG("%s(CFG_SET_FOCUS_RECT), rc=%d\n", __func__, rc);
			break;
#endif
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_PREVIEW_FPS
	case CFG_SET_PREVIEW_FPS:
			rc = icp_hd_set_preview_fps(cfg_data.cfg.preview_fps);
			SKYCDBG("icp_hd_set_exposure_mode OK! rc = [%d], cfg_data.cfg.preview_fps =[%d]\n", rc, cfg_data.cfg.preview_fps);
			break;
#endif
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_ANTIBANDING
	case CFG_SET_ANTIBANDING:
			rc = icp_hd_set_antibanding(cfg_data.cfg.antibanding);
			SKYCDBG("%s: CFG_SET_ANTIBANDING, rc=%d\n", __func__, rc);
			break;
#endif
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
		case CFG_SET_TUNE_VALUE:
			rc = icp_hd_set_tune_value(&cfg_data);
			break;
#endif
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_REFLECT
		case CFG_SET_REFLECT:
			rc = icp_hd_set_reflect(cfg_data.cfg.reflect);
			SKYCDBG("icp_hd_set_reflect OK! rc = [%d], cfg_data.cfg.reflect =[%d]\n", rc, cfg_data.cfg.reflect);
			break;
#endif
#if 1 //def F_PANTECH_CAMERA_ADD_CFG_DIMENSION
	case CFG_SET_DIMENSION:
			rc = icp_hd_set_dimension(&cfg_data.cfg.dimension);
			SKYCDBG("%s: CFG_SET_DIMENSION, rc=%d\n", __func__, rc);
			break;
#endif
		default:
			rc = -EINVAL;
			SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
			break;
		}

	/* up(&icp_hd_sem); */

	return rc;
}

int icp_hd_sensor_release(void)
{
	int rc = 0;
	
	SKYCDBG("%s start\n",__func__);
	/* down(&icp_hd_sem); */
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_LED_MODE	
	nbrightness = 0;
	icp_hd_set_led_mode(ICP_HD_CFG_LED_MODE_OFF);
#endif	
#if 1 //def F_PANTECH_CAMERA_FIX_CFG_SCENE_MODE
       old_scene_value = ICP_HD_CFG_SCENE_MODE_OFF;
#endif
#if 1 //def F_PANTECH_CAMERA_ADD_CFG_ANTISHAKE
	is_antishake = ICP_HD_CFG_ANTISHAKE_OFF;
#endif	
	icp_hd_reset(icp_hd_ctrl->sensordata, OFF);
#ifdef F_ICP_HD_POWER	
	SKYCERR(" icp_hd_sensor_release E\n");	
	rc = icp_hd_power(OFF);
	if (rc) {
		SKYCERR(" icp_hd_power failed rc=%d\n",rc);		
	}
#endif

	kfree(icp_hd_ctrl);
	icp_hd_ctrl = NULL;
	/* up(&icp_hd_sem); */
#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE
	//tunning value loading check flag
	icp_hd_tup_state = ICP_HD_TUNE_STATE_NONE;
	//icp_hd_done_set_tune_load = FALSE;
	//icp_hd_done_set_tune_value = FALSE;
	//icp_hd_tup_mode = ICP_HD_TUNE_STATE_TUNNING_MODE_OFF;
#endif

	SKYCDBG("%s end rc = %d\n",__func__, rc);
	return rc;
}

static int icp_hd_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int rc = 0;
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		rc = -ENOTSUPP;
		goto probe_failure;
	}

	icp_hd_sensorw = kzalloc(sizeof(struct icp_hd_work), GFP_KERNEL);
	if (!icp_hd_sensorw) {
		rc = -ENOMEM;
		goto probe_failure;
	}

	i2c_set_clientdata(client, icp_hd_sensorw);
	icp_hd_init_client(client);
	icp_hd_client = client;

	SKYCDBG("icp_hd_probe succeeded!\n");

	return rc;

probe_failure:
	kfree(icp_hd_sensorw);
	icp_hd_sensorw = NULL;
	SKYCERR("icp_hd_probe failed!\n");
	return rc;
}

static const struct i2c_device_id icp_hd_i2c_id[] = {
	{ "icp_hd", 0},
	{ },
};

static struct i2c_driver icp_hd_i2c_driver = {
	.id_table = icp_hd_i2c_id,
	.probe  = icp_hd_i2c_probe,
	.remove = __exit_p(icp_hd_i2c_remove),
	.driver = {
		.name = "icp_hd",
	},
};

static int32_t icp_hd_init_i2c(void)
{
	int32_t rc = 0;

	SKYCDBG("%s start\n",__func__);

	rc = i2c_add_driver(&icp_hd_i2c_driver);
	SKYCDBG("%s icp_hd_i2c_driver rc = %d\n",__func__, rc);
	if (IS_ERR_VALUE(rc))
		goto init_i2c_fail;

	SKYCDBG("%s end\n",__func__);
	return 0;

init_i2c_fail:
	SKYCERR("%s failed! (%d)\n", __func__, rc);
	return rc;
}

static int icp_hd_sensor_probe(const struct msm_camera_sensor_info *info,
				struct msm_sensor_ctrl *s)
{
	int rc = 0;
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);

	rc = icp_hd_init_i2c();	
	if (rc < 0 || icp_hd_client == NULL)
	{
		SKYCERR("%s rc = %d, icp_hd_client = %x\n",__func__, rc, (unsigned int)icp_hd_client);
		goto probe_init_fail;
	}
#if 0
	/* Input MCLK = 24MHz */
	msm_camio_clk_rate_set(24000000);
	msleep(5);

	rc = icp_hd_sensor_init_probe(info);
	if (rc < 0)
		goto probe_done;
#endif	
	//icp_hd_vreg_init();

	s->s_init = icp_hd_sensor_init;
	s->s_release = icp_hd_sensor_release;
	s->s_config  = icp_hd_sensor_config;	
	s->s_camera_type = BACK_CAMERA_2D;
	s->s_mount_angle = 90;//0;

	SKYCDBG("%s %s Success!:%d\n", __FILE__, __func__, __LINE__);
	return rc;

probe_init_fail:
	SKYCERR("%s %s Failed!:%d\n", __FILE__, __func__, __LINE__);
	return rc;
}

static int __icp_hd_probe(struct platform_device *pdev)
{
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
	return msm_camera_drv_start(pdev, icp_hd_sensor_probe);
}

static struct platform_driver msm_camera_driver = {
	.probe = __icp_hd_probe,
	.driver = {
		.name = "msm_camera_icp_hd",
		.owner = THIS_MODULE,
	},
};

static int __init icp_hd_init(void)
{
	SKYCDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
	return platform_driver_register(&msm_camera_driver);
}

module_init(icp_hd_init);
