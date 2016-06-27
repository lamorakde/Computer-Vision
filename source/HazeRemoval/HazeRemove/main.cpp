#include "opencv2/opencv.hpp"
#include "czh_binary_CV.h"
#include <iostream>

using namespace std;
using namespace cv;

void czh_dark_channel_prior(Mat & srcImage, Mat & dstImage, int size);	// ��ͨ��ͼ��
void czh_brightestPoints_inDarkChannel(Mat & darkChannelImage, vector<Point> & pointVector);	// Ѱ�Ұ�ͨ��ͼ������������

MatIterator_<uchar> it_uBegin, it_uEnd, it_uDst;
MatIterator_<float> it_fBegin, it_fEnd, it_fDst;

  int main()
  {
	  // ���������Ϣ
	  string programInformation = "�ó�����Ƴ�ͼ���е������ɷ�.";
	  string fileType = ".ppm";
	  czh_helpInformation(programInformation);

	  // �õ�����ͼ��������Ϣ�����
	  cout << "Enter the input image name without " << fileType << ": ";
	  string fileName, srcFileName;
	  fileName = "haze6";
	  // getline(cin, fileName);				// ��������ļ���
	  srcFileName = fileName + fileType;	// ȷ������ͼƬ�ļ�����

	  Mat src = imread(srcFileName);	// ��ȡԴͼ��
	  czh_imageOpenDetect(src, fileName, fileType);	// ���Դͼ���Ƿ�ɹ���

	  // ��ʾԴͼ����Ϣ
	  cout << "\nInput image name:\t" << srcFileName << endl
		  << "Image size:\t\t" << src.cols << "*" << src.rows << endl
		  << "Image pixels:\t\t" << src.cols*src.rows << endl
		  << "Image type:\t\t" << src.type() << endl;
	  // ��ʾ������Ϣ
	  // �������ɫ�ߣ���Ҫ�� omega ����
	  float omega = 0.75;
	  float t0 = 0.1;
	  int radius = 5;
	  int guidanceRadius = 2 * radius;	// �����˲��İ뾶
	  float guidanceEpsilon = 0.4;	// �����˲��Ĳ��� epsilon
	  cout << "\n�㷨����:" << endl << "\tomega : " << omega << endl << "\tt0 : " << t0 << endl;
	  cout << "\t��С�˲��뾶 : " << radius << endl;
	  cout << "\tGuide Filter �뾶 : " << guidanceRadius << endl << "\tGuide Filter Epsilon : " << guidanceEpsilon << endl;

	//...............................................//
	//												 //
	//				       STEP.1                    //
	//					 ��ͨ��ͼ��	        	   	 //
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
	//				    ȫ������ɷ�	      	   	 //
	//												 //
	//...............................................//
	
	// A Ϊȫ�������ɷ�
 	// �ڵõ��İ�ͨ��ͼ���У�Ѱ�����ȴ�Сǰ 0.1% ������
 	// ��Դͼ���ҵ���Щ��������������ȵ�ֵ
 	float A[3]{0, 0, 0};
	vector<Point> brightPoints;
 	czh_brightestPoints_inDarkChannel(image_dark_channel, brightPoints);	// ��ͨ�����������ĵ㣬��¼����

	float maxValue = (float)srcImage.at<Vec3b>(brightPoints[0].y, brightPoints[0].x)[0] + (float)srcImage.at<Vec3b>(brightPoints[0].y, brightPoints[0].x)[1]
		+ (float)srcImage.at<Vec3b>(brightPoints[0].y, brightPoints[0].x)[2];

 	for (int i = 0; i < brightPoints.size(); i++)	// ��Դͼ���ҵ���Щ��������������ȵ�ֵ
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
	//				    ͸����ͼ tx	      	    	 //
	//												 //
	//...............................................//


 	float minValue;
 	Mat I_div_A(srcImage.size(), CV_32F);

	MatIterator_<Vec3b>itBegin, itEnd;
	MatIterator_<float>ptrDst;
 	for (itBegin = srcImage.begin<Vec3b>(), itEnd = srcImage.end<Vec3b>(), ptrDst = I_div_A.begin<float>(); itBegin != itEnd; itBegin++, ptrDst++)
 	{	// ����Դͼÿ�����أ�ȡ��ǰ������ͨ������С��I/A
 		minValue = ((*itBegin)[0]) / A[0];
 		if (minValue > ((*itBegin)[1]) / A[1] )
 			minValue = ((*itBegin)[1]) / A[1];
 		if (minValue > ((*itBegin)[2]) / A[2])
 			minValue = ((*itBegin)[2]) / A[2];
 		*ptrDst = minValue;
 	}
 	czh_minFilter(I_div_A, I_div_A, radius);	// �����Ż�

 	Mat tx(srcImage.size(), CV_32FC1);	// tx Ϊ͸��ͼ
	tx = 1 - omega*I_div_A;
 	
	//...............................................//
	//												 //
	//			         STEP.3.2                    //
	//		      ͸��ͼ�����㷨�������˲�         	 //
	//												 //
	//...............................................//

	// ��������˲�ͼ��
	Mat txForFilter, txFiltered;


	tx.convertTo(txForFilter, CV_8UC1, 255);	// �����˲�ͼ��ת��Ϊ uchar
	graySrcImage.convertTo(graySrcImage, CV_8UC1);	// ������ͼ��ת��Ϊ uchar
	txFiltered.create(srcImage.size(), CV_32FC1);	// ׼��Ŀ����ͼ�����

	// ���������˲�����
	czh_guidedFilter(txForFilter, graySrcImage, txFiltered, guidanceRadius, guidanceEpsilon, CV_32FC1);

	//...............................................//
	//												 //
	//				       STEP.4                    //
	//				    ���ջָ�����	   	    	 //
	//												 //
	//...............................................//
 	
	// �� tx ��ĳ�����ر� t0 С��ʱ���� t0 ��������ػҶ�ֵ
	for (it_fBegin = tx.begin<float>(), it_fEnd = tx.end<float>(); it_fBegin != it_fEnd; it_fBegin++)
	{
		if ((*it_fBegin) < t0)
			(*it_fBegin) = t0;
	}

	// �� txFiltered ��ĳ�����ر� t0 С��ʱ���� t0 ��������ػҶ�ֵ
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

 	// ������

 	imshow("Դͼ", srcImage);
 	imshow("��ͨ��ͼ", image_dark_channel);
 	imshow("Ԥ��͸����ͼ", tx);
	imshow("����͸����ͼ", txFiltered);
 	imshow("��Ԥ��͸����ͼ�õ���ȥ��Ч��ͼ", dstImage);
	imshow("������͸����ͼ�õ���ȥ��Ч��ͼ",dstImage2);
 
  	waitKey();
  	system("pause");
  }

