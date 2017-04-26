#include "dpcm.h"
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
    char p, q;

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
    f = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    height = f.rows;
    width = f.cols;
    dpcm(f, height, width, p, q, out_fname.c_str());
    dpcm_decoder(out_fname.c_str());
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





