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

#ifndef CE1612_H
#define CE1612_H

#include <linux/types.h>
#include <mach/camera.h>


extern struct ce1612_reg ce1612_regs;

#define CE1612_EFFECT_MAX	11
#define CE1612_WHITEBALANCE_MAX 6
#define CE1612_BRIGHTNESS_MAX 9
#define CE1612_SCENE_MAX 11
#define CE1612_EXPOSURE_MAX 4
#define CE1612_FLICKER_MAX 4
#define CE1612_ISO_MAX 5
 
enum ce1612_width
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

typedef enum {
    CE1612_CFG_LED_MODE_OFF,
    CE1612_CFG_LED_MODE_AUTO,
    CE1612_CFG_LED_MODE_ON,
    CE1612_CFG_LED_MODE_MOVIE,
    CE1612_CFG_LED_MODE_FLASH,
    CE1612_CFG_LED_MODE_MAX
} CE1612_cfg_led_mode_e;

typedef enum {
    CE1612_CFG_SCENE_MODE_OFF,
    CE1612_CFG_SCENE_MODE_AUTO,		
    CE1612_CFG_SCENE_MODE_PORTRAIT,
    CE1612_CFG_SCENE_MODE_LANDSCAPE,
    CE1612_CFG_SCENE_MODE_INDOOR,
    CE1612_CFG_SCENE_MODE_SPORTS,
    CE1612_CFG_SCENE_MODE_NIGHT,
    CE1612_CFG_SCENE_MODE_BEACH,
    CE1612_CFG_SCENE_MODE_WINTER,
    CE1612_CFG_SCENE_MODE_SUNSET,
    CE1612_CFG_SCENE_MODE_TEXT,
    CE1612_CFG_SCENE_MODE_PARTY,
    CE1612_CFG_SCENE_MODE_MAX,
} CE1612_cfg_scene_mode_e;

typedef enum {
    CE1612_CFG_FOCUS_STEP_0,
    CE1612_CFG_FOCUS_STEP_1,
    CE1612_CFG_FOCUS_STEP_2,
    CE1612_CFG_FOCUS_STEP_3,
    CE1612_CFG_FOCUS_STEP_4,
    CE1612_CFG_FOCUS_STEP_5,
    CE1612_CFG_FOCUS_STEP_6,
    CE1612_CFG_FOCUS_STEP_7,
    CE1612_CFG_FOCUS_STEP_8,
    CE1612_CFG_FOCUS_STEP_9,
    CE1612_CFG_FOCUS_STEP_10,
    CE1612_CFG_FOCUS_STEP_MAX
} CE1612_cfg_focus_step_e;

typedef enum {
    CE1612_CFG_ANTISHAKE_OFF,
    CE1612_CFG_ANTISHAKE_ON,
    CE1612_CFG_ANTISHAKE_MAX
} CE1612_cfg_antishake_e;

typedef enum {
    CE1612_CFG_EXPOSURE_NORMAL,
    CE1612_CFG_EXPOSURE_CENTER,
    CE1612_CFG_EXPOSURE_AVERAGE,
    CE1612_CFG_EXPOSURE_SPOT,
    CE1612_CFG_EXPOSURE_MAX
} CE1612_cfg_exposure_e;

typedef enum {
    CE1612_CFG_REFLECT_NONE,
    CE1612_CFG_REFLECT_MIRROR,
    CE1612_CFG_REFLECT_WATER,
    CE1612_CFG_REFLECT_MIRROR_WATER,
    CE1612_CFG_REFLECT_MAX
} CE1612_cfg_reflect_e;

typedef enum {
    CE1612_CFG_FLICKER_OFF,
    CE1612_CFG_FLICKER_60HZ,
    CE1612_CFG_FLICKER_50HZ,
    CE1612_CFG_FLICKER_AUTO,
    CE1612_CFG_FLICKER_MAX
} CE1612_cfg_flicker_e;

struct ce1612_i2c_reg_conf
{
	unsigned char cmd;
	unsigned char data[10];
//	enum ce1612_width width;
	unsigned int width;
	unsigned short mdelay_time;
};

#define CE1612_CFG_BRIGHT_MAX_PARAMS 			1
#define CE1612_CFG_WB_MAX_PARAMS 				1
#define CE1612_CFG_EFFECT_MAX_PARAMS 			1
#define CE1612_CFG_EXPOSURE_MAX_PARAMS 			1
#define CE1612_CFG_PREVIEW_FPS_MAX_PARAMS 		12//5
#define CE1612_CFG_FLICKER_MAX_PARAMS 			5//17
#define CE1612_CFG_ANTISHAKE_MAX_PARAMS 		2
#define CE1612_CFG_FOCUS_STEP_MAX_PARAMS 		5
#define CE1612_CFG_AUTOFOCUS_TRIGGER_MAX_PARAMS 		1
#define CE1612_CFG_SCENE_MODE_MAX_PARAMS 		2
#define CE1612_CFG_REFLECT_MAX_PARAMS 			2//6

struct ce1612_reg
{
	/* Init Paramter */
	const struct ce1612_i2c_reg_conf * init_parm;
	uint16_t init_parm_size;
	
	const struct ce1612_i2c_reg_conf *preview_cfg_settings;
	uint16_t preview_cfg_settings_size;
//preview 1080p	
	const struct ce1612_i2c_reg_conf *preview_1920_1080_cfg_settings;
	uint16_t preview_1920_1080_cfg_settings_size;
	const struct ce1612_i2c_reg_conf *preview_1280_960_cfg_settings;
	uint16_t preview_1280_960_cfg_settings_size;
	const struct ce1612_i2c_reg_conf *snapshot_cfg_settings;
	uint16_t snapshot_cfg_settings_size;

//preview frame rate
	const struct ce1612_i2c_reg_conf (*preview_fps_cfg_settings)[CE1612_CFG_PREVIEW_FPS_MAX_PARAMS];
	uint16_t preview_fps_cfg_settings_size;
	
//effect
	const uint8_t * effect_data;

//wb	
	const uint8_t * wb_data;

//brightness	
	const uint8_t * bright_data;

//scene mode
	const uint8_t * scene_data;
	
//exposure	
	const uint8_t * exposure_data;

//flicker
	const uint8_t * flicker_data;

//iso
	const uint8_t * iso_data;

//antishake
	const struct ce1612_i2c_reg_conf (*antishake_cfg_settings)[CE1612_CFG_ANTISHAKE_MAX_PARAMS];
	uint16_t antishake_cfg_settings_size;

//manual foucs
	const struct ce1612_i2c_reg_conf (*focus_step_cfg_settings)[CE1612_CFG_FOCUS_STEP_MAX_PARAMS];
	uint16_t focus_step_cfg_settings_size;

//autofocus
	const struct ce1612_i2c_reg_conf *autofocus_trigger_cfg_settings;
	uint16_t autofocus_trigger_cfg_settings_size;

        const struct ce1612_i2c_reg_conf *autofocus_rect_cfg_settings;
	uint16_t autofocus_rect_cfg_settings_size;

//reflect
	const struct ce1612_i2c_reg_conf (*reflect_cfg_settings)[CE1612_CFG_REFLECT_MAX_PARAMS];
	uint16_t reflect_cfg_settings_size;

};

#endif /* CE1612_H */
