#include "delta_mod.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>



using namespace cv;
using namespace std;


void print_error(const int &argc);


int main( int argc, char **argv)
{
    string out_fname;
    string in_fname;

    Mat f;
    int height;
    int width;
    float delta;

    if( argc < 3  || argc > 4)
    {
        print_error(argc);
        return -1;
    }
    in_fname = string(argv[1]);
    if( argc == 4 )
        out_fname = string(argv[3]);
    delta = atof(argv[2]);
    f = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    height = f.rows;
    width = f.cols;
    cout << "Encoding Image Using: " << out_fname << endl;
    cout << "Image was encoded using 1-bit per pixel." << endl;
    delta_modulation(f, height, width, delta, out_fname.c_str());
    cout << endl;
    cout << "Decoding Image" << endl;
    Mat fnew = delta_decoder( out_fname.c_str());
    cout << endl;
    cout << "Root Mean Square: " << rootMeanSquare(f, fnew) << endl; 
    cout << "Signal to Noise Ratio: " << signalToNoise(f, fnew) << endl;
}

void print_error(const int &argc)
{
    if( argc < 3 )
        cout << "To few arguments.\n";
    else
        cout << "To many arguments.\n";

    cout << "Program Usage:\n";
    cout << "./delta_mod input_fname delta output_fname\n\n";
}





