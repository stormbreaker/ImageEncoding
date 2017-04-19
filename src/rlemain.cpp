#include "runlength.h"
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

int main()
{
	Mat image = imread("/home/student/7285523/csc442final/images/Hummingbird.jpg", CV_LOAD_IMAGE_COLOR);
	
	runlengthEncodeRange(image, image.rows, image.cols);
	runlengthDecodeRange();


	return 0;
}
