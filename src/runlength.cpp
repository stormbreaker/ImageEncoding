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

void writeHeader(int height, int width, string filetype)
{
	
}

void runlengthEncodeRange(Mat image, int height, int width, bool isGrayscale)
{
	unsigned char currentRunLength = 0;
	Vec3b basePixel = {0, 0, 0};  // initialize is good but 
	unsigned char count = 0;

	unsigned char tempChars[3];

	basePixel = image.at<Vec3b>(0, 0);

	for (int rowIndex = 0; rowIndex < height; rowIndex++)
	{
		for (int columnIndex = 0; columnIndex < width; columnIndex++)
		{
			//cout << rowIndex << " " << columnIndex << endl;
			if (abs(image.at<Vec3b>(rowIndex, columnIndex)[RED] - basePixel[RED]) > TOLERANCERANGE && abs(image.at<Vec3b>(rowIndex, columnIndex)[GREEN] - basePixel[GREEN]) > TOLERANCERANGE && abs(image.at<Vec3b>(rowIndex, columnIndex)[BLUE] - basePixel[BLUE]) > TOLERANCERANGE)
			{
				for (int i = 0; i < 3; i++)
				{
					tempChars[i] = basePixel[i];
				}
				writeToStream(count, tempChars, false);
				count = 0;
				basePixel = image.at<Vec3b>(rowIndex, columnIndex); 
			}
			if (count == 255)
			{
				for (int i = 0; i < 3; i++)
				{
					tempChars[i] = basePixel[i];
				}
				writeToStream(count, tempChars, false);
				count = 0;
			}
			count++;
		}
	}
}

void runlengthEncodeRangeChannels(Mat image, int height, int width, bool isGrayscale)
{
	unsigned char currentRunLength = 0;
	unsigned char currentIntensity = 0;

	unsigned char count;

	currentIntensity = image.at<Vec3b>(0, 0)[0];

	for (int channel = 0; channel < 3; channel++)
	{
		count = 0;
		for (int rowIndex = 0; rowIndex < height; rowIndex++)
		{
			for (int columnIndex = 0; columnIndex < width; columnIndex++)
			{
				if (abs(image.at<Vec3b>(rowIndex, columnIndex)[channel] - currentIntensity) > TOLERANCERANGE)
				{
					// figure out how to get into stream
					count = 0;
					currentIntensity = image.at<Vec3b>(rowIndex, columnIndex)[channel];
				}
				if (count == 255)
				{
					// figure out how to get into a stream
				}
				count++;
			}
		}
	}
}

void runlengthDecodeRange(char** compressedBytes, bool isGrayscale)
{
	// read in width and height
	// use width to count number of pixels to read.  will need a temporary column counter
	// read in the pairs
}

void runlengthDecodeRangeChannels(char** compressedBytes, bool isGrayscale)
{
	
}
