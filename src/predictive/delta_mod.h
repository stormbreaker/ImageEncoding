#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include "../statistics/statistics.h"

#include <string>
#include <fstream>
#include <iostream>
using namespace std;
using namespace cv;


#ifndef _DELTA_MOD_H_
#define _DELTA_MOD_H_
void store_bit( unsigned char &code, const double &edot);
void write_char( ofstream &fout, const unsigned char &edot);
void write_header( ofstream &fout, const int &height, const int &width, const char* ext, const float &delta);
void read_char( ifstream &fin, unsigned char &edot);
void read_header(ifstream &fin, int &height, int &width, char* ext, float &delta);
void delta_modulation(Mat f, const int &height, const int &width, const float &delta, const char * fname);
double inline dm_predictor(const double &alpha, const double &fdot);
double inline dm_quantizer( const double &en, const double &delta );
Mat delta_decoder(const char * fname);



#endif
