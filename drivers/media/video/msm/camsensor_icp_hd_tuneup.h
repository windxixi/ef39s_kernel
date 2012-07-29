/*
================================================================================
TUNEUP Interface for MICRON ICP_HD CMOS 3.1MP sensor

DESCRIPTION
    This file contains the definitions needed for the camera tuneup interface.

Copyright (c) 2007 by PANTECH, Incorporated. All Rights Reserved.
================================================================================
*/

#if !defined(__CAMSENSOR_ICP_HD_TUNEUP_H__)
#define __CAMSENSOR_ICP_HD_TUNEUP_H__
#include <linux/types.h>
/*
================================================================================
INCLUDE FILES
================================================================================
*/
//#include "camsensor_ICP_HD_config_ef13s.h"

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

//#define ICP_HD_TUP_FNAME       "camsensor_ICP_HD_tuneup_ef13s.txt"

#define ICP_HD_TUP_INIT_MARK            "ICP_HD_INIT"

#define ICP_HD_TUP_WB_AUTO_MARK         "ICP_HD_WB_AUTO"
#define ICP_HD_TUP_WB_CLOUDY_MARK       "ICP_HD_WB_CLOUDY"
#define ICP_HD_TUP_WB_DAYLIGHT_MARK     "ICP_HD_WB_DAYLIGHT"
#define ICP_HD_TUP_WB_FLUORESCENT_MARK  "ICP_HD_WB_FLUORESCENT"
#define ICP_HD_TUP_WB_INCANDESCENT_MARK "ICP_HD_WB_INCANDESCENT"

#define ICP_HD_TUP_BRIGHT_M5_MARK       "ICP_HD_BRIGHT_M5"
#define ICP_HD_TUP_BRIGHT_M4_MARK       "ICP_HD_BRIGHT_M4"
#define ICP_HD_TUP_BRIGHT_M3_MARK       "ICP_HD_BRIGHT_M3"
#define ICP_HD_TUP_BRIGHT_M2_MARK       "ICP_HD_BRIGHT_M2"
#define ICP_HD_TUP_BRIGHT_M1_MARK       "ICP_HD_BRIGHT_M1"
#define ICP_HD_TUP_BRIGHT_0_MARK        "ICP_HD_BRIGHT_0"
#define ICP_HD_TUP_BRIGHT_P1_MARK       "ICP_HD_BRIGHT_P1"
#define ICP_HD_TUP_BRIGHT_P2_MARK       "ICP_HD_BRIGHT_P2"
#define ICP_HD_TUP_BRIGHT_P3_MARK       "ICP_HD_BRIGHT_P3"
#define ICP_HD_TUP_BRIGHT_P4_MARK       "ICP_HD_BRIGHT_P4"
#define ICP_HD_TUP_BRIGHT_P5_MARK       "ICP_HD_BRIGHT_P5"

#define ICP_HD_TUP_EXPOSURE_NORMAL      "ICP_HD_EXPOSURE_NORMAL"
#define ICP_HD_TUP_EXPOSURE_SPOT        "ICP_HD_EXPOSURE_SPOT"
#define ICP_HD_TUP_EXPOSURE_AVERAGE     "ICP_HD_EXPOSURE_AVERAGE"

#define ICP_HD_TUP_FFPS_5_MARK   "ICP_HD_FFPS_5"
#define ICP_HD_TUP_FFPS_6_MARK   "ICP_HD_FFPS_6"
#define ICP_HD_TUP_FFPS_7_MARK   "ICP_HD_FFPS_7"
#define ICP_HD_TUP_FFPS_8_MARK   "ICP_HD_FFPS_8"
#define ICP_HD_TUP_FFPS_9_MARK   "ICP_HD_FFPS_9"
#define ICP_HD_TUP_FFPS_10_MARK  "ICP_HD_FFPS_10"
#define ICP_HD_TUP_FFPS_11_MARK  "ICP_HD_FFPS_11"
#define ICP_HD_TUP_FFPS_12_MARK  "ICP_HD_FFPS_12"
#define ICP_HD_TUP_FFPS_13_MARK  "ICP_HD_FFPS_13"
#define ICP_HD_TUP_FFPS_14_MARK  "ICP_HD_FFPS_14"
#define ICP_HD_TUP_FFPS_15_MARK  "ICP_HD_FFPS_15"

