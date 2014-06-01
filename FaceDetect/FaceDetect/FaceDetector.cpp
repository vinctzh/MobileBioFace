#include "FaceDetector.hpp"
#include <iostream>
using namespace std;

double calcArea2(float corner1_x, float corner2_x, float corner1_y, float corner2_y)
{
	double width = abs(corner1_x - corner2_x);
	double heigth = abs(corner1_y - corner2_y);

	return width*heigth;
}

FaceDetector::FaceDetector(long maxDetectNum) {
	this->maxDetectNum = maxDetectNum;
	this->detected_faces = 0;
	this->buffer = (unsigned char**)malloc(sizeof(unsigned char*) * 100);

	omp_set_num_threads(24);

	for(int i = 0; i < 100; i++) 
	{
		this->buffer[i] = (unsigned char*)malloc(sizeof(unsigned char) * VIDEO_HEIGHT * VIDEO_WIDTH);
	}
}


FaceDetector::~FaceDetector(void) {
	for(int i = 0; i < 100; i++) 
	{
		free(this->buffer[i]);
	}
	free(this->buffer);
}

void FaceDetector::Detect(IplImage* pImg) {
	int after_mer;
	Mat draw_point = _FaceDetection(pImg, after_mer);
	
	#pragma omp parallel for  
	for (int i = 0;i<after_mer;i++)
	{			   
		double area = calcArea2(draw_point.at<float>(i,1), draw_point.at<float>(i,3), draw_point.at<float>(i,0), draw_point.at<float>(i,2));
		if (draw_point.at<float>(i,5)>15 /*&& area < FACE_AREA_MAX && area > FACE_AREA_MIN*/)
		{
			cvRectangle(pImg,cvPoint(draw_point.at<float>(i,1),draw_point.at<float>(i,0)),
			cvPoint(draw_point.at<float>(i,3),draw_point.at<float>(i,2)),cvScalar(0,0,255),2);	
			//LT_pos[detected_faces] = Point2f(draw_point.at<float>(i,1),draw_point.at<float>(i,0));
			//RB_pos[detected_faces] = Point2f(draw_point.at<float>(i,3),draw_point.at<float>(i,2));
			LT_pos.push_back(Point2f(draw_point.at<float>(i,1),draw_point.at<float>(i,0)));
			RB_pos.push_back(Point2f(draw_point.at<float>(i,3),draw_point.at<float>(i,2)));
			detected_faces++;
		}
	}

}

