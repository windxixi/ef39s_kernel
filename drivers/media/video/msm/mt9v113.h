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

#ifndef MT9V113_H
#define MT9V113_H

#include <linux/types.h>
#include <mach/camera.h>

extern struct mt9v113_reg mt9v113_regs;


#define C_PANTECH_CAMERA_MIN_BRIGHTNESS 0
#define C_PANTECH_CAMERA_MAX_BRIGHTNESS 8
#define C_PANTECH_CAMERA_MIN_PREVIEW_FPS	5
#define C_PANTECH_CAMERA_MAX_PREVIEW_FPS	30
#define C_PANTECH_CAMERA_MIN_FOCUS_STEP 0	/* 무한대 (default) */
#define C_PANTECH_CAMERA_MAX_FOCUS_STEP 9	/* 매크로 */

enum mt9v113_width
{
	WORD_LEN,
	TRIPLE_LEN,
	BYTE_LEN,
	ZERO_LEN,  //hhs 20110217
	POLL_REG,
	POLL_MCU_VAR, //polling을 빠져나오는 조건이 정해준 값과 같을 경우
	DELAY_T
};

struct mt9v113_i2c_reg_conf
{
	unsigned short waddr;
	unsigned short wdata;
	enum mt9v113_width width;
	unsigned short mdelay_time;
};

#define MT9V113_CFG_BRIGHT_MAX_PARAMS 		15
#define MT9V113_CFG_WB_MAX_PARAMS 	        12
#define MT9V113_CFG_EFFECT_MAX_PARAMS 		10
#define MT9V113_CFG_EXPOSURE_MAX_PARAMS      8
#define MT9V113_CFG_PREVIEW_FPS_MAX_PARAMS 	28
#define MT9V113_CFG_FLICKER_MAX_PARAMS 		10
#define MT9V113_CFG_REFLECT_MAX_PARAMS 		 8

struct mt9v113_reg
{
	/* Init Paramter */
	const struct mt9v113_i2c_reg_conf * init_parm;
	uint16_t init_parm_size;

	const struct mt9v113_i2c_reg_conf *preview_cfg_settings;
	uint16_t preview_cfg_settings_size;
	const struct mt9v113_i2c_reg_conf *snapshot_cfg_settings;
	uint16_t snapshot_cfg_settings_size;

//preview frame rate
	const struct mt9v113_i2c_reg_conf (*preview_fps_cfg_settings)[MT9V113_CFG_PREVIEW_FPS_MAX_PARAMS];
	uint16_t preview_fps_cfg_settings_size;

//effect
	const struct mt9v113_i2c_reg_conf (*effect_cfg_settings)[MT9V113_CFG_EFFECT_MAX_PARAMS];
	uint16_t effect_cfg_settings_size;

//wb
	const struct mt9v113_i2c_reg_conf (*wb_cfg_settings)[MT9V113_CFG_WB_MAX_PARAMS];
	uint16_t wb_cfg_settings_size;

//brightness
	const struct mt9v113_i2c_reg_conf (*bright_cfg_settings)[MT9V113_CFG_BRIGHT_MAX_PARAMS];
	uint16_t bright_cfg_settings_size;

//exposure
	const struct mt9v113_i2c_reg_conf (*exposure_cfg_settings)[MT9V113_CFG_EXPOSURE_MAX_PARAMS];
	uint16_t exposure_cfg_settings_size;

//flicker
	const struct mt9v113_i2c_reg_conf (*flicker_cfg_settings)[MT9V113_CFG_FLICKER_MAX_PARAMS];
	uint16_t flicker_cfg_settings_size;

	//flicker
	const struct mt9v113_i2c_reg_conf (*reflect_cfg_settings)[MT9V113_CFG_REFLECT_MAX_PARAMS];
	uint16_t reflect_cfg_settings_size;

};

#endif /* S5K6AAFX13_H */
