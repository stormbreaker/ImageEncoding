/*
	Author: Benjamin Kaiser
	Description:  This file is the implementation for the statistical calculation functions.
*/
#include "statistics.h"
/*
	Author: Benjamin Kaiser
	Description:  This function computes the root mean square error between two images.
	In order to do this, it takes in two OpenCV Matrix object representation of an image.
	It performs an error check to ensure that the two images are of the same size.
	It then loops through the two images and converts the RGB values into the Y of
	YIQ so that we only have a single value to compare together.  It then takes the difference
	of the two intensities, squares it and sums the results up.
	When the entire image has been looped through, the sum is divided by the number of pixels
	in the image and the square root of this is returned.  
*/
double rootMeanSquare(Mat image1, Mat image2)
{
	// calculation variables
	double sum = 0;
	double average;
	int numberOfPixels = image1.rows * image1.cols;

	// error check
	if (image1.rows != image2.rows || image1.cols != image2.cols)
	{
		return INT_MAX;
	}

	//process image
	for (int i = 0; i < image1.rows; i++)
	{
		for (int j = 0; j < image1.cols; j++)
		{
			// convert RGB to YIQ
			double intensity1 = .299 * image1.at<Vec3b>(i, j)[2] + .587 * image1.at<Vec3b>(i, j)[1] + .114 * image1.at<Vec3b>(i, j)[0];
			double intensity2 = .299 * image2.at<Vec3b>(i, j)[2] + .587 * image2.at<Vec3b>(i, j)[1] + .114 * image2.at<Vec3b>(i, j)[0];
			// sum difference of squares
			sum += pow(intensity1 - intensity2, 2);
		}
	}

	average = sum/numberOfPixels;
	return sqrt(average);	
}

double MeanSquare(Mat image1, Mat image2 )
{
    // calculation variables
    double sum = 0;
    double average;
    int numberOfPixels = image1.rows * image1.cols;

    // error check
    if (image1.rows != image2.rows || image1.cols != image2.cols)
    {
        return INT_MAX;
    }

    //process image
    for (int i = 0; i < image1.rows; i++)
    {
        for (int j = 0; j < image1.cols; j++)
        {
            // convert RGB to YIQ
            double intensity1 = .299 * image1.at<Vec3b>(i, j)[2] + .587 * image1.at<Vec3b>(i, j)[1] + .114 * image1.at<Vec3b>(i, j)[0];
            double intensity2 = .299 * image2.at<Vec3b>(i, j)[2] + .587 * image2.at<Vec3b>(i, j)[1] + .114 * image2.at<Vec3b>(i, j)[0];
            // sum difference of squares
            sum += pow(intensity1 - intensity2, 2);
        }
    }

    average = sum/numberOfPixels;
    return average;
}




double signalToNoise( Mat image1, Mat image2 )
{
    double sum = 0;
    double average;
    int numberOfPixels = image1.rows * image1.cols;
    double intensity;
    int r, c;
    //error check
    if( image1.rows != image2.rows || image1.cols != image2.cols)
        return INT_MAX;
    average = numberOfPixels * MeanSquare(image1, image2);
    //precess image
    for( r = 0; r < image1.rows; r++)
    {
        for( c = 0; c < image1.cols; c++)
        {
			intensity = .299 * image2.at<Vec3b>(r, c)[2] + .587 * image2.at<Vec3b>(r, c)[1] + .114 * image2.at<Vec3b>(r, c)[0];
            sum += intensity * intensity;
        }
    }
    average = sum/average;
    return average;
}
