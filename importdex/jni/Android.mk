LOCAL_PATH := $(call my-dir)

# prebuilt start
include $(CLEAR_VARS)
LOCAL_MODULE := android_runtime
LOCAL_SRC_FILES := prebuilt/libandroid_runtime.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
include $(PREBUILT_SHARED_LIBRARY)
# prebuilt end

include $(CLEAR_VARS)

LOCAL_MODULE:= importdex
LOCAL_SRC_FILES :=  importdex.cpp
# LOCAL_CFLAGS += -DHAVE_SYS_UIO_H
#    LOCAL_LDLIBS := -L$(NDK_PLATFORMS_ROOT)/$(TARGET_PLATFORM)/arch-arm/usr/lib -L$(LOCAL_PATH) -llog -lz -lm -landroid_runtime
#LOCAL_SHARED_LIBRARIES := android_runtime
LOCAL_LDLIBS += -llog
#-L$(LOCAL_PATH)
#-landroid_runtime
#LOCAL_STATIC_LIBRARIES := android_runtime
#LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog
LOCAL_SHARED_LIBRARIES = libandroid_runtime
LOCAL_CFLAGS += -fPIE
LOCAL_LDFLAGS  := -fPIE
include $(BUILD_SHARED_LIBRARY)

# include $(LOCAL_PATH)/prebuilt/Android.mk
