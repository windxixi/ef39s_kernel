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

#ifndef S5K6AAFX13_H
#define S5K6AAFX13_H

#include <linux/types.h>
#include <mach/camera.h>

#if 1 //wsyang_temp //F_PANTECH_CAMERA_ADD_BURST_TUNING
#define BURST_MODE_INIT // 110412 전면카메라 init시 0x0F12 번지에 write하는 값들을 BURST모드와 한다.
#endif

extern struct s5k6aafx13_reg s5k6aafx13_regs;

enum s5k6aafx13_width
{
	WORD_LEN,
	TRIPLE_LEN,
	BYTE_LEN,
	ZERO_LEN
#ifdef BURST_MODE_INIT
    ,BURST_1,
    BURST_2,
    BURST_3,
    BURST_4,
    BURST_5,
    BURST_6,
    BURST_7,
    BURST_8,
    BURST_9,
    BURST_10,
    BURST_11,
    BURST_12,
    BURST_13,
    BURST_14,
    BURST_15,
    BURST_16,
    BURST_17,
    BURST_18,
    BURST_19,
    BURST_20,
    BURST_21,
    BURST_22,
    BURST_23,
    BURST_24,
    BURST_25,
    BURST_26,
    BURST_27,
    BURST_28,
    BURST_29,
    BURST_30,
    BURST_31,
    BURST_32,
    BURST_33,
    BURST_34,
    BURST_35,
    BURST_36,
    BURST_37,
    BURST_38,
    BURST_39,
    BURST_END
#endif
};

struct s5k6aafx13_i2c_reg_conf
{
	unsigned short waddr;
	unsigned short wdata;
	enum s5k6aafx13_width width;
	unsigned short mdelay_time;
};

#define S5K6AAFX13_CFG_BRIGHT_MAX_PARAMS 		8
#define S5K6AAFX13_CFG_WB_MAX_PARAMS 			15
#define S5K6AAFX13_CFG_EFFECT_MAX_PARAMS 		5
#define S5K6AAFX13_CFG_EXPOSURE_MAX_PARAMS 		35
#define S5K6AAFX13_CFG_PREVIEW_FPS_MAX_PARAMS 	41//28//24
#define S5K6AAFX13_CFG_FLICKER_MAX_PARAMS 		5
#define S5K6AAFX13_CFG_REFLECT_MAX_PARAMS 		20

struct s5k6aafx13_reg
{
	/* Init Paramter */
	const struct s5k6aafx13_i2c_reg_conf * init_parm;
	uint16_t init_parm_size;

	const struct s5k6aafx13_i2c_reg_conf *preview_cfg_settings;
	uint16_t preview_cfg_settings_size;
	const struct s5k6aafx13_i2c_reg_conf *snapshot_cfg_settings;
	uint16_t snapshot_cfg_settings_size;

// inside arm processor go	
	const struct s5k6aafx13_i2c_reg_conf *arm_go_cfg_settings;
	uint16_t arm_go_cfg_settings_size;

// S/W standby enter	
	const struct s5k6aafx13_i2c_reg_conf *standby_enter_cfg_settings;
	uint16_t standby_enter_cfg_settings_size;

// S/W standby exit	
	const struct s5k6aafx13_i2c_reg_conf *standby_exit_cfg_settings;
	uint16_t standby_exit_cfg_settings_size;

//preview frame rate
	const struct s5k6aafx13_i2c_reg_conf (*preview_fps_cfg_settings)[S5K6AAFX13_CFG_PREVIEW_FPS_MAX_PARAMS];
	uint16_t preview_fps_cfg_settings_size;
	
//effect
	const struct s5k6aafx13_i2c_reg_conf (*effect_cfg_settings)[S5K6AAFX13_CFG_EFFECT_MAX_PARAMS];
	uint16_t effect_cfg_settings_size;

//wb	
	const struct s5k6aafx13_i2c_reg_conf (*wb_cfg_settings)[S5K6AAFX13_CFG_WB_MAX_PARAMS];
	uint16_t wb_cfg_settings_size;

//brightness	
	const struct s5k6aafx13_i2c_reg_conf (*bright_cfg_settings)[S5K6AAFX13_CFG_BRIGHT_MAX_PARAMS];
	uint16_t bright_cfg_settings_size;
	
//exposure	
	const struct s5k6aafx13_i2c_reg_conf (*exposure_cfg_settings)[S5K6AAFX13_CFG_EXPOSURE_MAX_PARAMS];
	uint16_t exposure_cfg_settings_size;

//flicker
	const struct s5k6aafx13_i2c_reg_conf (*flicker_cfg_settings)[S5K6AAFX13_CFG_FLICKER_MAX_PARAMS];
	uint16_t flicker_cfg_settings_size;	

