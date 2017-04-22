#include "runlength.h"
#include "statistics.h"
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

int main(int argc, char** argv)
{

	string imagePath;
	string encodeFlag;
	string typeFlag;

	if (argc != 4)
	{
		cout << "Usage: rle <flag [-e|-d]> <encode type [-r|-b]> <file path>" << endl;
		return 0;
	}
	encodeFlag = argv[1];
	typeFlag = argv[2];
	imagePath = argv[3];

	Mat image = imread(imagePath, CV_LOAD_IMAGE_COLOR);
	Mat decodedImage;

	if (encodeFlag == "-e")
	{
		if (typeFlag == "-r")
		{
			runlengthEncodeRange(image, image.rows, image.cols, imagePath);
		}
		else if (typeFlag == "-b")
		{
			runlengthEncodeBitPlane(image, image.rows, image.cols, imagePath);
		}
	}
	else if (encodeFlag == "-d")
	{
		if (typeFlag == "-r")
		{
			runlengthDecodeRange(imagePath);
		}
		else if (typeFlag == "-b")
		{
			runlengthDecodeBitPlane(imagePath);
		}
	}

	return 0;
}
