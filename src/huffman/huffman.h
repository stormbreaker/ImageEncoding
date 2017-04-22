/*
                             ***** huffman.h *****

Contains the include files and functions prototypes for the Huffman Project.

CSC542 Digital Image Processing - Spring 2017

Author: Scott Carda
*/

#ifndef __HUFFMAN_H__
#define __HUFFMAN_H__

/******************************************************************************/
/*                                Include Files                               */
/******************************************************************************/

#include <vector> // Vectors are used throughout the program
#include <algorithm> // The sort method is used on vectors and arrays
#include <iostream> // For printing to the console

// OpenCV includes
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Project header files
#include "bitstream.h"
#include "huffnode.h"

using namespace cv;

/******************************************************************************/
/*                             Function Prototypes                            */
/******************************************************************************/

// Huffman Encoding and Decoding Algorithms
void huffman_encode( Mat img, string outfile );
void huffman_decode( Mat &img, string infile );

// Huffman Tree Functions
huffnode* huffman_tree( int* hist, int size );
void cascade_bitstring( huffnode* root );
void get_leaves( huffnode* leaves[256], huffnode* root );
unsigned char read_next_huff( bitstream &bin, huffnode* root );

// Reading and Writing the Huffman Encoding Scheme
void read_hist( bitstream &bout, int* hist, int size );
void write_hist( bitstream &bout, int* hist, int size );
void read_hufftree( bitstream &bin, huffnode* &root );
void write_hufftree( bitstream &bout, huffnode* root );

// Statistics Functions
double avg_bits_compressed( huffnode* leaves[256], int height, int width );
double calc_entropy( int height, int width, int* hist, int size );

// Miscellaneous Functions
void get_hist( int* hist, Mat img );
void usage();

#endif
