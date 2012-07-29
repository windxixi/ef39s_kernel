/*
================================================================================
TUNEUP Interface for PixelPlus PO4010N CMOS CIF sensor

DESCRIPTION
    This file contains the definitions needed for the camera tuneup interface.

Copyright (c) 2007 by PANTECH, Incorporated. All Rights Reserved.
================================================================================
*/

/*
================================================================================
INCLUDE FILES
================================================================================
*/

//#include "customer.h"
//#include "comdef.h"

#if defined(F_PANTECH_CAMERA_SENSOR_TUNEUP)
//nclude <stdio.h>
#include <linux/string.h>
//#include "camera_dbg.h"
#include <linux/types.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>

#include "camsensor_parser.h"
#include "camsensor_icp_hd_tuneup.h"



/*
================================================================================
LOCAL FEATURES
================================================================================
*/

/*
================================================================================
LOCAL CONSTANTS
================================================================================
*/

/*
================================================================================
LOCAL MACROS
================================================================================
*/
#ifdef F_PANTECH_CAMERA_TUP_PRINT_LOAD_VALUE
#define CAM_INFO(fmt, arg...) printk(KERN_ERR "Camera: " fmt "\n" , ## arg)
#else
#define CAM_INFO(fmt, arg...) do{}while(0)
#endif	
#define CAM_ERR(fmt, arg...)  printk(KERN_ERR  "%s: " fmt "\n" , \
						__func__ , ## arg)
#define CAM_WAIT(fmt, args...) do { } while (0)	

/*
================================================================================
LOCAL DATA TYPES
================================================================================
*/

/*
================================================================================
LOCAL VARIABLES
================================================================================
*/

static int32_t icp_hd_tup_init_num_params = 0;
static int32_t icp_hd_tup_wb_num_params[ICP_HD_CFG_WB_MAX];
static int32_t icp_hd_tup_bright_num_params[ICP_HD_CFG_BRIGHT_MAX];
static int32_t icp_hd_tup_exposure_num_params[ICP_HD_CFG_EXPOSURE_MAX];
static int32_t icp_hd_tup_ffps_num_params[ICP_HD_CFG_FFPS_MAX - ICP_HD_CFG_FFPS_MIN + 1];
static int32_t icp_hd_tup_reflect_num_params[ICP_HD_CFG_REFLECT_MAX];
static int32_t icp_hd_tup_effect_num_params[ICP_HD_CFG_EFFECT_MAX];

static int32_t icp_hd_tup_flicker_num_params[ICP_HD_CFG_FLICKER_MAX];
static int32_t icp_hd_tup_antishake_num_params[ICP_HD_CFG_ANTISHAKE_MAX];
static int32_t icp_hd_tup_focus_step_num_params[ICP_HD_CFG_FOCUS_STEP_MAX];
static int32_t icp_hd_tup_scene_num_params[ICP_HD_CFG_SCENE_MODE_MAX];


static icp_hd_cfg_param_t icp_hd_tup_init_params[ICP_HD_TUP_INIT_MAX_PARAMS];
static icp_hd_cfg_param_t icp_hd_tup_wb_params[ICP_HD_CFG_WB_MAX][ICP_HD_TUP_WB_MAX_PARAMS];
static icp_hd_cfg_param_t icp_hd_tup_bright_params[ICP_HD_CFG_BRIGHT_MAX][ICP_HD_TUP_BRIGHT_MAX_PARAMS];
static icp_hd_cfg_param_t icp_hd_tup_exposure_params[ICP_HD_CFG_EXPOSURE_MAX][ICP_HD_TUP_EXPOSURE_MAX_PARAMS];
static icp_hd_cfg_param_t icp_hd_tup_ffps_params[ICP_HD_CFG_FFPS_MAX - ICP_HD_CFG_FFPS_MIN + 1][ICP_HD_TUP_FFPS_MAX_PARAMS];
static icp_hd_cfg_param_t icp_hd_tup_reflect_params[ICP_HD_CFG_REFLECT_MAX][ICP_HD_TUP_REFLECT_MAX_PARAMS];
static icp_hd_cfg_param_t icp_hd_tup_effect_params[ICP_HD_CFG_EFFECT_MAX][ICP_HD_TUP_EFFECT_MAX_PARAMS];

static icp_hd_cfg_param_t icp_hd_tup_flicker_params[ICP_HD_CFG_FLICKER_MAX][ICP_HD_TUP_FLICKER_MAX_PARAMS];
static icp_hd_cfg_param_t icp_hd_tup_antishake_params[ICP_HD_CFG_ANTISHAKE_MAX][ICP_HD_TUP_ANTISHAKE_MAX_PARAMS];
static icp_hd_cfg_param_t icp_hd_tup_focus_step_params[ICP_HD_CFG_FOCUS_STEP_MAX][ICP_HD_TUP_FOCUS_STEP_MAX_PARAMS];
static icp_hd_cfg_param_t icp_hd_tup_scene_params[ICP_HD_CFG_SCENE_MODE_MAX][ICP_HD_TUP_SCENE_MAX_PARAMS];

/*
================================================================================
LOCAL API DECLARATIONS 
================================================================================
*/

static void icp_hd_tup_init (void);
static int8_t * icp_hd_tup_load_file (void);
static void icp_hd_tup_load_params (const int8_t *stream);
static void icp_hd_tup_load_init_params (const int8_t *stream);
static void icp_hd_tup_load_wb_params (const int8_t *stream);
static void icp_hd_tup_load_bright_params (const int8_t *stream);
static void icp_hd_tup_load_exposure_params (const int8_t *stream);
static void icp_hd_tup_load_ffps_params (const int8_t *stream);
static void icp_hd_tup_load_reflect_params (const int8_t *stream);
static void icp_hd_tup_load_effect_params (const int8_t *stream);
static void icp_hd_tup_load_flicker_params (const int8_t *stream);
static void icp_hd_tup_load_antishake_params (const int8_t *stream);
static void icp_hd_tup_load_focus_step_params (const int8_t *stream);
static void icp_hd_tup_load_scene_params (const int8_t *stream);

static int32_t icp_hd_tup_parse_params (const int8_t *stream, icp_hd_cfg_param_t *params);
static void icp_hd_tup_print_params (icp_hd_cfg_param_t *params, int32_t num_params);

/*
================================================================================
IMPORTED VARIABLES, API DECLARATIONS
================================================================================
*/

/*
================================================================================
GLOBAL API DEFINITIONS
================================================================================
*/

/*
================================================================================
FUNCTION
================================================================================
*/
void camsensor_icp_hd_tup_init (int8_t *stream)
{
#if 0
    int8_t *stream = NULL;
#endif
    CAM_INFO(">>%s ()", __func__);

    icp_hd_tup_init();
#if 0
    stream = icp_hd_tup_load_file();
#endif
    if (stream == NULL)
    {
        CAM_INFO("<<%s ()", __func__);
        return;
    }
    CAM_INFO("TUNEUP file is loaded!");

    icp_hd_tup_load_params((const int8_t *)stream);
    CAM_INFO("TUNEUP params are loaded!");

    kfree(stream);

    CAM_INFO("<<%s ()", __func__);
    return;
}

