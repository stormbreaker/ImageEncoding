#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "framework.h"
#include "runlength.h"

int main( int argc, char **argv )
{

    cv::Mat image;
    int method = valid_enc_args( argv, argc, image);
    if(method <  0 )
    {
        print_errors(method);
        return -1;
    }

    switch( method)
    {
        case 0:
            //call rle-r
			runlengthEncodeRange(image, image.rows, image.cols, false);
            break;
        case 1:
            //call rle-b;
            break;
        case 2: 
            //call huff;
            break;
        case 3: 
            //call previous pixel
            break;
        case 4:
            //cal delta modulation
            break;
        case 5:
            //differntial pulse code modulation
            break;
        default: print_errors(method);
    }
    return 0;
}