#define ICP_HD_TUP_REFLECT_NONE_MARK   "ICP_HD_REFLECT_NONE"
#define ICP_HD_TUP_REFLECT_MIRROR_MARK "ICP_HD_REFLECT_MIRROR"
#define ICP_HD_TUP_REFLECT_WATER_MARK  "ICP_HD_REFLECT_WATER"
#define ICP_HD_TUP_REFLECT_MIRROR_WATER_MARK "ICP_HD_REFLECT_MIRROR_WATER"

#define ICP_HD_TUP_EFFECT_NONE_MARK     "ICP_HD_EFFECT_NONE"
#define ICP_HD_TUP_EFFECT_GRAY_MARK     "ICP_HD_EFFECT_GRAY"
#define ICP_HD_TUP_EFFECT_GREEN_MARK    "ICP_HD_EFFECT_GREEN"
#define ICP_HD_TUP_EFFECT_COOL_MARK     "ICP_HD_EFFECT_COOL"
#define ICP_HD_TUP_EFFECT_YELLOW_MARK   "ICP_HD_EFFECT_YELLOW"
#define ICP_HD_TUP_EFFECT_SEPIA_MARK    "ICP_HD_EFFECT_SEPIA"
#define ICP_HD_TUP_EFFECT_PURPLE_MARK   "ICP_HD_EFFECT_PURPLE"
#define ICP_HD_TUP_EFFECT_RED_MARK      "ICP_HD_EFFECT_RED"
#define ICP_HD_TUP_EFFECT_PINK_MARK     "ICP_HD_EFFECT_PINK"
#define ICP_HD_TUP_EFFECT_AQUA_MARK     "ICP_HD_EFFECT_AQUA"
#define ICP_HD_TUP_EFFECT_NEGATIVE_MARK "ICP_HD_EFFECT_NEGATIVE"
#define ICP_HD_TUP_EFFECT_SOLARIZE_1_MARK "ICP_HD_EFFECT_SOLARIZE_1"
#define ICP_HD_TUP_EFFECT_SOLARIZE_2_MARK "ICP_HD_EFFECT_SOLARIZE_2"

#define ICP_HD_TUP_FLICKER_NONE_MARK   "ICP_HD_FLICKER_NONE"
#define ICP_HD_TUP_FLICKER_60HZ_MARK "ICP_HD_FLICKER_60HZ"
#define ICP_HD_TUP_FLICKER_50HZ_MARK  "ICP_HD_FLICKER_50HZ"
#define ICP_HD_TUP_FLICKER_AUTO_MARK "ICP_HD_FLICKER_AUTO"

#define ICP_HD_TUP_ANTISHAKE_NONE_MARK   "ICP_HD_ANTISHAKE_NONE"
#define ICP_HD_TUP_ANTISHAKE_ON_MARK "ICP_HD_ANTISHAKE_ON"

#define ICP_HD_TUP_FOCUS_STEP_0_MARK       "ICP_HD_FOCUS_STEP_0"
#define ICP_HD_TUP_FOCUS_STEP_1_MARK       "ICP_HD_FOCUS_STEP_1"
#define ICP_HD_TUP_FOCUS_STEP_2_MARK       "ICP_HD_FOCUS_STEP_2"
#define ICP_HD_TUP_FOCUS_STEP_3_MARK       "ICP_HD_FOCUS_STEP_3"
#define ICP_HD_TUP_FOCUS_STEP_4_MARK       "ICP_HD_FOCUS_STEP_4"
#define ICP_HD_TUP_FOCUS_STEP_5_MARK       "ICP_HD_FOCUS_STEP_5"
#define ICP_HD_TUP_FOCUS_STEP_6_MARK       "ICP_HD_FOCUS_STEP_6"
#define ICP_HD_TUP_FOCUS_STEP_7_MARK       "ICP_HD_FOCUS_STEP_7"
#define ICP_HD_TUP_FOCUS_STEP_8_MARK       "ICP_HD_FOCUS_STEP_8"
#define ICP_HD_TUP_FOCUS_STEP_9_MARK       "ICP_HD_FOCUS_STEP_9"
#define ICP_HD_TUP_FOCUS_STEP_10_MARK       "ICP_HD_FOCUS_STEP_10"

