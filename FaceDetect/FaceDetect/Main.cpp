#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

#include "FaceDetector.hpp"

int main(){
	FaceDetector faceDetector;
	faceDetector.LoadTrainingParams("npd_model_1.mat");

	string imgPath = "1.jpg";
	Mat img = imread(imgPath);
	imshow("org",img);
	IplImage* img_1 ;
	img_1 = &IplImage(img);

	faceDetector.Detect(img_1);
	Mat rst = Mat(img_1);

	imshow("rst",rst);

	waitKey(0);
	return 0;
}