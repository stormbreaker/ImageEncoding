#include "image_io.h"




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

