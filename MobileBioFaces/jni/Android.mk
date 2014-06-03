LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

OpenCV_INSTALL_MODULES := on
OPENCV_CAMERA_MODULES := off

OPENCV_LIB_TYPE := STATIC

ifeq ("$(wildcard $(OPENCV_MK_PATH))","")
#try to load OpenCV.mk from default install location
include D:\OpenCV-2.4.9-android-sdk\sdk\native\jni\OpenCV.mk
else 
include $(OPENCV_MK_PATH)
endif

LOCAL_MODULE	:= FACEDETECTION
LOCAL_SRC_FILES := jniDetection.cpp \
				FaceDetector.cpp

LOCAL_LDLIBS += -fopenmp -lm -llog -landroid 

include $(BUILD_SHARED_LIBRARY)