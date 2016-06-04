#ifndef CHENG_IMAGE_FUNCTION_H_
#define CHENG_IMAGE_FUNCTION_H_
#include "opencv2/opencv.hpp"
#include <vector>

using namespace std;
using namespace cv;

void czh_inverseBinary(Mat & src_bw_Image, Mat & dst_bw_Image);	// 使用 LUT 反转图像中黑白像素
void czh_endPoint(Mat & src_skeleton_Image, Mat & dst_endPoint_Image);	// 程序将会寻找该骨骼图像的端点
void czh_writeWhitePixel(Mat & srcImage, vector<Point2f> & dstWhitePoints);// 找出该图像中的白色像素并将像素坐标存于输入参数矢量
void czh_skeleton(Mat & srcImage, Mat & dstImage, int iterations = 10);	// 该函数输入一个二值图像或者灰度图像，输出该图像的骨骼

#endif
