LOCAL_PATH := $(call my-dir)

CINDER_PATH        := $(LOCAL_PATH)/../../../..
CINDER_MODULE_PATH := ../../../../android/obj/local/$(TARGET_ARCH_ABI)

include $(CINDER_PATH)/android/jni/CinderModules.mk

include $(CLEAR_VARS)

LOCAL_MODULE     := Accelerometer
LOCAL_C_INCLUDES := $(CINDER_PATH)/include \
					$(CINDER_PATH)/boost 

LOCAL_SRC_FILES := ../../src/iPhoneAccelerometerApp.cpp

LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv1_CM -lz
LOCAL_STATIC_LIBRARIES := cinder android_native_app_glue 

include $(CINDER_PATH)/android/jni/cinder/Configure.mk
ifdef USE_FREEIMAGE
LOCAL_STATIC_LIBRARIES += freeimage
endif
ifdef USE_FREETYPE
LOCAL_STATIC_LIBRARIES += ft2
endif

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)