/*
================================================================================
FUNCTION
================================================================================
*/
u_int32_t camsensor_icp_hd_tup_get_init_params
(
    icp_hd_cfg_param_t **init_params
)
{
    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- *init_params = 0x%08x", *init_params, 0);

    *init_params = &icp_hd_tup_init_params[0];
    CAM_INFO("<<%s (%d) (0x%08x)", __func__, icp_hd_tup_init_num_params, *init_params);

    return icp_hd_tup_init_num_params;
}

/*
================================================================================
FUNCTION
================================================================================
*/
u_int32_t camsensor_icp_hd_tup_get_wb_params
(
    icp_hd_cfg_wb_e wb,
    icp_hd_cfg_param_t **wb_params
)
{
    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- wb = %d, *wb_params = 0x%08x", wb, *wb_params, 0);

    if ((wb < ICP_HD_CFG_WB_AUTO) || (wb >= ICP_HD_CFG_WB_MAX))
    {
        CAM_INFO("Invalid param! [wb = %d]", wb, 0, 0);
        return 0;
    }

    *wb_params = &icp_hd_tup_wb_params[wb][0];
    CAM_INFO("<<%s (%d) (0x%08x)", __func__, icp_hd_tup_wb_num_params[wb], *wb_params);

    return icp_hd_tup_wb_num_params[wb];
}

/*
================================================================================
FUNCTION
================================================================================
*/
u_int32_t camsensor_icp_hd_tup_get_bright_params
(
    icp_hd_cfg_bright_e bright,
    icp_hd_cfg_param_t **bright_params
)
{
    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- bright = %d, *bright_params = 0x%08x", bright, *bright_params, 0);

    if ((bright < ICP_HD_CFG_BRIGHT_M5) || (bright >= ICP_HD_CFG_BRIGHT_MAX))
    {
        CAM_INFO("Invalid param! [bright = %d]", bright, 0, 0);
        return 0;
    }

    *bright_params = &icp_hd_tup_bright_params[bright][0];
    CAM_INFO("<<%s (%d) (0x%08x)", __func__, icp_hd_tup_bright_num_params[bright], *bright_params);

    return icp_hd_tup_bright_num_params[bright];
}

/*
================================================================================
FUNCTION
================================================================================
*/
u_int32_t camsensor_icp_hd_tup_get_exposure_params
(
    icp_hd_cfg_exposure_e exposure,
    icp_hd_cfg_param_t **exposure_params
)
{
    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- exposure = %d, *exposure_params = 0x%08x", exposure, *exposure_params, 0);

    if ((exposure < ICP_HD_CFG_EXPOSURE_NORMAL) || (exposure >= ICP_HD_CFG_EXPOSURE_MAX))
    {
        CAM_INFO("Invalid param! [exposure = %d]", exposure, 0, 0);
        return 0;
    }

    *exposure_params = &icp_hd_tup_exposure_params[exposure][0];
    CAM_INFO("<<%s (%d) (0x%08x)", __func__, icp_hd_tup_exposure_num_params[exposure], *exposure_params);

    return icp_hd_tup_exposure_num_params[exposure];
}

/*
================================================================================
FUNCTION
================================================================================
*/
u_int32_t camsensor_icp_hd_tup_get_ffps_params
(
    u_int32_t ffps,
    icp_hd_cfg_param_t **ffps_params
)
{
    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- ffps = %d, *ffps_params = 0x%08X", ffps, *ffps_params, 0);

    if ((ffps < ICP_HD_CFG_FFPS_MIN) || (ffps > ICP_HD_CFG_FFPS_MAX))
    {
        CAM_INFO("Invalid param! [ffps = %d]", ffps, 0, 0);
        return 0;
    }

    *ffps_params = &icp_hd_tup_ffps_params[ffps - ICP_HD_CFG_FFPS_MIN][0];
    CAM_INFO("<<%s (%d) (0x%08X)", __func__, icp_hd_tup_ffps_num_params[ffps - ICP_HD_CFG_FFPS_MIN], *ffps_params);

    return icp_hd_tup_ffps_num_params[ffps - ICP_HD_CFG_FFPS_MIN];
}

/*
================================================================================
FUNCTION
================================================================================
*/
u_int32_t camsensor_icp_hd_tup_get_reflect_params
(
    icp_hd_cfg_reflect_e reflect,
    icp_hd_cfg_param_t **reflect_params
)
{
    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- reflect = %d, *reflect_params = 0x%08X", reflect, *reflect_params, 0);

    if ((reflect < ICP_HD_CFG_REFLECT_NONE) || (reflect >= ICP_HD_CFG_REFLECT_MAX))
    {
        CAM_INFO("Invalid param! [reflect = %d]", reflect, 0, 0);
        return 0;
    }

    *reflect_params = &icp_hd_tup_reflect_params[reflect][0];
    CAM_INFO("<<%s (%d) (0x%08X)", __func__, icp_hd_tup_reflect_num_params[reflect], *reflect_params);

    return icp_hd_tup_reflect_num_params[reflect];
}

/*
================================================================================
FUNCTION
================================================================================
*/
u_int32_t camsensor_icp_hd_tup_get_effect_params
(
    icp_hd_cfg_effect_e effect,
    icp_hd_cfg_param_t **effect_params
)
{
    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- effect = %d, *effect_params = 0x%08X", effect, *effect_params, 0);

    if ((effect < ICP_HD_CFG_EFFECT_NONE) || (effect >= ICP_HD_CFG_EFFECT_MAX))
    {
        CAM_INFO("Invalid param! [effect = %d]", effect, 0, 0);
        return 0;
    }

    *effect_params = &icp_hd_tup_effect_params[effect][0];
    CAM_INFO("<<%s (%d) (0x%08X)", __func__, icp_hd_tup_effect_num_params[effect], *effect_params);

    return icp_hd_tup_effect_num_params[effect];
}

/*
================================================================================
FUNCTION
================================================================================
*/
u_int32_t camsensor_icp_hd_tup_get_flicker_params
(
    icp_hd_cfg_flicker_e flicker,
    icp_hd_cfg_param_t **flicker_params
)
{
    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- flicker = %d, *flicker_params = 0x%08X", flicker, *flicker_params, 0);

    if ((flicker < ICP_HD_CFG_FLICKER_OFF) || (flicker >= ICP_HD_CFG_FLICKER_MAX))
    {
        CAM_INFO("Invalid param! [flicker = %d]", flicker, 0, 0);
        return 0;
    }

    *flicker_params = &icp_hd_tup_flicker_params[flicker][0];
    CAM_INFO("<<%s (%d) (0x%08X)", __func__, icp_hd_tup_flicker_num_params[flicker], *flicker_params);

    return icp_hd_tup_flicker_num_params[flicker];
}

/*
================================================================================
FUNCTION
================================================================================
*/
u_int32_t camsensor_icp_hd_tup_get_antishake_params
(
    icp_hd_cfg_antishake_e antishake,
    icp_hd_cfg_param_t **antishake_params
)
{
    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- antishake = %d, *antishake_params = 0x%08X", antishake, *antishake_params, 0);

    if ((antishake < ICP_HD_CFG_ANTISHAKE_OFF) || (antishake >= ICP_HD_CFG_ANTISHAKE_MAX))
    {
        CAM_INFO("Invalid param! [antishake = %d]", antishake, 0, 0);
        return 0;
    }

    *antishake_params = &icp_hd_tup_antishake_params[antishake][0];
    CAM_INFO("<<%s (%d) (0x%08X)", __func__, icp_hd_tup_antishake_num_params[antishake], *antishake_params);

    return icp_hd_tup_antishake_num_params[antishake];
}

