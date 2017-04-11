#ifndef BTK_RUNLENGTH
#define BTK_RUNLENGTH

#include <opencv2/core/core.hpp>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;
using namespace cv;

typedef unsigned char uchar;

void runlengthEncodeRange(Mat image, int height, int width, bool isGrayscale);
void runlengthDecodeRange(char** compressedBytes, bool isGrayscale);

#endif
