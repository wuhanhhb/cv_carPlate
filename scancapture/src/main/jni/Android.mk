LOCAL_PATH := $(call my-dir)/src
include $(CLEAR_VARS)
OPENCV_LIB_TYPE:=STATIC
#OPENCV_CAMERA_MODULES:=on
#OPENCV_INSTALL_MODULES:=on
include F:/work/face/OpenCV-2.4.10-android-sdk/sdk/native/jni/OpenCV.mk  
#LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog $(NDK_HOME)/sources/cxx-stl/stlport/libs/armeabi/libstlport_static.a 
#LOCAL_C_INCLUDES += $(NDK_HOME)/sources/cxx-stl/stlport/stlport
FILE_LIST := $(wildcard $(LOCAL_PATH)/core/*.cpp)
#FILE_LIST += $(wildcard $(LOCAL_PATH)/train/ann_train.cpp)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)
#LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/, , $(FILE_LIST))
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_MODULE     := imageproc  
LOCAL_LDLIBS += -llog
#LOCAL_FORCE_STATIC_EXECUTABLE := true
#include $(BUILD_EXECUTABLE)
include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_STATIC_LIBRARY)

#include $(LOCAL_PATH)/../prebuilt/Android.mk
