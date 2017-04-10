#include "runlength.h"
#include <opencv2/highgui/highgui.hpp>


const int TOLERANCERANGE = 4; // this is a decent change across all three channels

enum colors {BLUE, GREEN, RED};

int imageTypeCheck(Mat image)
{
	int temp = image.type();
	bool grayscale = false;
	switch (temp)
	{
		case CV_8UC3:
			cout << "RGB" << endl;
			grayscale = false;
			break;
		case CV_8UC1:
			cout << "grayscale" << endl;
			grayscale = true;
			break;
		default:
			cout << "um" << endl;
			break;
	};
	return grayscale;
}

unsigned char** compressedImage = NULL;

void writeToStream(unsigned char count, char* rgbValues, bool isGrayscale)
{
	if (isGrayscale)
	{
		// write a single byte
	}
	else
	{
		// write all three bytes
	}
}

void runlengthEncodeRange(Mat image, int height, int width, bool isGrayscale)
{
	unsigned char currentRunLength = 0;
	Vec3b basePixel = {0, 0, 0};  
	int count = 0;
	for (int rowIndex = 0; rowIndex < height; rowIndex++)
	{
		for (int columnIndex = 0; columnIndex < width; columnIndex++)
		{
			//cout << rowIndex << " " << columnIndex << endl;
			if (abs(image.at<Vec3b>(rowIndex, columnIndex)[RED] - basePixel[RED]) > 4 && abs(image.at<Vec3b>(rowIndex, columnIndex)[GREEN] - basePixel[GREEN]) > 4 && abs(image.at<Vec3b>(rowIndex, columnIndex)[BLUE] - basePixel[BLUE]) > 4)
			{
				// TODO write data to compression stream
				count++;
				if (rowIndex < 20)
					cout << count << "occurred: " << rowIndex << " " << columnIndex << endl;
				basePixel = image.at<Vec3b>(rowIndex, columnIndex);
			}
		}
		// TODO write out count and value no matter what
	}
}

void runlengthDecodeRange(char** compressedBytes, bool isGrayscale)
{

}

int main()
{
	Mat image = imread("/home/student/7285523/csc442final/Images/circles.png", -1);
	bool grayscaleFlag = imageTypeCheck(image);
	runlengthEncodeRange(image, image.rows, image.cols, grayscaleFlag);
}