Mat FaceDetector::_FaceDetection(IplImage* pImg, int &after_mer) {

	IplImage* gray;
	vector<int> height(50);
	vector<int> width(50);
	vector<const unsigned char*> pPyramidImages(50);     //装载金字塔图像
	Mat detectedpoint = Mat(800,6,CV_32F);
	
	/***********************************************************金字塔图像************************************************/

	gray = cvCreateImage( cvSize(pImg->width,pImg->height), 8, 1 );//变为灰度图片
	cvCvtColor(pImg,gray,CV_RGB2GRAY);

	/***********************************************detection*******************************************************/

	int numLevels = 1;


	float scale=1.2;
	int hit=0;//检测到目标的个数

	CvSize p_size =cvGetSize(pImg);


	int window_row =p_size.height*scale;
	int window_col =p_size.width*scale;
	int k=0;
	int count2 = 0;

	for(;;)
	{    
		window_row = window_row/scale;
		window_col = window_col/scale;
		if (window_row<TrainedParams.objSize||window_col<TrainedParams.objSize)
		{
			break;
		}

	/*	for (int i=0;i<15;i++)
		{
			cout<<TrainedParams.pNumStageTrees[i]<<endl;
		}*/

		CvSize dst_cvsize;
		IplImage *dst=0;

		dst_cvsize.width=(int)( window_col); //将图片缩小为原来的scale倍
		dst_cvsize.height=(int)(window_row);
		dst=cvCreateImage(dst_cvsize,8,1);
		cvResize(gray,dst,CV_INTER_NN);
		Mat small_img(dst,0); 

		width[k]=window_col;
		height[k]= window_row;

		int count = 0; //将每一层金字塔都装在 pPyramidImages[k]中，便于下一步检测
		int step = dst->height;
		
		for (int i = 0; i < width[k]; i++)
		{
			for (int j = 0; j < height[k]; j++)
			{
				buffer[count2][count ++ ] = (unsigned char) dst->imageData[j * dst->widthStep + i];
			}
		}
		pPyramidImages[k]=buffer[count2++];
		k++;
		numLevels++;
		cvReleaseImage(&dst);
	}

	/*****************************************************检测过程**********************************************************/


	// pre-determined offset of pixels in a subwindow
	vector<int> offset(TrainedParams.objSize * TrainedParams.objSize);
	for(int k = 0; k < numLevels-1; k++) // process each pyramid image
	{
		int h =height[k];
		int w =width[k];
		double factor = pow(scale,k);

		// determine the step of the sliding subwindow
		int winStep = 2;
		int m = min(h, w);
		if(factor > 2 && m < 240) winStep = 1;

		// calculate the offset values of each pixel in a subwindow
		for(int p1=0, p2=0, gap=h-TrainedParams.objSize, j=0; j < TrainedParams.objSize; j++) // column coordinate
		{
			for(int i = 0; i < TrainedParams.objSize; i++) // row coordinate
			{
				offset[p1++] = p2++;
			}

			p2 += gap;
		}

		int colMax = w - TrainedParams.objSize + 1;
		int rowMax = h - TrainedParams.objSize + 1;
		#pragma omp parallel for  // process each subwindow

		for(int c = 0; c < colMax; c += winStep) // slide in column
		{
			const unsigned char *pPixel = pPyramidImages[k] + c * h; 
			for(int r = 0; r < rowMax; r += winStep, pPixel += winStep) // slide in row
			{    

				int treeIndex = 0;
				double _score = 0;
				int s;
				
				for(s = 0; s < TrainedParams.numStages; s++) 
				{
					double fx = 0;

					// test each tree classifier
					for(int j = 0; j < TrainedParams.pNumStageTrees[s]; j++)
					{
						short int node = TrainedParams.pTreeRoot[treeIndex];

						// test the current tree classifier
						while(node > -1) // branch node
						{
							float p1 = (float) pPixel[offset[TrainedParams.pPoints1[node]]];//points 为该有用特征在1*400的向量中的位置。offset为在大图片中将一个20*20的窗口变为1*400时的补偿量
							float p2 = (float) pPixel[offset[TrainedParams.pPoints2[node]]];//pixel[]指向偏移的位置

							float fea;

							if(p1 == 0 && p2 == 0) fea = 0;
							else fea = (p1 - p2) / (p1 + p2);

							if(fea < TrainedParams.pCutpoint[node]) node = TrainedParams.pLeftChild[node];
							else node = TrainedParams.pRightChild[node];
						}

						// leaf node
						node = - node - 1;
						fx = fx + TrainedParams.pFit[node];
						treeIndex++;
					}

					fx = 1.0 / (1.0 + exp(-2.0*fx));
					if(fx < TrainedParams.pStageThreshold[s])  
						break; // negative samples
					_score = _score + fx;
				}
				
				if(s == TrainedParams.numStages) // a face detected
				{
					double _row = floor(r * factor) + 1;
					double _col = floor(c * factor) + 1;
					double _size = floor(TrainedParams.objSize * factor);                    
					

					#pragma omp critical // modify the record by a single thread
					{
						//将检测到目标的坐标保存在矩阵当中
						detectedpoint.at<float>(hit,0)=_row;
						detectedpoint.at<float>(hit,1)=_col;
						detectedpoint.at<float>(hit,2)=_row+_size;
						detectedpoint.at<float>(hit,3)=_col+_size;
						detectedpoint.at<float>(hit,4)=_size;
						detectedpoint.at<float>(hit,5)=_score;

						hit++;
					}
					
				}
			}
		}
	} 

	/************************************************合并检测窗口*********************************************************/


	Mat predicate;
	predicate=Mat::zeros(hit,1,CV_32F);
	float lable=1;

	for (int i=0;i<hit;i++)
	{

		for (int j=i+1;j<hit;j++)
		{
			float delta = 0.3 * min(detectedpoint.at<float>(i,4), detectedpoint.at<float>(j,4));//设定合并的范围
			if (abs(detectedpoint.at<float>(i,0)-detectedpoint.at<float>(j,0))<delta&&abs(detectedpoint.at<float>(i,1)-detectedpoint.at<float>(j,1))<delta&&detectedpoint.at<float>(i,0)+detectedpoint.at<float>(i,4)-detectedpoint.at<float>(j,0)-detectedpoint.at<float>(j,4)<delta&&detectedpoint.at<float>(i,1)+detectedpoint.at<float>(i,4)-detectedpoint.at<float>(j,1)-detectedpoint.at<float>(j,4)<delta)
			{
				if (predicate.at<float>(i,0)==0)
				{
					predicate.at<float>(i,0)=lable;
					lable++;
				}
				if (predicate.at<float>(j,0)==0)
				{
					predicate.at<float>(j,0)=predicate.at<float>(i,0);
				}
				if (predicate.at<float>(j,0)!=0&&predicate.at<float>(i,0)==0)
				{
					predicate.at<float>(i,0)=predicate.at<float>(j,0);
				}

			}
		}
	}

	/*******************************************合并相邻窗口***********************************************************/


	Mat rectspoint = Mat(hit,6,CV_32F);
	//rectspoint=cvCreateMat(hit,6,CV_32F);
	int numfaces=0;
	if (lable>1)
	{
		int after=lable-1;

		for (int numCandidates=1; numCandidates<lable; numCandidates++)
		{
			int num=0;
			int rectspoint1=0,rectspoint2=0,rectspoint3=0,rectspoint4=0,rec_score=0;

			#pragma omp parallel for
			for (int i=0;i<hit;i++)
			{
			#pragma omp critical
				if (predicate.at<float>(i,0)==numCandidates)
				{
					rectspoint1=rectspoint1+detectedpoint.at<float>(i,0);
					rectspoint2=rectspoint2+detectedpoint.at<float>(i,1);
					rectspoint3=rectspoint3+detectedpoint.at<float>(i,2);
					rectspoint4=rectspoint4+detectedpoint.at<float>(i,3);
					rec_score=rec_score+ detectedpoint.at<float>(i,5);
					num++;
				}
			#pragma omp critical
				if (predicate.at<float>(i,0)==0&&numCandidates==1)
				{
					rectspoint.at<float>(after,0)=detectedpoint.at<float>(i,0);
					rectspoint.at<float>(after,1)=detectedpoint.at<float>(i,1);
					rectspoint.at<float>(after,2)=detectedpoint.at<float>(i,2);
					rectspoint.at<float>(after,3)=detectedpoint.at<float>(i,3);
					rectspoint.at<float>(after,4)=detectedpoint.at<float>(i,4);
					rectspoint.at<float>(after,5)=detectedpoint.at<float>(i,5);
					after++;
				}

			}
			#pragma omp critical
			{
				rectspoint.at<float>(numCandidates-1,0)=rectspoint1/num;
				rectspoint.at<float>(numCandidates-1,1)=rectspoint2/num;
				rectspoint.at<float>(numCandidates-1,2)=rectspoint3/num;
				rectspoint.at<float>(numCandidates-1,3)=rectspoint4/num;
				rectspoint.at<float>(numCandidates-1,4)=abs(rectspoint.at<float>(numCandidates-1,0)-rectspoint.at<float>(numCandidates-1,2));//检测框的大小
				rectspoint.at<float>(numCandidates-1,5)=rec_score;
			}
			numfaces=numfaces+(num-1);
		}
	}


	if (lable==1)
	{
		int after=lable-1;

		#pragma omp parallel for
		for (int i=0;i<hit;i++)
		{
			#pragma omp critical
			if (predicate.at<float>(i,0)==0)
			{
				rectspoint.at<float>(after,0)=detectedpoint.at<float>(i,0);
				rectspoint.at<float>(after,1)=detectedpoint.at<float>(i,1);
				rectspoint.at<float>(after,2)=detectedpoint.at<float>(i,2);
				rectspoint.at<float>(after,3)=detectedpoint.at<float>(i,3);
				rectspoint.at<float>(after,4)=detectedpoint.at<float>(i,4);
				rectspoint.at<float>(after,5)=detectedpoint.at<float>(i,5);
				after++;
			}

		}
	}
	/****************************************************合并包含窗口**********************************************************/
	////找到可以合并的窗口
	numfaces=hit-numfaces;
	predicate=Mat::zeros(numfaces,1,CV_32F);
	lable=1;

	for (int i=0;i<numfaces;i++)
	{
		for (int j=0;j<numfaces;j++)
		{
			float delta = floor(rectspoint.at<float>(j,4)*0.2);
			if (i == j)
			{
				continue;
			}

			if (rectspoint.at<float>(i,0) >= rectspoint.at<float>(j,0)-delta
				&& rectspoint.at<float>(i,1) >= rectspoint.at<float>(j,1)-delta
				&& rectspoint.at<float>(i,0)+rectspoint.at<float>(i,4) <= rectspoint.at<float>(j,0)+rectspoint.at<float>(j,4)+delta
				&& rectspoint.at<float>(i,1)+rectspoint.at<float>(i,4) <= rectspoint.at<float>(j,1)+rectspoint.at<float>(j,4)+delta)
			{
				if (predicate.at<float>(i,0)==0)
				{
					predicate.at<float>(i,0)=lable;
					lable++;
				}
				if (predicate.at<float>(j,0)==0)
				{
					predicate.at<float>(j,0)=predicate.at<float>(i,0);
				}
				if (predicate.at<float>(j,0)!=0)
				{
					predicate.at<float>(i,0)=predicate.at<float>(j,0);
				}
			}

		}
	}
	//////合并包含窗口///
	Mat draw_point = Mat(numfaces,6,CV_32F);
	//draw_point=cvCreateMat(numfaces,6,CV_32F);
	after_mer=0;
	if (lable==1)
	{
		lable=2;//若没有可以合并的窗口，则下面的循环只循环一次，直接将前面点的值赋予后面的点
	}
	for (int numCandidates=1; numCandidates<lable; numCandidates++)
	{
		int num=0;
		int rectspoint1=0,rectspoint2=0,rectspoint3=0,rectspoint4=0,rec_size=0,rec_score;

		#pragma omp parallel for
		for (int i=0;i<numfaces;i++)
		{
			if (predicate.at<float>(i,0)==numCandidates)
			{
				#pragma omp critical
				if (rectspoint.at<float>(i,4)>rec_size)
				{
					rectspoint1=rectspoint.at<float>(i,0);
					rectspoint2=rectspoint.at<float>(i,1);
					rectspoint3=rectspoint.at<float>(i,2);
					rectspoint4=rectspoint.at<float>(i,3);
					rec_size=rectspoint.at<float>(i,4);
					rec_score=rectspoint.at<float>(i,5);

				}
				num++;

			}
			#pragma omp critical
			if (predicate.at<float>(i,0)==0&&numCandidates==1)
			{
				draw_point.at<float>(after_mer,0)=rectspoint.at<float>(i,0);
				draw_point.at<float>(after_mer,1)=rectspoint.at<float>(i,1);
				draw_point.at<float>(after_mer,2)=rectspoint.at<float>(i,2);
				draw_point.at<float>(after_mer,3)=rectspoint.at<float>(i,3);
				draw_point.at<float>(after_mer,4)=rectspoint.at<float>(i,4);
				draw_point.at<float>(after_mer,5)=rectspoint.at<float>(i,5);
				predicate.at<float>(i,0)=-1;//将赋过值得点标记为-1
				after_mer++;

			}

		}

		if (num>0)
		{
		#pragma omp critical
			{
				draw_point.at<float>(after_mer,0)=rectspoint1;//+0.2*abs(rectspoint.at<float>(after_mer,0)-rectspoint.at<float>(after_mer,2));
				draw_point.at<float>(after_mer,1)=rectspoint2;//+0.2*abs(rectspoint.at<float>(after_mer,0)-rectspoint.at<float>(after_mer,2));
				draw_point.at<float>(after_mer,2)=rectspoint3;//-0.2*abs(rectspoint.at<float>(after_mer,0)-rectspoint.at<float>(after_mer,2));
				draw_point.at<float>(after_mer,3)=rectspoint4;//-0.2*abs(rectspoint.at<float>(after_mer,0)-rectspoint.at<float>(after_mer,2));
				draw_point.at<float>(after_mer,4)=abs(rectspoint.at<float>(after_mer,0)-rectspoint.at<float>(after_mer,2));//检测框的大小
				draw_point.at<float>(after_mer,5)=rec_score+rectspoint.at<float>(after_mer,5);
			}
			after_mer++;
		}

	}
	cvReleaseImage(&gray);
	releaseVector(height);
	releaseVector(width);
	releaseVector(pPyramidImages);
	return draw_point;
}

