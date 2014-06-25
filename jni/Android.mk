LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := Arkanoid
LOCAL_SRC_FILES := Arkanoid.cpp \
				   Ball.cpp \
				   Battledore.cpp\
				   GameObject.cpp
				   
				   
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2
LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_SHARED_LIBRARY)
$(call import-module,android/native_app_glue)    