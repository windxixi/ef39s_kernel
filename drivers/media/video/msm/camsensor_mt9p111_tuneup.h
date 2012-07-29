/*
================================================================================
TUNEUP Interface for MICRON MT9P111 CMOS 3.1MP sensor

DESCRIPTION
    This file contains the definitions needed for the camera tuneup interface.

Copyright (c) 2007 by PANTECH, Incorporated. All Rights Reserved.
================================================================================
*/

#if !defined(__CAMSENSOR_MT9P111_TUNEUP_H__)
#define __CAMSENSOR_MT9P111_TUNEUP_H__
#include <linux/types.h>
/*
================================================================================
INCLUDE FILES
================================================================================
*/
//#include "camsensor_mt9p111_config_ef13s.h"

/*
================================================================================
GLOBAL FEATURES
================================================================================
*/

/*
================================================================================
GLOBAL CONSTANTS
================================================================================
*/

//#define MT9P111_TUP_FNAME       "camsensor_mt9p111_tuneup_ef13s.txt"

#define MT9P111_TUP_INIT_MARK            "MT9P111_INIT"

#define MT9P111_TUP_WB_AUTO_MARK         "MT9P111_WB_AUTO"
#define MT9P111_TUP_WB_CLOUDY_MARK       "MT9P111_WB_CLOUDY"
#define MT9P111_TUP_WB_DAYLIGHT_MARK     "MT9P111_WB_DAYLIGHT"
#define MT9P111_TUP_WB_FLUORESCENT_MARK  "MT9P111_WB_FLUORESCENT"
#define MT9P111_TUP_WB_INCANDESCENT_MARK "MT9P111_WB_INCANDESCENT"

#define MT9P111_TUP_BRIGHT_M5_MARK       "MT9P111_BRIGHT_M5"
#define MT9P111_TUP_BRIGHT_M4_MARK       "MT9P111_BRIGHT_M4"
#define MT9P111_TUP_BRIGHT_M3_MARK       "MT9P111_BRIGHT_M3"
#define MT9P111_TUP_BRIGHT_M2_MARK       "MT9P111_BRIGHT_M2"
#define MT9P111_TUP_BRIGHT_M1_MARK       "MT9P111_BRIGHT_M1"
#define MT9P111_TUP_BRIGHT_0_MARK        "MT9P111_BRIGHT_0"
#define MT9P111_TUP_BRIGHT_P1_MARK       "MT9P111_BRIGHT_P1"
#define MT9P111_TUP_BRIGHT_P2_MARK       "MT9P111_BRIGHT_P2"
#define MT9P111_TUP_BRIGHT_P3_MARK       "MT9P111_BRIGHT_P3"
#define MT9P111_TUP_BRIGHT_P4_MARK       "MT9P111_BRIGHT_P4"
#define MT9P111_TUP_BRIGHT_P5_MARK       "MT9P111_BRIGHT_P5"

#define MT9P111_TUP_EXPOSURE_NORMAL      "MT9P111_EXPOSURE_NORMAL"
#define MT9P111_TUP_EXPOSURE_SPOT        "MT9P111_EXPOSURE_SPOT"
#define MT9P111_TUP_EXPOSURE_AVERAGE     "MT9P111_EXPOSURE_AVERAGE"

#define MT9P111_TUP_FFPS_5_MARK   "MT9P111_FFPS_5"
#define MT9P111_TUP_FFPS_6_MARK   "MT9P111_FFPS_6"
#define MT9P111_TUP_FFPS_7_MARK   "MT9P111_FFPS_7"
#define MT9P111_TUP_FFPS_8_MARK   "MT9P111_FFPS_8"
#define MT9P111_TUP_FFPS_9_MARK   "MT9P111_FFPS_9"
#define MT9P111_TUP_FFPS_10_MARK  "MT9P111_FFPS_10"
#define MT9P111_TUP_FFPS_11_MARK  "MT9P111_FFPS_11"
#define MT9P111_TUP_FFPS_12_MARK  "MT9P111_FFPS_12"
#define MT9P111_TUP_FFPS_13_MARK  "MT9P111_FFPS_13"
#define MT9P111_TUP_FFPS_14_MARK  "MT9P111_FFPS_14"
#define MT9P111_TUP_FFPS_15_MARK  "MT9P111_FFPS_15"

