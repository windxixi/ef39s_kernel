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

#ifndef ICP_HD_H
#define ICP_HD_H

#include <linux/types.h>
#include <mach/camera.h>

//for camera retry
#define ICP_HD_MODULE_ABNORMAL_OPERATION_DEFENCE
#ifdef ICP_HD_MODULE_ABNORMAL_OPERATION_DEFENCE
#define ICP_HD_REG_POLLING_ERROR 3
#endif

//#define BURST_MODE_4096_BYTE // icp_hd REV2 모듈에서 4096 BYTE BURST_MODE test
//#define BURST_MODE_64_BYTE // icp_hd REV2 모듈에서 4096 BYTE BURST_MODE test
//#define BURST_MODE_64_TO_4096_BYTE // icp_hd REV2 모듈에서 4096 BYTE BURST_MODE test
//#define BURST_MODE_4096_BYTE_CAPTURE_12FPS // 110322 캡쳐 12FPS로 온 세팅
//#define BURST_MODE_4096_BYTE_CAPTURE_10FPS // 110323 캡쳐 10FPS로 온 세팅
//#define BURST_MODE_4096_BYTE_CAPTURE_11FPS // 110329 캡쳐 11FPS로 온 세팅 및 진입시간 단축 코드 + 1080 30FPS
//#define BURST_MODE_8192_BYTE_CAPTURE_11FPS // 110329 캡쳐 11FPS로 온 세팅 및 진입시간 단축 코드 + 1080 30FPS + 8192 burst mode
//#define BURST_MODE_8192_BYTE_COMPRESS_INIT // 110331 캡쳐 11FPS로 온 세팅 및 compressed initial data 적용
//#define BURST_MODE_8192_BYTE_COMPRESS_INIT_OTP // 110331 캡쳐 11FPS로 온 세팅 및 compressed initial data 적용 + OTP
//#define BURST_MODE_8192_BYTE_COMPRESS_INIT_OTP_TUNE_UPDATE //110408 야외촬영을 위한 튜닝값 적용 
#define BURST_MODE_8192_BYTE_COMPRESS_INIT_NODATA //110415 No preview display setting reg data befroe init setting values
// #define DEBUG_I2C_VALUE

extern struct icp_hd_reg icp_hd_regs;

enum icp_hd_width
{
	DWORD_LEN,
	WORD_LEN,
	TRIPLE_LEN,
	BYTE_LEN,
	ZERO_LEN,
	POLL_MCU_VAR, //polling을 빠져나오는 조건이 정해준 값과 같을 경우
	POLL_MCU_VAR_BYTE, // 1 Byte polling
	POLL_MCU_VAR_NOT // polling을 빠져나오는 조건이 정해준 값과 같지 않을 경우 100818_psj
	,BURST_WORD_1
	,BURST_WORD_2
	,BURST_WORD_3
	,BURST_WORD_4
	,BURST_WORD_5
	,BURST_WORD_6
	,BURST_WORD_7
	,BURST_WORD_8
	,BURST_WORD_9
	,BURST_WORD_10
	,BURST_WORD_11
	,BURST_WORD_12
	,BURST_WORD_13
};

struct icp_hd_i2c_reg_conf
{
	unsigned short waddr;
	unsigned int dwdata;
	enum icp_hd_width width;
	unsigned short mdelay_time;
};

#define ICP_HD_CFG_BRIGHT_MAX_PARAMS 			1
#define ICP_HD_CFG_WB_MAX_PARAMS 				1
#define ICP_HD_CFG_EFFECT_MAX_PARAMS 			1
#define ICP_HD_CFG_EXPOSURE_MAX_PARAMS 			1
#define ICP_HD_CFG_PREVIEW_FPS_MAX_PARAMS 		3
#define ICP_HD_CFG_FLICKER_MAX_PARAMS 			5//17
#define ICP_HD_CFG_ANTISHAKE_MAX_PARAMS 		2
#define ICP_HD_CFG_FOCUS_STEP_MAX_PARAMS 		5
#define ICP_HD_CFG_AUTOFOCUS_TRIGGER_MAX_PARAMS 		1
#define ICP_HD_CFG_SCENE_MODE_MAX_PARAMS 		13// 2
#define ICP_HD_CFG_SCENE_MODE_OFF_MAX_PARAMS 	3
#define ICP_HD_CFG_REFLECT_MAX_PARAMS 			2//6

