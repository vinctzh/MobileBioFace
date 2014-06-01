#include <iostream>
#include <opencv2/opencv.hpp>
#include <time.h>

using namespace std;
using namespace cv;

#include "FaceDetector.hpp"

int main(){
	clock_t start,finish;
	FaceDetector faceDetector;
	faceDetector.LoadTrainingParams("npd_model_1.mat");

	string imgPath = "1.jpg";
	Mat img = imread(imgPath);
	imshow("org",img);
	IplImage* img_1 ;
	img_1 = &IplImage(img);

	start = clock();
	faceDetector.Detect(img_1);
	finish = clock();

	double totaltime=(double)(finish-start)/CLOCKS_PER_SEC;
	cout<<"Time elapsed:"<<totaltime<<endl;
	Mat rst = Mat(img_1);

	imshow("rst",rst);

	waitKey(0);
	return 0;
}