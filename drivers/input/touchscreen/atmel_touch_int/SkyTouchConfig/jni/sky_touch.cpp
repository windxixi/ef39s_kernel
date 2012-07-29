/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               SKY_TOUCH . C P P

DESCRIPTION

Copyright (c) 2009 by  Pantech All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

when                who                                  what,where,why
--------------  ------------                        -------------------------------------------------
2010-06-07  sung.myungjin                      Create.
============================================================================*/

/*---------------------------------------------------------------------------------------------------
** Include
**-------------------------------------------------------------------------------------------------*/
#include <utils/Log.h>
#include "jni.h"
#include "JNIHelp.h"
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/ioctl.h>

/*---------------------------------------------------------------------------------------------------
** Define
**-------------------------------------------------------------------------------------------------*/
#define TOUCH_CHARGER_MODE_SET 	801

/*---------------------------------------------------------------------------------------------------
** Type define
**-------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------
** Function Declaration
**-------------------------------------------------------------------------------------------------*/
static jint sky_touch_ioctl_native(JNIEnv *env, jobject object, jint cmd, jint arg);
static jint sky_touch_mode_restore(JNIEnv *env, jobject object);
static jint sky_touch_mode_set(JNIEnv *env, jobject object, jint enable);
static jint sky_touch_mode_get(JNIEnv *env, jobject object);
/*---------------------------------------------------------------------------------------------------
** variable
**-------------------------------------------------------------------------------------------------*/
static const char *classPathName_touch ="com/pantech/device/touch/sky_touch";
char const*const TOUCH_DEVICE_FILE
        = "/dev/qt602240";
//[[mjsung
//static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
//]]mjsung

char const* const TOUCH_MODE_PATH = "/data/system/touch_mode.dat";

static JNINativeMethod methods_touch[] = {
    {"sky_touch_ioctl_java", "(II)I", (void*)sky_touch_ioctl_native},
    {"sky_touch_mode_restore_java", "()I", (void*)sky_touch_mode_restore},
    {"sky_touch_mode_set_java", "(I)I", (void*)sky_touch_mode_set},
    {"sky_touch_mode_get_java", "()I", (void*)sky_touch_mode_get},
};

int sky_touch_ctl(int cmd, int arg)
{
	int ret=-1;
	int fd = open(TOUCH_DEVICE_FILE, O_RDWR);
	if (fd >= 0) {
		ret = ioctl(fd, cmd, arg);
		close(fd);
	}

	return ret;
}

static jint sky_touch_mode_restore(JNIEnv *env, jobject object)
{
	FILE  *fd = NULL;
	char buf[32];
	int len = 0;

	memset(buf, 0, sizeof(buf));
//	memset(mode, 0, sizeof(mode));
	fd = fopen(TOUCH_MODE_PATH, "r");
	if(fd != NULL) {
		len = fread(buf, 1, sizeof(buf), fd);
//		LOGE("### buf %s, len :%d", buf, strlen(buf));
//		sprintf(mode, "%s", buf);
		if(len != 0) {
			if(!strcmp(buf, "On")) {
//				LOGE("### touch mode On\n");
			}
			else if(!strcmp(buf, "Off")) {
//				LOGE("### touch mode Off\n");
			}
			else {
				LOGE("### Error : Unknow touch mode");
			}
		}
		else {
			LOGE("### Touch Mode read failed");
		}
		
		fclose(fd);
	}
	else {
		fd = fopen(TOUCH_MODE_PATH, "w");
		if(fd != NULL) {
			fprintf(fd, "On");
//			LOGE("Create touch_mode.dat\n"); 
			fclose(fd);
		}
		else {
			LOGE("### Error : Create file -> %s\n", TOUCH_MODE_PATH);
			return 1;
		}
	}

	return len;
}

static jint sky_touch_mode_set(JNIEnv *env, jobject object, jint enable)
{
	FILE  *fd = NULL;
	char buf[32];

	memset(buf, 0, sizeof(buf));
	
	fd = fopen(TOUCH_MODE_PATH, "w");
	if(fd != NULL) {
		if(enable) {
			fprintf(fd, "On");
			sky_touch_ctl(TOUCH_CHARGER_MODE_SET, 1);
//			LOGE("~~~ Touch ON ~~~ ");
		}
		else {
			fprintf(fd, "Off");
			sky_touch_ctl(TOUCH_CHARGER_MODE_SET, 0);
//			LOGE("~~~ Touch Off ~~~ ");
		} 
		
		fclose(fd);
		return 0;
	}
	else {
		LOGE("Touch Mode open failed");
		return 1;
	}
	
	return 0;
}

static jint sky_touch_mode_get(JNIEnv *env, jobject object)
{
	FILE  *fd = NULL;
	char buf[32];
	int len=0;

	memset(buf, 0, sizeof(buf));
	
	fd = fopen(TOUCH_MODE_PATH, "r");
	if(fd != NULL) {
		len = fread(buf, 1, sizeof(buf), fd);
//		LOGE("### buf %s, len :%d", buf, strlen(buf));
		if(len != 0) {
			if(!strcmp(buf, "On")) {
//				LOGE("### touch mode On\n");
				return JNI_TRUE;
			}
			else if(!strcmp(buf, "Off")) {
//				LOGE("### touch mode Off\n");
				return JNI_FALSE;
			}
			else {
				LOGE("### Error : Unknow touch mode");
			}
		}
		else {
			LOGE("### Touch Mode read failed");
		}
		
		fclose(fd);
	}
	else {
		LOGE("Touch mode Get failed");
	}
	
	return JNI_TRUE;
}
/*---------------------------------------------------------------------------------------------------
** Name
**	sky_touch_ioctl_native
** Description
**	
** Returns 
**	
**-------------------------------------------------------------------------------------------------*/
static jint sky_touch_ioctl_native(JNIEnv *env, jobject object, jint cmd, jint arg)
{
	return sky_touch_ctl(cmd, arg);
}

/*---------------------------------------------------------------------------------------------------
** Name 
**	registerNativeMethods_test
** Description
**	Register several native methods for one class.
** Returns
**
**-------------------------------------------------------------------------------------------------*/
static int registerNativeMethods_test(JNIEnv* env, const char* className,
    JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;

    clazz = env->FindClass(className);
    if (clazz == NULL) {
        LOGE("Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }

    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        LOGE("RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

/*---------------------------------------------------------------------------------------------------
** Name
**	registerNatives_test
** Description
**	Register native methods for all classes we know about.
** Returns 
**	JNI_TRUE on success.
**-------------------------------------------------------------------------------------------------*/
static int registerNatives_test(JNIEnv* env)
{
  if (!registerNativeMethods_test(env, classPathName_touch,
                 methods_touch, sizeof(methods_touch) / sizeof(methods_touch[0]))) {
    return JNI_FALSE;
  }
  return JNI_TRUE;
}

/*---------------------------------------------------------------------------------------------------
** Name
**	JNI_OnLoad
** Description
**	
** Returns 
**	
**-------------------------------------------------------------------------------------------------*/
typedef union {
    JNIEnv* env;
    void* venv;
} UnionJNIEnvToVoid;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    jint result = -1;
    JNIEnv* env = NULL;

    LOGI("JNI_OnLoad");

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK) {
        LOGE("ERROR: GetEnv failed");
        goto bail;
    }
    env = uenv.env;

//[[mjsung
    //pthread_mutex_init(&g_lock, NULL);
//]]mjsung

    if (registerNatives_test(env) != JNI_TRUE) {
        LOGE("ERROR: registerNatives failed");
        goto bail;
    }

    result = JNI_VERSION_1_4;

bail:
    return result;
}