#define ICP_HD_TUP_SCENE_NONE_MARK     		"ICP_HD_SCENE_NONE"
#define ICP_HD_TUP_SCENE_PORTRAIT_MARK   		"ICP_HD_SCENE_PORTRAIT"
#define ICP_HD_TUP_SCENE_LANDSCAPE_MARK    		"ICP_HD_SCENE_LANDSCAPE"
#define ICP_HD_TUP_SCENE_INDOOR_MARK     "ICP_HD_SCENE_INDOOR"
#define ICP_HD_TUP_SCENE_SPORTS_MARK   		"ICP_HD_SCENE_SPORTS"
#define ICP_HD_TUP_SCENE_NIGHT_MARK  		"ICP_HD_SCENE_NIGHT"
#define ICP_HD_TUP_SCENE_BEACH_MARK   	"ICP_HD_SCENE_BEACH"
#define ICP_HD_TUP_SCENE_WINTER_MARK      		"ICP_HD_SCENE_WINTER"
#define ICP_HD_TUP_SCENE_SUNSET_MARK     	"ICP_HD_SCENE_SUNSET"
#define ICP_HD_TUP_SCENE_TEXT_MARK     	"ICP_HD_SCENE_TEXT"
#define ICP_HD_TUP_SCENE_PARTY_MARK 			"ICP_HD_SCENE_PARTY"

#define ICP_HD_TUP_AUTOFOCUS_TRIGGER_MARK 			"ICP_HD_AUTOFOCUS_TRIGGER"

#define ICP_HD_TUP_AUTOFOCUS_RECT_MARK 			"ICP_HD_AUTOFOCUS_RECT"

#define ICP_HD_TUP_PATCH_CONFIG_MARK 			"ICP_HD_PATCH_CONFIG"

#define ICP_HD_TUP_BEGIN_MARK   "@BEGIN"
#define ICP_HD_TUP_END_MARK     "@END"
#define ICP_HD_TUP_NOP_MARK     "NOP="
#define ICP_HD_TUP_REG_MARK     "REG="
#define ICP_HD_TUP_REG_DATA_1BYTE_MARK     "REG_D1="
#define ICP_HD_TUP_DELAY_MARK   "DELAY="
#define ICP_HD_TUP_POLL_REG_MARK       "POLL_REG="
#define ICP_HD_TUP_POLL_MCU_VAR_MARK   "POLL_MCU_VAR="


#define ICP_HD_CFG_FFPS_MIN   5
#define ICP_HD_CFG_FFPS_MAX   15

#define ICP_HD_TUP_INIT_MAX_PARAMS      4096
#define ICP_HD_TUP_WB_MAX_PARAMS        128
#define ICP_HD_TUP_BRIGHT_MAX_PARAMS    128
#define ICP_HD_TUP_EXPOSURE_MAX_PARAMS  128
#define ICP_HD_TUP_FFPS_MAX_PARAMS      32
#define ICP_HD_TUP_REFLECT_MAX_PARAMS   32
#define ICP_HD_TUP_EFFECT_MAX_PARAMS    32
#define ICP_HD_TUP_FLICKER_MAX_PARAMS  32
#define ICP_HD_TUP_ANTISHAKE_MAX_PARAMS      32
#define ICP_HD_TUP_FOCUS_STEP_MAX_PARAMS   32
#define ICP_HD_TUP_SCENE_MAX_PARAMS    128

