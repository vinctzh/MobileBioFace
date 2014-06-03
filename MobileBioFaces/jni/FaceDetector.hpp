#pragma once
#ifndef __FaceDetector__
#define __FaceDetector__

#define FACE_AREA_MAX 70*70
#define	FACE_AREA_MIN 20*20
#define VIDEO_WIDTH  640	
#define VIDEO_HEIGHT 480

#include <iostream>
#include <vector>
#include <string>
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include <omp.h>

using namespace cv;

typedef struct
{
	int objSize;
	int numStages;
	int numLeafNodes;
	const int *pNumStageTrees;
	const double *pStageThreshold ;
	const short int *pTreeRoot;
	
	const short int *pPoints1;
	const short int *pPoints2;
	const float *pCutpoint;
	const short int *pLeftChild;
	const short int *pRightChild;
	const double *pFit;
} TrainedParameters;

template<typename T>
void releaseVector(vector<T> &v)
{
	v.clear();
	vector<T>(v).swap(v);
}



class FaceDetector
{
public:
	FaceDetector(int h,int w);
	void Detect(IplImage* pImg);
	int LoadTrainingParams(char* filename);
	~FaceDetector(void);
private:
	Mat _FaceDetection(IplImage* pImg, int &after_mer);
public:
	vector<Point2f> LT_pos; // at most 20 faces; left top corners
	vector<Point2f> RB_pos;		//right bottom corners;
	vector<int> faces;
	int detected_faces;
	Mat draw_point;
private:
	long maxDetectNum;
	TrainedParameters TrainedParams;
	unsigned char**  buffer;

	Mat numStagesTrees,stageThreshold,treeRoot,fit,pix1,pix2,cutPts,lftChild,rgtChild;
};

#endif

