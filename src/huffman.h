#ifndef __HUFFMAN_H__
#define __HUFFMAN_H__

#include <vector>
#include <algorithm>
//#include <string>
#include <iostream>
#include <iomanip>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "bitstream.h"
#include "huffnode.h"

using namespace cv;

void huffman_encode( Mat img, string outfile );
void cascade_bitstring( huffnode* root );
void get_leaves( vector<huffnode*> &leaves, huffnode* root );
void get_hist( int* hist, Mat img );
huffnode* huffman_tree( int* hist, int size );
void huff_print_bit( huffnode* root );
void huff_print( huffnode* root, int bars = 0 );
unsigned char read_next_huff( bitstream &bout, huffnode* root );

void write_hist( bitstream &bout, int* hist, int size );
void read_hist( bitstream &bout, int* hist, int size );
void write_hufftree( bitstream &bout, huffnode* root );
void read_hufftree( bitstream &bout, huffnode* &root );

#endif
