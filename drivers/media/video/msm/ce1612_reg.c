/* Copyright (c) 2010, PANTECH. All rights reserved.
 */
 
#include "ce1612.h"

//#define FULL_SIZE_SET //최초에 받은 13000줄  3264*2448 세팅 -> MCLK를 12M로 세팅해도 preview안됨 camif error 발생
//#define _640_480_SIZE_SET // aptina에서 받은 24000줄 640*480 세팅 -> MCLK를 12M 세팅하면 preview됨
#define _1280_960_SIZE_SET // aptina에서 받은 3만줄 1280*960 세팅

/* register configration */
static const struct ce1612_i2c_reg_conf ce1612_init_parm[] =
{
	{0xF0, {0x0,}, 0, 70},
	{0x54, {0x1C, 0x01, 0x00,}, 3, 0},
	{0x73, {0x00, 0x00, 0x01, 0x01,}, 4, 0},
};


static const uint8_t ce1612_effect_data[CE1612_EFFECT_MAX] = {
	0x00, 	// none
	0x01,	// mono	
	0x02, 	// negative
	0x00, 	// solarize 미지원
	0x03, 	// sepia
	0x00, 	// posterize : have to set 3DH, 07H command
	0x04, 	//whiteboard
	0x06, 	// blackboard
	0x08,	// aqua
	0x05, 	//whiteboard
	0x07 	// blackboard
};

static const uint8_t ce1612_wb_data[CE1612_WHITEBALANCE_MAX] = {
	0x00, 	// AWB
	0x00, 	// CUSTOM
	0x01,	// INCANDESCENT	
	0x02, 	// FLUORESCENT
	0x03, 	// DAYLIGHT
	0x04, 	// CLOUDY_DAYLIGHT
};

static const uint8_t ce1612_bright_data[CE1612_BRIGHTNESS_MAX] = {
	0x00, 	// -4[EV]
	0x01,	// -3[EV]
	0x02, 	// -4[EV]
	0x03, 	// -1[EV]
	0x04, 	//  0[EV]
	0x05, 	// 1[EV]
	0x06,	// 2[EV]
	0x07, 	// 3[EV]
	0x08, 	// 4[EV]
};

static const uint8_t ce1612_scene_data[CE1612_SCENE_MAX] = {
	0x00, 	// none
	0x00,	// Potrait	
	0x01, 	// LandScape
	0x02, 	// Indoor
	0x03, 	//  Sports
	0x04, 	// Night
	0x05,	// Beach
	0x06, 	// Snow
	0x07, 	// Sunset
	0x08, 	// TEXT
	0x09, 	// Party
};

static const uint8_t ce1612_exposure_data[CE1612_EXPOSURE_MAX] = {
	0x01, 	// CENTER WEIGHT (normal)
	0x00, 	// AVERAGE
	0x01,	// CENTER WEIGHT	
	0x02, 	// SPOT
};

static const uint8_t ce1612_flicker_data[CE1612_FLICKER_MAX] = {
	0x00, 	// OFF
	0x03,	// 60Hz
	0x02, 	// 50HZ
	0x01, 	// AUTO
};

static const uint8_t ce1612_iso_data[CE1612_ISO_MAX] = {
	0x00, 	// AUTO
	0x02,	// 100
	0x03, 	// 200
	0x04, 	// 400
	0x05, 	// 800
};

struct ce1612_reg ce1612_regs = {
	.init_parm = &ce1612_init_parm[0],
	.init_parm_size = ARRAY_SIZE(ce1612_init_parm),

	.effect_data = ce1612_effect_data,
	.wb_data = ce1612_wb_data,
	.bright_data = ce1612_bright_data,
	.scene_data = ce1612_scene_data,
	.exposure_data = ce1612_exposure_data,
	.flicker_data = ce1612_flicker_data,
	.iso_data = ce1612_iso_data,
#if 0 //jjhwang
	.preview_cfg_settings = ce1612_preview_cfg,
	.preview_cfg_settings_size = ARRAY_SIZE(ce1612_preview_cfg),
	.preview_1920_1080_cfg_settings = ce1612_1920_1080_preview_cfg,
	.preview_1920_1080_cfg_settings_size = ARRAY_SIZE(ce1612_1920_1080_preview_cfg),
	.preview_1280_960_cfg_settings = ce1612_1280_960_preview_cfg,
	.preview_1280_960_cfg_settings_size = ARRAY_SIZE(ce1612_1280_960_preview_cfg),
	.snapshot_cfg_settings = ce1612_snapshot_cfg,
	.snapshot_cfg_settings_size = ARRAY_SIZE(ce1612_snapshot_cfg),

//preview frame rate
//	.preview_fps_cfg_settings = &ce1612_preview_fps_settings_cfg,
	.preview_fps_cfg_settings = ce1612_preview_fps_settings_cfg,
	.preview_fps_cfg_settings_size = CE1612_CFG_PREVIEW_FPS_MAX_PARAMS,	

//antishake
	.antishake_cfg_settings = ce1612_antishake_settings_cfg,
	.antishake_cfg_settings_size = CE1612_CFG_ANTISHAKE_MAX_PARAMS,

//manual foucs
	.focus_step_cfg_settings = ce1612_focus_step_settings_cfg,
	.focus_step_cfg_settings_size = CE1612_CFG_FOCUS_STEP_MAX_PARAMS,	
	
//autofocus
	.autofocus_trigger_cfg_settings = ce1612_autofocus_trigger_settings_cfg,
	.autofocus_trigger_cfg_settings_size = ARRAY_SIZE(ce1612_autofocus_trigger_settings_cfg),

//autofoucs rect
	.autofocus_rect_cfg_settings = ce1612_autofocus_rect_settings_cfg,
	.autofocus_rect_cfg_settings_size = ARRAY_SIZE(ce1612_autofocus_rect_settings_cfg),

//scene mode
	.scene_mode_cfg_settings = ce1612_scene_mode_settings_cfg,
	.scene_mode_cfg_settings_size = CE1612_CFG_SCENE_MODE_MAX_PARAMS,
//reflect	
       .reflect_cfg_settings = ce1612_reflect_settings_cfg,
	.reflect_cfg_settings_size = CE1612_CFG_REFLECT_MAX_PARAMS,
#endif
};