#define MT9P111_TUP_REFLECT_NONE_MARK   "MT9P111_REFLECT_NONE"
#define MT9P111_TUP_REFLECT_MIRROR_MARK "MT9P111_REFLECT_MIRROR"
#define MT9P111_TUP_REFLECT_WATER_MARK  "MT9P111_REFLECT_WATER"
#define MT9P111_TUP_REFLECT_MIRROR_WATER_MARK "MT9P111_REFLECT_MIRROR_WATER"

#define MT9P111_TUP_EFFECT_NONE_MARK     "MT9P111_EFFECT_NONE"
#define MT9P111_TUP_EFFECT_GRAY_MARK     "MT9P111_EFFECT_GRAY"
#define MT9P111_TUP_EFFECT_GREEN_MARK    "MT9P111_EFFECT_GREEN"
#define MT9P111_TUP_EFFECT_COOL_MARK     "MT9P111_EFFECT_COOL"
#define MT9P111_TUP_EFFECT_YELLOW_MARK   "MT9P111_EFFECT_YELLOW"
#define MT9P111_TUP_EFFECT_SEPIA_MARK    "MT9P111_EFFECT_SEPIA"
#define MT9P111_TUP_EFFECT_PURPLE_MARK   "MT9P111_EFFECT_PURPLE"
#define MT9P111_TUP_EFFECT_RED_MARK      "MT9P111_EFFECT_RED"
#define MT9P111_TUP_EFFECT_PINK_MARK     "MT9P111_EFFECT_PINK"
#define MT9P111_TUP_EFFECT_AQUA_MARK     "MT9P111_EFFECT_AQUA"
#define MT9P111_TUP_EFFECT_NEGATIVE_MARK "MT9P111_EFFECT_NEGATIVE"
#define MT9P111_TUP_EFFECT_SOLARIZE_1_MARK "MT9P111_EFFECT_SOLARIZE_1"
#define MT9P111_TUP_EFFECT_SOLARIZE_2_MARK "MT9P111_EFFECT_SOLARIZE_2"

#define MT9P111_TUP_FLICKER_NONE_MARK   "MT9P111_FLICKER_NONE"
#define MT9P111_TUP_FLICKER_60HZ_MARK "MT9P111_FLICKER_60HZ"
#define MT9P111_TUP_FLICKER_50HZ_MARK  "MT9P111_FLICKER_50HZ"
#define MT9P111_TUP_FLICKER_AUTO_MARK "MT9P111_FLICKER_AUTO"

#define MT9P111_TUP_ANTISHAKE_NONE_MARK   "MT9P111_ANTISHAKE_NONE"
#define MT9P111_TUP_ANTISHAKE_ON_MARK "MT9P111_ANTISHAKE_ON"

#define MT9P111_TUP_FOCUS_STEP_0_MARK       "MT9P111_FOCUS_STEP_0"
#define MT9P111_TUP_FOCUS_STEP_1_MARK       "MT9P111_FOCUS_STEP_1"
#define MT9P111_TUP_FOCUS_STEP_2_MARK       "MT9P111_FOCUS_STEP_2"
#define MT9P111_TUP_FOCUS_STEP_3_MARK       "MT9P111_FOCUS_STEP_3"
#define MT9P111_TUP_FOCUS_STEP_4_MARK       "MT9P111_FOCUS_STEP_4"
#define MT9P111_TUP_FOCUS_STEP_5_MARK       "MT9P111_FOCUS_STEP_5"
#define MT9P111_TUP_FOCUS_STEP_6_MARK       "MT9P111_FOCUS_STEP_6"
#define MT9P111_TUP_FOCUS_STEP_7_MARK       "MT9P111_FOCUS_STEP_7"
#define MT9P111_TUP_FOCUS_STEP_8_MARK       "MT9P111_FOCUS_STEP_8"
#define MT9P111_TUP_FOCUS_STEP_9_MARK       "MT9P111_FOCUS_STEP_9"
#define MT9P111_TUP_FOCUS_STEP_10_MARK       "MT9P111_FOCUS_STEP_10"

