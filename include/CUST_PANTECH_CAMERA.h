#ifndef __CUST_PANTECH_CAMERA_H__
#define __CUST_PANTECH_CAMERA_H__


/*------------------------------------------------------------------------------

(1)  하드웨어 구성

EF39S   : APQ8060, CE1612(8M ISP), S5K6AAFX13(1.3M)
EF40K   : APQ8060, CE1612(8M ISP), S5K6AAFX13(1.3M)
PRESTO  : APQ8060, MT9P111(5M SOC), MT9V113(VGA)


(2)  카메라 관련 모든 kernel/userspace/android 로그를 제거

kernel/arch/arm/config/msm8660-perf-(모델명)_(보드버전)_defconfig 를 수정한다.

	# CONFIG_MSM_CAMERA_DEBUG is not set (default)

CUST_PANTECH_CAMERA.h 에서 F_PANTECH_CAMERA_LOG_PRINTK 을 #undef 한다.

	#define F_PANTECH_CAMERA_LOG_PRINTK (default)

모든 소스 파일에서 F_PANTECH_CAMERA_LOG_OEM 을 찾아 주석 처리한다.
	선언 된 경우, 해당 파일에 사용된 SKYCDBG/SKYCERR 매크로를 이용한
	메시지들을 활성화 시킨다. (user-space only)

모든 소스 파일에서 F_PANTECH_CAMERA_LOG_CDBG 를 찾아 주석 처리한다.
	선언 된 경우, 해당 파일에 사용된 CDBG 매크로를 이용한 메시지들을
	활성화 시킨다. (user-space only)

모든 소스 파일에서 F_PANTECH_CAMERA_LOG_VERBOSE 를 찾아 주석 처리한다.
	선언 된 경우, 해당 파일에 사용된 LOGV/LOGD/LOGI 매크로를 이용한
	메시지들을 활성화 시킨다. (user-space only)


(4)  안면인식 관련 기능 빌드 환경

vendor/qcom/android-open/libcamera2/Android.mk 를 수정한다.
	3rd PARTY 솔루션 사용 여부를 결정한다.

	PANTECH_CAMERA_FD_ENGINE := 0		미포함
	PANTECH_CAMERA_FD_ENGINE := 1		올라웍스 솔루션 사용
	PANTECH_CAMERA_FD_ENGINE := 2		기타 솔루션 사용

CUST_PANTECH_CAMERA.h 에서 F_PANTECH_CAMERA_ADD_CFG_FACE_FILTER 를 #undef 한다.
	안면인식 기능 관련 인터페이스 포함 여부를 결정한다.

libOlaEngine.so 를 기존 libcamera.so 에 링크하므로 기존 대비 libcamera.so 의
크기가 증가하여 링크 오류가 발생 가능하며, 이 경우 아래 파일들에서
liboemcamera.so 의 영역을 줄여 libcamera.so 의 영역을 확보할 수 있다.

build/core/prelink-linux-arm-2G.map (for 2G/2G)
build/core/prelink-linux-arm.map (for 1G/3G)

------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*  MODEL-SPECIFIC                                                            */
/*  해당 모델에만 적용되는 또는 해당 모델에서만 검증된 FEATURE 목록           */
/*----------------------------------------------------------------------------*/
#if defined(CONFIG_SKY_EF39S_BOARD)
#define F_PANTECH_CAMERA
#define F_PANTECH_CAMERA_TARGET_EF39S
#define F_PANTECH_CAMERA_EF39S_40K
#define F_PANTECH_CAMERA_SKT
#elif defined(CONFIG_SKY_EF40S_BOARD)
#define F_PANTECH_CAMERA
#define F_PANTECH_CAMERA_TARGET_EF40S
#define F_PANTECH_CAMERA_EF39S_40K
#define F_PANTECH_CAMERA_SKT
#elif defined(CONFIG_SKY_EF40K_BOARD)
#define F_PANTECH_CAMERA
#define F_PANTECH_CAMERA_TARGET_EF40K
#define F_PANTECH_CAMERA_EF39S_40K
#elif defined(CONFIG_PANTECH_PRESTO_BOARD)
#define F_PANTECH_CAMERA
#define F_PANTECH_CAMERA_TARGET_PRESTO
#define F_PANTECH_CAMERA_PRESTO
/* AT&T모델의 경우 추가되어야 하는 Featrue */
#define F_PANTECH_CAMERA_ATT
#endif

#ifdef F_PANTECH_CAMERA

#ifndef CONFIG_PANTECH_CAMERA
#define CONFIG_PANTECH_CAMERA
#define CONFIG_PANTECH_CAMERA_TUNER
#endif

/* QC FD 엔진은 사용하지 않고, 해당 callback 구조만 도용하여 처리한다.
 * 실제 FD 엔진은 OlaWorks 를 사용하고, QC FD 안정화 전까지는 최대한
 * 간단하게 인터페이스를 구현한다.
 * QC FD 적용 후에도 (만약 YUV 에도 사용 가능하다면...) 관련 코드 수정은
 * 필요하므로 별도의 feature 로 분리한다. */
#define F_PANTECH_CAMERA_FD

/* MSM7230 is used VFE31
   QSD8250 is used VFE41
   Sensor Definition
 */

/*
 * MEDIA_RECORDER_INFO_FILESIZE_PROGRESS CallBack, Only Use KT Phone
 * 비디오 파일 사이즈를 기록하는데 파일 사이즈를 APP에 알려주기 위해서
 * 추가
 * KT폰에서만 사용한다.
 */
#define F_PANTECH_CAMERA_ADD_EVT_RECSIZE

#ifdef F_PANTECH_CAMERA_SKT
/* F_PANTECH_CAMERA_TODO, SKT FOTA DCMO (Device Control Management Object)
 * SKT 향에만 적용되며, UI VOB에서만 define을 연다.
 * "pantech/development/sky_fota/sky_fota.h" 파일이 있어야 한다.
*/
#define F_PANTECH_CAMERA_FOTA_DCMO_CAMERA_LOCK
#endif


/*
 * InitDefaultParameters() 함수에서 surpported camera paramters를 설정을 할 때 카메라에 맞게 설정 한다.
 * effect, wb, scene등 기능을 지원 하더라도 카메라에 따라서 sub 항목 list 가 달라진다.
 * open 한 카메라에서 지원 가능한 sub list masking value를 받아서, 실제 지원 가능한 sub list만 설정한다.
 * vendor/qcom/proprietary/mm-camera/targets/tgtcommon/sensor/ 의 각 카메라 파일에서 making value를 정의한다.
*/
#define F_PANTECH_CAMERA_QUERY_SUBPARM