struct icp_hd_reg
{
	/* Init Paramter */
	const struct icp_hd_i2c_reg_conf * init_parm;
	uint16_t init_parm_size;
//burst test
	/*const*/ unsigned char *init_burst_settings_1;
	uint16_t 	init_burst_settings_size_1;
	/*const*/ unsigned char *init_burst_settings_2;
	uint16_t 	init_burst_settings_size_2;
	/*const*/ unsigned char *init_burst_settings_3;
	uint16_t 	init_burst_settings_size_3;
	/*const*/ unsigned char *init_burst_settings_4;
	uint16_t 	init_burst_settings_size_4;
	/*const*/ unsigned char *init_burst_settings_5;
	uint16_t 	init_burst_settings_size_5;
	/*const*/ unsigned char *init_burst_settings_6;
	uint16_t 	init_burst_settings_size_6;
	/*const*/ unsigned char *init_burst_settings_7;
	uint16_t 	init_burst_settings_size_7;
	/*const*/ unsigned char *init_burst_settings_8;
	uint16_t 	init_burst_settings_size_8;
	/*const*/ unsigned char *init_burst_settings_9;
	uint16_t 	init_burst_settings_size_9;
	/*const*/ unsigned char *init_burst_settings_10;
	uint16_t 	init_burst_settings_size_10;
	/*const*/ unsigned char *init_burst_settings_11;
	uint16_t 	init_burst_settings_size_11;
	/*const*/ unsigned char *init_burst_settings_12;
	uint16_t 	init_burst_settings_size_12;
	/*const*/ unsigned char *init_burst_settings_13;
	uint16_t 	init_burst_settings_size_13;
	
	const struct icp_hd_i2c_reg_conf *preview_cfg_settings;
	uint16_t preview_cfg_settings_size;
//preview 1080p	
	const struct icp_hd_i2c_reg_conf *preview_1920_1080_cfg_settings;
	uint16_t preview_1920_1080_cfg_settings_size;
	const struct icp_hd_i2c_reg_conf *preview_1280_960_cfg_settings;
	uint16_t preview_1280_960_cfg_settings_size;
	const struct icp_hd_i2c_reg_conf *snapshot_cfg_settings;
	uint16_t snapshot_cfg_settings_size;
//AF, AWB stable time for FLASH auto 	mode
	const struct icp_hd_i2c_reg_conf *half_sutter_cfg_settings;
	uint16_t half_sutter_cfg_settings_size;
//preview frame rate
	const struct icp_hd_i2c_reg_conf (*preview_fps_cfg_settings)[ICP_HD_CFG_PREVIEW_FPS_MAX_PARAMS];
	uint16_t preview_fps_cfg_settings_size;
	
//effect
	const struct icp_hd_i2c_reg_conf (*effect_cfg_settings)[ICP_HD_CFG_EFFECT_MAX_PARAMS];
	uint16_t effect_cfg_settings_size;

//wb	
	const struct icp_hd_i2c_reg_conf (*wb_cfg_settings)[ICP_HD_CFG_WB_MAX_PARAMS];
	uint16_t wb_cfg_settings_size;

//brightness	
	const struct icp_hd_i2c_reg_conf (*bright_cfg_settings)[ICP_HD_CFG_BRIGHT_MAX_PARAMS];
	uint16_t bright_cfg_settings_size;
	
//exposure	
	const struct icp_hd_i2c_reg_conf (*exposure_cfg_settings)[ICP_HD_CFG_EXPOSURE_MAX_PARAMS];
	uint16_t exposure_cfg_settings_size;

//flicker
	const struct icp_hd_i2c_reg_conf (*flicker_cfg_settings)[ICP_HD_CFG_FLICKER_MAX_PARAMS];
	uint16_t flicker_cfg_settings_size;	

//antishake
	const struct icp_hd_i2c_reg_conf (*antishake_cfg_settings)[ICP_HD_CFG_ANTISHAKE_MAX_PARAMS];
	uint16_t antishake_cfg_settings_size;

//manual foucs
	const struct icp_hd_i2c_reg_conf (*focus_step_cfg_settings)[ICP_HD_CFG_FOCUS_STEP_MAX_PARAMS];
	uint16_t focus_step_cfg_settings_size;

//autofocus
	const struct icp_hd_i2c_reg_conf *autofocus_trigger_cfg_settings;
	uint16_t autofocus_trigger_cfg_settings_size;

        const struct icp_hd_i2c_reg_conf *autofocus_rect_cfg_settings;
	uint16_t autofocus_rect_cfg_settings_size;

//scene mode
	const struct icp_hd_i2c_reg_conf (*scene_mode_cfg_settings)[ICP_HD_CFG_SCENE_MODE_MAX_PARAMS];
	uint16_t scene_mode_cfg_settings_size;

//scene mode off
	const struct icp_hd_i2c_reg_conf (*scene_mode_off_cfg_settings)[ICP_HD_CFG_SCENE_MODE_OFF_MAX_PARAMS];
	uint16_t scene_mode_off_cfg_settings_size;
//reflect
	const struct icp_hd_i2c_reg_conf (*reflect_cfg_settings)[ICP_HD_CFG_REFLECT_MAX_PARAMS];
	uint16_t reflect_cfg_settings_size;

};

#endif /* ICP_HD_H */