#define MT9P111_TUP_SCENE_NONE_MARK     		"MT9P111_SCENE_NONE"
#define MT9P111_TUP_SCENE_BEACH_MARK   		"MT9P111_SCENE_BEACH"
#define MT9P111_TUP_SCENE_INDOOR_MARK    		"MT9P111_SCENE_INDOOR"
#define MT9P111_TUP_SCENE_LANDSCAPE_MARK     "MT9P111_SCENE_LANDSCAPE"
#define MT9P111_TUP_SCENE_NIGHT_MARK   		"MT9P111_SCENE_NIGHT"
#define MT9P111_TUP_SCENE_PARTY_MARK  		"MT9P111_SCENE_PARTY"
#define MT9P111_TUP_SCENE_PORTRAIT_MARK   	"MT9P111_SCENE_PORTRAIT"
#define MT9P111_TUP_SCENE_SNOW_MARK      		"MT9P111_SCENE_SNOW"
#define MT9P111_TUP_SCENE_SPORTS_MARK     	"MT9P111_SCENE_SPORTS"
#define MT9P111_TUP_SCENE_SUNSET_MARK     	"MT9P111_SCENE_SUNSET"
#define MT9P111_TUP_SCENE_TEXT_MARK 			"MT9P111_SCENE_TEXT"

#define MT9P111_TUP_AUTOFOCUS_TRIGGER_MARK 			"MT9P111_AUTOFOCUS_TRIGGER"

#define MT9P111_TUP_AUTOFOCUS_RECT_MARK 			"MT9P111_AUTOFOCUS_RECT"

#define MT9P111_TUP_PATCH_CONFIG_MARK 			"MT9P111_PATCH_CONFIG"

#define MT9P111_TUP_BEGIN_MARK   "@BEGIN"
#define MT9P111_TUP_END_MARK     "@END"
#define MT9P111_TUP_NOP_MARK     "NOP="
#define MT9P111_TUP_REG_MARK     "REG="
#define MT9P111_TUP_REG_DATA_1BYTE_MARK     "REG_D1="
#define MT9P111_TUP_DELAY_MARK   "DELAY="
#define MT9P111_TUP_POLL_REG_MARK       "POLL_REG="
#define MT9P111_TUP_POLL_MCU_VAR_MARK   "POLL_MCU_VAR="


#define MT9P111_CFG_FFPS_MIN   5
#define MT9P111_CFG_FFPS_MAX   15

#define MT9P111_TUP_INIT_MAX_PARAMS      4096
#define MT9P111_TUP_WB_MAX_PARAMS        128
#define MT9P111_TUP_BRIGHT_MAX_PARAMS    128
#define MT9P111_TUP_EXPOSURE_MAX_PARAMS  128
#define MT9P111_TUP_FFPS_MAX_PARAMS      32
#define MT9P111_TUP_REFLECT_MAX_PARAMS   32
#define MT9P111_TUP_EFFECT_MAX_PARAMS    32
#define MT9P111_TUP_FLICKER_MAX_PARAMS  32
#define MT9P111_TUP_ANTISHAKE_MAX_PARAMS      32
#define MT9P111_TUP_FOCUS_STEP_MAX_PARAMS   32
#define MT9P111_TUP_SCENE_MAX_PARAMS    128