/*
 * 카메라 드라이버가 어플이 종료되지 않았을 때, suspend 되는 것을 막는다.
 * power control 에 의해 커널 드라이버가 suspend 되는 것을 막는다.
 * 일반적인 경우 카메라 어플이 카메라 드라이버를 종료 시키며, 이 때 커널 드라이버도 내려간다.
 * HD 영상통화의 경우 조도 센서의 control이 불가능해 LCD가 OFF 되는 상황에서 suspend가 발생한다.
 * 이 때 커널 드라이버가 suspend 되지 않도록 한다.
 * kernel/arch/arm/mach-msm/include/mach/camera.h 
 * 커널에서만 적용되는 피쳐이며 위 파일에서 정의하여 사용한다.
 * #define F_PANTECH_CAMERA_FIX_SUSPENDLOCK_ADD
*/

/*
* 카메라 control driver의 응답(waiting) 시간을 mm_camera_interface.c 에서 설정한 값으로 적용 한다.
* 무조건 10s 설정에서 timeout_ms를 사용하도록 했다.
* EF39/40의 CE1612 ISP FW를 user 영역에서 update하기 위해서는 최소 25s 이상의 시간이 소요 된다.
 * kernel/drivers/media/video/msm/msm_camera.c
 * 커널에서만 적용되는 피쳐이며 위 파일에서 정의하여 사용한다.
 * #define F_PANTECH_CAMERA_FIX_MSM_CTRL_TIMEOUT
*/

 /* PANTECH_CAMERA_PSJ_110401
 *  VT의 프리뷰 관련 수정 된 사항에 대한 Feature
 *  VT main카메라의 90도 rotation적용 및 8x60 부터 필요한 2K align버퍼에 대한 처리가 있음
 * preview buffer는 수정하지 않으며 video buffer의 data를 preview buffer 기준으로
 * 90도 rotation 시킨다.
 * 해당 기능을 사용하기 위해 먼저 F_PANTECH_CAMERA_CFG_VT에서 추가한 "pantech-vt"
 * parameter를 "on"으로 설정 하여야 한다.
 */
#define F_PANTECH_CAMERA_FIX_VT_PREVIEW

/*
 * pantech VT 카메라에 따른 적용을 위해 "pantech-vt" 파라미터를 추가 하였다.
 * "pantech-vt"를 "on"으로 설정 함에 따라 VT에서 video buffer를 rotation 하는 부분과
 * sub camera의 video 버퍼를 flip 하지 않도록 한다.
*/
 #define F_PANTECH_CAMERA_CFG_VT

/*
 * pantech VT는 호가 연결되면 전송 버퍼를 video 버퍼로부터 얻기 위해 start recording
 * 을 시작하며 따라서 connect/disconnect 시에 촬영음이 발생한다.
 * pantech VT에서 촬영음이 발생하는 것을 막기 위해 CameraService에
 * CAMERA_CMD_SET_SHUTTER_DISABLE commad를 추가 하였다.
*/
 #define F_PANTECH_CAMERA_VT_SHUTTER_DISABLE


/* PANTECH_CAMERA_PSJ_110302
 * 기존 FEATURE폰에서 사용하던 퀄컴의 IPL함수를 사용하기 위해 IPL LIB로드
 * ipl_reflect 테스트 완료
*/
#define F_PANTECH_CAMERA_USE_IPLLIB


/*
 * CamNote 어플리케이션에서 사용하는 effect를 지원하기 위해 SW IPL을 사용하도록 한다.
 * whiteboard, blackboard, whiteboard-color, blackboard-color
 * EF39S 이후 내수 모델에 적용된다.
*/
#ifdef F_PANTECH_CAMERA_USE_IPLLIB
#define F_PANTECH_CAMERA_CFG_CAMNOTE
#endif


/*
 * 기존 카메라 셔터음과 레코딩 셔터음에 레코딩 종료 음과 캠노트에서 사용하는 카메라 셔터음을 추가 한다.
 * 카메라 셔터음 -> /system/media/audio/ui/camera_click.ogg
 * 캠코더 시작음 -> /system/media/audio/ui/VideoRecord.ogg
 * 캠코더 종료음 -> /system/media/audio/ui/Cam_End.ogg
 * 캠노트 셔터음 -> /system/media/audio/ui/CAMNOTE_SOUND_4.ogg
 * 캠코더 종료음원이 없는 경우에는 캠코더 시작음을 사용하도록 한다.
 * 캠노트 셔터음이 없는 경우에는 카메라 셔터음을 사용 하도록 한다.
*/
#define F_PANTECH_CAMERA_ADD_SHUTTER_FILES

/* 
 *  runPreviewThread에서 previewWidth와 previewHeight가 변경된 Preview Size로 설정 되어지도록 수정
*/
#define F_PANTECH_CAMERA_FIX_PREVIEWSIZE

/* 내수 CS 부서에서는 소비자 시료 분석을 위해 별도 PC 프로그램을 사용하여
 * 카메라 구동 시간 정보를 PC 로 추출한다.
 *
 * 구현 방법은 공정 커맨드 사양서에 명시되어 있으므로 관련 코드들은 공정 커맨드
 * 관련 모듈에 포함되어 있으나, 공정 커맨드 용 PC 프로그램을 사용하지 않고 별도
 * 프로그램을 사용하여, 시료의 DIAG 포트로부터 구동 시간 정보를 확인할 수 있다.
 *
 * 공정 커맨드 사양서 v10.35 기반 구현
 * PhoneInfoDisplay v4.0 프로그램으로 확인
 * 사양서와 프로그램은 DS2팀 박경호 선임에게 문의 */
#define F_PANTECH_CAMERA_FACTORY_PROC_CMD

