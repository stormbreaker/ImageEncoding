#include <iostream>

using namespace std;

const int TOLERANCERANGE = 4; // this is a decent change across all three channels

struct pixel {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};

// not sure how to 
char** compressedImage = NULL;

void writeToStream(unsigned char count, char* rgbValues, bool isGrayscale)
{
	if (isGrayscale)
	{
	}
	else
	{
	}
}

void runlengthEncodeRange(pixel** image, int height, int width, bool isGrayscale)
{
	unsigned char currentRunLength = 0;
	pixel basePixel;  

	for (int rowIndex = 0; rowIndex < height; rowIndex++)
	{
		for (int columnIndex = 0; columnIndex < width; rowIndex++)
		{
			if (abs(image[rowIndex][columnIndex].red - basePixel.red) > 4 && abs(image[rowIndex][columnIndex].green - basePixel.green) > 4 && abs(image[rowIndex][columnIndex].blue - basePixel.blue) > 4)
			{
				// TODO write data to compression stream
				basePixel = image[rowIndex][columnIndex];
			}
			else
			{
				currentRunLength++;
				if (currentRunLength == 255)
				{
					//TODO write data to compression stream
					currentRunLength = 0;
				}
			}
		}
		// TODO write out count and value no matter what
	}
}

void runlengthDecodeRange(char** compressedBytes, )
{

}

void runlengthEncodeBitPlane(char** compress /*other params*/)
{

}

void runlengthDecodeBitPlane()
{
	
}

int main()
{
	cout << "Hi" << endl;
}
