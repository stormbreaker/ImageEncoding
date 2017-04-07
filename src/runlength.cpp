#include <iostream>

using namespace std;

const int TOLERANCERANGE = 4; // this is a decent change across all three channels

struct pixel {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};

void runlengthEncodeRange(pixel** image, int height, int width, bool isGrayscale)
{
	unsigned char currentRunLength = 0;
	pixel basePixel;  

	for (int rowIndex = 0; rowIndex < height; rowIndex++)
	{
		for (int columnIndex = 0; columnIndex < width; rowIndex++)
		{
			if (abs(image[rowIndex][columnIndex].red - basePixel.red) > 4 && abs(image[rowIndex][columnIndex].green - basePixel.green) && abs(image[rowIndex][columnIndex].blue - basePixel.green))
			{
				
			}
		}
	}
}

void runlengthDecodeRange()
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
