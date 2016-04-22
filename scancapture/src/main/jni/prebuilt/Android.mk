LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := opencv_info
LOCAL_SRC_FILES := libopencv_info.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := opencv_java
LOCAL_SRC_FILES := libopencv_java.so
include $(PREBUILT_SHARED_LIBRARY)

