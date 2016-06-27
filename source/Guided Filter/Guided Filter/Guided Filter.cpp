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
	// 因为 Trackbar 的调整参数必须为整形，所以 g_epsilon 为轨迹条显示的值/1000
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
	// 程序帮助信息
	string programInformation = "Guided Filter : 引导滤波.";
	string fileType = ".ppm";
	czh_helpInformation(programInformation);

	// 得到输入图像名称信息并输出
	std::cout << "Enter the input image name without " << fileType << ": ";
	string fileName, srcFileName;
	getline(cin, fileName);				// 获得输入文件名
	srcFileName = fileName + fileType;	// 确定输入图片文件类型

	Mat src = imread(srcFileName, 0);	// 读取源图像
	czh_imageOpenDetect(src, fileName, fileType);	// 检测源图像是否成功打开
	// 显示源图像信息
	std::cout << "\nInput image name:\t" << srcFileName << endl
		<< "Image size:\t\t" << src.cols << "*" << src.rows << endl
		<< "Image pixels:\t\t" << src.cols*src.rows << endl
		<< "Image type:\t\t" << src.type() << endl;

	// 时钟信息，记录程序算法使用时间
	double timeTotal = static_cast<double>(getTickCount());
	double time = static_cast<double>(getTickCount());

	// 开始核心图像处理算法

	g_srcImage = src;
	g_guidanceFilter = src;
	g_radius = 2;
	g_epsilon = 1;
	namedWindow("Guided Filter");
	createTrackbar("Radius", WINDOW_NAME, &g_radius, g_radiusMax, on_Trackbar_GuidedFilter);
	createTrackbar("Epsilon", WINDOW_NAME, &g_epsilon, g_epsilonMax, on_Trackbar_GuidedFilter);


	// 程序结束信息：报告程序所花时间
	timeTotal = ((double)getTickCount() - timeTotal) / getTickFrequency();
	std::cout << "Program finished :\t" << timeTotal << " second used.\n" << endl;

	waitKey(0);
	system("pause");
}