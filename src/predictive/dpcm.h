#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp>

#include <fstream>
#include <iostream>
using namespace std;
using namespace cv;

#ifndef _DPCM_H_
#define _DPCM_H_
void write_char( ofstream &fout, const char &edot);
void write_header( ofstream &fout, const int &height, const int &width, const char* ext, const char *p, const char *q);
void read_char( ifstream &fin, unsigned char &edot);
void read_header(ifstream &fin, int &height, int &width, char* ext, char& p, char &q);
double first_predicter(const double &fdot );
double second_predicter( const double &xprev, const double &yprev);
double four_level( const double &en );
double four_level( const double &en );
double eight_level( const double &en );
void store_bits( unsigned char &code, const double &edot, const char &levels);
void store_two( unsigned char &code, const double &edot);
void store_four( unsigned char &code, const double &edot);
void dpcm( Mat f, const int &height, const int &width, const char &p, const char &q, const char *fname);
void dpcm_second( Mat f, const int &height, const int &width, ofstream &fout, const char& levels, double (*qauntizer)(const double&) );
void dpcm_first( Mat f, const int &height, const int &width, ofstream &fout, const char& levels, double (*qauntizer)(const double&) );
Mat dpcm_decoder( const char *fname);
void dpcm_first_dec( Mat f, const int &height, const int &width, ifstream &fin, const char &q);

void dpcm_second_dec( Mat f, const int &height, const int &width, ifstream &fin, const char &q);
double get_delta( char bit, char shift, char pix, char count);
double decode_4_level( char bit, char pix, char count);
double decode_8_level( char bit, char pix, char count);


#endif