/* 카메라 장치 파일 OPEN 에 실패한 경우 (단순 I2C 버스 R/W 오류, 카메라 미장착)
 * 오류 처리를 위해 수정한다.
 *
 * 장치 파일을 OPEN 하는 과정에서 VFE 초기화 이후 카메라 HW 초기화가 이루어
 * 지는데, HW 초기화에 실패할 경우 VFE 는 초기화 된 상태로 남게되고 다음
 * OPEN 시도 시 HW 초기화에 성공한다 하더라도 이미 VFE 가 초기화된 상태이므로
 * VFE 초기화 시 오류가 발생한다.
 *
 * 호출순서 : vfefn.vfe_init() -> sctrl.s_init()
 *
 * HW 초기화에 실패할 경우, 이미 초기화된 VFE 를 RELEASE (vfe_release) 시켜
 * 다음 열기 시도 시 정상 동작하도록 수정한다.
 *
 * ECLAIR 버전에서는 위와 같은 에러 처리에도 불구하고 센서가 연결되어 있지
 * 않거나 센서 하드웨어에 이상이 발생한 경우 카메라 응용이 ANR 오류로 인해
 * 비정상 종료되고 이후 재부팅 전까지는 지속하여 재진입이 불가능하다.
 *
 * 센서가 비 정상인 경우, ISP 초기화 시 ISP 와 센서를 프리뷰 모드로 설정하는
 * 과정에서 3초 간 POLLING 수행하며, 이로 인해 타임아웃 발생하고 ANR 오류로
 * 이어진다. 비 정상 종료 이후 카메라 재진입 시 센서가 정상이라 하더라도 ANR
 * 오류 이후 응용이 비 정상적으로 종료되었으므로 FRAMEWORK 내부는 비 정상인
 * 상태로 유지되고, 이로 인해 재부팅 전까지는 카메라 응용 진입 시 "Unable to
 * connect camera device" 팝업과 함께 무조건 진입에 실패한다.
 *
 * ISP 초기화 시 프리뷰 모드 설정 이전에, ISP 를 통해 센서의 특정 레지스터를
 * 1회 READ 하고 비 정상일 경우, 이를 FRAMWORK 을 통해 응용으로 전달하여
 * 정상적으로 종료되도록 수정한다.
 *
 * 또한 ISP 자체에 이상이 발생한 경우에도, PROBE 시에 오류 발생하여 해당
 * 디바이스 파일들을 생성할 수 없으므로 FRAMWORK 내부에서 함께 처리 가능하다.
 *
 * EF10S 의 경우, BAYER 센서만 커넥터로 연결되어 있고, MV9337 은 ON-BOARD
 * 되어 있으므로, BAYER 센서가 연결되어 있지 않아도, MV9337 만 정상이라면,
 * PROBE 시 정상 동작하였으나, EF12S 의 경우, 카메라 모듈에 MV9335 가 함께
 * 인스톨되어 있어, 커넥터에 모듈이 연결되지 않으면 PROBE 시 I2C R/W 실패가
 * 지속 발생, RETRY 수행하면서 부팅 시간이 10초 이상 지연되고, 이로 인해
 * 다른 모듈들의 초기화에 직접적인 영향을 미친다. */
#define F_PANTECH_CAMERA_INVALIDATE_CAMERA_CLIENT


/* 단말에서 촬영된 사진의 EXIF TAG 정보 중 제조사 관련 정보를 수정한다. */
#define F_PANTECH_CAMERA_OEM_EXIF_TAG


/* 지원 가능한 촬영 해상도 테이블을 수정한다. 
 *
 * HAL 에서는 유효한 촬영 해상도를 테이블 형태로 관리하고 테이블에 포함된 
 * 해상도 이외의 설정 요청은 오류로 처리한다. */
#define F_PANTECH_CAMERA_CUST_PICTURE_SIZES


/* 지원 가능한 프리뷰 해상도 테이블을 수정한다. 
 *
 * HAL 에서는 유효한 프리뷰 해상도를 테이블 형태로 관리하고 테이블에 포함된 
 * 해상도 이외의 설정 요청은 오류로 처리한다. */
#define F_PANTECH_CAMERA_CUST_PREVIEW_SIZES


/* 카메라 IOCTL 커맨드 MSM_CAM_IOCTL_SENSOR_IO_CFG 를 확장한다.
 *
 * 추가한 기능 및 SOC/ISP 카메라를 감안하지 않고 미구현된 부분들을 수정 및 추가 
 * 구현한다. */
#define F_PANTECH_CAMERA_CUST_MSM_CAMERA_CFG


/* SKY캠코더 녹화파일이 Qparser로 확인시 에러발생.(deocde thumnail할수없음)
 * 캠코더 레코딩시 구글캠코더와 SKY캠코더의 차이중 하나가
 * app에서 내려오는 stagefrightrecorder의 mMaxFileDurationUs 값이다.
 * (SKY캠코더: 3600000000(us)=1시간 / 구글캠코더: 600000000(us)=10분.)
 * mMaxFileDurationUs의 차이로인해 Mpeg4write에서
 * SKY캠코더는 64bitfileoffset / 구글캠코더는 32bitfileoffset를 사용하게 된다.
 * 이를 32bitfileoffset으로 동일하게 설정하기 위해서 해당부분을 수정한다.
 * 임시로 수정되는 부분이므로 추가 검토 및 지속적인 모니터링이 필요함.
*/
#define F_PANTECH_CAMERA_VIDEO_REC_FILEOFFSET


/*
 * 카메라 디바이스가 open되지 않은 상태에서 Mediarecorder를 start하는 경우 카메라를 open 하여 preview를 start 하도록 수정.
 * 카카오톡의 동영상촬영 메뉴 선택시 Preview 화면이 나오지 않는 문제로 인해 수정하였음.
 * PVCore와 달리 Stagefright recorder의 경우에는 레코드 init이 아닌 촬영 시작할 때, 카메라를 체크하여 카메라가 open 되지 않은 경우 open 한다.
 * Stagefright recorder에서도 prepare 단계에서 카메라를 체크하여 startpreview 하도록 수정하였다.
*/
#define F_PANTECH_CAMERA_FIX_CHECK_CAM_PREVIEW


/* 
 * stagefright로 인코딩 된 동영상이 KT에서 전송이 안되는 현상
 * 동영상 트랙 헤더의 "pasp" 부분을 KT서버에서 파싱을 못하는 것으로 보여지며 SKT나 LG향에서는 정상적으로 동작이 되어짐
 * KT향의 경우 서버 수정이 불가한 상황을 대비하여 헤더의 해당 부분을 넣지 않도록 함. 향 후 KT에서 수정 될 수 있는 사항
 * pasp 가 없어도 문제 없으므로 통신사 상관 없이 빠지도록 한다.
 */
#define F_PANTECH_CAMERA_FIX_MMS_PASP

/* PANTECH_CAMERA_PSJ_110401
 * 기존 모델들도 Video buffer에 대한 align을 퀄컴 소스에서 처리를  했었으나 8x60에서는 2K(2047) align이 적용 되어있다.
 * 이 버퍼를 얻어가기 위해서는 2K align된 버퍼를 다시 정상으로 돌려야 하는데 이에 대한 작업을 위하여 버퍼 변환하는 함수 추가
 */
#define F_PANTECH_CAMERA_ADD_ALIGN_VIDEO_BUFFER

