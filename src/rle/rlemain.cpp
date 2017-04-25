/*
	Author: Benjamin Kaiser
	Description:  This file contains the main function for the run length encoding executable.
	This is compiled via the Makefile using "make rle".  This program then handles encoding or
	decoding files using either a range method or the bitplane method.  The value for the range
	tolerance is stored in the runlength.h file as a constant.  

	Usage: rle <flag [-e|-d]> <encode type [-r|-b]> <file path>
*/
#include "runlength.h"
#include <cmath>
#include <opencv2/highgui/highgui.hpp>

/*
	Author: Benjamin Kaiser
	Description:  This is the main function for the runlength encoding portion of the project.
	It's usage is described in the file header.  This then processes the flags and the file
	path and calls the proper function depending on these parameters.  
*/
int main(int argc, char** argv)
{
	//variables to hold command-line arguments
	string imagePath;
	string encodeFlag;
	string typeFlag;

	//check proper usage
	if (argc < 4)
	{
		cout << "Usage: rle <flag [-e|-d]> <encode type [-r|-b]> <file path>" << endl;
		return 0;
	}

	//gather command line arguments
	encodeFlag = argv[1];
	typeFlag = argv[2];
	imagePath = argv[3];

	//image variables
	Mat image = imread(imagePath, CV_LOAD_IMAGE_COLOR);
	Mat decodedImage;

	// determine which encode or decode function to call
	if (encodeFlag == "-e")
	{
		if (typeFlag == "-r")
		{
			runlengthEncodeRange(image, image.rows, image.cols, imagePath);
		}
		else if (typeFlag == "-b")
		{
			runlengthEncodeBitPlane(image, image.rows, image.cols, imagePath);
			//runlengthEncodeBitPlaneIgnore(image, image.rows, image.cols, imagePath);
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
			//runlengthDecodeBitPlaneIgnore(imagePath);
		}
	}

	return 0;
}
