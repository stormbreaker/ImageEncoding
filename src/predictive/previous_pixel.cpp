#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>


using namespace cv;
using namespace std;
// Calculates the total amount of entropy in a
// given image based on its histogram and size
double calc_entropy( int height, int width, int* hist, int size )
{
    int total = height * width; // Total number of symbols
    double entropy = 0.0; // Start calculated entropy at 0
    double probability; // The fraction of symbols that are a specific value
    
    // Sum the products of the probabilities with their log
    for( int i = 0; i < size; i++ )
    {
        probability = (double)hist[i] / total;
        if( probability != 0 )
            entropy += probability * log2( probability );
    }
    
    // The entropy is the negation of the calculated sum
    return -entropy;
}

/******************************************************************************/
/*                           Miscellaneous Functions                          */
/******************************************************************************/

// Create the histogram for the image
void get_hist( int* hist, Mat img )
{
	int height = img.rows;
	int width = img.cols; 

	// Loop through img, counting the number of times each value occurs
    for( int chan = 0; chan < 3; chan++)
	    for ( int r = 0; r < height; r++ )
		    for ( int c = 0; c < width; c++ )
			    hist[(int) img.at<Vec3b>( r, c )[chan]]++;
	return;
}

void prediction_1D( Mat f,Mat &f_hat )
{
    const int order = 1;
    const int alpha = 1;
    int r, c;
    int pre;
    for( r = 0; r < f.rows; r++ )
    {
        for( c = order; c < f.cols; c++ )
        {
            pre = c-1;
            f_hat.at<Vec3b>(r,c) = alpha*f.at<Vec3b>(r,pre);
        }
    }

    for( r = order; r < f.rows; r++ )
    {
        pre = r-1;
        f_hat.at<Vec3b>(r,0) = alpha*f.at<Vec3b>(pre,0);
    }
}

int main( int argc, char** argv )
{

    int hist[256] = {0};
    double entropy;
    int height, width;

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
    get_hist(hist,error);
    height = image.rows;
    width = image.cols;
    entropy = calc_entropy( height, width, hist, 256);
    cout << "Entropy: " << entropy << endl;
    cout << "Encoding will need: " << ceil(entropy) << " bits/pixel" << endl;
    cout << "Compression is limited to about 24/" << entropy << " or "<< ceil(24/entropy) <<":1" << endl;
    
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