/* PANTECH_CAMERA_PSJ_110401
 * GB부터 전면카메라 구동시 좌우가 반전이 되어있으며 Surface만 돌아가 있는 것이므로 
 * Layer1에서 다시 돌려주는 부분이 필요하다
 * 이 부분은 GB Framework에 정의된 부분이므로 어떤것이 맞는 것인지는 향 후 타모델과 비교하여 정립하여야 할 것이다.
 */
/*#define F_PANTECH_CAMERA_ADD_REFLECT_FOR_SUBCAM*/


/*----------------------------------------------------------------------------*/
/*  SENSOR CONFIGURATION                                                      */
/*  모델 별 사용 센서(ISP)를 위해 수정/추가된 FEATURE 목록                    */
/*----------------------------------------------------------------------------*/
/* 카메라의 개수에 상관 없이 오직 SOC/ISP 카메라(들)만 사용할 경우 선언한다.
 *
 * 영상통화를 위해 두 개의 카메라를 사용하고, 하나는 SOC 타입, 다른 하나는
 * BAYER 타입인 경우에는 선언하지 않는다. 선언할 경우, BAYER 카메라를 위한
 * 일부 코드들이 실행되지 않는다.
 *
 * SOC/ISP 카메라만 사용하므로, BAYER 관련 코드들을 
 * 검증하지 않았고, 일부는 아래 FEATURE 들을 사용하여 주석 처리하였다. */
#define F_PANTECH_CAMERA_YUV_SENSOR_ONLY

#ifdef F_PANTECH_CAMERA_TARGET_EF39S
/* ISP backend camera ISP */
#if (BOARD_REV == PT10)
#define F_PANTECH_CAMERA_ICP_HD
#else
#define F_PANTECH_CAMERA_CE1612
#endif
/* 1.3M front camera sensor */
#define F_PANTECH_CAMERA_S5K6AAFX13
#endif

#ifdef F_PANTECH_CAMERA_TARGET_PRESTO
#if (BOARD_REV >= WS20)
#define F_PANTECH_CAMERA_S5K4ECGX
#else
#define F_PANTECH_CAMERA_MT9P111
#endif
#define F_PANTECH_CAMERA_MT9V113
#endif

#if defined(F_PANTECH_CAMERA_TARGET_EF40S) || \
    defined(F_PANTECH_CAMERA_TARGET_EF40K)
#if (BOARD_REV >= WS20)
#define F_PANTECH_CAMERA_MT9D113
#else
#define F_PANTECH_CAMERA_S5K6AAFX13
#endif
#define F_PANTECH_CAMERA_CE1612
#endif

#define F_PANTECH_CAMERA_CUST_ORIENTATION

#ifdef F_PANTECH_CAMERA_CE1612
#define F_PANTECH_CAMERA_CFG_WDR
#define F_PANTECH_CAMERA_ADD_CFG_UPDATE_ISP
#define F_PANTECH_CAMERA_ADD_CFG_READ_REG

#define F_PANTECH_CAMERA_CFG_STOP_CAPTURE
#endif	/*#ifdef F_PANTECH_CAMERA_CE1612*/

#ifdef F_PANTECH_CAMERA_YUV_SENSOR_ONLY

/*#define F_PANTECH_CAMERA_TUNER*/

/* ISP 자체에서 지원 센서 ZOOM 을 설정하기 위한 인터페이스를 추가한다. 
 * EF10S/EF12S 에서는 QUALCOMM ZOOM 을 사용하며, 참고용으로 코드들은 남겨둔다.
 *
 * ISP 자체 ZOOM 의 경우, 프리뷰/스냅샷 모드에서 이미 ZOOM 이 적용된 이미지가 
 * 출력되며 두 가지 모드를 지원한다.
 *
 * 1) DIGITAL (SUBSAMPLE & RESIZE)
 *     프리뷰/스냅샷 해상도별로 동일한 배율을 지원한다. ISP 내부에서 
 *     이미지를 SUBSAMPLE 하여 RESIZE 후 출력하며, 이로 인해 ZOOM 레벨이
 *     0 이 아닌 값으로 설정된 경우 프리뷰 FPS 가 1/2 로 감소된다.
 * 2) SUBSAMPLE ONLY
 *     프리뷰/스냅샷 해상도별로 상이한 배율을 지원한다. ISP 내부에서 
 *     SUBSAMPLE 만 적용하므로 낮은 해상도에서는 높은 배율을 지원하고 최대 
 *     해상도에서는 ZOOM 자체가 불가능하다. 프리뷰 FPS 는 감소되지 않는다.
 *
 * QUALCOMM ZOOM 적용 시, 카메라의 경우 모든 해상도에서 동일 배율 ZOOM 이 
 * 가능하므로 이를 사용하며, 추후 참고를 위해 해당 코드들은 남겨둔다. 
 *
 * 관련 FEATURE : F_PANTECH_CAMERA_ADD_CFG_DIMENSION */
/* #define F_PANTECH_CAMERA_ADD_CFG_SZOOM */


/* ISP 에서 지원되는 손떨림 보정 기능 (Digital Image Stabilization) 을 위한
 * 인터페이스를 추가한다. 
 *
 * 상하/좌우 일정 패턴으로 흔들릴 경우만 보정 가능하다. 
 * 장면 모드를 OFF 이외의 모드로 설정할 경우, 기존 손떨림 보정 설정은 
 * 무시된다. */
#define F_PANTECH_CAMERA_ADD_CFG_ANTISHAKE


/* AF WINDOW 설정을 위한 인터페이스를 수정한다. SPOT FOCUS 기능 구현 시 
 * 사용한다.
 *
 * ISP 에서는 프리뷰 모드 출력 해상도를 기준으로 가로/세로를 각각 16 개의 
 * 구간으로 총 256 개 블럭으로 나누어 블럭 단위로 AF WINDOW 설정이 가능하다. 
 * 응용에서는 프리뷰 해상도를 기준으로 사용자가 터치한 지점의 좌표를 HAL 로 
 * 전달하고, HAL 에서는 이를 블럭 좌표로 변환하여 ISP 에 설정한다. 
 * 이후 AF 수행 시 이 WINDOW 에 포함된 이미지에서만 FOCUS VALUE 를 측정하여
 * 렌즈의 위치를 결정한다.
 *
 * ISP 의 출력은 고정된 상태에서 QUALCOMM ZOOM 을 사용하여 SUBSAMPLE/RESIZE
 * 하기 때문에 ZOOM 이 0 레벨 이상으로 설정된 경우, HAL 에서 좌표-to-블록
 * 변환식이 복잡해지고, 특정 ZOOM 레벨 이상일 경우 몇 개의 블록 안에 전체
 * 프리뷰 영역이 포함되어 버리므로 설정 자체가 의미가 없다.
 * 그러므로, 응용은 SPOT FOCUS 기능 사용 시에는 ZOOM 기능을 사용할 수 없도록 
 * 처리 해야한다. */