/*
================================================================================
FUNCTION
================================================================================
*/
u_int32_t camsensor_icp_hd_tup_get_focus_step_params
(
    icp_hd_cfg_focus_step_e focus_step,
    icp_hd_cfg_param_t **focus_step_params
)
{
    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- focus_step = %d, *focus_step_params = 0x%08X", focus_step, *focus_step_params, 0);

    if ((focus_step < ICP_HD_CFG_FOCUS_STEP_0) || (focus_step >= ICP_HD_CFG_FOCUS_STEP_MAX))
    {
        CAM_INFO("Invalid param! [focus_step = %d]", focus_step, 0, 0);
        return 0;
    }

    *focus_step_params = &icp_hd_tup_focus_step_params[focus_step][0];
    CAM_INFO("<<%s (%d) (0x%08X)", __func__, icp_hd_tup_focus_step_num_params[focus_step], *focus_step_params);

    return icp_hd_tup_focus_step_num_params[focus_step];
}

/*
================================================================================
FUNCTION
================================================================================
*/
u_int32_t camsensor_icp_hd_tup_get_scene_params
(
    icp_hd_cfg_scene_mode_e scene,
    icp_hd_cfg_param_t **scene_params
)
{
    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- scene = %d, *scene_params = 0x%08X", scene, *scene_params, 0);

    if ((scene < ICP_HD_CFG_SCENE_MODE_OFF) || (scene >= ICP_HD_CFG_SCENE_MODE_MAX))
    {
        CAM_INFO("Invalid param! [scene = %d]", scene, 0, 0);
        return 0;
    }

    *scene_params = &icp_hd_tup_scene_params[scene][0];
    CAM_INFO("<<%s (%d) (0x%08X)", __func__, icp_hd_tup_scene_num_params[scene], *scene_params);

    return icp_hd_tup_scene_num_params[scene];
}
/*
================================================================================
FUNCTION
================================================================================
*/
u_int32_t camsensor_icp_hd_tup_get_all_params
(
    icp_hd_tup_params *params_struct_prt
)
{
    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- *params_struct = 0x%08x", params_struct_prt, 0, 0);
    
    params_struct_prt->icp_hd_tup_init_params = &icp_hd_tup_init_params[0];
    params_struct_prt->icp_hd_tup_wb_params = &icp_hd_tup_wb_params[0][0];
    params_struct_prt->icp_hd_tup_bright_params = &icp_hd_tup_bright_params[0][0];
    params_struct_prt->icp_hd_tup_exposure_params = &icp_hd_tup_exposure_params[0][0];
    params_struct_prt->icp_hd_tup_ffps_params = &icp_hd_tup_ffps_params[0][0];
    params_struct_prt->icp_hd_tup_reflect_params = &icp_hd_tup_reflect_params[0][0];
    params_struct_prt->icp_hd_tup_effect_params = &icp_hd_tup_effect_params[0][0];

    CAM_INFO("<<%s (0x%08x) (0x%08x)", __func__, params_struct_prt->icp_hd_tup_init_params, &icp_hd_tup_init_params[0]);
    return icp_hd_tup_init_num_params;
}

/*
================================================================================
LOCAL API DEFINITIONS
================================================================================
*/

/*
================================================================================
FUNCTION
================================================================================
*/
static void icp_hd_tup_init (void)
{
    CAM_INFO(">>%s ()", __func__);

    icp_hd_tup_init_num_params = 0;
    memset(icp_hd_tup_init_params, 0, sizeof(icp_hd_tup_init_params));

    memset(icp_hd_tup_wb_num_params, 0, sizeof(icp_hd_tup_wb_num_params));
    memset(icp_hd_tup_wb_params, 0, sizeof(icp_hd_tup_wb_params));

    memset(icp_hd_tup_bright_num_params, 0, sizeof(icp_hd_tup_bright_num_params));
    memset(icp_hd_tup_bright_params, 0, sizeof(icp_hd_tup_bright_params));

    memset(icp_hd_tup_exposure_num_params, 0, sizeof(icp_hd_tup_exposure_num_params));
    memset(icp_hd_tup_exposure_params, 0, sizeof(icp_hd_tup_exposure_params));

    memset(icp_hd_tup_ffps_num_params, 0, sizeof(icp_hd_tup_ffps_num_params));
    memset(icp_hd_tup_ffps_params, 0, sizeof(icp_hd_tup_ffps_params));

    memset(icp_hd_tup_reflect_num_params, 0, sizeof(icp_hd_tup_reflect_num_params));
    memset(icp_hd_tup_reflect_params, 0, sizeof(icp_hd_tup_reflect_params));

    memset(icp_hd_tup_effect_num_params, 0, sizeof(icp_hd_tup_effect_num_params));
    memset(icp_hd_tup_effect_params, 0, sizeof(icp_hd_tup_effect_params));

    memset(icp_hd_tup_flicker_num_params, 0, sizeof(icp_hd_tup_flicker_num_params));
    memset(icp_hd_tup_flicker_params, 0, sizeof(icp_hd_tup_flicker_params));

    memset(icp_hd_tup_antishake_num_params, 0, sizeof(icp_hd_tup_antishake_num_params));
    memset(icp_hd_tup_antishake_params, 0, sizeof(icp_hd_tup_antishake_params));

    memset(icp_hd_tup_focus_step_num_params, 0, sizeof(icp_hd_tup_focus_step_num_params));
    memset(icp_hd_tup_focus_step_params, 0, sizeof(icp_hd_tup_focus_step_params));

    memset(icp_hd_tup_scene_num_params, 0, sizeof(icp_hd_tup_scene_num_params));
    memset(icp_hd_tup_scene_params, 0, sizeof(icp_hd_tup_scene_params));	

    CAM_INFO("<<%s ()", __func__);
    return;
}

