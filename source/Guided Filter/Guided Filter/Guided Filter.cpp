#include "opencv2/opencv.hpp"
#include <iostream>
#include "czh_binary_CV.h"

using namespace std;
using namespace cv;

#define WINDOW_NAME "Guided Filter"
int g_radius;
int g_radiusMax = 50;
int g_epsilon;
int g_epsilonMax = 1000;

Mat g_srcImage, g_guidanceFilter, g_dstImage;

void on_Trackbar_GuidedFilter(int, void*)
{
	// ��Ϊ Trackbar �ĵ�����������Ϊ���Σ����� g_epsilon Ϊ�켣����ʾ��ֵ/1000
	if (g_epsilon < 1)
	{
		g_epsilon = 1;
	}
	float realEpsilon = (float)g_epsilon / g_epsilonMax;
	czh_guidedFilter(g_srcImage, g_guidanceFilter, g_dstImage, g_radius, realEpsilon);
	
	imshow(WINDOW_NAME, g_dstImage);
}

int main()
{
	// ���������Ϣ
	string programInformation = "Guided Filter : �����˲�.";
	string fileType = ".ppm";
	czh_helpInformation(programInformation);

	// �õ�����ͼ��������Ϣ�����
	std::cout << "Enter the input image name without " << fileType << ": ";
	string fileName, srcFileName;
	getline(cin, fileName);				// ��������ļ���
	srcFileName = fileName + fileType;	// ȷ������ͼƬ�ļ�����

	Mat src = imread(srcFileName, 0);	// ��ȡԴͼ��
	czh_imageOpenDetect(src, fileName, fileType);	// ���Դͼ���Ƿ�ɹ���
	// ��ʾԴͼ����Ϣ
	std::cout << "\nInput image name:\t" << srcFileName << endl
		<< "Image size:\t\t" << src.cols << "*" << src.rows << endl
		<< "Image pixels:\t\t" << src.cols*src.rows << endl
		<< "Image type:\t\t" << src.type() << endl;

	// ʱ����Ϣ����¼�����㷨ʹ��ʱ��
	double timeTotal = static_cast<double>(getTickCount());
	double time = static_cast<double>(getTickCount());

	// ��ʼ����ͼ�����㷨

	g_srcImage = src;
	g_guidanceFilter = src;
	g_radius = 2;
	g_epsilon = 1;
	namedWindow("Guided Filter");
	createTrackbar("Radius", WINDOW_NAME, &g_radius, g_radiusMax, on_Trackbar_GuidedFilter);
	createTrackbar("Epsilon", WINDOW_NAME, &g_epsilon, g_epsilonMax, on_Trackbar_GuidedFilter);


	// ���������Ϣ�������������ʱ��
	timeTotal = ((double)getTickCount() - timeTotal) / getTickFrequency();
	std::cout << "Program finished :\t" << timeTotal << " second used.\n" << endl;

	waitKey(0);
	system("pause");
}