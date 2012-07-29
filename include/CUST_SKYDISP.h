/*
 lived 2009.11.05
 FEATURE define
*/

#ifndef F_SKYDISP_FRAMEWORK_FEATURE
#define F_SKYDISP_FRAMEWORK_FEATURE

/* Debug Msg */
#if defined(FEATURE_AARM_RELEASE_MODE)
#define SKYDISP_MSG(...)
#else
#define SKYDISP_MSG(...)   ((void)LOG(LOG_WARN, LOG_TAG, __VA_ARGS__))
#endif

/* PMEM ���� ����ȭ
 * MSM_FB_SIZE �� MSM_PMEM_SF_SIZE ũ�� ����
#define F_SKYDISP_PMEM_OPTIMIZE
 */

/* 
 *32bpp ����� ������ �����Ǿ���� �κе� 
 * KERNEL : used     
 * USER   : used
 */
#ifndef CONFIG_F_SKYDISP_FRAMEBUFFER_32
#define CONFIG_F_SKYDISP_FRAMEBUFFER_32
#endif

/* 
 * init ���μ������� �Ѹ��� �� Cursor ���ֱ� 
 * KERNEL : used     
 * USER   : not used
 */
#ifndef CONFIG_F_SKYDISP_NO_CURSOR_IN_BOOT
#define CONFIG_F_SKYDISP_NO_CURSOR_IN_BOOT
#endif

/* Android Boot Animation �߿� ��Ⱑ 6���� User Set Value��
 * ���� �Ǵ� ������ ���� -> ������ Feature 
 */
#define F_SKYDISP_SET_BACKLIGHT_BEFORE_BOOTANIM

/* 
 *gralloc ��⿡ refresh rate ��� �߸��� �κ� ���� 
#define F_SKYDISP_FIX_REFRESH_RATE
 */

/* 
 * Backlight ���� ���� ���� 
 * KERNEL : used     
 * USER   : not used
 */
#ifndef CONFIG_F_SKYDISP_QBUG_FIX_BACKLIGHT
#define CONFIG_F_SKYDISP_QBUG_FIX_BACKLIGHT
#endif

/* Qualcomm�� �۾��� HDMI ���� DUAL Display�� �۾� ����
 * ����, HDMI�� ����ϴ� ���� �Ʒ� Feature�� undefine�ϰ�
 * Qualcomm�� HDMI_DUAL_DISPLAY �� define�ؾ� �Ѵ�.
 * android/device/qcom/msm8660_surf/ ���� 
 * BoardConfig.mk -> TARGET_HAVE_HDMI_OUT := false
 * system.prop -> ro.hdmi.enable=false
 * �� �� ������ ���� Config/Feature �̴�. ���� �� ��
#define F_SKYDISP_REMOVE_HDMI
 */

/*
 * SHARP MIPI ���� ���� ���� ���� Feature
 * KERNEL : used     
 * USER   : not used
 */
#ifndef T_PRESTO
#ifndef CONFIG_F_SKYDISP_QBUG_FIX_MIPI_ERROR
#define CONFIG_F_SKYDISP_QBUG_FIX_MIPI_ERROR
#endif
#endif

/*
 * SHARP LCD Veil View ���� Feature
#define F_SKYDISP_VEIL_VIEW
 */

/*
 * SKY Boot Logo in Kernel Feature
 * KERNEL : used     
 * USER   : not used
 */
#ifndef CONFIG_F_SKYDISP_BOOT_LOGO_IN_KERNEL
#define CONFIG_F_SKYDISP_BOOT_LOGO_IN_KERNEL
#endif

/*
 * for TARGET_USES_OVERLAY feature fix
#define F_SKYDISP_OVERLAY_FIX
 */

/*
 * for Mirror Flip Effect for Camera
 * KERNEL : not used     
 * USER   : used
 */
#define F_SKYDISP_GBUG_OVERLAY_FLIP

/*
 * Overlay�� ���� SurfaceView���� Landscape<->Portrait ��ȯ ��
 * ȭ���� �ϱ׷����� ������ ����
 * 1080 ���Ŀ��� �ʿ��Ѱ�?
#define F_SKYDISP_GBUG_FIX_OVERLAY_ORIENTATION
 */

/*
 * LCD Module Reset ##1199 Test Menu
 * KERNEL : used     
 * USER   : used
 */
#ifndef CONFIG_F_SKYDISP_LCD_RESET
#define CONFIG_F_SKYDISP_LCD_RESET
#endif

/*
 * LCD ���� on/off ���, sleep������ �Դ´�
 * �ʿ��� �� ���� �ǰڴ�.
 * EF33/34/35���� Battery Charging�ÿ� �־���
 * KERNEL : used     
 * USER   : used
 */
#ifndef CONFIG_F_SKYDISP_LCD_FORCE_ONOFF
#define CONFIG_F_SKYDISP_LCD_FORCE_ONOFF
#endif

/*
 * Overlay 2�� ��� �����ϵ��� �Ѵ�.
 * 35L�� GIPS �������� GLSurfaceView�� ���� ������ ���ʿ�
 * 40K������ �ʿ��ϴ�
 * Kang Seong-Goo, 2011.08.22, Qualcomm ��� ������� ����
#if defined(T_EF40K)
#define F_SKYDISP_DUAL_OVERLAY
#endif
 */

/*
 * Surface Information �߸��� ��� ����
 */
#define F_SKYDISP_FIX_INVALID_SURFACE
 
/*
 * Overlay ��� ��, MDP_OV_PLAY_NOWAIT�� ���� ó�� �߰�
 */
/*#define F_SKYDISP_OV_PLAY_NOWAIT*/

/*
 * LCD Gamma Table Test
#define F_SKYDISP_LCD_GAMMA_TEST
 */

/*
 * 8660 MIPI Video mode LCD display
 * by Qualcomm SR fix
#define F_SKYDISP_MIPI_VIDEO_LK_DISPLAY
 */

/*
 * MHL-HDMI ���� ��, Feature �ڸ�/�ػ� ���� API ���
 * KERNEL : used     
 * USER   : used
 */
#ifndef T_PRESTO
#ifndef CONFIG_F_SKYDISP_HDMI_OPTION
#define CONFIG_F_SKYDISP_HDMI_OPTION
#endif
#endif

/*
 * 2011.08.19
 * GPU composition ��� ��, ASHMEM ����� ���� Feature
 * BoardConfig.mk�� TARGET_GRALLOC_USES_ASHMEM := true �⺻ ������ �� �ϸ�,
 * Kernel config�� /dev/pmem ���� ��ü�� ���� ���� Feature��
 * android/system/core/rootdir/ueventd.rc ���� /dev/pmem permission ������ ����
 * KERNEL : used     
 * USER   : not used
#define CONFIG_F_SKYDISP_USE_ASHMEM
 */

/*
 * Beam On Bug Fix, because the screen does not display at Beam on  
 * KERNEL : used     
 * USER   : used
 * Author : LS4 p13156 LKS
 */
#ifdef T_PRESTO
#define CONFIG_F_SKYDISP_BEAM_ON_BUG_FIX
#endif

/*
 * 2011.09.21
 * Camera ���� ��, rotation �Ǵ� �� ���� �ڵ�
 * KERNEL : not used
 * USER   : used
 */
#define F_SKYDISP_FIX_EXIT_CAMERA

/*
 * 2011.10.01
 * setCrop ��, YCbCr ���� Cb/Cr flip ���� ����
 * KERNEL : not used
 * USER   : used
#define F_SKYDISP_FIX_Y_CBCR_FLIP
 */

#endif  /* SKY_FRAMEWORK_FEATURE */