#define F_PANTECH_CAMERA_FIX_CFG_FOCUS_RECT


/* QUALCOMM BAYER 솔루션 기반의 화이트밸런스 설정 인터페이스를 수정한다. 
 *
 * 장면 모드를 OFF 이외의 모드로 설정할 경우, 기존 화이트밸런스 설정은 
 * 무시된다. */
#define F_PANTECH_CAMERA_FIX_CFG_WB


/* QUALCOMM BAYER 솔루션 기반의 노출 설정 인터페이스를 수정한다. 
 *
 * 장면 모드를 OFF 이외의 모드로 설정할 경우, 기존 노출 설정은 무시된다. */
#define F_PANTECH_CAMERA_FIX_CFG_EXPOSURE


/* 장면 모드 설정을 위한 인터페이스를 추가한다. 
 *
 * 장면 모드를 OFF 이외의 값으로 설정할 경우 기존 화이트밸런스/측광/손떨림보정/
 * ISO 설정은 무시된다. 응용에서 장면 모드를 다시 OFF 로 초기화 하는 경우, 
 * 화이트밸런스/측광/손떨림보정/ISO 는 HAL 에서 기존 설정대로 자동 복구되므로,
 * 응용은 복구할 필요 없다. (HW 제약사항이므로, HAL 에서 제어한다.) */
#define F_PANTECH_CAMERA_FIX_CFG_SCENE_MODE


/* 플리커 설정을 위한 인터페이스를 수정한다.
 *
 * 2.1 SDK 에는 총 네 가지 모드 (OFF/50Hz/60Hz/AUTO) 가 명시되어 있으나, 
 * ISP 의 경우 OFF/AUTO 가 지원되지 않는다. 그러므로, 응용이 OFF 로 설정 
 * 시에는 커널 드라이버에서 60Hz 로 설정하고, AUTO 로 설정할 경우 HAL 에서 
 * 시스템 설정 값 중 국가 코드 ("gsm.operator.numeric", 앞 3자리 숫자) 를 읽고, 
 * 국가별 Hz 값으로 변환하여 해당 값으로 설정한다.
 *
 * 기획팀 문의 결과, 플리커는 일반적인 기능이 아니므로, 국가 코드를 인식하여 
 * 자동으로 설정할 수 있도록 하고, 수동 설정 메뉴는 삭제 처리한다. */
#define F_PANTECH_CAMERA_FIX_CFG_ANTIBANDING

/*
 * AT&T 향에는 Shutter sound On/OFF기능이 들어간다. 이것을 추가하기 위해서
 * 이 기능 구현을 추가하기 위해서 넣은 코드 
 */
#ifdef F_PANTECH_CAMERA_ATT
#define F_PANTECH_CAMERA_SET_SHUTTER_SOUND
#endif

/* 
 * ATT의 경우에는 FOCUS MODE의 경우 AUTO, INFINITY, MACRO 3가지 모드가 존재한다.
 * PANTECH_CAMERA의 경우에는 Manual모드가 존재를 하고 이 Manual모드에서 Focus Step를 조절 할 수 있다.
 * 하지만 ATT의 경우에는 이러한 Focus Step을 조절 하는 UI가 존재하지 않는다.
 * INFINITY의 경우는 FOCUS_STEP을 MAX를 설정해줘야 하고,
 * MACRO의 경우는 FOCSU_STEP를 MIN값으로 설정해야하기 
 * setFocusMode에서 FOCUS_STEP를 값을 넣어준다.
 */
#ifdef F_PANTECH_CAMERA_ATT
#define F_PANTECH_CAMERA_ATT_FOCUS_STEP
/* Add By Lee Jitae 2011-06-08
 * GS3팀 AT&T향에 4MP 사이즈가 들어가야 한다는 요청으로 추가한 
 * 사이즈이다. 내수 사양에는 들어가지 않는 사이즈
 */
#define F_PANTECH_CAMERA_ATT_PICTURE_SIZE
#endif

/* 플래쉬 LED 설정을 위한 인터페이스를 수정한다.
 *
 * QUALCOMM 에서는 별도의 IOCTL (MSM_CAM_IOCTL_FLASH_LED_CFG) 커맨드를
 * 사용하여 구현되어 있으며, PMIC 전원을 사용하는 LED 드라이버를 제공한다.
 * MAXIM칩과 후면카메라의 컨트롤로 FLASH구현
 *
 * AUTO 모드로 설정할 경우, 저조도 일 경우에만 AF 수행 중 AF/AE 를 위해
 * 잠시 ON 되고, 실제 스냅샷 시점에서 한 번 더 ON 된다. */
#define F_PANTECH_CAMERA_FIX_CFG_LED_MODE

/* 플래쉬 모드 Auto에 대해서 ISP에서 AF,Capture flash on을 지원하지 않는 경우
 * 또는 gpio를 통해 직접 flash를 control 하는 경우에, 캡쳐시 flash on을 설정하고
 * 캡쳐 완료시에 flash off를 설정 하도록 한다.
 * 이때, kernel driver의 sensor_config에서 flash on을 시키며, HAL에서 캡쳐 frame을 받으면 off한다.
 * auto인 경우에 카메라 모듈로 부터 brightness를 읽어서 이에 따라 flash on을 결정한다.
 * ISP 에서 AF, Capture flash On/Auto를 지원하는 경우에는 정의 하지 않는다.
*/

#if defined(F_PANTECH_CAMERA_S5K4ECGX) || defined(F_PANTECH_CAMERA_MT9P111)
#define F_PANTECH_CAMERA_LED_MODE_AUTO
#endif

/* ISO 설정을 위한 인터페이스를 수정한다.
 *
 * 기획팀 문의 결과, AUTO 모드에서의 화질에 큰 이상이 없으므로 수동으로
 * ISO 를 변경할 수 있는 메뉴는 삭제 처리한다.
 * 장면 모드를 OFF 이외의 모드로 설정할 경우, 기존 ISO 설정은 무시된다. */
#define F_PANTECH_CAMERA_FIX_CFG_ISO

/* 특수효과 설정을 위한 인터페이스를 수정한다.
 *
 * SDK 2.1 에 명시된 효과들 중 일부만 지원한다. MV9337/MV9335 의 경우 SDK 에
 * 명시되지 않은 효과들도 지원하지만 응용에서 사용하지 않으므로 별도 추가는
 * 하지 않는다. */
#define F_PANTECH_CAMERA_FIX_CFG_EFFECT


