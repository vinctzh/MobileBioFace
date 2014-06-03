#include <jni.h>
#include <opencv2/opencv.hpp>
#include <android/log.h>
#include <vector>
#include "FaceDetector.hpp"
#include <iostream>

using namespace cv;
using namespace std;

#define LOG_TAG "Test"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

// com.beyare.mobilebiofaces
extern "C"{
JNIEXPORT jint JNICALL Java_com_beyare_mobilebiofaces_libDetection_DETECTION(
		JNIEnv* env, jobject obj, jintArray imgRGB, jintArray rstRGB,jint* faces,int w, int h);

JNIEXPORT jint JNICALL Java_com_beyare_mobilebiofaces_libDetection_DETECTION(
		JNIEnv* env, jobject obj, jintArray imgRGB, jintArray rstRGB,jint* faces, int w, int h)
{
//	jint* src = env->GetIntArrayElements(buf, 0);
//	jint* dst = env->GetIntArrayElements(outBuf,0);

	LOGI("OK BEGIN GET SRC AND DST");
	jint* src = env->GetIntArrayElements(imgRGB,0);
	jint* dst = env->GetIntArrayElements(rstRGB,0);

	LOGI("BEGIN generate pMatInRGB");
	Mat pMatInRGBA = Mat(Size(w,h),CV_8UC4,(unsigned char*)src);
	Mat pOut	  = Mat(Size(w,h),CV_8UC4,(unsigned char*)dst);

	Mat tmp;
	cvtColor(pMatInRGBA,tmp,CV_RGBA2BGR);
	cout<<"Test"<<endl;
	LOGI("OK BEGIN generate pMatInRGB");

	FaceDetector faceDetector(h,w);

	if (faceDetector.LoadTrainingParams("npd_trained.xml") == -1)
	{
		LOGE("fail to load params");
	}

	IplImage t = IplImage(tmp);
	IplImage* img = &t;
	LOGI("BEGIN detecting faces");
	faceDetector.Detect(img);
	cvtColor(Mat(img),pOut,CV_BGR2RGBA);
	LOGI("OK after detecting faces");
	for (int i=0;i<faceDetector.detected_faces;i++)
	{
//		LOGI("1");
//		*faces++ = faceDetector.faces[i*4+0];
//		LOGE("1");
//		faces[i*4+1] = faceDetector.faces[i*4+1];
//		faces[i*4+2] = faceDetector.faces[i*4+2];
//		faces[i*4+3] = faceDetector.faces[i*4+3];
//		LOGE("11");
	}
	LOGI("Now exist jni");
	env->ReleaseIntArrayElements(imgRGB, src, 0);
	env->ReleaseIntArrayElements(rstRGB, dst, 0);
	return faceDetector.detected_faces;
}
}
