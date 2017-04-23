/*
	Author: Benjamin Kaiser
	Description:  This file contains all the function headers for our statistical calculation
	functions.  These are then implemented in statistics.cpp.  It also includes the proper 
	include statements to use the OpenCV framework for the statistics.  
*/
#ifndef BTK_STATISTICS
#define BTK_STATISTICS

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

double rootMeanSquare(Mat image1, Mat image2);

#endif