/* MANUAL FOCUS 설정을 위한 인터페이스를 수정한다. 
 *
 * MANUAL FOCUS 모드로 설정할 경우, 즉 응용이 임의로 렌즈의 위치를
 * 이동시킬 경우, 스냅샷 직전에 AF 가 수행되지 않는 것이 정상이며, 이로 인해 
 * ISP 에서 조도를 측정하여 보정할 수 있는 시간이 없으므로 플래쉬가 ON 될
 * 경우, 프리뷰/스냅샷 이미지가 상이할 수 있다. 그러므로 응용은 MANUAL FOCUS 
 * 모드 진입 시 반드시 플래쉬 모드를 OFF 로 설정해야 하고, AUTO FOCUS 또는 
 * SPOT FOCUS 모드 진입 시 원래 모드로 복구시켜야 한다. */
#define F_PANTECH_CAMERA_FIX_CFG_FOCUS_STEP


/* 밝기 설정을 위한 인터페이스를 수정한다. */
#define F_PANTECH_CAMERA_FIX_CFG_BRIGHTNESS

/* 프리뷰 회전각 설정을 위한 인터페이스를 수정한다.
 *
 * 스냅샷 이후 JPEG 인코드 시와 안면인식 필터 (셀프샷) 적용 시 카메라의 
 * 회전 상태를 입력하여야 한다. 응용은 OrientationListener 등록 후 아래와 같은
 * 시점에 HAL 에 회전각 값을 설정 해주어야 한다.
 * 
 * JPEG 인코드
 *     인코드 직전에 설정
 * 셀프샷 모드
 *     변경 시 매번 설정
*/
#define F_PANTECH_CAMERA_FIX_CFG_ROTATION


/* AF 동작을 위한 인터페이스를 수정한다. 
 * AF 모드는 NORMAL, MACRO, SPOT 이 있다.
 * NORMAL : 10 ~ 무한대 
 * MACRO : 10 ~ 30 
 * SPOT : 터치 좌표 (NORMAL )AF
*/
#define F_PANTECH_CAMERA_FIX_CFG_AF

#define F_PANTECH_CAMERA_FIX_CFG_REFLECT


/* 안면인식 기반 이미지 필터 설정을 위한 인터페이스를 추가한다.
 *
 * EF10S/EF12S 에서는 올라웍스 솔루션을 사용하며, 프리뷰/스냅샷 이미지에서 
 * 얼굴 위치를 검출하여 얼굴 영역에 마스크나 특수효과를 적용할 수 있다. 
 *
 * vendor/qcom/android-open/libcamera2/Android.mk 에서 올라웍스 라이브러리를
 * 링크시켜야만 동작한다. PANTECH_CAMERA_FD_ENGINE 를 1 로 설정할 경우 올라웍스 
 * 솔루션을 사용하고, 0 으로 설정할 경우 카메라 파라미터만 추가된 상태로 관련 
 * 기능들은 동작하지 않는다. 다른 솔루션을 사용할 경우 이 값을 확장하여 
 * 사용한다. */
#define F_PANTECH_CAMERA_ADD_CFG_FACE_FILTER

/* 프리뷰 FPS 설정을 위한 인터페이스를 변경한다. 
 *
 * 1 ~ 30 까지 설정 가능하며 의미는 다음과 같다.
 *
 * 5 ~ 29 : fixed fps (조도에 관계 없이 고정) ==> 캠코더 프리뷰 시 사용
 * 30 : 8 ~ 30 variable fps (조도에 따라 자동 조절) ==> 카메라 프리뷰 시 사용
 *
 * MV9337/MV9335 은 프리뷰 모드에서 고정 1 ~ 30 프레임과 변동 8 ~ 30 프레임을 
 * 지원하며, EF10S/EF12S 에서는 동영상 녹화 시 24fps (QVGA MMS 의 경우 15fps) 으로
 * 설정하고, 카메라 프리뷰 시에는 가변 8 ~ 30fps 으로 설정한다. */
#define F_PANTECH_CAMERA_FIX_CFG_PREVIEW_FPS


/* 멀티샷 설정을 위한 인터페이스를 추가한다.
 * 
 * 연속촬영, 분할촬영 4컷/9컷 모드에서 매 촬영 시마다 센서 모드를 변경하는 
 * 것을 방지한다. 첫 번째 촬영에서 센서를 스냅샷 모드로 변경하고, VFE/CAMIF
 * 설정까지 완료된 상태이므로, 두 번째 촬영부터는 스냅샷/썸네일/JPEG 버퍼만
 * 초기화하고 VFE/CAMIF 에 스냅샷 명령만 송신한다.
 *
 * 제약사항 : 저조도에서 플래쉬 모드를 자동으로 설정하고 연속촬영, 분할촬영 
 * 4컷/9컷 모드로 설정 후 촬영 시, 첫 번째 촬영에서만 플래쉬가 ON 되고, 
 * 두 번째부터는 ON 되지 않으므로 연속촬영, 분할촬영 4컷/9컷 모드에서는
 * 응용이 반드시 플래쉬를 OFF 해야하며, 이외 모드로 설정 시 이전 설정을
 * 스스로 복구시켜야 한다. */
#define F_PANTECH_CAMERA_ADD_CFG_MULTISHOT


/*  Continuous AF를 ON, OFF를 할 수 있다
 *  AF 가능한 SOC/ISP 모듈에서 CAF를 지원하는 경우 사용한다.
 *  CE1612 르네사스 ISP 에서는 AF-C를 설정하며, camcorder에서 사용한다.
 *  이는 Android Dev에서 권고하는 "continuous-video" 이다.
*/
#define F_PANTECH_CAMERA_ADD_CFG_CAF


/*  Continuous AF를 ON, OFF를 할 수 있다
 *  "pantech-caf-camera" 파라미터를 추가하여 사용하며, "continuous-video"와 달리 full-scan을 하여 빠르게 focusing을 한다.
 *  "continuous-video"와 달리 "focus-mode"로 설정하지 않기 때문에 영역 연동 포커싱과 같이 사용 할 수 있다.
 *  CE1612 르네사스 ISP 에서는 AF-T를 설정하며, camera에서 사용한다.
 *  CE1612 르네사스 ISP 에서는 피사체의 변화를 검출한다.
*/
#define F_PANTECH_CAMERA_ADD_CFG_CAF_CAMERA
#endif /* F_PANTECH_CAMERA_YUV_SENSOR_ONLY */


