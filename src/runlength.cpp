#include "runlength.h"


const int TOLERANCERANGE = 4; // this is a decent change across all three channels

enum colors {BLUE, GREEN, RED};

unsigned char** compressedImage = NULL;

void writeToStream(unsigned char count, unsigned char* rgbValues, bool isGrayscale)
{
	if (isGrayscale)
	{
		// write a single byte
	}
	else
	{
		// write all three bytes
		cout << (int)count << " " << (int)rgbValues[0] << " " << (int)rgbValues[1] << " " << (int)rgbValues[0] << endl;
	}
}

void runlengthEncodeRange(Mat image, int height, int width, bool isGrayscale)
{
	unsigned char currentRunLength = 0;
	Vec3b basePixel = {0, 0, 0};  
	unsigned char count = 0;

	unsigned char tempChars[3];

	for (int rowIndex = 0; rowIndex < height; rowIndex++)
	{
		for (int columnIndex = 0; columnIndex < width; columnIndex++)
		{
			//cout << rowIndex << " " << columnIndex << endl;
			if (abs(image.at<Vec3b>(rowIndex, columnIndex)[RED] - basePixel[RED]) > 4 && abs(image.at<Vec3b>(rowIndex, columnIndex)[GREEN] - basePixel[GREEN]) > 4 && abs(image.at<Vec3b>(rowIndex, columnIndex)[BLUE] - basePixel[BLUE]) > 4)
			{
				for (int i = 0; i < 3; i++)
				{
					tempChars[i] = basePixel[i];
				}
				writeToStream(count, tempChars, false);
				count = 0;
				basePixel = image.at<Vec3b>(rowIndex, columnIndex); 
			}
			count++;
		}
	}
}

void runlengthDecodeRange(char** compressedBytes, bool isGrayscale)
{
	// read in width and height
	// use width to count number of pixels to read.  will need a temporary column counter
	// read in the pairs
}
