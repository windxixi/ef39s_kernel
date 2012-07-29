LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

#
#   SKY_TOUCH
#

#include $(BUILD_MULTI_PREBUILT) 

#include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	sky_touch.cpp

LOCAL_SHARED_LIBRARIES := \
    libnativehelper \
    libcutils \
    libutils \
    libui

LOCAL_STATIC_LIBRARIES :=

LOCAL_C_INCLUDES += \
	external/tremor/Tremor \
	$(JNI_H_INCLUDE) \

LOCAL_CFLAGS +=

LOCAL_LDLIBS := -lpthread

LOCAL_MODULE:= libsky_touch

LOCAL_PRELINK_MODULE:= false

include $(BUILD_SHARED_LIBRARY)