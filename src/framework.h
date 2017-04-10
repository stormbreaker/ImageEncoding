#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <ctype.h>
#ifndef _FRAMEWORK_H_
#define _FRAMEWORK_H_

int valid_enc_args( char** argv, int argc, cv::Mat &image);
int get_encode_method( char* arg);
bool valid_method_args( char **argv, int argc, int &method);
int valid_dec_args( char**argv, int argc);
void print_errors( int error_code);
void print_help();
bool is_digit( char *arg);

#endif
