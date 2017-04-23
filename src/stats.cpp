#include "statistics.h"
#include <iostream>
#include <iomanip>
using namespace std;
using namespace cv;

int main(int argc, char** argv)
{

	Mat image1 = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	Mat image2 = imread(argv[2], CV_LOAD_IMAGE_COLOR);

	cout << rootMeanSquare(image1, image2) << endl;

	return 0;
}