/*
================================================================================
FUNCTION
================================================================================
*/
static int8_t * icp_hd_tup_load_file()
{
#if 0
 	CAM_INFO("%s  start\n",__func__);
	FILE *fs_handle = NULL;
	int32_t fs_size = NULL;
	int8_t *fs_stream = NULL;
	
	fs_handle = fopen(ICP_HD_TUP_FNAME, "r");
	if(fs_handle == NULL)
	{
	   CAM_INFO("<<%s (NULL) (file_name = %s, fs_handle = %d)", __func__, ICP_HD_TUP_FNAME, fs_handle);
	   return NULL;
	}	

	fseek(fs_handle, 0, SEEK_END);
	fs_size = ftell(fs_handle);
	CAM_INFO("fs_size = %d", fs_size);

	fs_stream = (int8_t *)malloc(fs_size);
	if (fs_stream == NULL)
	{
	    CAM_INFO("<<%s (NULL)", __func__);
	    fclose(fs_handle);
	    return NULL;
	}

	if(fread(fs_stream, sizeof(int8_t), fs_size, fs_handle) != fs_size)
	{
	     CAM_INFO("fread fs_size = %d", fs_size);
            fclose(fs_handle);
	     return NULL;
	}

	fclose(fs_handle);

	CAM_INFO("%s  end\n",__func__);

	return fs_stream;
#endif
return 0;
#if 0 //[[ PANTECH_CAMERA_PSJ_0100405_BEGIN -- fs.h는 amss에서만 사용가능하므로 Aarm에서는 사용할 수 없다 stdio.h함수를 사용하여 변경
	fs_handle_type fs_handle = FS_NULL_HANDLE;
	fs_rsp_msg_type fs_rsp;
	int32_t fs_fsize = 0;
	int8_t *fs_stream = NULL;

	CAM_INFO("%s  start\n",__func__);

	fs_open(ICP_HD_TUP_FNAME, FS_OA_READONLY, NULL, NULL, &fs_rsp);
	if((fs_rsp.open.status != FS_OKAY_S) || (fs_rsp.open.handle == FS_NULL_HANDLE))
	{
	   CAM_INFO("<<%s (NULL) (%d, %d)", __func__, fs_rsp.open.status, fs_rsp.open.handle);
	   return NULL;
	}	
	fs_handle = fs_rsp.open.handle;

	fs_file_size(ICP_HD_TUP_FNAME, NULL, &fs_rsp);
	if ((fs_rsp.file_size.status != FS_OKAY_S) || (fs_rsp.file_size.size == 0))
	{
	    CAM_INFO("<<%s (NULL) (%d, %d)", __func__, fs_rsp.file_size.status, fs_rsp.file_size.size);
	    fs_close(fs_handle, NULL, &fs_rsp);
	    return NULL;
	}
	fs_fsize = fs_rsp.file_size.size;

	fs_stream = (int8_t *)malloc(fs_fsize);
	if (fs_stream == NULL)
	{
	    CAM_INFO("<<%s (NULL)", __func__);
	    fs_close(fs_handle, NULL, &fs_rsp);
	    return NULL;
	}

	fs_read(fs_handle, (void *)fs_stream, fs_fsize, NULL, &fs_rsp);
	if ((fs_rsp.read.status != FS_OKAY_S) || (fs_rsp.read.count == 0))
	{
	    free(fs_stream);
	    CAM_INFO("<<%s (NULL) (%d, %d)", __func__, fs_rsp.read.status, fs_rsp.read.count);
	    fs_close(fs_handle, NULL, &fs_rsp);
	    return NULL;
	}

	fs_close(fs_handle, NULL, &fs_rsp);
	if (fs_rsp.close.status != FS_OKAY_S)
	{
	    free(fs_stream);
	    CAM_INFO("<<%s (NULL) (%d)", __func__, fs_rsp.close.status, 0);
	    return NULL;
	}

	CAM_INFO("%s  end\n",__func__);

	return fs_stream;
#endif //]] PANTECH_CAMERA_PSJ_0100405_END -- fs.h는 amss에서만 사용가능하므로 Aarm에서는 사용할 수 없다 stdio.h함수를 사용하여 변경
}

/*
================================================================================
FUNCTION
================================================================================
*/
static void icp_hd_tup_load_params (const int8_t *stream)
{
    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- stream = 0x%08x", stream, 0, 0);

    icp_hd_tup_load_init_params(stream);
    icp_hd_tup_load_wb_params(stream);
    icp_hd_tup_load_bright_params(stream);
    icp_hd_tup_load_exposure_params(stream);
    icp_hd_tup_load_ffps_params(stream);
    //icp_hd_tup_load_reflect_params(stream);
    //icp_hd_tup_load_effect_params(stream);
    icp_hd_tup_load_flicker_params(stream);
    icp_hd_tup_load_antishake_params(stream);
    icp_hd_tup_load_focus_step_params(stream);
    icp_hd_tup_load_scene_params(stream);	

    CAM_INFO("<<%s ()", __func__);
    return;
}

/*
================================================================================
FUNCTION
================================================================================
*/
static void icp_hd_tup_load_init_params (const int8_t *stream)
{
    int8_t *my_ptr = NULL;

    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- stream = 0x%08x", stream, 0, 0);

    my_ptr = strstr(stream, ICP_HD_TUP_INIT_MARK);
    if (my_ptr == NULL)
    {
        CAM_INFO("Can't find INIT mark! [stream = 0x%08x]", stream, 0, 0);
        return;
    }

    icp_hd_tup_init_num_params = icp_hd_tup_parse_params(my_ptr, &icp_hd_tup_init_params[0]);    
    CAM_INFO("Number of params in %s = %d", ICP_HD_TUP_INIT_MARK, icp_hd_tup_init_num_params, 0);

    CAM_INFO("========== %s ==========", ICP_HD_TUP_INIT_MARK, 0, 0);
#ifdef F_PANTECH_CAMERA_TUP_PRINT_LOAD_VALUE		
    icp_hd_tup_print_params(&icp_hd_tup_init_params[0], icp_hd_tup_init_num_params);
#endif

    CAM_INFO("<<%s ()", __func__);
    return;
}

/*
================================================================================
FUNCTION
================================================================================
*/
static void icp_hd_tup_load_wb_params (const int8_t *stream)
{
    int8_t *my_ptr = NULL;
    int8_t *my_mark = NULL;
    int32_t i = 0;

    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- stream = 0x%08x", stream, 0, 0);

    for (i = ICP_HD_CFG_WB_AUTO; i < ICP_HD_CFG_WB_MAX; i++)
    {
        switch (i)
        {
            case ICP_HD_CFG_WB_AUTO: my_mark = (int8_t *)ICP_HD_TUP_WB_AUTO_MARK; break;
            case ICP_HD_CFG_WB_CLOUDY: my_mark = (int8_t *)ICP_HD_TUP_WB_CLOUDY_MARK; break;
            case ICP_HD_CFG_WB_DAYLIGHT: my_mark = (int8_t *)ICP_HD_TUP_WB_DAYLIGHT_MARK; break;
            case ICP_HD_CFG_WB_FLUORESCENT: my_mark = (int8_t *)ICP_HD_TUP_WB_FLUORESCENT_MARK; break;
            case ICP_HD_CFG_WB_INCANDESCENT: my_mark = (int8_t *)ICP_HD_TUP_WB_INCANDESCENT_MARK; break;
        }
        
        my_ptr = strstr(stream, my_mark);
        if (my_ptr == NULL)
        {
            CAM_INFO("Can't find WB mark! [stream = 0x%08x][i = %d]", stream, i, 0);
            return;
        }
        icp_hd_tup_wb_num_params[i] = icp_hd_tup_parse_params(my_ptr, &icp_hd_tup_wb_params[i][0]);
        CAM_INFO("Number of params in %s = %d", my_mark, icp_hd_tup_wb_num_params[i], 0);

        CAM_INFO("========== %s ==========", my_mark, 0, 0);
#ifdef F_PANTECH_CAMERA_TUP_PRINT_LOAD_VALUE			
        icp_hd_tup_print_params(&icp_hd_tup_wb_params[i][0], icp_hd_tup_wb_num_params[i]);
#endif
    }

    CAM_INFO("<<%s ()", __func__);
    return;
}

