/*
	Author: Benjamin Kaiser
	Description:  This file contains all the constants and function prototypes
	for everything necessary to implement a run length encoder.  These include
	the encoding algorithms, the decoding algorithms, and the helper functions
	for these.  
*/
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

// the value to be used +/- per channel
const int TOLERANCERANGE = 16;

// encoder prototypes
void runlengthEncodeRange(Mat image, int height, int width, string file);
void runlengthEncodeBitPlane(Mat image, int height, int width, string file);

void runlengthEncodeBitPlaneIgnore(Mat image, int height, int width, string file);


// decoder prototypes
Mat runlengthDecodeBitPlane(string filePath);
Mat runlengthDecodeRange(string filePath);
Mat runlengthDecodeBitPlaneIgnore(string filePath);

// helper function prototypes
void writeHeader(int height, int width, char* filetype, int bits, char lossless, string file);
void writeToStream(unsigned char count, unsigned char* rgbValues);
void writeChannelToFile(vector<unsigned char> channel, string file);

#endif