#define TRUE  1
#define FALSE 0

 typedef enum {
 #ifndef BRIGHTNESS_STEP_8
    ICP_HD_CFG_BRIGHT_M5,
#endif    
    ICP_HD_CFG_BRIGHT_M4,
    ICP_HD_CFG_BRIGHT_M3,
    ICP_HD_CFG_BRIGHT_M2,
    ICP_HD_CFG_BRIGHT_M1,
    ICP_HD_CFG_BRIGHT_0,
    ICP_HD_CFG_BRIGHT_P1,
    ICP_HD_CFG_BRIGHT_P2,
    ICP_HD_CFG_BRIGHT_P3,
    ICP_HD_CFG_BRIGHT_P4,
#ifndef BRIGHTNESS_STEP_8    
    ICP_HD_CFG_BRIGHT_P5,
#endif    
    ICP_HD_CFG_BRIGHT_MAX
} ICP_HD_cfg_bright_e;

 typedef enum {
    ICP_HD_CFG_LED_MODE_OFF,
    ICP_HD_CFG_LED_MODE_AUTO,
    ICP_HD_CFG_LED_MODE_ON,
    ICP_HD_CFG_LED_MODE_MOVIE,
    ICP_HD_CFG_LED_MODE_SNAP,
    ICP_HD_CFG_LED_MODE_MAX
} ICP_HD_cfg_led_mode_e;

 typedef enum {
    ICP_HD_CFG_WB_AUTO,
    ICP_HD_CFG_WB_CUSTOM,
    ICP_HD_CFG_WB_CLOUDY,
    ICP_HD_CFG_WB_DAYLIGHT,
    ICP_HD_CFG_WB_FLUORESCENT,
    ICP_HD_CFG_WB_INCANDESCENT,
    ICP_HD_CFG_WB_MAX
} ICP_HD_cfg_wb_e;

typedef enum {
    ICP_HD_CFG_EFFECT_NONE,
    ICP_HD_CFG_EFFECT_GRAY,
    ICP_HD_CFG_EFFECT_GREEN,
    ICP_HD_CFG_EFFECT_COOL,
    ICP_HD_CFG_EFFECT_YELLOW,
    ICP_HD_CFG_EFFECT_SEPIA,
    ICP_HD_CFG_EFFECT_PURPLE,
    ICP_HD_CFG_EFFECT_RED,
    ICP_HD_CFG_EFFECT_PINK,
    ICP_HD_CFG_EFFECT_AQUA,
    ICP_HD_CFG_EFFECT_NEGATIVE,
    ICP_HD_CFG_EFFECT_SOLARIZE_1,
    ICP_HD_CFG_EFFECT_SOLARIZE_2,
    ICP_HD_CFG_EFFECT_MAX
} ICP_HD_cfg_effect_e;

 typedef enum {
    ICP_HD_CFG_EXPOSURE_NORMAL,
    ICP_HD_CFG_EXPOSURE_CENTER,
    ICP_HD_CFG_EXPOSURE_AVERAGE,
    ICP_HD_CFG_EXPOSURE_SPOT,
    ICP_HD_CFG_EXPOSURE_MAX
} ICP_HD_cfg_exposure_e;

typedef enum {
    ICP_HD_CFG_FLICKER_OFF,
    ICP_HD_CFG_FLICKER_60HZ,
    ICP_HD_CFG_FLICKER_50HZ,
    ICP_HD_CFG_FLICKER_AUTO,
    ICP_HD_CFG_FLICKER_MAX
} ICP_HD_cfg_flicker_e;

