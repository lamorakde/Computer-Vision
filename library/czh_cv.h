#ifndef CHENG_IMAGE_FUNCTION_H_
#define CHENG_IMAGE_FUNCTION_H_
#include "opencv2/opencv.hpp"
#include <vector>

using namespace std;
using namespace cv;
//static enum { RED, GREEN, BLUE, WHITE, BLACK } color;

// 该函数自动判断输出图像Mat对象的类型，以决定输出图像是灰度图像(pgm)或者是三通道图像(ppm)
// function:		 write the output image
// Mat &dstImage:    the image to be wrote
// string imageName: the name of output image
void czh_imwrite(Mat &dstImage, const string imageName);

// 程序信息：程序功能
// function:	show the program function
void czh_helpInformation(string const &functionInfo);	

// 检测图像对象是否已经成功打开
// function:	detect if the image is well read
void czh_imageOpenDetect(Mat & srcImage, string & fileName, string & fileType);

// 使用 LUT 反转图像中黑白像素
// function:	inverse the white and black pixel
// Mat & src_bw_Image:		input image
// Mat & dst_bw_Image:		output image
void czh_inverseBinary(Mat & src_bw_Image, Mat & dst_bw_Image);	

// 程序将会寻找该骨骼图像的端点
// function: looking for the endpoints of skeleton image
// Mat & src_skeleton_Image:		input image
// Mat & dst_endPoint_Image:		output image
void czh_endPoint(Mat & src_skeleton_Image, Mat & dst_endPoint_Image);	

// 找出该图像中的白色像素并将像素坐标存于输入参数矢量
// function: find the white pixels in the input image and write their coordinates in a vector
// Mat & srcImage:						input image
// vector<Point2f> & dstWhitePoints:	output vector
void czh_writeWhitePixel(Mat & srcImage, vector<Point2f> & dstWhitePoints);

// 该函数使用zhang的并行算法输入一个二值图像或者灰度图像，输出该图像的骨骼
// function: thin algorithm, skeleton algorithm, with ZHANG's paralle method
// Mat & srcImage:		input image
// Mat & dstImage:		output image
// int iterations:		number of iterations
void czh_thin_parallel(Mat & srcImage, Mat & dstImage, int iterations = 10);	

// 该函数使用查表法输入一个二值图像或者灰度图像，输出该图像的骨骼
// function: thin algorithm, skeleton algorithm, with LUT method
// Mat & srcImage:		input image
// Mat & dstImage:		output image
// int iterations:		number of iterations
void czh_thin_LUT(Mat & srcImage, Mat & dstImage, int iterations = 10);	

// 该函数输入一个三通道RGB图像，抽取该图像中的 color 颜色的像素，保存为白色像素
// function: extraction of color
// Mat & srcImage:		input image
// Mat & dstImage:		output image
// int color:			"RED" for extraction of red color
void czh_extractColor(Mat &srcImage, Mat & dstImage, int color);	

// 该函数输入一个单通道图像，做最小值滤波，窗口大小 size * size
// function:	given a one channel image, do min filter
// Mat & srcImage:		input image
// Mat & dstImage:		output image
// int size:			window size is 'size*size'
void czh_minFilter(Mat &srcImage, Mat & dstImage, int size);	

// 该函数计算以点 (x0, y0) 为圆心， radius为半径的圆并将圆上的点的坐标导入 vector<Point>矢量 pointsOfCircle 之中
// function:	draw a circle with the center (x0,y0), radius with 'radius', the coordinates will be saved in vector<Point> &pointsOfCircle
void czh_middlePointCircle(int x0, int y0, int radius, vector<Point> &pointsOfCircle); 

// Bresenham 优化画圆算法
// function: Bresenham circle algorithm
// draw a circle with the center (x0,y0), radius with 'radius', the coordinates will be saved in vector<Point> &pointsOfCircle
void czh_BresenhamCircle(int x0, int y0, int radius, vector<Point> &pointsOfCircle);	

// Bresenham 优化画圆算法
// function: Bresenham line algorithm
// Point pt0: start point
// Point pt1: end point
// vector<Point> &linePoints: save the line pixels
void czh_BresenhamLine(Point pt0, Point pt1, vector<Point> &linePoints);	

// 展示以选定点为圆心，radius为半径的圆的灰度变化
// function: show the profile of a circle with given coordinate
// x0,y0,radius are the parameters of the center of circle
void czh_Circle_Profile(const Mat & srcImage, const int x0, const int y0, const int radius); 

// 展示以选定的两个点为端点的直线在图中的灰度变化
// function: show the profile of a line with given parameters
// Point pt0, Point pt1 are the start and end points
// bool sameSize: true if the profile image has the same width with the original image
Mat czh_Line_Profile(const Mat & srcImage, Point pt0, Point pt1, bool sameSize);	

// 该函数会使用 two - pass 算法针对二值图像做连通域标记
// function: labelling algorithm with two pass method
void czh_labeling(Mat & src, Mat & dst);	

// backup of the last function
void czh_labeling_backup(Mat & src, Mat & dst);	// 该函数会使用 two - pass 算法针对二值图像做连通域标记,网上函数备份版

//根据OpenCV里的标准霍夫线变换改编的，核心算法并未改变，但是修改了判定条件
// hough line function wrote with opencv
void czh_myHoughLines(const Mat & img, std::vector<Vec2f>& lines, float rho, float theta, int thresholdTotal, int thresholdRho, int thresholdAngle);	

// 该函数针对已经被连通域标记处理过的图像寻找每个连通域的重心，输出至 dstImage 之中,并把重心坐标输出到 centerPoints 之中
// function：find the gravity center of a labelled image
// Mat & labelledImage:				input labelled image
// Mat & dstImage:					output image
// vector<Point> & centerPoints:	vector to save the gravity center pixels
void czh_centerOfGravity(Mat & labelledImage, Mat & dstImage, vector<Point> & centerPoints);	

// 该函数实现了 Matlab 中 bwareaopen 函数的功能：对一个二值图像去噪，去除连通域面积小于 threshold 的点
// function: denoising, eliminate the connect zone if the surface is less than a threshold
// Mat & srcImage, Mat & dstImage:	input and output image 
// int zoneHeight, int zoneWidth:	height and width of sub-processing zone
// int threshold:	surface threshold
void czh_bwAreaOpen(Mat & srcImage, Mat & dstImage, int zoneHeight, int zoneWidth, int threshold);		

// 该函数实现了何凯明博士的 guided filter 引导滤波算法
// function: realisation of the guided filter of Kaiming HE
void czh_guidedFilter(Mat & srcImage, Mat & guidanceImage, Mat & dstImage, int radius, float epsilon, int outputType);	

// 该函数查找图像中的 valley 像素
// function: find the valley pixels in the image
// Mat & srcImage, Mat & dstImage: input and output image
// int valleyRadius: radius of valley
// int valleyThreshold1, int valleyThreshold2: thresholds
void czh_findValley(Mat & srcImage, Mat & dstImage, int valleyRadius, int valleyThreshold1, int valleyThreshold2);	
#endif
