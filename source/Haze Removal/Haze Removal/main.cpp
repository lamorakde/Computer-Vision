#include "opencv2/opencv.hpp"
#include "czh_binary_CV.h"
#include <iostream>

using namespace std;
using namespace cv;

void czh_dark_channel_prior(Mat & srcImage, Mat & dstImage, int size);	// 暗通道图像
void czh_brightestPoints_inDarkChannel(Mat & darkChannelImage, vector<Point> & pointVector);	// 寻找暗通道图像中亮度最大点

MatIterator_<uchar> it_uBegin, it_uEnd, it_uDst;
MatIterator_<float> it_fBegin, it_fEnd, it_fDst;

  int main()
  {
	  // 程序帮助信息
	  string programInformation = "该程序会移除图像中的雾气成分.";
	  string fileType = ".ppm";
	  czh_helpInformation(programInformation);

	  // 得到输入图像名称信息并输出
	  cout << "Enter the input image name without " << fileType << ": ";
	  string fileName, srcFileName;
	  fileName = "haze6";
	  // getline(cin, fileName);				// 获得输入文件名
	  srcFileName = fileName + fileType;	// 确定输入图片文件类型

	  Mat src = imread(srcFileName);	// 读取源图像
	  czh_imageOpenDetect(src, fileName, fileType);	// 检测源图像是否成功打开

	  // 显示源图像信息
	  cout << "\nInput image name:\t" << srcFileName << endl
		  << "Image size:\t\t" << src.cols << "*" << src.rows << endl
		  << "Image pixels:\t\t" << src.cols*src.rows << endl
		  << "Image type:\t\t" << src.type() << endl;
	  // 显示参数信息
	  // 如果出现色斑，需要把 omega 降低
	  float omega = 0.75;
	  float t0 = 0.1;
	  int radius = 5;
	  int guidanceRadius = 2 * radius;	// 引导滤波的半径
	  float guidanceEpsilon = 0.4;	// 引导滤波的参数 epsilon
	  cout << "\n算法参数:" << endl << "\tomega : " << omega << endl << "\tt0 : " << t0 << endl;
	  cout << "\t最小滤波半径 : " << radius << endl;
	  cout << "\tGuide Filter 半径 : " << guidanceRadius << endl << "\tGuide Filter Epsilon : " << guidanceEpsilon << endl;

	//...............................................//
	//												 //
	//				       STEP.1                    //
	//					 暗通道图像	        	   	 //
	//												 //
	//...............................................//
	
	Mat srcImage = src;
	Mat graySrcImage, image_dark_channel;
	cvtColor(srcImage, graySrcImage, CV_BGR2GRAY);

   	image_dark_channel.create(srcImage.size(), CV_8UC1);
   	czh_dark_channel_prior(srcImage, image_dark_channel, radius);

	//...............................................//
	//												 //
	//				       STEP.2                    //
	//				    全球大气成分	      	   	 //
	//												 //
	//...............................................//
	
	// A 为全球大气光成分
 	// 在得到的暗通道图像中，寻找亮度大小前 0.1% 的像素
 	// 在源图中找到这些像素中有最高亮度的值
 	float A[3]{0, 0, 0};
	vector<Point> brightPoints;
 	czh_brightestPoints_inDarkChannel(image_dark_channel, brightPoints);	// 暗通道中亮度最大的点，记录坐标

	float maxValue = (float)srcImage.at<Vec3b>(brightPoints[0].y, brightPoints[0].x)[0] + (float)srcImage.at<Vec3b>(brightPoints[0].y, brightPoints[0].x)[1]
		+ (float)srcImage.at<Vec3b>(brightPoints[0].y, brightPoints[0].x)[2];

 	for (int i = 0; i < brightPoints.size(); i++)	// 在源图中找到这些像素中有最高亮度的值
 	{
		if (((float)srcImage.at<Vec3b>(brightPoints[i].y, brightPoints[i].x)[0] + (float)srcImage.at<Vec3b>(brightPoints[i].y, brightPoints[i].x)[1] +
			(float)srcImage.at<Vec3b>(brightPoints[i].y, brightPoints[i].x)[2]) >= maxValue)
		{
			A[0] = (float)srcImage.at<Vec3b>(brightPoints[i].y, brightPoints[i].x)[0];
			A[1] = (float)srcImage.at<Vec3b>(brightPoints[i].y, brightPoints[i].x)[1];
			A[2] = (float)srcImage.at<Vec3b>(brightPoints[i].y, brightPoints[i].x)[2];
		}
 	}

	//...............................................//
	//												 //
	//				     STEP.3.1                    //
	//				    透射率图 tx	      	    	 //
	//												 //
	//...............................................//


 	float minValue;
 	Mat I_div_A(srcImage.size(), CV_32F);

	MatIterator_<Vec3b>itBegin, itEnd;
	MatIterator_<float>ptrDst;
 	for (itBegin = srcImage.begin<Vec3b>(), itEnd = srcImage.end<Vec3b>(), ptrDst = I_div_A.begin<float>(); itBegin != itEnd; itBegin++, ptrDst++)
 	{	// 遍历源图每个像素，取当前像素三通道中最小的I/A
 		minValue = ((*itBegin)[0]) / A[0];
 		if (minValue > ((*itBegin)[1]) / A[1] )
 			minValue = ((*itBegin)[1]) / A[1];
 		if (minValue > ((*itBegin)[2]) / A[2])
 			minValue = ((*itBegin)[2]) / A[2];
 		*ptrDst = minValue;
 	}
 	czh_minFilter(I_div_A, I_div_A, radius);	// 可以优化

 	Mat tx(srcImage.size(), CV_32FC1);	// tx 为透射图
	tx = 1 - omega*I_div_A;
 	
	//...............................................//
	//												 //
	//			         STEP.3.2                    //
	//		      透射图另外算法：引导滤波         	 //
	//												 //
	//...............................................//

	// 求得引导滤波图像
	Mat txForFilter, txFiltered;


	tx.convertTo(txForFilter, CV_8UC1, 255);	// 将待滤波图像转化为 uchar
	graySrcImage.convertTo(graySrcImage, CV_8UC1);	// 将引导图像转化为 uchar
	txFiltered.create(srcImage.size(), CV_32FC1);	// 准备目标结果图像对象

	// 进行引导滤波操作
	czh_guidedFilter(txForFilter, graySrcImage, txFiltered, guidanceRadius, guidanceEpsilon, CV_32FC1);

	//...............................................//
	//												 //
	//				       STEP.4                    //
	//				    最终恢复运算	   	    	 //
	//												 //
	//...............................................//
 	
	// 当 tx 中某个像素比 t0 小的时候，用 t0 替代该像素灰度值
	for (it_fBegin = tx.begin<float>(), it_fEnd = tx.end<float>(); it_fBegin != it_fEnd; it_fBegin++)
	{
		if ((*it_fBegin) < t0)
			(*it_fBegin) = t0;
	}

	// 当 txFiltered 中某个像素比 t0 小的时候，用 t0 替代该像素灰度值
	for (it_fBegin = txFiltered.begin<float>(), it_fEnd = txFiltered.end<float>(); it_fBegin != it_fEnd; it_fBegin++)
	{
		if ((*it_fBegin) < t0)
			(*it_fBegin) = t0;
	}

	Mat dstImage(srcImage.size(), srcImage.type());
 	for(int i=0;i<dstImage.rows;i++)
 		for (int j = 0; j < dstImage.cols; j++)
 		{
 			dstImage.at<Vec3b>(i, j)[0] = ( (srcImage.at<Vec3b>(i, j)[0] - A[0]) / tx.at<float>(i, j) ) + A[0];
 			dstImage.at<Vec3b>(i, j)[1] = ( (srcImage.at<Vec3b>(i, j)[1] - A[1]) / tx.at<float>(i, j) ) + A[1];
 			dstImage.at<Vec3b>(i, j)[2] = ( (srcImage.at<Vec3b>(i, j)[2] - A[2]) / tx.at<float>(i, j) ) + A[2];
 		}
 	
	Mat dstImage2(srcImage.size(), srcImage.type());
	for (int i = 0; i < dstImage2.rows; i++)
		for (int j = 0; j < dstImage2.cols; j++)	
		{
			dstImage2.at<Vec3b>(i, j)[0] = ((srcImage.at<Vec3b>(i, j)[0] - A[0]) / txFiltered.at<float>(i, j)) + A[0];
			dstImage2.at<Vec3b>(i, j)[1] = ((srcImage.at<Vec3b>(i, j)[1] - A[1]) / txFiltered.at<float>(i, j)) + A[1];
			dstImage2.at<Vec3b>(i, j)[2] = ((srcImage.at<Vec3b>(i, j)[2] - A[2]) / txFiltered.at<float>(i, j)) + A[2];
		}

 	// 测试区

 	imshow("源图", srcImage);
 	imshow("暗通道图", image_dark_channel);
 	imshow("预估透射率图", tx);
	imshow("引导透射率图", txFiltered);
 	imshow("以预估透射率图得到的去雾效果图", dstImage);
	imshow("以引导透射率图得到的去雾效果图",dstImage2);
 
  	waitKey();
  	system("pause");
  }