#ifdef F_PANTECH_CAMERA_ADD_CFG_FACE_FILTER
/* 안면인식 솔루션 사용 시, 검출된 다수의 얼굴들에 선택적으로 필터를 적용할
 * 수 있는 인터페이스를 추가한다.
 *
 * 프리뷰 이미지 상 다수의 얼굴 영역들을 검출하고 해당 영역을 터치할 경우
 * 해당 얼굴 영역에 필터를 적용할 지 여부를 ON/OFF 방식으로 선택할 수 있다.
*/
#define F_PANTECH_CAMERA_ADD_CFG_FACE_FILTER_RECT
#endif

/* 
 * Gingerbread에서 OpenCore에서 Stagefright를 사용하도록 수정 되었다.
 * EF31S와 EF32K에서 비디오 recording시 stagefright 사용이 문제가 되어
 * 우선 OpenCore를 사용하도록 한다.
 * Stagefright에 대해서는 디버깅 진행 후 적용 한다.
*/
/*#define F_PANTECH_CAMERA_CFG_OPENCORE*/

#define F_PANTECH_CAMERA_ADD_CFG_DIMENSION
/* PANTECH_CAMERA PSJ 110224
 * 1080P의 세팅을 dynamic하게 바꿀 수 있도록 드라이버 및 퀄컴 관련 코드 추가 및 수정
 * ISP의 출력이 1080P로 세팅되며 MSM 에서 해당 사이즈를 받을 수 있도록 set dimension을 함
 * 기존의 set dimension 함수에 구현이 되어 있어 F_PANTECH_CAMERA_ADD_CFG_DIMENSION feature에
 * dependency를 가지고 있음
 */
#ifdef F_PANTECH_CAMERA_ADD_CFG_DIMENSION
#define F_PANTECH_CAMERA_1080P_PREVIEW
#endif

/*
 * Gingerbread의 CameraService에서 lockIfMessageWanted(int32_t msgType) 함수가 추가 되었다.
 * CameraService의 callback 처리 함수에서 mLock을 check하여 LOCK 상태이면, UNLOCK까지 waiting 한다.
 * capture 수행 도중 UI 로부터 command가 내려오면 callback 함수에서 이로 인해 지연이 발생한다.
 * capture 수행 중 카메라 종료시 이로 인해 CameraHAL보다 UI가 먼저 종료 되는 경우가 발생한다.
 * UI가 먼저 종료되고 CameraHAL 종료전에 다시 Camera가 실행되면 정상적으로 Open 하지 못한다.
 * lockIfMessageWanted 함수를 사용 하지 않도록 수정하였다.
*/
#define F_PANTECH_CAMERA_FIX_CS_TRYLOCK

/*
camera id별로 검색하여 각각 app에서 후면 카메라, 전면 카메라 따로 동작시 진입 가능하게 되어
진입시 open이 비슷한 시기에 되거나(홈키 long 키, 전환), setparameter가 셋팅되는 현상등이 발생하여,
전혀 의도하지 않은 값이 써져 오동작 하는 문제로
froyo와 마찬가지로 전 후면 모든 카메라가 이전 카메라 release 이전에는 진입 불가하도록 수정

HW, QCH 모두 개별의 카메라 동작을 지원한다면 아래를 제거한 후 테스트 할 것.
*/
#define F_PANTECH_CAMERA_FIX_CS_CONNECT


/*
 * zoom crop을 포인터로 받아서 사용하는 경우 처리가 늦어져 crop의 data가 refresh 될 수 있음
*/
#define F_PANTECH_CAMERA_FIX_PREVIEW_ZOOM

/* CTS qualcomm bug 수정 
 */
#define F_PANTECH_CAMERA_FIX_CFG_FOCUS_DISTANCES

#define F_PANTECH_CAMERA_FIX_CFG_FPS_RANGE

#define F_PANTECH_CAMERA_FIX_CANCEL_AF


/*카메라 재 진입시 이전에 비정상 종료 등의 이유로 카메라 드라이버가 정상 종료 되지 않았을때
를 위한 방어 코드 - 재 진입시 드라이버 비정상 상태이면, 현재 카메라 reject하고 driver 종료*/
#define F_PANTECH_CAMERA_DEVICE_CONNECT_FAIL_FIXED


/* 카메라 Open 할때 I2C fail로 인해 진입 실패 한경우, 재진입 안되는 문제 수정
 * 커널 영역의 수정 부분이 있으며, 유저영역과 피쳐를 분리하여 사용하게 된다.
 * 커널영의의 피쳐는 아래 경로에 정의하여 사용한다.
 * kernel/drivers/media/video/msm/msm_camera.c
 * 커널과 유저 영역에 적용되는 피쳐이다.
 */
#define F_PANTECH_CAMERA_FIX_MSM_OPEN_FAIL

/* vfe가 비 정상 상태 일 때, 카메라 종료시 vfe에 잘못 된 명령을 보내 stop등의 명령 처리시 
config_proc.c의 *((int *)(0xc0debadd)) = 0xdeadbeef; 비정상 처리로 인한 mediaserver 죽는 문제에
대한 방어 코드 적용 */
#define F_PANTECH_CAMERA_DEADBEEF_ERROR_FIX

/* 커널 영역 코드에서 SKYCDBG/SKYCERR 매크로를 사용하여 출력되는 메시지들을
 * 활성화 시킨다. 
 * kernel/arch/arm/mach-msm/include/mach/camera.h 
 * 위 파일에서 #define F_PANTECH_CAMERA_LOG_PRINTK을 정의하여 로그를 열 수 있다.
*/
/* 유저 영역(vendor)의 SKYCDBG/SKYCERR 메세지 on off */
#define F_PANTECH_CAMERA_LOG_OEM		

/* 유저 영역의 LOGV/LOGD/LOGI 메세지 on off */
#define F_PANTECH_CAMERA_LOG_VERBOSE

/* QUALCOMM 릴리즈 코드에 디버그 용 메시지를 추가한다. */
#define F_PANTECH_CAMERA_LOG_DEBUG

/*
 * 비디오 레코딩 시작과 종료 음이 끊기는 현상에 대한 디버깅
 * 효과음 재생 중 시스템 부하로 인해 소리가 끊기는 경우 발생
 * 레코딩 시작음 start 이 후 시작음 종료 될 때까지 wating
 * EF31S/EF32K 에서는 Sound쪽 kernel message가 나오는 경우 소리가 끊기며,
 * kernel message를 막거나 release build에서는 현상 발생 안함.
*/
#define F_PANTECH_CAMERA_QBUG_REC_BEEP_SOUND


