#include "delta_mod.h"
#include <math.h>

/************************************************************************
 *  Function:store_bit 
 *  Author: Christopher Smith
 *  Description: Stores the bit based on the sign of edot into the code that
 *  will be encoded in the compressed image.
 *  Parameters: code is the the byte that will be written to the image and
 *  contains the information of the sign of the delta value at that point in the
 *  image.
 *              edot the value to base the bit to be stored
 *              0 if negative otherwise a 1
 *              
 ************************************************************************/
void store_bit( char &code, const double &edot)
{
    code <<= 1;
    code =  edot < 0 ? code |  0 : code |  1;
}
/************************************************************************
 *  Function: write_char
 *  Author: Christopher Smith
 *  Description: Writes a single character to the binary file
 *  Parameters: fout file stream to write to
 *              edot the encoded character to be written
 *              
 ************************************************************************/
void write_char( ofstream &fout, const char &edot)
{
    fout.write((char*)&edot,1);
}
/************************************************************************
 *  Function: write_header
 *  Author: Christopher Smith
 *  Description: Writes the image binary file header so the decoder
 *               can read in what style quantizer/predictor were used
 *  Parameters: fout file stream to write to
 *              
 *              height, width, and ext are characteristics of the original image
 *              before it is compressed
 *
 *              delta is the delta value used to quantize the image              
 ************************************************************************/
void write_header( ofstream &fout, const int &height, const int &width, const char* ext, const float &delta)
{
    fout.write((char*)&height, 4);
    fout.write((char*)&width, 4);
    fout.write(ext,3);
    fout.write((char*)&delta,sizeof(float));
}
/************************************************************************
 *  Function: read_char 
 *  Author: Christopher Smith
 *  Description: Reads a single character from the binary file
 *  Parameters: fin file stream to read from
 *              edot the encoded character to be read
 *              
 ************************************************************************/
void read_char( ifstream &fin, unsigned char &edot)
{
    fin.read((char*)&edot,1);
}
/************************************************************************
 *  Function: read_header
 *  Author: Christopher Smith
 *  Description: Reads the image binary file header so the decoder
 *               can read in what delta value was used to quantize the values
 *  Parameters: fout file stream to write to
 *              
 *              height, width, and ext are characteristics of the original image
 *              before it is compressed
 *
 *              delta is the delta value used to quantize the image              
 ************************************************************************/
void read_header(ifstream &fin, int &height, int &width, char* ext, float &delta)
{
    fin.seekg(0, fin.beg);
    fin.read((char*)&height, 4);
    fin.read((char*)&width, 4);
    fin.read(ext, 3);
    fin.read((char*)&delta, sizeof(float));    
}
/************************************************************************
 *  Function: delta_modulation
 *  Author: Christopher Smith
 *  Description: Performs a special case of the differential pulse code
 *  modulation algorithm where the quantizer is a two step function based on the
 *  delta value. It quantizes it to positive or negative delta based on the
 *  current pixel in the image minus the prediction error of the previous pixel. 
 *  It also uses a first order predictor based only on the previous pixel.
 *  Parameters: f is the image to be encoded
 *              height and width are image characteristics to loop over
 *              delta is the quantizer value to use
 *              fname is the file name write the compressed image to
 ************************************************************************/
void delta_modulation(Mat f, const int &height, const int &width, const float &delta, const char * fname)
{
    unsigned char channels = 3;
    unsigned char delta_sign;
    ofstream fout;
    double fn;
    double fhat;
    double fdot;
    double en;
    double edot;
    const double alpha =  1;
    char count = 0;
    char code;
    int r, c;
    char ext[3] = {'P', 'N', 'G'};
    Vec3b tmp;
    if( f.type() == CV_8UC1 )
    {
        channels = 1;
    }
    fout.open(fname, ios::out | ios::binary);
    write_header( fout, height, width, ext, delta );

    for(unsigned char chan = 0; chan < channels; chan++)
    {
        for( r = 0; r < height; r++ )
        {
            c = 0;
            tmp = f.at<Vec3b>(r, c);
            write_char(fout, tmp[chan]);
            fhat = tmp[chan];
            fdot = fhat;
            fhat = dm_predictor( alpha, fhat);
            code = 0;
            count = 0;
            for( c = 1; c < width; c++ )
            {
                tmp = f.at<Vec3b>(r, c);
                fn = tmp[chan];
                fhat = dm_predictor(alpha, fdot);
                en = fn - fhat;
                edot = dm_quantizer(en, delta);
                fdot = edot + fhat;
                store_bit( code, edot);
                count++;
                if( count == 8 )
                {
                    write_char( fout, code);
                    count = 0;
                }
            }
            if( count != 0)
            {
                write_char( fout, code);
            }
        }
    }
}
/************************************************************************
 *  Function: dm_predictor
 *  Author: Christopher Smith
 *  Description: First order predicter for the delta modulator
 *  Parameters: fout file stream to write to
 *              edot the encoded character to be written
 *              
 ************************************************************************/

double inline dm_predictor(const double &alpha, const double &fdot)
{
    return alpha * fdot;    
}
/************************************************************************
 *  Function: dm_quantizer 
 *  Author: Christopher Smith
 *  Description: Delta Modulation quantizer
 *  Parameters: en is the sign to check to return either positive or negative
 *  delta if it is greater than zero or less than respecitively
 *              
 *              delta is the value to be returned based on the sign of en
 *              
 ************************************************************************/

double inline dm_quantizer( const double &en, const double &delta )
{
    if( en > 0)
        return delta;
    return -delta;
}
/************************************************************************
 *  Function: delta_decoder
 *  Author: Christopher Smith
 *  Description: Decodes a binary file and uses the delta read in and the bits as
 *  codes for either positive or negative delta.
 *  Parameters: fname is the file name to read in to start decoding.
 ************************************************************************/

void delta_decoder(const char * fname)
{
    Mat f;
    unsigned char channels = 3;
    unsigned char delta_sign;
    ifstream fin;
    float delta;
    unsigned char encode;
    unsigned char prev;
    unsigned char pix;
    char bit;
    const char alpha =  1;
    int r, c;
    int height, width;
    char ext[3];
    Vec3b tmp;
    fin.open(fname, ios::in | ios::binary);
    read_header( fin, height, width, ext, delta );
    f = Mat(height, width, CV_8UC3, cvScalar(0));
    for(unsigned char chan = 0; chan < channels; chan++)
    {
        for( r = 0; r < height; r++ )
        {
            tmp = f.at<Vec3b>(r, 0);
            read_char(fin, pix);
            tmp[chan] = pix;
            prev = pix;
            bit = 1;
            f.at<Vec3b>(r,0) = tmp;
            read_char( fin, encode);
            for( c = 1; c < width; c++ )
            {
                tmp = f.at<Vec3b>(r, c);
                pix = ( encode & bit ) == 0 ? round(prev-delta) : round(prev + delta);
                tmp[chan] = pix;
                f.at<Vec3b>(r,c) = tmp;
                prev = ( encode & bit ) == 0 ? round(prev-delta) : round( prev + delta);
                bit <<= 1;
                
                if( bit == 0 )
                {
                    bit = 1;
                    read_char( fin, encode);
                }
            }
        }
    }
    string name(fname);
    name.pop_back();
    name.pop_back();
    name.pop_back();
    name.pop_back();
    name += "_decoded.";
    name += "png";
    imwrite(name, f);
}