void czh_dark_channel_prior(Mat & srcImage, Mat & dstImage, int radius)
{
	// 暗通道计算
	Mat src, temp, dst;
	srcImage.copyTo(src);
	temp.create(srcImage.size(), CV_8UC1);
	dst.create(srcImage.size(), CV_8UC1);
	MatIterator_<Vec3b>itBegin, itEnd;
	MatIterator_<uchar>ptrDst;
	int minValue;

	for (itBegin = src.begin<Vec3b>(), itEnd = src.end<Vec3b>(), ptrDst = temp.begin<uchar>(); itBegin != itEnd; itBegin++, ptrDst++)
	{	// 遍历源图每个像素，取当前像素三通道中最小值
		minValue = (*itBegin)[0];
		if (minValue > (*itBegin)[1])
			minValue = (*itBegin)[1];
		if (minValue > (*itBegin)[2])
			minValue = (*itBegin)[2];
		*ptrDst = minValue;
	}
	czh_minFilter(temp, dst, radius);
	dstImage = dst;
}

void czh_brightestPoints_inDarkChannel(Mat & darkChannelImage, vector<Point> & pointVector)
{
	// 找出暗通道图像中亮度值最大的前 0.1% 个点，并压入Point类型的vector之中
	// 并未优化，速度慢
	vector<Point>().swap(pointVector);	// 清空输入向量
	Mat src;
	darkChannelImage.copyTo(src);
	int pointNumber = 0.1*0.01*src.cols*src.rows;	// 取暗通道亮度前0.1%的点
	int maxValue[3];
	maxValue[0] = (int)src.at<uchar>(0, 0);
	maxValue[1] = 0;
	maxValue[2] = 0;
	Point tempPoint;

	for (int it = 0; it < pointNumber; it++)	// 总共需要 pointNumber 个亮点的像素坐标
	{
		for (int i = 0; i < src.rows; i++)		// 每次寻找最大值时都遍历图像中每个像素
		{
			for (int j = 0; j < src.cols; j++)
			{
				if (maxValue[0] < (int)src.at<uchar>(i, j))	// 选出灰度值最大的点，并记录他们的x,y坐标
				{
					maxValue[0] = (int)src.at<uchar>(i, j);
					maxValue[1] = i;
					maxValue[2] = j;
				}
			}
		}
		// Point 对象的 x 对应列，y 对应行
		// 把当前迭代灰度值最大的像素的坐标赋给tempPoint
		tempPoint.x = maxValue[2];
		tempPoint.y = maxValue[1];
		// 把 tempPoint 压入向量之中
		pointVector.push_back(tempPoint);
		// 最后把当前灰度值最大像素的灰度值置0，这样就不会影响下次迭代
		src.at<uchar>(maxValue[1], maxValue[2]) = 0;
		// 并重置最大值为0
		maxValue[0] = 0;
	}
}