/* 1600x1200, 1600x960 해상도에서 "zoom" 파라미터를 21 로 설정 후 스냅샷 시
 * 썸네일 YUV 이미지를 CROP 하는 과정에서 발생하는 BUFFER OVERRUN 문제를
 * 임시 수정한다.
 *
 * QualcommCameraHardware::receiveRawPicture() 에서 crop_yuv420(thumbnail)
 * 호출 시 파라미터는 width=512, height=384, cropped_width=504,
 * cropped_height=380 이며 memcpy 도중에 SOURCE 주소보다 DESTINATION 주소가
 * 더 커지는 상황이 발생한다.
 *
 * R5040 에서 QUALCOMM 수정 확인 후 삭제한다. (SR#308616)
 * - R407705 FIXED ISSUE 6 번 참조 */
/* #define F_PANTECH_CAMERA_QBUG_ZOOM_CAUSE_BUFFER_OVERRUN */


/* 모든 해상도에서 ZOOM 을 특정 레벨 이상으로 설정할 경우,
 * EXIFTAGID_EXIF_PIXEL_X_DIMENSION, EXIFTAGID_EXIF_PIXEL_Y_DIMENSION 태그
 * 정보에 잘못된 값이 저장되는 문제를 임시 수정한다.
 *
 * R5040 에서 QUALCOMM 수정 확인 후 삭제한다. (SR#307343)
 * - R4077 FIXED ISSUE 12 번 참조
 * - vendor/qcom/proprietary/mm-still/jpeg/src/jpeg_writer.c 의 기존 임시 수정
 *   코드는 구조가 변경되어 삭제 */
/* #define F_PANTECH_CAMERA_QBUG_EXIF_IMAGE_WIDTH_HEIGHT */


/* PANTECH_CAMERA_PSJ_100610
 * 촬영이 종료되지 않은 상황에서 Stop preview를 하여 메모리가 해제되는 상황 방지
*/
/* capture 관련 분석 후 적용 되어야 함*/
/*#define F_PANTECH_CAMERA_QBUG_SNAPSHOT_RELEASE_INTERRUPT*/

/* 동영상 녹화 시작/종료를 빠르게 반복하거나, 이어잭을 장착한 상태에서 연속촬영
 * 모드로 촬영할 경우, MediaPlayer 가 오동작하면서 HALT 발생한다.
 *
 * MediaPlayer 의 경우, 동일한 음원을 재생 중에 또 다시 재생 시도할 경우 100%
 * 오동작하므로 동일 음원을 연속하여 재생해야 할 경우, 반드시 이전 재생이 완료
 * 되었는지 여부를 확인 후 재생해야 한다. */
#define F_PANTECH_CAMERA_QBUG_SKIP_CAMERA_SOUND

/* 촬영음/녹화음 재생 중에 응용이 종료될 경우, CLIENT 소멸 시에 해당 촬영음/
 * 녹화음 오브젝트가 강제로 disconnect/clear 되면서 MEDIA SERVER 가 죽는 것을
 * 방지한다. */
#define F_PANTECH_CAMERA_QBUG_STOP_CAMERA_SOUND

/* 8x60에서 녹화된 동영상(MMS/MPEG4/320x240)이 기존 출시모델들에서 decoding시 깨짐현상 발생함.
 * 퀄컴SR을 통해 관련내용 workaround로 적용함.
 * recording size가 320x240 보다 작거나 같을경우 Simple Profile/Level0 로 강제 설정하여 encoding 함.
*/
#define F_PANTECH_CAMERA_QBUG_DECODING_OTHER_CHIPSET

/* CTS qualcomm bug 수정
 */
#define F_PANTECH_CAMERA_QBUG_CTS

/*----------------------------------------------------------------------------*/
/*  MODEL-SPECIFIC CONSTANTS                                                  */
/*  모델 관련 상수 선언                                       */
/*----------------------------------------------------------------------------*/

/* camera select enum */
#define C_PANTECH_CAMERA_SELECT_MAIN_CAM 		0
#define C_PANTECH_CAMERA_SELECT_SUB_CAM 		1


/* 카메라 동작 시간을 저장하기 위한 데이터 파일명이다. */
#ifdef F_PANTECH_CAMERA_FACTORY_PROC_CMD
#define C_PANTECH_CAMERA_FNAME_FACTORY_PROC_CMD	"/data/.factorycamera.dat"
#endif


/* 설정 가능한 최소/최대 포커스 레벨이다. */
#ifdef F_PANTECH_CAMERA_FIX_CFG_FOCUS_STEP
#define C_PANTECH_CAMERA_MIN_FOCUS_STEP 0	/* 무한대 (default) */
#define C_PANTECH_CAMERA_MAX_FOCUS_STEP 9	/* 매크로 */
#endif


/* 설정 가능한 최소/최대 밝기 단계이다. */
#ifdef F_PANTECH_CAMERA_FIX_CFG_BRIGHTNESS
#define C_PANTECH_CAMERA_MIN_BRIGHTNESS 0	/* -4 */
#define C_PANTECH_CAMERA_MAX_BRIGHTNESS 8	/* +4 */
#endif


#ifdef F_PANTECH_CAMERA_OEM_EXIF_TAG
#define C_PANTECH_CAMERA_EXIF_MAKE		"PANTECH"
#define C_PANTECH_CAMERA_EXIF_MAKE_LEN		8		/* including NULL */
#ifdef F_PANTECH_CAMERA_TARGET_EF39S
#define C_PANTECH_CAMERA_EXIF_MODEL		"IM-A800S"
#define C_PANTECH_CAMERA_EXIF_MODEL_LEN		9		/* including NULL */
#endif
#ifdef F_PANTECH_CAMERA_TARGET_EF40S
#define C_PANTECH_CAMERA_EXIF_MODEL		"IM-A810S"
#define C_PANTECH_CAMERA_EXIF_MODEL_LEN		9		/* including NULL */
#endif
#ifdef F_PANTECH_CAMERA_TARGET_EF40K
#define C_PANTECH_CAMERA_EXIF_MODEL		"IM-A810K"
#define C_PANTECH_CAMERA_EXIF_MODEL_LEN		9		/* including NULL */
#endif
#ifdef F_PANTECH_CAMERA_TARGET_PRESTO
#define C_PANTECH_CAMERA_EXIF_MODEL		"PRESTO"
#define C_PANTECH_CAMERA_EXIF_MODEL_LEN		7		/* including NULL */
#endif
#endif


/* 센서가 프리뷰 모드일 경우, 출력 가능한 FPS 의 최소/최대 값이다. */
#ifdef F_PANTECH_CAMERA_FIX_CFG_PREVIEW_FPS
#define C_PANTECH_CAMERA_MIN_PREVIEW_FPS	5
//#define C_PANTECH_CAMERA_MAX_PREVIEW_FPS	30
#endif

#endif /* F_PANTECH_CAMERA */

#endif /* CUST_PANTECH_CAMERA.h */