/*
================================================================================
FUNCTION
================================================================================
*/
static void icp_hd_tup_load_bright_params (const int8_t *stream)
{
    int8_t *my_ptr = NULL;
    int8_t *my_mark = NULL;
    int32_t i = 0;

    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- stream = 0x%08x", stream, 0, 0);

    for (i = ICP_HD_CFG_BRIGHT_M5; i < ICP_HD_CFG_BRIGHT_MAX; i++)
    {
        switch (i)
        {
#ifndef BRIGHTNESS_STEP_8            
            case ICP_HD_CFG_BRIGHT_M5: my_mark = (int8_t *)ICP_HD_TUP_BRIGHT_M5_MARK; break;
#endif			
            case ICP_HD_CFG_BRIGHT_M4: my_mark = (int8_t *)ICP_HD_TUP_BRIGHT_M4_MARK; break;
            case ICP_HD_CFG_BRIGHT_M3: my_mark = (int8_t *)ICP_HD_TUP_BRIGHT_M3_MARK; break;
            case ICP_HD_CFG_BRIGHT_M2: my_mark = (int8_t *)ICP_HD_TUP_BRIGHT_M2_MARK; break;
            case ICP_HD_CFG_BRIGHT_M1: my_mark = (int8_t *)ICP_HD_TUP_BRIGHT_M1_MARK; break;
            case ICP_HD_CFG_BRIGHT_0: my_mark = (int8_t *)ICP_HD_TUP_BRIGHT_0_MARK; break;
            case ICP_HD_CFG_BRIGHT_P1: my_mark = (int8_t *)ICP_HD_TUP_BRIGHT_P1_MARK; break;
            case ICP_HD_CFG_BRIGHT_P2: my_mark = (int8_t *)ICP_HD_TUP_BRIGHT_P2_MARK; break;
            case ICP_HD_CFG_BRIGHT_P3: my_mark = (int8_t *)ICP_HD_TUP_BRIGHT_P3_MARK; break;
            case ICP_HD_CFG_BRIGHT_P4: my_mark = (int8_t *)ICP_HD_TUP_BRIGHT_P4_MARK; break;
#ifndef BRIGHTNESS_STEP_8            			
            case ICP_HD_CFG_BRIGHT_P5: my_mark = (int8_t *)ICP_HD_TUP_BRIGHT_P5_MARK; break;
#endif			
        }
        
        my_ptr = strstr(stream, my_mark);
        if (my_ptr == NULL)
        {
            CAM_INFO("Can't find BRIGHT mark! [stream = 0x%08x][i = %d]", stream, i, 0);
            return;
        }
        icp_hd_tup_bright_num_params[i] = icp_hd_tup_parse_params(my_ptr, &icp_hd_tup_bright_params[i][0]);
        CAM_INFO("Number of params in %s = %d", my_mark, icp_hd_tup_bright_num_params[i], 0);

        CAM_INFO("========== %s ==========", my_mark, 0, 0);
#ifdef F_PANTECH_CAMERA_TUP_PRINT_LOAD_VALUE			
        icp_hd_tup_print_params(&icp_hd_tup_bright_params[i][0], icp_hd_tup_bright_num_params[i]);
#endif
    }

    CAM_INFO("<<%s ()", __func__);
    return;
}

/*
================================================================================
FUNCTION
================================================================================
*/
static void icp_hd_tup_load_exposure_params (const int8_t *stream)
{
    int8_t *my_ptr = NULL;
    int8_t *my_mark = NULL;
    int32_t i = 0;

    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- stream = 0x%08x", stream, 0, 0);

    for (i = ICP_HD_CFG_EXPOSURE_NORMAL; i < ICP_HD_CFG_EXPOSURE_MAX; i++)
    {
        switch (i)
        {
            case ICP_HD_CFG_EXPOSURE_NORMAL:  my_mark = (int8_t *)ICP_HD_TUP_EXPOSURE_NORMAL; break;
            case ICP_HD_CFG_EXPOSURE_SPOT:    my_mark = (int8_t *)ICP_HD_TUP_EXPOSURE_SPOT; break;
            case ICP_HD_CFG_EXPOSURE_AVERAGE: my_mark = (int8_t *)ICP_HD_TUP_EXPOSURE_AVERAGE; break;
        }
        
        my_ptr = strstr(stream, my_mark);
        if (my_ptr == NULL)
        {
            CAM_INFO("Can't find EXPOSURE mark! [stream = 0x%08x][i = %d]", stream, i, 0);
            return;
        }
        icp_hd_tup_exposure_num_params[i] = icp_hd_tup_parse_params(my_ptr, &icp_hd_tup_exposure_params[i][0]);
        CAM_INFO("Number of params in %s = %d", my_mark, icp_hd_tup_exposure_num_params[i], 0);

        CAM_INFO("========== %s ==========", my_mark, 0, 0);
#ifdef F_PANTECH_CAMERA_TUP_PRINT_LOAD_VALUE			
        icp_hd_tup_print_params(&icp_hd_tup_exposure_params[i][0], icp_hd_tup_exposure_num_params[i]);
#endif
    }

    CAM_INFO("<<%s ()", __func__);
    return;
}

/*
================================================================================
FUNCTION
================================================================================
*/
static void icp_hd_tup_load_ffps_params (const int8_t *stream)
{
    int8_t *my_ptr = NULL;
    int8_t *my_mark = NULL;
    int32_t i = 0;

    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- stream = 0x%08x", stream, 0, 0);

    for (i = ICP_HD_CFG_FFPS_MIN; i <= ICP_HD_CFG_FFPS_MAX; i++)
    {
        switch (i)
        {
            case 5: my_mark = (int8_t *)ICP_HD_TUP_FFPS_5_MARK; break;
            case 6: my_mark = (int8_t *)ICP_HD_TUP_FFPS_6_MARK; break;
            case 7: my_mark = (int8_t *)ICP_HD_TUP_FFPS_7_MARK; break;
            case 8: my_mark = (int8_t *)ICP_HD_TUP_FFPS_8_MARK; break;
            case 9: my_mark = (int8_t *)ICP_HD_TUP_FFPS_9_MARK; break;
            case 10: my_mark = (int8_t *)ICP_HD_TUP_FFPS_10_MARK; break;
            case 11: my_mark = (int8_t *)ICP_HD_TUP_FFPS_11_MARK; break;
            case 12: my_mark = (int8_t *)ICP_HD_TUP_FFPS_12_MARK; break;
            case 13: my_mark = (int8_t *)ICP_HD_TUP_FFPS_13_MARK; break;
            case 14: my_mark = (int8_t *)ICP_HD_TUP_FFPS_14_MARK; break;
            case 15: my_mark = (int8_t *)ICP_HD_TUP_FFPS_15_MARK; break;
        }
        
        my_ptr = strstr(stream, my_mark);
        if (my_ptr == NULL)
        {
            CAM_INFO("Can't find fixed FPS mark! [stream = 0x%08x][i = %d]", stream, i, 0);
            return;
        }
        icp_hd_tup_ffps_num_params[i - ICP_HD_CFG_FFPS_MIN] = icp_hd_tup_parse_params(my_ptr, &icp_hd_tup_ffps_params[i - ICP_HD_CFG_FFPS_MIN][0]);
        CAM_INFO("Number of params in %s = %d", my_mark, icp_hd_tup_ffps_num_params[i - ICP_HD_CFG_FFPS_MIN], 0);

        CAM_INFO("========== %s ==========", my_mark, 0, 0);
#ifdef F_PANTECH_CAMERA_TUP_PRINT_LOAD_VALUE			
        icp_hd_tup_print_params(&icp_hd_tup_ffps_params[i - ICP_HD_CFG_FFPS_MIN][0], icp_hd_tup_ffps_num_params[i - ICP_HD_CFG_FFPS_MIN]);
#endif
    }

    CAM_INFO("<<%s ()", __func__);
    return;
}

