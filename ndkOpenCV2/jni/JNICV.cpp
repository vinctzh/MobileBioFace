#include <jni.h>
//#include <iostream>
#include <opencv2/opencv.hpp>
#include <android/log.h>

using namespace cv;
#define LOG_TAG "Test"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
//using namespace std;

extern "C"{
//com.beyare.ndkopencv2
JNIEXPORT void JNICALL Java_com_beyare_ndkopencv2_libJNICV_JNICV(
		JNIEnv* env, jobject obj, jintArray buf, jintArray outBuf, int w, int h);

JNIEXPORT void JNICALL Java_com_beyare_ndkopencv2_libJNICV_JNICV(
		JNIEnv* env, jobject obj, jintArray buf, jintArray outBuf, int w, int h)
{

	LOGI("1");
	jboolean isCopy = false;
	jint* src = env->GetIntArrayElements(buf, 0);
	jint* dst = env->GetIntArrayElements(outBuf,0);


	Mat pMatInRGBA = Mat(Size(w,h),CV_8UC4,(unsigned char*)src);
	Mat pOut       = Mat(Size(w,h),CV_8UC4,(unsigned char*)dst);
	LOGI("2");
	Mat imgGray;
	cvtColor(pMatInRGBA,imgGray,CV_RGBA2GRAY);

	Canny(imgGray,imgGray,30,90);
	cvtColor(imgGray,pOut,CV_GRAY2RGBA);
	LOGI("3");
	env->ReleaseIntArrayElements(buf, src, 0);
	env->ReleaseIntArrayElements(outBuf, dst, 0);
}
}
