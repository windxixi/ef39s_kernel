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

/* PMEM 관련 최적화
 * MSM_FB_SIZE 와 MSM_PMEM_SF_SIZE 크기 조정
#define F_SKYDISP_PMEM_OPTIMIZE
 */

/* 
 *32bpp 적용과 관련해 수정되어야할 부분들 
 * KERNEL : used     
 * USER   : used
 */
#ifndef CONFIG_F_SKYDISP_FRAMEBUFFER_32
#define CONFIG_F_SKYDISP_FRAMEBUFFER_32
#endif

/* 
 * init 프로세스에서 뿌리는 중 Cursor 없애기 
 * KERNEL : used     
 * USER   : not used
 */
#ifndef CONFIG_F_SKYDISP_NO_CURSOR_IN_BOOT
#define CONFIG_F_SKYDISP_NO_CURSOR_IN_BOOT
#endif

/* Android Boot Animation 중에 밝기가 6에서 User Set Value로
 * 변경 되는 문제가 있음 -> 수정한 Feature 
 */
#define F_SKYDISP_SET_BACKLIGHT_BEFORE_BOOTANIM

/* 
 *gralloc 모듈에 refresh rate 계산 잘못된 부분 수정 
#define F_SKYDISP_FIX_REFRESH_RATE
 */

/* 
 * Backlight 관련 문제 수정 
 * KERNEL : used     
 * USER   : not used
 */
#ifndef CONFIG_F_SKYDISP_QBUG_FIX_BACKLIGHT
#define CONFIG_F_SKYDISP_QBUG_FIX_BACKLIGHT
#endif

/* Qualcomm이 작업한 HDMI 관련 DUAL Display용 작업 제거
 * 차후, HDMI를 사용하는 모델은 아래 Feature를 undefine하고
 * Qualcomm의 HDMI_DUAL_DISPLAY 를 define해야 한다.
 * android/device/qcom/msm8660_surf/ 에서 
 * BoardConfig.mk -> TARGET_HAVE_HDMI_OUT := false
 * system.prop -> ro.hdmi.enable=false
 * 위 두 파일이 관련 Config/Feature 이다. 유의 할 것
#define F_SKYDISP_REMOVE_HDMI
 */

/*
 * SHARP MIPI 관련 현재 오류 수정 Feature
 * KERNEL : used     
 * USER   : not used
 */
#ifndef T_PRESTO
#ifndef CONFIG_F_SKYDISP_QBUG_FIX_MIPI_ERROR
#define CONFIG_F_SKYDISP_QBUG_FIX_MIPI_ERROR
#endif
#endif

/*
 * SHARP LCD Veil View 관련 Feature
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
 * Overlay를 쓰는 SurfaceView에서 Landscape<->Portrait 전환 시
 * 화면이 일그러지는 문제점 수정
 * 1080 이후에도 필요한가?
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
 * LCD 강제 on/off 기능, sleep에서도 먹는다
 * 필요할 때 쓰면 되겠다.
 * EF33/34/35에서 Battery Charging시에 넣었다
 * KERNEL : used     
 * USER   : used
 */
#ifndef CONFIG_F_SKYDISP_LCD_FORCE_ONOFF
#define CONFIG_F_SKYDISP_LCD_FORCE_ONOFF
#endif

/*
 * Overlay 2개 사용 가능하도록 한다.
 * 35L은 GIPS 엔진으로 GLSurfaceView를 쓰기 때문에 불필요
 * 40K에서만 필요하다
 * Kang Seong-Goo, 2011.08.22, Qualcomm 기능 사용으로 삭제
#if defined(T_EF40K)
#define F_SKYDISP_DUAL_OVERLAY
#endif
 */

/*
 * Surface Information 잘못된 경우 수정
 */
#define F_SKYDISP_FIX_INVALID_SURFACE
 
/*
 * Overlay 사용 시, MDP_OV_PLAY_NOWAIT에 대한 처리 추가
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
 * MHL-HDMI 적용 시, Feature 자막/해상도 조정 API 등등
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
 * GPU composition 사용 시, ASHMEM 사용을 위한 Feature
 * BoardConfig.mk의 TARGET_GRALLOC_USES_ASHMEM := true 기본 동작은 다 하며,
 * Kernel config로 /dev/pmem 생성 자체를 막기 위한 Feature임
 * android/system/core/rootdir/ueventd.rc 에서 /dev/pmem permission 설정도 막음
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
 * Camera 종료 시, rotation 되는 거 방지 코드
 * KERNEL : not used
 * USER   : used
 */
#define F_SKYDISP_FIX_EXIT_CAMERA

/*
 * 2011.10.01
 * setCrop 시, YCbCr 에서 Cb/Cr flip 현상 개선
 * KERNEL : not used
 * USER   : used
#define F_SKYDISP_FIX_Y_CBCR_FLIP
 */

#endif  /* SKY_FRAMEWORK_FEATURE */
