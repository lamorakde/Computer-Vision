#ifndef CHENG_IMAGE_FUNCTION_H_
#define CHENG_IMAGE_FUNCTION_H_
#include "opencv2/opencv.hpp"
#include <vector>

using namespace std;
using namespace cv;
//static enum { RED, GREEN, BLUE, WHITE, BLACK } color;

// �ú����Զ��ж����ͼ��Mat��������ͣ��Ծ������ͼ���ǻҶ�ͼ��(pgm)��������ͨ��ͼ��(ppm)
// function:		 write the output image
// Mat &dstImage:    the image to be wrote
// string imageName: the name of output image
void czh_imwrite(Mat &dstImage, const string imageName);

// ������Ϣ��������
// function:	show the program function
void czh_helpInformation(string const &functionInfo);	

// ���ͼ������Ƿ��Ѿ��ɹ���
// function:	detect if the image is well read
void czh_imageOpenDetect(Mat & srcImage, string & fileName, string & fileType);

// ʹ�� LUT ��תͼ���кڰ�����
// function:	inverse the white and black pixel
// Mat & src_bw_Image:		input image
// Mat & dst_bw_Image:		output image
void czh_inverseBinary(Mat & src_bw_Image, Mat & dst_bw_Image);	

// ���򽫻�Ѱ�Ҹù���ͼ��Ķ˵�
// function: looking for the endpoints of skeleton image
// Mat & src_skeleton_Image:		input image
// Mat & dst_endPoint_Image:		output image
void czh_endPoint(Mat & src_skeleton_Image, Mat & dst_endPoint_Image);	

// �ҳ���ͼ���еİ�ɫ���ز���������������������ʸ��
// function: find the white pixels in the input image and write their coordinates in a vector
// Mat & srcImage:						input image
// vector<Point2f> & dstWhitePoints:	output vector
void czh_writeWhitePixel(Mat & srcImage, vector<Point2f> & dstWhitePoints);

// �ú���ʹ��zhang�Ĳ����㷨����һ����ֵͼ����߻Ҷ�ͼ�������ͼ��Ĺ���
// function: thin algorithm, skeleton algorithm, with ZHANG's paralle method
// Mat & srcImage:		input image
// Mat & dstImage:		output image
// int iterations:		number of iterations
void czh_thin_parallel(Mat & srcImage, Mat & dstImage, int iterations = 10);	

// �ú���ʹ�ò������һ����ֵͼ����߻Ҷ�ͼ�������ͼ��Ĺ���
// function: thin algorithm, skeleton algorithm, with LUT method
// Mat & srcImage:		input image
// Mat & dstImage:		output image
// int iterations:		number of iterations
void czh_thin_LUT(Mat & srcImage, Mat & dstImage, int iterations = 10);	

// �ú�������һ����ͨ��RGBͼ�񣬳�ȡ��ͼ���е� color ��ɫ�����أ�����Ϊ��ɫ����
// function: extraction of color
// Mat & srcImage:		input image
// Mat & dstImage:		output image
// int color:			"RED" for extraction of red color
void czh_extractColor(Mat &srcImage, Mat & dstImage, int color);	

// �ú�������һ����ͨ��ͼ������Сֵ�˲������ڴ�С size * size
// function:	given a one channel image, do min filter
// Mat & srcImage:		input image
// Mat & dstImage:		output image
// int size:			window size is 'size*size'
void czh_minFilter(Mat &srcImage, Mat & dstImage, int size);	

// �ú��������Ե� (x0, y0) ΪԲ�ģ� radiusΪ�뾶��Բ����Բ�ϵĵ�����굼�� vector<Point>ʸ�� pointsOfCircle ֮��
// function:	draw a circle with the center (x0,y0), radius with 'radius', the coordinates will be saved in vector<Point> &pointsOfCircle
void czh_middlePointCircle(int x0, int y0, int radius, vector<Point> &pointsOfCircle); 

// Bresenham �Ż���Բ�㷨
// function: Bresenham circle algorithm
// draw a circle with the center (x0,y0), radius with 'radius', the coordinates will be saved in vector<Point> &pointsOfCircle
void czh_BresenhamCircle(int x0, int y0, int radius, vector<Point> &pointsOfCircle);	

// Bresenham �Ż���Բ�㷨
// function: Bresenham line algorithm
// Point pt0: start point
// Point pt1: end point
// vector<Point> &linePoints: save the line pixels
void czh_BresenhamLine(Point pt0, Point pt1, vector<Point> &linePoints);	

// չʾ��ѡ����ΪԲ�ģ�radiusΪ�뾶��Բ�ĻҶȱ仯
// function: show the profile of a circle with given coordinate
// x0,y0,radius are the parameters of the center of circle
void czh_Circle_Profile(const Mat & srcImage, const int x0, const int y0, const int radius); 

// չʾ��ѡ����������Ϊ�˵��ֱ����ͼ�еĻҶȱ仯
// function: show the profile of a line with given parameters
// Point pt0, Point pt1 are the start and end points
// bool sameSize: true if the profile image has the same width with the original image
Mat czh_Line_Profile(const Mat & srcImage, Point pt0, Point pt1, bool sameSize);	

// �ú�����ʹ�� two - pass �㷨��Զ�ֵͼ������ͨ����
// function: labelling algorithm with two pass method
void czh_labeling(Mat & src, Mat & dst);	

// backup of the last function
void czh_labeling_backup(Mat & src, Mat & dst);	// �ú�����ʹ�� two - pass �㷨��Զ�ֵͼ������ͨ����,���Ϻ������ݰ�

//����OpenCV��ı�׼�����߱任�ı�ģ������㷨��δ�ı䣬�����޸����ж�����
// hough line function wrote with opencv
void czh_myHoughLines(const Mat & img, std::vector<Vec2f>& lines, float rho, float theta, int thresholdTotal, int thresholdRho, int thresholdAngle);	

// �ú�������Ѿ�����ͨ���Ǵ������ͼ��Ѱ��ÿ����ͨ������ģ������ dstImage ֮��,����������������� centerPoints ֮��
// function��find the gravity center of a labelled image
// Mat & labelledImage:				input labelled image
// Mat & dstImage:					output image
// vector<Point> & centerPoints:	vector to save the gravity center pixels
void czh_centerOfGravity(Mat & labelledImage, Mat & dstImage, vector<Point> & centerPoints);	

// �ú���ʵ���� Matlab �� bwareaopen �����Ĺ��ܣ���һ����ֵͼ��ȥ�룬ȥ����ͨ�����С�� threshold �ĵ�
// function: denoising, eliminate the connect zone if the surface is less than a threshold
// Mat & srcImage, Mat & dstImage:	input and output image 
// int zoneHeight, int zoneWidth:	height and width of sub-processing zone
// int threshold:	surface threshold
void czh_bwAreaOpen(Mat & srcImage, Mat & dstImage, int zoneHeight, int zoneWidth, int threshold);		

// �ú���ʵ���˺ο�����ʿ�� guided filter �����˲��㷨
// function: realisation of the guided filter of Kaiming HE
void czh_guidedFilter(Mat & srcImage, Mat & guidanceImage, Mat & dstImage, int radius, float epsilon, int outputType);	

// �ú�������ͼ���е� valley ����
// function: find the valley pixels in the image
// Mat & srcImage, Mat & dstImage: input and output image
// int valleyRadius: radius of valley
// int valleyThreshold1, int valleyThreshold2: thresholds
void czh_findValley(Mat & srcImage, Mat & dstImage, int valleyRadius, int valleyThreshold1, int valleyThreshold2);	
#endif