/*
================================================================================
FUNCTION
================================================================================
*/
static void icp_hd_tup_load_reflect_params (const int8_t *stream)
{
    int8_t *my_ptr = NULL;
    int8_t *my_mark = NULL;
    int32_t i = 0;

    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- stream = 0x%08x", stream, 0, 0);

    for (i = 0; i < ICP_HD_CFG_REFLECT_MAX; i++)
    {
        switch (i)
        {
            case ICP_HD_CFG_REFLECT_NONE: my_mark = (int8_t *)ICP_HD_TUP_REFLECT_NONE_MARK; break;
            case ICP_HD_CFG_REFLECT_MIRROR: my_mark = (int8_t *)ICP_HD_TUP_REFLECT_MIRROR_MARK; break;
            case ICP_HD_CFG_REFLECT_WATER: my_mark = (int8_t *)ICP_HD_TUP_REFLECT_WATER_MARK; break;
            case ICP_HD_CFG_REFLECT_MIRROR_WATER: my_mark = (int8_t *)ICP_HD_TUP_REFLECT_MIRROR_WATER_MARK; break;
        }
        
        my_ptr = strstr(stream, my_mark);
        if (my_ptr == NULL)
        {
            CAM_INFO("Can't find REFLECT mark! [stream = 0x%08x][i = %d]", stream, i, 0);
            return;
        }
        icp_hd_tup_reflect_num_params[i] = icp_hd_tup_parse_params(my_ptr, &icp_hd_tup_reflect_params[i][0]);
        CAM_INFO("Number of params in %s = %d", my_mark, icp_hd_tup_reflect_num_params[i], 0);

        CAM_INFO("========== %s ==========", my_mark, 0, 0);
#ifdef F_PANTECH_CAMERA_TUP_PRINT_LOAD_VALUE			
        icp_hd_tup_print_params(&icp_hd_tup_reflect_params[i][0], icp_hd_tup_reflect_num_params[i]);
#endif
    }

    CAM_INFO("<<%s ()", __func__);
    return;
}

/*
================================================================================
FUNCTION
================================================================================
*/
static void icp_hd_tup_load_effect_params (const int8_t *stream)
{
    int8_t *my_ptr = NULL;
    int8_t *my_mark = NULL;
    int32_t i = 0;

    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- stream = 0x%08x", stream, 0, 0);

    for (i = 0; i < ICP_HD_CFG_EFFECT_MAX; i++)
    {
        switch (i)
        {
            case ICP_HD_CFG_EFFECT_NONE: my_mark = (char *)ICP_HD_TUP_EFFECT_NONE_MARK; break;
            case ICP_HD_CFG_EFFECT_GRAY: my_mark = (char *)ICP_HD_TUP_EFFECT_GRAY_MARK; break;
            case ICP_HD_CFG_EFFECT_GREEN: my_mark = (char *)ICP_HD_TUP_EFFECT_GREEN_MARK; break;
            case ICP_HD_CFG_EFFECT_COOL: my_mark = (char *)ICP_HD_TUP_EFFECT_COOL_MARK; break;
            case ICP_HD_CFG_EFFECT_YELLOW: my_mark = (char *)ICP_HD_TUP_EFFECT_YELLOW_MARK; break;
            case ICP_HD_CFG_EFFECT_SEPIA: my_mark = (char *)ICP_HD_TUP_EFFECT_SEPIA_MARK; break;
            case ICP_HD_CFG_EFFECT_PURPLE: my_mark = (char *)ICP_HD_TUP_EFFECT_PURPLE_MARK; break;
            case ICP_HD_CFG_EFFECT_RED: my_mark = (char *)ICP_HD_TUP_EFFECT_RED_MARK; break;
            case ICP_HD_CFG_EFFECT_PINK: my_mark = (char *)ICP_HD_TUP_EFFECT_PINK_MARK; break;
            case ICP_HD_CFG_EFFECT_AQUA: my_mark = (char *)ICP_HD_TUP_EFFECT_AQUA_MARK; break;
            case ICP_HD_CFG_EFFECT_NEGATIVE: my_mark = (char *)ICP_HD_TUP_EFFECT_NEGATIVE_MARK; break;
            case ICP_HD_CFG_EFFECT_SOLARIZE_1: my_mark = (char *)ICP_HD_TUP_EFFECT_SOLARIZE_1_MARK; break;
            case ICP_HD_CFG_EFFECT_SOLARIZE_2: my_mark = (char *)ICP_HD_TUP_EFFECT_SOLARIZE_2_MARK; break;
        }
        
        my_ptr = strstr(stream, my_mark);
        if (my_ptr == NULL)
        {
            CAM_INFO("Can't find EFFECT mark! [stream = 0x%08x][i = %d]", stream, i, 0);
            return;
        }
        icp_hd_tup_effect_num_params[i] = icp_hd_tup_parse_params(my_ptr, &icp_hd_tup_effect_params[i][0]);
        CAM_INFO("Number of params in %s = %d", my_mark, icp_hd_tup_effect_num_params[i], 0);

        CAM_INFO("========== %s ==========", my_mark, 0, 0);
#ifdef F_PANTECH_CAMERA_TUP_PRINT_LOAD_VALUE			
        icp_hd_tup_print_params(&icp_hd_tup_effect_params[i][0], icp_hd_tup_effect_num_params[i]);
#endif
    }

    CAM_INFO("<<%s ()", __func__);
    return;
}

/*
================================================================================
FUNCTION
================================================================================
*/
static void icp_hd_tup_load_flicker_params (const int8_t *stream)
{
    int8_t *my_ptr = NULL;
    int8_t *my_mark = NULL;
    int32_t i = 0;

    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- stream = 0x%08x", stream, 0, 0);

    for (i = 0; i < ICP_HD_CFG_FLICKER_MAX; i++)
    {
        switch (i)
        {
            case ICP_HD_CFG_FLICKER_OFF: my_mark = (int8_t *)ICP_HD_TUP_FLICKER_NONE_MARK; break;
            case ICP_HD_CFG_FLICKER_60HZ: my_mark = (int8_t *)ICP_HD_TUP_FLICKER_60HZ_MARK; break;
            case ICP_HD_CFG_FLICKER_50HZ: my_mark = (int8_t *)ICP_HD_TUP_FLICKER_50HZ_MARK; break;
            case ICP_HD_CFG_FLICKER_AUTO: my_mark = (int8_t *)ICP_HD_TUP_FLICKER_AUTO_MARK; break;           
        }
        
        my_ptr = strstr(stream, my_mark);
        if (my_ptr == NULL)
        {
            CAM_INFO("Can't find FLICKER mark! [stream = 0x%08x][i = %d]", stream, i, 0);
            return;
        }
        icp_hd_tup_flicker_num_params[i] = icp_hd_tup_parse_params(my_ptr, &icp_hd_tup_flicker_params[i][0]);
        CAM_INFO("Number of params in %s = %d", my_mark, icp_hd_tup_flicker_num_params[i], 0);

        CAM_INFO("========== %s ==========", my_mark, 0, 0);
#ifdef F_PANTECH_CAMERA_TUP_PRINT_LOAD_VALUE			
        icp_hd_tup_print_params(&icp_hd_tup_flicker_params[i][0], icp_hd_tup_flicker_num_params[i]);
#endif
    }

    CAM_INFO("<<%s ()", __func__);
    return;
}