typedef enum {
    ICP_HD_CFG_ANTISHAKE_OFF,
    ICP_HD_CFG_ANTISHAKE_ON,
    ICP_HD_CFG_ANTISHAKE_MAX
} ICP_HD_cfg_antishake_e;

 typedef enum {
    ICP_HD_CFG_FOCUS_STEP_0,
    ICP_HD_CFG_FOCUS_STEP_1,
    ICP_HD_CFG_FOCUS_STEP_2,
    ICP_HD_CFG_FOCUS_STEP_3,
    ICP_HD_CFG_FOCUS_STEP_4,
    ICP_HD_CFG_FOCUS_STEP_5,
    ICP_HD_CFG_FOCUS_STEP_6,
    ICP_HD_CFG_FOCUS_STEP_7,
    ICP_HD_CFG_FOCUS_STEP_8,
    ICP_HD_CFG_FOCUS_STEP_9,
    ICP_HD_CFG_FOCUS_STEP_10,
    ICP_HD_CFG_FOCUS_STEP_MAX
} ICP_HD_cfg_focus_step_e;

typedef enum {
    ICP_HD_CFG_AUTOFOCUS_TRIGGER_NORMAL,
    ICP_HD_CFG_AUTOFOCUS_TRIGGER_MACRO,
    ICP_HD_CFG_AUTOFOCUS_TRIGGER_AUTO,
    ICP_HD_CFG_AUTOFOCUS_TRIGGER_INFINITY,
    ICP_HD_CFG_AUTOFOCUS_TRIGGER_MAX
} ICP_HD_cfg_autofocus_trigger_e;

typedef enum {
    ICP_HD_CFG_SCENE_MODE_OFF,
    ICP_HD_CFG_SCENE_MODE_AUTO,		
    ICP_HD_CFG_SCENE_MODE_PORTRAIT,
    ICP_HD_CFG_SCENE_MODE_LANDSCAPE,
    ICP_HD_CFG_SCENE_MODE_INDOOR,
    ICP_HD_CFG_SCENE_MODE_SPORTS,
    ICP_HD_CFG_SCENE_MODE_NIGHT,
    ICP_HD_CFG_SCENE_MODE_BEACH,
    ICP_HD_CFG_SCENE_MODE_WINTER,
    ICP_HD_CFG_SCENE_MODE_SUNSET,
    ICP_HD_CFG_SCENE_MODE_TEXT,
    ICP_HD_CFG_SCENE_MODE_PARTY,
    ICP_HD_CFG_SCENE_MODE_MAX,
} ICP_HD_cfg_scene_mode_e;

typedef enum {
    ICP_HD_CFG_REFLECT_NONE,
    ICP_HD_CFG_REFLECT_MIRROR,
    ICP_HD_CFG_REFLECT_WATER,
    ICP_HD_CFG_REFLECT_MIRROR_WATER,
    ICP_HD_CFG_REFLECT_MAX
} ICP_HD_cfg_reflect_e;

#if 0 //def F_PANTECH_CAMERA_TUP_LOAD_FILE

typedef enum
{
   ICP_HD_TUNE_STATE_NONE,
   ICP_HD_TUNE_STATE_LOAD_VALUE,
   ICP_HD_TUNE_STATE_SET_VALUE
} ICP_HD_tune_state_type;

typedef enum
{
   ICP_HD_TUNE_STATE_TUNNING_MODE_OFF,
   ICP_HD_TUNE_STATE_TUNNING_MODE_ON
 } ICP_HD_tune_mode_type;

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
} ICP_HD_cfg_op_e;

 typedef struct {
    ICP_HD_cfg_op_e op;
    u_int16_t addr;
    u_int16_t data;
} ICP_HD_cfg_param_t;

typedef struct {
    ICP_HD_cfg_op_e op;
    unsigned short addr;
    unsigned short mask;
    unsigned short data;
} ICP_HD_cfg_param_ext_t;

