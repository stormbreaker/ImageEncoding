#include "statistics.h"

double rootMeanSquare(Mat image1, Mat image2)
{

	double sum = 0;
	double average;
	int numberOfPixels = image1.rows * image1.cols;

	if (image1.rows != image2.rows || image1.cols != image2.cols)
	{
		return -1;
	}
	for (int i = 0; i < image1.rows; i++)
	{
		for (int j = 0; j < image1.cols; j++)
		{
			double intensity1 = .299 * image1.at<Vec3b>(i, j)[2] + .587 * image1.at<Vec3b>(i, j)[1] + .114 * image1.at<Vec3b>(i, j)[0];
			double intensity2 = .299 * image2.at<Vec3b>(i, j)[2] + .587 * image2.at<Vec3b>(i, j)[1] + .114 * image2.at<Vec3b>(i, j)[0];
			sum += pow(intensity1 - intensity2, 2);
		}
	}
	average = sum/numberOfPixels;
	return sqrt(average);	
}
