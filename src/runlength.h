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

const int TOLERANCERANGE = 32; // this is a decent change across all three channels

void runlengthEncodeRange(Mat image, int height, int width, string file);
void runlengthEncodeBitPlane(Mat image, int height, int width, string file);

Mat runlengthDecodeBitPlane(string filePath);
Mat runlengthDecodeRange(string filePath);


void writeHeader(int height, int width, char* filetype, int bits, char lossless, string file);
void writeToStream(unsigned char count, unsigned char* rgbValues);
void writeChannelToFile(vector<unsigned char> channel, string file);

#endif