typedef struct{
	ICP_HD_cfg_param_t* ICP_HD_tup_init_params;
	ICP_HD_cfg_param_t* ICP_HD_tup_wb_params;
	ICP_HD_cfg_param_t* ICP_HD_tup_bright_params;
	ICP_HD_cfg_param_t* ICP_HD_tup_exposure_params;
	ICP_HD_cfg_param_t* ICP_HD_tup_ffps_params;
	ICP_HD_cfg_param_t* ICP_HD_tup_reflect_params;
	ICP_HD_cfg_param_t* ICP_HD_tup_effect_params;
}ICP_HD_tup_params;



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
	ICP_HD_cfg_param_t* ICP_HD_tup_init_params;
	ICP_HD_cfg_param_t* ICP_HD_tup_wb_params;
	ICP_HD_cfg_param_t* ICP_HD_tup_bright_params;
	ICP_HD_cfg_param_t* ICP_HD_tup_exposure_params;
	ICP_HD_cfg_param_t* ICP_HD_tup_ffps_params;
	ICP_HD_cfg_param_t* ICP_HD_tup_reflect_params;
	ICP_HD_cfg_param_t* ICP_HD_tup_effect_params;
}ICP_HD_tup_params;
#endif

typedef struct {
    u_int32_t num_params;
    ICP_HD_cfg_param_t *params;
} ICP_HD_params_t;

typedef struct {
    ICP_HD_params_t init;
    ICP_HD_params_t wb[ICP_HD_CFG_WB_MAX];
    ICP_HD_params_t bright[ICP_HD_CFG_BRIGHT_MAX];
    ICP_HD_params_t exposure[ICP_HD_CFG_EXPOSURE_MAX];
    ICP_HD_params_t ffps[ICP_HD_CFG_FFPS_MAX - ICP_HD_CFG_FFPS_MIN + 1];
    ICP_HD_params_t reflect[ICP_HD_CFG_REFLECT_MAX];
    ICP_HD_params_t effect[ICP_HD_CFG_EFFECT_MAX];
    ICP_HD_params_t flicker[ICP_HD_CFG_FLICKER_MAX];
    ICP_HD_params_t antishake[ICP_HD_CFG_ANTISHAKE_MAX];
    ICP_HD_params_t focus_step[ICP_HD_CFG_FOCUS_STEP_MAX];
    ICP_HD_params_t scene[ICP_HD_CFG_SCENE_MODE_MAX];
} ICP_HD_params_tbl_t;
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

extern void camsensor_ICP_HD_tup_init (int8_t *stream);
extern u_int32_t camsensor_ICP_HD_tup_get_init_params (ICP_HD_cfg_param_t **init_params);
extern u_int32_t camsensor_ICP_HD_tup_get_wb_params (ICP_HD_cfg_wb_e wb, ICP_HD_cfg_param_t **wb_params);
extern u_int32_t camsensor_ICP_HD_tup_get_bright_params (ICP_HD_cfg_bright_e bright, ICP_HD_cfg_param_t **bright_params);
extern u_int32_t camsensor_ICP_HD_tup_get_exposure_params (ICP_HD_cfg_exposure_e exposure, ICP_HD_cfg_param_t **exposure_params);
extern u_int32_t camsensor_ICP_HD_tup_get_ffps_params (u_int32_t ffps, ICP_HD_cfg_param_t **ffps_params);
extern u_int32_t camsensor_ICP_HD_tup_get_reflect_params (ICP_HD_cfg_reflect_e reflect, ICP_HD_cfg_param_t **reflect_params);
extern u_int32_t camsensor_ICP_HD_tup_get_effect_params (ICP_HD_cfg_effect_e effect, ICP_HD_cfg_param_t **effect_params);
extern u_int32_t camsensor_ICP_HD_tup_get_flicker_params (ICP_HD_cfg_flicker_e flicker, ICP_HD_cfg_param_t **flicker_params);
extern u_int32_t camsensor_ICP_HD_tup_get_antishake_params (ICP_HD_cfg_antishake_e antishake, ICP_HD_cfg_param_t **antishake_params);
extern u_int32_t camsensor_ICP_HD_tup_get_focus_step_params (ICP_HD_cfg_focus_step_e focus_step, ICP_HD_cfg_param_t **focus_step_params);
extern u_int32_t camsensor_ICP_HD_tup_get_scene_params (ICP_HD_cfg_scene_mode_e effect, ICP_HD_cfg_param_t **scene_params);

#endif
#endif /* __CAMSENSOR_ICP_HD_TUNEUP_H__ */