	//flicker
	const struct s5k6aafx13_i2c_reg_conf (*reflect_cfg_settings)[S5K6AAFX13_CFG_REFLECT_MAX_PARAMS];
	uint16_t reflect_cfg_settings_size;

#ifdef BURST_MODE_INIT
    unsigned char *init_burst_settings_1;
    uint16_t    init_burst_settings_size_1;
    unsigned char *init_burst_settings_2;
    uint16_t    init_burst_settings_size_2;
    unsigned char *init_burst_settings_3;
    uint16_t    init_burst_settings_size_3;
    unsigned char *init_burst_settings_4;
    uint16_t    init_burst_settings_size_4;
    unsigned char *init_burst_settings_5;
    uint16_t    init_burst_settings_size_5;
    unsigned char *init_burst_settings_6;
    uint16_t    init_burst_settings_size_6;
    unsigned char *init_burst_settings_7;
    uint16_t    init_burst_settings_size_7;
    unsigned char *init_burst_settings_8;
    uint16_t    init_burst_settings_size_8;
    unsigned char *init_burst_settings_9;
    uint16_t    init_burst_settings_size_9;
    unsigned char *init_burst_settings_10;
    uint16_t    init_burst_settings_size_10;

    unsigned char *init_burst_settings_11;
    uint16_t    init_burst_settings_size_11;
    unsigned char *init_burst_settings_12;
    uint16_t    init_burst_settings_size_12;
    unsigned char *init_burst_settings_13;
    uint16_t    init_burst_settings_size_13;
    unsigned char *init_burst_settings_14;
    uint16_t    init_burst_settings_size_14;
    unsigned char *init_burst_settings_15;
    uint16_t    init_burst_settings_size_15;
    unsigned char *init_burst_settings_16;
    uint16_t    init_burst_settings_size_16;
    unsigned char *init_burst_settings_17;
    uint16_t    init_burst_settings_size_17;
    unsigned char *init_burst_settings_18;
    uint16_t    init_burst_settings_size_18;
    unsigned char *init_burst_settings_19;
    uint16_t    init_burst_settings_size_19;
    unsigned char *init_burst_settings_20;
    uint16_t    init_burst_settings_size_20;

    unsigned char *init_burst_settings_21;
    uint16_t    init_burst_settings_size_21;
    unsigned char *init_burst_settings_22;
    uint16_t    init_burst_settings_size_22;
    unsigned char *init_burst_settings_23;
    uint16_t    init_burst_settings_size_23;
    unsigned char *init_burst_settings_24;
    uint16_t    init_burst_settings_size_24;
    unsigned char *init_burst_settings_25;
    uint16_t    init_burst_settings_size_25;
    unsigned char *init_burst_settings_26;
    uint16_t    init_burst_settings_size_26;
    unsigned char *init_burst_settings_27;
    uint16_t    init_burst_settings_size_27;
    unsigned char *init_burst_settings_28;
    uint16_t    init_burst_settings_size_28;
    unsigned char *init_burst_settings_29;
    uint16_t    init_burst_settings_size_29;
    unsigned char *init_burst_settings_30;
    uint16_t    init_burst_settings_size_30;

    unsigned char *init_burst_settings_31;
    uint16_t    init_burst_settings_size_31;
    unsigned char *init_burst_settings_32;
    uint16_t    init_burst_settings_size_32;
    unsigned char *init_burst_settings_33;
    uint16_t    init_burst_settings_size_33;
    unsigned char *init_burst_settings_34;
    uint16_t    init_burst_settings_size_34;
    unsigned char *init_burst_settings_35;
    uint16_t    init_burst_settings_size_35;
    unsigned char *init_burst_settings_36;
    uint16_t    init_burst_settings_size_36;
    unsigned char *init_burst_settings_37;
    uint16_t    init_burst_settings_size_37;
    unsigned char *init_burst_settings_38;
    uint16_t    init_burst_settings_size_38;    
    unsigned char *init_burst_settings_39;
    uint16_t    init_burst_settings_size_39;
#endif
};

#endif /* S5K6AAFX13_H */
