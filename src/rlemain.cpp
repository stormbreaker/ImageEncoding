#include "runlength.h"
#include <cmath>
#include <opencv2/highgui/highgui.hpp>

bool imageTypeCheck(Mat image)
{
	int temp = image.type();
	bool grayscale = false;
	switch (temp)
	{
		case CV_8UC3:
			//cout << "RGB" << endl;
			grayscale = false;
			break;
		case CV_8UC1:
			//cout << "grayscale" << endl;
			grayscale = true;
			break;
		default:
			//cout << "um" << endl;
			break;
	};
	return grayscale;
}

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

int main()
{
	Mat image = imread("/home/student/7285523/csc442final/images/binary.png", CV_LOAD_IMAGE_COLOR);
	Mat decodedImage;	

	//runlengthEncodeRange(image, image.rows, image.cols);
	//decodedImage = runlengthDecodeRange();


	runlengthEncodeBitPlane(image, image.rows, image.cols);
	decodedImage = runlengthDecodeBitPlane();

	//decodedImage = imread("/home/student/7285523/csc442final/decoded.png", CV_LOAD_IMAGE_COLOR);

	cout << rootMeanSquare(image, decodedImage) << endl;

	return 0;
}
