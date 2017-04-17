#ifndef BTK_RUNLENGTH
#define BTK_RUNLENGTH

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;
using namespace cv;

typedef unsigned char uchar;

void runlengthEncodeRange(Mat image, int height, int width);
void runlengthDecodeRange();
void writeHeader(int height, int width, char* filetype, int bits, char lossless);
void writeToStream(unsigned char count, unsigned char* rgbValues);

#endif