#define TRUE  1
#define FALSE 0

 typedef enum {
 #ifndef BRIGHTNESS_STEP_8
    MT9P111_CFG_BRIGHT_M5,
#endif    
    MT9P111_CFG_BRIGHT_M4,
    MT9P111_CFG_BRIGHT_M3,
    MT9P111_CFG_BRIGHT_M2,
    MT9P111_CFG_BRIGHT_M1,
    MT9P111_CFG_BRIGHT_0,
    MT9P111_CFG_BRIGHT_P1,
    MT9P111_CFG_BRIGHT_P2,
    MT9P111_CFG_BRIGHT_P3,
    MT9P111_CFG_BRIGHT_P4,
#ifndef BRIGHTNESS_STEP_8    
    MT9P111_CFG_BRIGHT_P5,
#endif    
    MT9P111_CFG_BRIGHT_MAX
} mt9p111_cfg_bright_e;

 typedef enum {
    MT9P111_CFG_WB_AUTO,
    MT9P111_CFG_WB_CUSTOM,
    MT9P111_CFG_WB_CLOUDY,
    MT9P111_CFG_WB_DAYLIGHT,
    MT9P111_CFG_WB_FLUORESCENT,
    MT9P111_CFG_WB_INCANDESCENT,
    MT9P111_CFG_WB_MAX
} mt9p111_cfg_wb_e;

typedef enum {
    MT9P111_CFG_EFFECT_NONE,
    MT9P111_CFG_EFFECT_GRAY,
    MT9P111_CFG_EFFECT_GREEN,
    MT9P111_CFG_EFFECT_COOL,
    MT9P111_CFG_EFFECT_YELLOW,
    MT9P111_CFG_EFFECT_SEPIA,
    MT9P111_CFG_EFFECT_PURPLE,
    MT9P111_CFG_EFFECT_RED,
    MT9P111_CFG_EFFECT_PINK,
    MT9P111_CFG_EFFECT_AQUA,
    MT9P111_CFG_EFFECT_NEGATIVE,
    MT9P111_CFG_EFFECT_SOLARIZE_1,
    MT9P111_CFG_EFFECT_SOLARIZE_2,
    MT9P111_CFG_EFFECT_MAX
} mt9p111_cfg_effect_e;

 typedef enum {
    MT9P111_CFG_EXPOSURE_NORMAL,
    MT9P111_CFG_EXPOSURE_CENTER,
    MT9P111_CFG_EXPOSURE_AVERAGE,
    MT9P111_CFG_EXPOSURE_SPOT,
    MT9P111_CFG_EXPOSURE_MAX
} mt9p111_cfg_exposure_e;

typedef enum {
    MT9P111_CFG_FLICKER_OFF,
    MT9P111_CFG_FLICKER_60HZ,
    MT9P111_CFG_FLICKER_50HZ,
    MT9P111_CFG_FLICKER_AUTO,
    MT9P111_CFG_FLICKER_MAX
} mt9p111_cfg_flicker_e;

typedef enum {
    MT9P111_CFG_ANTISHAKE_OFF,
    MT9P111_CFG_ANTISHAKE_ON,
    MT9P111_CFG_ANTISHAKE_MAX
} mt9p111_cfg_antishake_e;

 typedef enum {
    MT9P111_CFG_FOCUS_STEP_0,
    MT9P111_CFG_FOCUS_STEP_1,
    MT9P111_CFG_FOCUS_STEP_2,
    MT9P111_CFG_FOCUS_STEP_3,
    MT9P111_CFG_FOCUS_STEP_4,
    MT9P111_CFG_FOCUS_STEP_5,
    MT9P111_CFG_FOCUS_STEP_6,
    MT9P111_CFG_FOCUS_STEP_7,
    MT9P111_CFG_FOCUS_STEP_8,
    MT9P111_CFG_FOCUS_STEP_9,
    MT9P111_CFG_FOCUS_STEP_10,
    MT9P111_CFG_FOCUS_STEP_MAX
} mt9p111_cfg_focus_step_e;

typedef enum {
    MT9P111_CFG_AUTOFOCUS_TRIGGER_NORMAL,
    MT9P111_CFG_AUTOFOCUS_TRIGGER_MACRO,
    MT9P111_CFG_AUTOFOCUS_TRIGGER_AUTO,
    MT9P111_CFG_AUTOFOCUS_TRIGGER_INFINITY,
    MT9P111_CFG_AUTOFOCUS_TRIGGER_MAX
} mt9p111_cfg_autofocus_trigger_e;