/*
================================================================================
FUNCTION
================================================================================
*/
static void icp_hd_tup_load_antishake_params (const int8_t *stream)
{
    int8_t *my_ptr = NULL;
    int8_t *my_mark = NULL;
    int32_t i = 0;

    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- stream = 0x%08x", stream, 0, 0);

    for (i = 0; i < ICP_HD_CFG_ANTISHAKE_MAX; i++)
    {
        switch (i)
        {
            case ICP_HD_CFG_ANTISHAKE_OFF: my_mark = (int8_t *)ICP_HD_TUP_ANTISHAKE_NONE_MARK; break;
            case ICP_HD_CFG_ANTISHAKE_ON: my_mark = (int8_t *)ICP_HD_TUP_ANTISHAKE_ON_MARK; break;
        }
        
        my_ptr = strstr(stream, my_mark);
        if (my_ptr == NULL)
        {
            CAM_INFO("Can't find ANTISHAKE mark! [stream = 0x%08x][i = %d]", stream, i, 0);
            return;
        }
        icp_hd_tup_antishake_num_params[i] = icp_hd_tup_parse_params(my_ptr, &icp_hd_tup_antishake_params[i][0]);
        CAM_INFO("Number of params in %s = %d", my_mark, icp_hd_tup_antishake_num_params[i], 0);

        CAM_INFO("========== %s ==========", my_mark, 0, 0);
#ifdef F_PANTECH_CAMERA_TUP_PRINT_LOAD_VALUE			
        icp_hd_tup_print_params(&icp_hd_tup_antishake_params[i][0], icp_hd_tup_antishake_num_params[i]);
#endif
    }

    CAM_INFO("<<%s ()", __func__);
    return;
}

/*
================================================================================
FUNCTION
================================================================================
*/
static void icp_hd_tup_load_focus_step_params (const int8_t *stream)
{
    int8_t *my_ptr = NULL;
    int8_t *my_mark = NULL;
    int32_t i = 0;

    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- stream = 0x%08x", stream, 0, 0);

    for (i = 0; i < ICP_HD_CFG_FOCUS_STEP_MAX; i++)
    {
        switch (i)
        {
            case ICP_HD_CFG_FOCUS_STEP_0: my_mark = (int8_t *)ICP_HD_TUP_FOCUS_STEP_0_MARK; break;
            case ICP_HD_CFG_FOCUS_STEP_1: my_mark = (int8_t *)ICP_HD_TUP_FOCUS_STEP_1_MARK; break;
            case ICP_HD_CFG_FOCUS_STEP_2: my_mark = (int8_t *)ICP_HD_TUP_FOCUS_STEP_2_MARK; break;
            case ICP_HD_CFG_FOCUS_STEP_3: my_mark = (int8_t *)ICP_HD_TUP_FOCUS_STEP_3_MARK; break;
            case ICP_HD_CFG_FOCUS_STEP_4: my_mark = (int8_t *)ICP_HD_TUP_FOCUS_STEP_4_MARK; break;
            case ICP_HD_CFG_FOCUS_STEP_5: my_mark = (int8_t *)ICP_HD_TUP_FOCUS_STEP_5_MARK; break;
            case ICP_HD_CFG_FOCUS_STEP_6: my_mark = (int8_t *)ICP_HD_TUP_FOCUS_STEP_6_MARK; break;
            case ICP_HD_CFG_FOCUS_STEP_7: my_mark = (int8_t *)ICP_HD_TUP_FOCUS_STEP_7_MARK; break;
            case ICP_HD_CFG_FOCUS_STEP_8: my_mark = (int8_t *)ICP_HD_TUP_FOCUS_STEP_8_MARK; break;
            case ICP_HD_CFG_FOCUS_STEP_9: my_mark = (int8_t *)ICP_HD_TUP_FOCUS_STEP_9_MARK; break;
            case ICP_HD_CFG_FOCUS_STEP_10: my_mark = (int8_t *)ICP_HD_TUP_FOCUS_STEP_10_MARK; break;
        }
        
        my_ptr = strstr(stream, my_mark);
        if (my_ptr == NULL)
        {
            CAM_INFO("Can't find FOCUS_STEP mark! [stream = 0x%08x][i = %d]", stream, i, 0);
            return;
        }
        icp_hd_tup_focus_step_num_params[i] = icp_hd_tup_parse_params(my_ptr, &icp_hd_tup_focus_step_params[i][0]);
        CAM_INFO("Number of params in %s = %d", my_mark, icp_hd_tup_focus_step_num_params[i], 0);

        CAM_INFO("========== %s ==========", my_mark, 0, 0);
#ifdef F_PANTECH_CAMERA_TUP_PRINT_LOAD_VALUE		
        icp_hd_tup_print_params(&icp_hd_tup_focus_step_params[i][0], icp_hd_tup_focus_step_num_params[i]);
#endif
    }

    CAM_INFO("<<%s ()", __func__);
    return;
}

/*
================================================================================
FUNCTION
================================================================================
*/
static void icp_hd_tup_load_scene_params (const int8_t *stream)
{
    int8_t *my_ptr = NULL;
    int8_t *my_mark = NULL;
    int32_t i = 0;

    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- stream = 0x%08x", stream, 0, 0);

    for (i = 0; i < ICP_HD_CFG_SCENE_MODE_MAX; i++)
    {
        switch (i)
        {
            case ICP_HD_CFG_SCENE_MODE_OFF: my_mark = (int8_t *)ICP_HD_TUP_SCENE_NONE_MARK; break;
            case ICP_HD_CFG_SCENE_MODE_PORTRAIT: my_mark = (int8_t *)ICP_HD_TUP_SCENE_PORTRAIT_MARK; break;
            case ICP_HD_CFG_SCENE_MODE_LANDSCAPE: my_mark = (int8_t *)ICP_HD_TUP_SCENE_LANDSCAPE_MARK; break;
            case ICP_HD_CFG_SCENE_MODE_INDOOR: my_mark = (int8_t *)ICP_HD_TUP_SCENE_INDOOR_MARK; break;
            case ICP_HD_CFG_SCENE_MODE_SPORTS: my_mark = (int8_t *)ICP_HD_TUP_SCENE_SPORTS_MARK; break;
  	     case ICP_HD_CFG_SCENE_MODE_NIGHT: my_mark = (int8_t *)ICP_HD_TUP_SCENE_NIGHT_MARK; break;
            case ICP_HD_CFG_SCENE_MODE_BEACH: my_mark = (int8_t *)ICP_HD_TUP_SCENE_BEACH_MARK; break;
            case ICP_HD_CFG_SCENE_MODE_WINTER: my_mark = (int8_t *)ICP_HD_TUP_SCENE_WINTER_MARK; break;
            case ICP_HD_CFG_SCENE_MODE_SUNSET: my_mark = (int8_t *)ICP_HD_TUP_SCENE_SUNSET_MARK; break;
            case ICP_HD_CFG_SCENE_MODE_TEXT: my_mark = (int8_t *)ICP_HD_TUP_SCENE_TEXT_MARK; break;
            case ICP_HD_CFG_SCENE_MODE_PARTY: my_mark = (int8_t *)ICP_HD_TUP_SCENE_PARTY_MARK; break;
        }

        my_ptr = strstr(stream, my_mark);
        if (my_ptr == NULL)
        {
            CAM_INFO("Can't find SCENE mark! [stream = 0x%08x][i = %d]", stream, i, 0);
            return;
        }
        icp_hd_tup_scene_num_params[i] = icp_hd_tup_parse_params(my_ptr, &icp_hd_tup_scene_params[i][0]);
        CAM_INFO("Number of params in %s = %d", my_mark, icp_hd_tup_scene_num_params[i], 0);

        CAM_INFO("========== %s ==========", my_mark, 0, 0);
#ifdef F_PANTECH_CAMERA_TUP_PRINT_LOAD_VALUE
        icp_hd_tup_print_params(&icp_hd_tup_scene_params[i][0], icp_hd_tup_scene_num_params[i]);
#endif
    }

    CAM_INFO("<<%s ()", __func__);
    return;
}

