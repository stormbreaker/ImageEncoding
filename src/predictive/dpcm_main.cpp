#include "dpcm.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
#include "../statistics/statistics.h"


using namespace cv;
using namespace std;


void print_error(const int &argc);


int main( int argc, char **argv)
{
    string out_fname;
    string in_fname;
    Mat f_dec;
    Mat f;
    int height;
    int width;
    char p, q;
    int bits = 3;
    if( argc != 5 )
    {
        print_error(argc);
        return -1;
    }
    in_fname = string(argv[1]);
    if( argc == 5 )
        out_fname = string(argv[4]);
    else
    {
        
    }
    p = atoi(argv[2]);
    q = atoi(argv[3]);
    if( q == 4)
        bits = 2;
    f = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    height = f.rows;
    width = f.cols;
    cout << "Encoding Image Using: " << out_fname << endl;
    cout << "Image was encoded using "<< bits*3 <<"-bits/pixel." << endl;
    dpcm(f, height, width, p, q, out_fname.c_str());
    cout << endl;
    cout << "Decoding Image" << endl;
    f_dec = dpcm_decoder(out_fname.c_str());
    cout << endl;
    cout << "Root Mean Square: " << rootMeanSquare(f, f_dec) << endl; 
    cout << "Signal to Noise Ratio: " << signalToNoise(f, f_dec) << endl;


}

void print_error(const int &argc)
{
    if( argc < 5 )
        cout << "To few arguments.\n";
    else
        cout << "To many arguments.\n";

    cout << "Program Usage:\n";
    cout << "./delta_mod input_fname {1 or 2} {4 or 8} output_fname\n\n";
    cout << "The first argument after the input file name is the order of the predictor follewed by either a 4 or 8 level quantizer.\n\n";
}