template<typename T>
void initMat(Mat& m, const T* num)
{
	for(int i=0;i<m.rows;i++)
		for(int j=0;j<m.cols;j++)
		{
			m.at<T>(i,j)=num[i*m.cols+j];
			//cout<<*(num+i*m.cols+j)<<endl;
		}


	cout<<endl;
}
void FaceDetector::LoadTrainingParams(const char* file) {
	//file="D:\\YZY\\HIK_NPD\\npd_model_1.mat";//读取检测文件
	//MATFile *pmat;
	//MATFile *pm;    
	//mxArray *pa,*img;
	//pmat=matOpen(file, "r");//打开文件，返回指向文件指针 
	//pa = matGetVariable(pmat, "npdModel"); 
	//TrainedParams.objSize = *(int*) mxGetData(mxGetField(pa, 0, "objSize"));
	//TrainedParams.numStages = *(int*) mxGetData(mxGetField(pa, 0, "numStages"));
	//TrainedParams.numLeafNodes = *(int*) mxGetData(mxGetField(pa, 0, "numLeafNodes"));
	//TrainedParams.pNumStageTrees = (int*) mxGetData(mxGetField(pa, 0, "numStageTrees"));
	//TrainedParams.pStageThreshold = mxGetPr(mxGetField(pa, 0, "stageThreshold"));
	//TrainedParams.pTreeRoot = (short int *) mxGetData(mxGetField(pa, 0, "treeRoot"));
	//TrainedParams.pBranchNode = mxGetField(pa, 0, "branchNode");

	//TrainedParams.pPoints1 = (short int *) mxGetData(mxGetField(TrainedParams.pBranchNode, 0, "pixel1"));
	//TrainedParams.pPoints2 = (short int *) mxGetData(mxGetField(TrainedParams.pBranchNode, 0, "pixel2"));
	//TrainedParams.pCutpoint = (float *) mxGetPr(mxGetField(TrainedParams.pBranchNode, 0, "cutpoint"));
	//TrainedParams.pLeftChild = (short int *) mxGetData(mxGetField(TrainedParams.pBranchNode, 0, "leftChild"));
	//TrainedParams.pRightChild = (short int *) mxGetData(mxGetField(TrainedParams.pBranchNode, 0, "rightChild"));
	//TrainedParams.pFit = mxGetPr(mxGetField(pa, 0, "fit"));
	
	string filename = "npd_trained.xml";
	FileStorage fsRead(filename,FileStorage::READ);
	if (!fsRead.isOpened())	
	{		
		return;
	}

	TrainedParams.objSize = (int)fsRead["Object_Size"];
	FileNode fn;
	// Stages
	fn = fsRead["Stages"];	
	//Mat numStagesTrees,stageThreshold;
	fn["Num_Of_StageTrees"]>> numStagesTrees;
	fn["Stage_Threshold"] >> stageThreshold;	

	TrainedParams.numStages = (int)fn["Num_Of_Stage"];
	TrainedParams.pNumStageTrees = (int*) numStagesTrees.data;
	TrainedParams.pStageThreshold = (double*) stageThreshold.data;

	// Tree
	fn = fsRead["Tree"];
	//Mat treeRoot,fit;
	fn["Fit"]>>fit;
	fn["Tree_Root"]>>treeRoot;

	TrainedParams.numLeafNodes = fn["Num_Of_LeafNodes"];
	TrainedParams.pTreeRoot = (short*)treeRoot.data;
	TrainedParams.pFit = (double*)fit.data;

	// Branches
	fn = fsRead["Branches"];
	//Mat pix1,pix2,cutPts,lftChild,rgtChild;
	fn["Pixels1"]>>pix1;
	fn["Pixels2"]>>pix2;	
	fn["Cut_Points"]>>cutPts;

	fn["LeftChild"]>>lftChild;
	fn["RightChild"]>>rgtChild;
	TrainedParams.pPoints1 = (short*)pix1.data;
	TrainedParams.pPoints2 = (short*)pix2.data;
	TrainedParams.pCutpoint = (float*)cutPts.data;
	TrainedParams.pLeftChild = (short*)lftChild.data;
	TrainedParams.pRightChild = (short*)rgtChild.data;
	
	fsRead.release();
}