typedef enum {
    MT9P111_CFG_SCENE_MODE_OFF,
    MT9P111_CFG_SCENE_MODE_BEACH,
    MT9P111_CFG_SCENE_MODE_INDOOR,
    MT9P111_CFG_SCENE_MODE_LANDSCAPE,
    MT9P111_CFG_SCENE_MODE_NIGHT,
    MT9P111_CFG_SCENE_MODE_PARTY,
    MT9P111_CFG_SCENE_MODE_PORTRAIT,
    MT9P111_CFG_SCENE_MODE_SNOW,
    MT9P111_CFG_SCENE_MODE_SPORTS,
    MT9P111_CFG_SCENE_MODE_SUNSET,
    MT9P111_CFG_SCENE_MODE_TEXT,    
    MT9P111_CFG_SCENE_MODE_MAX,
} mt9p111_cfg_scene_mode_e;

typedef enum {
    MT9P111_CFG_REFLECT_NONE,
    MT9P111_CFG_REFLECT_MIRROR,
    MT9P111_CFG_REFLECT_WATER,
    MT9P111_CFG_REFLECT_MIRROR_WATER,
    MT9P111_CFG_REFLECT_MAX
} mt9p111_cfg_reflect_e;

#ifdef F_PANTECH_CAMERA_TUP_LOAD_FILE

typedef enum
{
   MT9P111_TUNE_STATE_NONE,
   MT9P111_TUNE_STATE_LOAD_VALUE,
   MT9P111_TUNE_STATE_SET_VALUE
} mt9p111_tune_state_type;

typedef enum
{
   MT9P111_TUNE_STATE_TUNNING_MODE_OFF,
   MT9P111_TUNE_STATE_TUNNING_MODE_ON
 } mt9p111_tune_mode_type;

 // PANTECH_CAMERA_PSJ_0100402 : 커널로 넘겨주기 위한 튜닝 파라미터 구조체 생성
 typedef enum {          // <addr>           <data>
    CAMOP_NOP,          // don't care       don't care
    CAMOP_DELAY,        // don't care       delay
    CAMOP_WRITE,        // address          data
    CAMOP_WRITE_DATA1,        // address 2byte          data 1byte일 경우 사용
    CAMOP_WRITE_BIT,    // address          position[15:8], value[7:0] (0 or non-zero)
    CAMOP_POLL_REG,     // 1st, address     mask
                        // 2nd, value       delay[15:8], retry[7:0] 
                        // 'address' 의 값이 'value' 가 아닌 경우, 'delay' 를 'retry' 번 하면서 polling
    CAMOP_POLL_MCU_VAR, // 1st, mcu var     mask
                        // 2nd, value       delay[15:8], retry[7:0] 
                        // 'mcu var' 의 값이 'value' 가 아닌 경우, 'delay' 를 'retry' 번 하면서 polling
    CAMOP_MAX
} mt9p111_cfg_op_e;

 typedef struct {
    mt9p111_cfg_op_e op;
    u_int16_t addr;
    u_int16_t data;
} mt9p111_cfg_param_t;

typedef struct {
    mt9p111_cfg_op_e op;
    unsigned short addr;
    unsigned short mask;
    unsigned short data;
} mt9p111_cfg_param_ext_t;

typedef struct{
	mt9p111_cfg_param_t* mt9p111_tup_init_params;
	mt9p111_cfg_param_t* mt9p111_tup_wb_params;
	mt9p111_cfg_param_t* mt9p111_tup_bright_params;
	mt9p111_cfg_param_t* mt9p111_tup_exposure_params;
	mt9p111_cfg_param_t* mt9p111_tup_ffps_params;
	mt9p111_cfg_param_t* mt9p111_tup_reflect_params;
	mt9p111_cfg_param_t* mt9p111_tup_effect_params;
}mt9p111_tup_params;



/*
================================================================================
GLOBAL MACROS
================================================================================
*/

/*
================================================================================
GLOBAL DATA TYPES
================================================================================
*/
#if 0
 // PANTECH_CAMERA_PSJ_0100402 : 커널로 넘겨주기 위한 튜닝 파라미터 구조체 생성