void czh_dark_channel_prior(Mat & srcImage, Mat & dstImage, int radius)
{
	// ��ͨ������
	Mat src, temp, dst;
	srcImage.copyTo(src);
	temp.create(srcImage.size(), CV_8UC1);
	dst.create(srcImage.size(), CV_8UC1);
	MatIterator_<Vec3b>itBegin, itEnd;
	MatIterator_<uchar>ptrDst;
	int minValue;

	for (itBegin = src.begin<Vec3b>(), itEnd = src.end<Vec3b>(), ptrDst = temp.begin<uchar>(); itBegin != itEnd; itBegin++, ptrDst++)
	{	// ����Դͼÿ�����أ�ȡ��ǰ������ͨ������Сֵ
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
	// �ҳ���ͨ��ͼ��������ֵ����ǰ 0.1% ���㣬��ѹ��Point���͵�vector֮��
	// ��δ�Ż����ٶ���
	vector<Point>().swap(pointVector);	// �����������
	Mat src;
	darkChannelImage.copyTo(src);
	int pointNumber = 0.1*0.01*src.cols*src.rows;	// ȡ��ͨ������ǰ0.1%�ĵ�
	int maxValue[3];
	maxValue[0] = (int)src.at<uchar>(0, 0);
	maxValue[1] = 0;
	maxValue[2] = 0;
	Point tempPoint;

	for (int it = 0; it < pointNumber; it++)	// �ܹ���Ҫ pointNumber ���������������
	{
		for (int i = 0; i < src.rows; i++)		// ÿ��Ѱ�����ֵʱ������ͼ����ÿ������
		{
			for (int j = 0; j < src.cols; j++)
			{
				if (maxValue[0] < (int)src.at<uchar>(i, j))	// ѡ���Ҷ�ֵ���ĵ㣬����¼���ǵ�x,y����
				{
					maxValue[0] = (int)src.at<uchar>(i, j);
					maxValue[1] = i;
					maxValue[2] = j;
				}
			}
		}
		// Point ����� x ��Ӧ�У�y ��Ӧ��
		// �ѵ�ǰ�����Ҷ�ֵ�������ص����긳��tempPoint
		tempPoint.x = maxValue[2];
		tempPoint.y = maxValue[1];
		// �� tempPoint ѹ������֮��
		pointVector.push_back(tempPoint);
		// ���ѵ�ǰ�Ҷ�ֵ������صĻҶ�ֵ��0�������Ͳ���Ӱ���´ε���
		src.at<uchar>(maxValue[1], maxValue[2]) = 0;
		// ���������ֵΪ0
		maxValue[0] = 0;
	}
}

