#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "../image_io.h"
using namespace cv;
using namespace std;

void check_max( const Vec3b &f, unsigned char &max)
{
    if( f[0] < max )
        max = f[0];
    if( f[1] < max )
        max = f[1];
    if( f[2] < max )
        max = f[2];
} 

void prediction_1D( Mat f,Mat &f_hat )
{
    const int order = 1;
    const int alpha = 1;
    int r, c;
    int pre;
    unsigned char max = 0;
    for( r = 0; r < f.rows; r++ )
    {
        for( c = order; c < f.cols; c++ )
        {
            pre = c-1;
            f_hat.at<Vec3b>(r,c) = alpha*f.at<Vec3b>(r,pre);
            check_max(f_hat.at<Vec3b>(r,c), max);
        }
    }

    for( r = order; r < f.rows; r++ )
    {
        pre = r-1;
        f_hat.at<Vec3b>(r,0) = alpha*f.at<Vec3b>(pre,0);
        check_max(f_hat.at<Vec3b>(r,0), max);
    }
    cout << (unsigned int)max << endl;
}

int main( int argc, char** argv )
{

    int aflag = 0;
    int bflag = 0;
    char *cvalue = NULL;
    int index;
    int c;
    int alpha[2] = {1};
    int order = 1;

    Mat f_hat, error, new_image;
    Mat image;
    image = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }
    
    f_hat = image.clone();
    prediction_1D(image, f_hat );
    error = image - f_hat;
    new_image = error + f_hat;
    
    
    
   /* namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Display window", image );                   // Show our image inside it.
    namedWindow("Prediction Error",WINDOW_AUTOSIZE );
    imshow( "Prediction Error", error);
    namedWindow("F_HAT", WINDOW_AUTOSIZE);
    imshow( "F_HAT", f_hat);
    namedWindow("New Image", WINDOW_AUTOSIZE );
    imshow( "New Image", new_image);
    */
    imwrite("tmp.png",error);
    waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}