typedef struct{
	mt9p111_cfg_param_t* mt9p111_tup_init_params;
	mt9p111_cfg_param_t* mt9p111_tup_wb_params;
	mt9p111_cfg_param_t* mt9p111_tup_bright_params;
	mt9p111_cfg_param_t* mt9p111_tup_exposure_params;
	mt9p111_cfg_param_t* mt9p111_tup_ffps_params;
	mt9p111_cfg_param_t* mt9p111_tup_reflect_params;
	mt9p111_cfg_param_t* mt9p111_tup_effect_params;
}mt9p111_tup_params;
#endif

typedef struct {
    u_int32_t num_params;
    mt9p111_cfg_param_t *params;
} mt9p111_params_t;

typedef struct {
    mt9p111_params_t init;
    mt9p111_params_t wb[MT9P111_CFG_WB_MAX];
    mt9p111_params_t bright[MT9P111_CFG_BRIGHT_MAX];
    mt9p111_params_t exposure[MT9P111_CFG_EXPOSURE_MAX];
    mt9p111_params_t ffps[MT9P111_CFG_FFPS_MAX - MT9P111_CFG_FFPS_MIN + 1];
    mt9p111_params_t reflect[MT9P111_CFG_REFLECT_MAX];
    mt9p111_params_t effect[MT9P111_CFG_EFFECT_MAX];
    mt9p111_params_t flicker[MT9P111_CFG_FLICKER_MAX];
    mt9p111_params_t antishake[MT9P111_CFG_ANTISHAKE_MAX];
    mt9p111_params_t focus_step[MT9P111_CFG_FOCUS_STEP_MAX];
    mt9p111_params_t scene[MT9P111_CFG_SCENE_MODE_MAX];
} mt9p111_params_tbl_t;
/*
================================================================================
GLOBAL VARIABLES
================================================================================
*/

/*
================================================================================
GLOBAL API DECLARATIONS
================================================================================
*/

extern void camsensor_mt9p111_tup_init (int8_t *stream);
extern u_int32_t camsensor_mt9p111_tup_get_init_params (mt9p111_cfg_param_t **init_params);
extern u_int32_t camsensor_mt9p111_tup_get_wb_params (mt9p111_cfg_wb_e wb, mt9p111_cfg_param_t **wb_params);
extern u_int32_t camsensor_mt9p111_tup_get_bright_params (mt9p111_cfg_bright_e bright, mt9p111_cfg_param_t **bright_params);
extern u_int32_t camsensor_mt9p111_tup_get_exposure_params (mt9p111_cfg_exposure_e exposure, mt9p111_cfg_param_t **exposure_params);
extern u_int32_t camsensor_mt9p111_tup_get_ffps_params (u_int32_t ffps, mt9p111_cfg_param_t **ffps_params);
extern u_int32_t camsensor_mt9p111_tup_get_reflect_params (mt9p111_cfg_reflect_e reflect, mt9p111_cfg_param_t **reflect_params);
extern u_int32_t camsensor_mt9p111_tup_get_effect_params (mt9p111_cfg_effect_e effect, mt9p111_cfg_param_t **effect_params);
extern u_int32_t camsensor_mt9p111_tup_get_flicker_params (mt9p111_cfg_flicker_e flicker, mt9p111_cfg_param_t **flicker_params);
extern u_int32_t camsensor_mt9p111_tup_get_antishake_params (mt9p111_cfg_antishake_e antishake, mt9p111_cfg_param_t **antishake_params);
extern u_int32_t camsensor_mt9p111_tup_get_focus_step_params (mt9p111_cfg_focus_step_e focus_step, mt9p111_cfg_param_t **focus_step_params);
extern u_int32_t camsensor_mt9p111_tup_get_scene_params (mt9p111_cfg_scene_mode_e effect, mt9p111_cfg_param_t **scene_params);

#endif
#endif /* __CAMSENSOR_MT9P111_TUNEUP_H__ */