/*
================================================================================
FUNCTION
================================================================================
*/
static int32_t icp_hd_tup_parse_params
(
    const int8_t *stream, 
    icp_hd_cfg_param_t *params
)
{
    int8_t *my_ptr = NULL;
//PANTECH_CAMERA    uint8 addr = 0;
//PANTECH_CAMERA    uint8 data = 0;
    u_int16_t addr = 0;
    u_int16_t data = 0;
    int32_t cnt = 0;

    CAM_INFO(">>%s ()", __func__);
    CAM_INFO("- stream = 0x%08x, params = 0x%08x", stream, params, 0);

    my_ptr = strstr(stream, ICP_HD_TUP_BEGIN_MARK);
    if (my_ptr == NULL)
    {
        CAM_INFO("Can't find BEGIN mark! [stream = 0x%08x]", stream, 0, 0);
        return 0;
    }

    my_ptr = camsensor_parser_find_line(my_ptr);
    if (my_ptr == NULL)
    {
        CAM_INFO("Can't find BEGIN mark! [stream = 0x%08x]", stream, 0, 0);
        return 0;
    }

    while (TRUE)
    {
        if (strncmp(my_ptr, ICP_HD_TUP_REG_MARK, strlen(ICP_HD_TUP_REG_MARK)) == 0)
        {
            my_ptr = (int8_t *)camsensor_parser_find_addr((u_int8_t *)(&my_ptr[4]), &addr);
            if (my_ptr != NULL)
            {
                my_ptr = (int8_t *)camsensor_parser_find_data((u_int8_t *)my_ptr, &data);
                if (my_ptr != NULL)
                {
                    (params+cnt)->op   = CAMOP_WRITE;
                    (params+cnt)->addr = addr;
                    (params+cnt)->data = data;
                    cnt++;
                }
            }
        }
	else if (strncmp(my_ptr, ICP_HD_TUP_REG_DATA_1BYTE_MARK, strlen(ICP_HD_TUP_REG_DATA_1BYTE_MARK)) == 0)
        {
            my_ptr = (int8_t *)camsensor_parser_find_addr((u_int8_t *)(&my_ptr[7]), &addr);
            if (my_ptr != NULL)
            {
                my_ptr = (int8_t *)camsensor_parser_find_data((u_int8_t *)my_ptr, &data);
                if (my_ptr != NULL)
                {
                    (params+cnt)->op   = CAMOP_WRITE_DATA1;
                    (params+cnt)->addr = addr;
                    (params+cnt)->data = data;
                    cnt++;
                }
            }
        }
        else if (strncmp(my_ptr, ICP_HD_TUP_NOP_MARK, strlen(ICP_HD_TUP_NOP_MARK)) == 0)
        {
            (params+cnt)->op   = CAMOP_NOP;
            (params+cnt)->addr = 0;
            (params+cnt)->data = 0;
            cnt++;
        }
        else if (strncmp(my_ptr, ICP_HD_TUP_DELAY_MARK, strlen(ICP_HD_TUP_DELAY_MARK)) == 0)
        {
            my_ptr = (int8_t *)camsensor_parser_find_data((u_int8_t *)&my_ptr[6], &data);
            if (my_ptr != NULL)
            {
                (params+cnt)->op   = CAMOP_DELAY;
                (params+cnt)->addr = 0;
                (params+cnt)->data = data;
                cnt++;
            }
        }
        else if (strncmp(my_ptr, ICP_HD_TUP_POLL_REG_MARK, strlen(ICP_HD_TUP_POLL_REG_MARK)) == 0)
        {
            my_ptr = (int8_t *)camsensor_parser_find_addr((u_int8_t *)&my_ptr[9], &addr);
            if (my_ptr != NULL)
            {
                my_ptr = (int8_t *)camsensor_parser_find_data(my_ptr, &data);
                if (my_ptr != NULL)
                {
                    (params+cnt)->op   = CAMOP_POLL_REG;
                    (params+cnt)->addr = addr;
                    (params+cnt)->data = data;
                    cnt++;
                }
            }
        }
        else if (strncmp(my_ptr, ICP_HD_TUP_POLL_MCU_VAR_MARK, strlen(ICP_HD_TUP_POLL_MCU_VAR_MARK)) == 0)
        {
            my_ptr = (int8_t *)camsensor_parser_find_addr((u_int8_t *)&my_ptr[13], &addr);
            if (my_ptr != NULL)
            {
                my_ptr = (int8_t *)camsensor_parser_find_data((u_int8_t *)my_ptr, &data);
                if (my_ptr != NULL)
                {
                    (params+cnt)->op   = CAMOP_POLL_MCU_VAR;
                    (params+cnt)->addr = addr;
                    (params+cnt)->data = data;
                    cnt++;
                }
            }
        }
        else if (strncmp(my_ptr, ICP_HD_TUP_END_MARK, strlen(ICP_HD_TUP_END_MARK)) == 0)
        {
            break;
        }

        my_ptr = camsensor_parser_find_line(my_ptr);
        if (my_ptr == NULL)
        {
            CAM_INFO("Can't find BEGIN mark! [stream = 0x%08x]", stream, 0, 0);
            return 0;
        }
    }

    CAM_INFO("<<%s (%d)", __func__, cnt, 0);
    return cnt;
}

/*
================================================================================
FUNCTION
================================================================================
*/
static void icp_hd_tup_print_params
(
    icp_hd_cfg_param_t *params,
    int32_t num_params
)
{
    int32_t i = 0;

    for (i = 0; i < num_params; i++)
    {
        switch ((params+i)->op)
        {
            case CAMOP_NOP:
                CAM_INFO("[0x%04x, 0x%04x] NOP", (params+i)->addr, (params+i)->data, 0);
                break;
            case CAMOP_DELAY:
                CAM_INFO("[0x%04x, 0x%04x] DELAY, %dms", (params+i)->addr, (params+i)->data, (params+i)->data);
                break;
            case CAMOP_WRITE:
                CAM_INFO("[0x%04x, 0x%04x] WRITE", (params+i)->addr, (params+i)->data, 0);
                break;
	     case CAMOP_WRITE_DATA1:
                CAM_INFO("[0x%04x, 0x%04x, 0x%02x] WRITE", (params+i)->addr, (params+i)->data, (u_int8_t)(params+i)->data);
                break;
            case CAMOP_POLL_REG:
                CAM_INFO("[0x%04x, 0x%04x] POLL_REG", (params+i)->addr, (params+i)->data, 0);
                break;
            case CAMOP_POLL_MCU_VAR:
                CAM_INFO("[0x%04x, 0x%04x] POLL_MCU_VAR", (params+i)->addr, (params+i)->data, 0);
                break;
            default:
                CAM_INFO("Invalid op code! [%d]", (params+i)->op, 0, 0);
                break;
        }
        
    }

    return;
}

#endif /* F_PANTECH_CAMERA_SENSOR_TUNEUP */
