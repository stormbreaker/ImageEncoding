#include "dpcm.h"

/*
 *Global Constants for the quantizer levels
 */
const double four_levels[] = {-8, -1, 1, 8};
const double four_bounds[] = {-4, 0, 4};

const double four_levels_second[] = {-20, -2, 2, 20};
const double four_sec_bounds[] = { -4, 0, 4};

const int eight_levels[] = {-25, -15, -8, -4, 4, 8, 15, 25};
/*
 * 
 * Global glag is used to set if a first or second order
 * predictor is being used so the correct constant is used above
 *
 */
bool flag;

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
 *              p and q are used to tell what order predictor and level
 *              quantizer were used
 *              
 ************************************************************************/

void write_header( ofstream &fout, const int &height, const int &width, const char* ext, const char *p, const char *q)
{
    fout.write((char*)&height, 4);
    fout.write((char*)&width, 4);
    fout.write(ext,3);
    fout.write(p,1);
    fout.write(q,1);
}
/************************************************************************
 *  Function: read_char
 *  Author: Christopher Smith
 *  Description: Reads a single character from the binary file
 *  Parameters: fin file stream to read from
 *              edot the encoded character to be read in
 *              
 ************************************************************************/
void read_char( ifstream &fin, unsigned char &edot)
{
    fin.read((char*)&edot,1);
}
/************************************************************************
 *  Function: read_header
 *  Author: Christopher Smith
 *  Description: Reads the header information from the binary file so that the
 *  image can be allocated properly and the right predictor is used, and the
 *  correct codewords are decoded properly based on quantizer used 
 *  Parameters: fin file stream to read from
 *              
 *              height, width, and ext are original file characteristics to be
 *              read in
 *
 *              p and q are then read in to tell what predictor to use and which
 *              quantizer was used on the image values.
 *              
 ************************************************************************/
void read_header(ifstream &fin, int &height, int &width, char* ext, char *p, char *q)
{
    fin.seekg(0, fin.beg);
    fin.read((char*)&height, 4);
    fin.read((char*)&width, 4);
    fin.read(ext, 3);
    fin.read(p,1); 
    fin.read(q,1);   
}
/************************************************************************
 *  Function: first_predicter
 *  Author: Christopher Smith
 *  Description: First order predicter that waits the previous predicter value
 *  by 1 based on the previous pixel
 *  Parameters: fdot previous pixel encoded value after quantization
 *              
 ************************************************************************/
double first_predicter(const double &fdot )
{
    return 1 * fdot;
}
/************************************************************************
 *  Function: first_predicter
 *  Author: Christopher Smith
 *  Description: second order predicter that waits the previous predicter value
 *  by .4 based on the previous pixel and .5 for the pixel above
 *  Parameters: fdot previous pixel encoded value after quantization
 *              
 ************************************************************************/
double second_predicter( const double &xprev, const double &yprev)
{
    return .4 * xprev + .5 * yprev;
}
/************************************************************************
 *  Function: four_level
 *  Author: Christopher Smith
 *  Description: four level quantizer for the first order predictor
 *  Parameters: en is the value of the current pixel in image minus the previous
 *  pixel predicter result.
 *              
 ************************************************************************/

double four_level( const double &en )
{
    if( en < four_bounds[0] )
        return four_levels[0];
    else if( en <= four_bounds[1] )
        return four_levels[1];
    else if( en <= four_bounds[2] )
        return four_levels[2];
    return four_levels[3];
}
/************************************************************************
 *  Function:four_level_second 
 *  Author: Christopher Smith
 *  Description: Four level quantizer for the second order predictor
 *  Parameters: en is the value of the current pixel minus the result of the
 *  predictor on the previous pixel and pixel above it.
 *              
 ************************************************************************/
double four_level_second( const double &en )
{
    if( en < four_sec_bounds[0] )
        return four_levels_second[0];
    else if( en <= four_sec_bounds[1] )
        return four_levels_second[1];
    else if( en <= four_sec_bounds[2] )
        return four_levels_second[2];
    return four_levels_second[3];
}
/************************************************************************
 *  Function:eight_level 
 *  Author: Christopher Smith
 *  Description: Eight level quantizer for both predictors. This quantization
 *  gives relatively good results for the first and second order predictors
 *  Parameters: en is the value to be quantized based on the current pixel minus
 *  the result of the predictor used.
 *              
 ************************************************************************/
double eight_level( const double &en )
{
    if( en < -20 )
        return eight_levels[0];
    else if( en < -12)
        return eight_levels[1];
    else if( en < -6)
        return eight_levels[2];
    else if( en < 0 )
        return eight_levels[3];
    else if( en < 6 )
        return eight_levels[4];
    else if( en < 12 )
        return eight_levels[5];
    else if( en < 20 )
        return eight_levels[6];
    return eight_levels[7];
}
/************************************************************************
 *  Function:store_bits 
 *  Author: Christopher Smith
 *  Description:This function will call the store_two and store_four functions
 *  based on the quantizer used for encoding.  
 *  Parameters: code is the byte that will store the code word that corresponds
 *  to edot
 *
 *              edot is the quantization value that will be encoded into code
 *
 *              levels in the number of levels in the quantizer. 
 *              
 ************************************************************************/
void store_bits( unsigned char &code, const double &edot, const char &levels)
{
    if( levels == 4 )
        store_two(code, edot);
    else
        store_four(code, edot);
}
/************************************************************************
 *  Function:store_two 
 *  Author: Christopher Smith
 *  Description:This function puts two bits in the code and shifts it down 2
 *  spots to prepare it for the next code to be stored in it.
 *  Parameters: code is the byte that will store the code word that corresponds
 *  to edot
 *              edot is the quantization value that will be encoded into code
 *
 ************************************************************************/
void store_two( unsigned char &code, const double &edot)
{
    code <<= 2;
    //if the first order predicter was used
    if(flag)
    {
        if( edot == four_levels[0] )
            code |= 0;
        else if( edot == four_levels[1] )
            code |= 1;
        else if( edot == four_levels[2] )
            code |= 2;
        else
            code |= 3;
    }
    else //second order predicter was used
    {
        if( edot == four_levels_second[0] )
            code |= 0;
        else if( edot == four_levels_second[1] )
            code |= 1;
        else if( edot == four_levels_second[2] )
            code |= 2;
        else
            code |= 3;
    }

}
/************************************************************************
 *  Function:store_four 
 *  Author: Christopher Smith
 *  Description:This function puts three bits in the code and shifts it down 3
 *  spots to prepare it for the next code to be stored in it.
 *  Parameters: code is the byte that will store the code word that corresponds
 *  to edot
 *              edot is the quantization value that will be encoded into code
 *
 ************************************************************************/
void store_four( unsigned char &code, const double &edot)
{
    code <<= 3;
    if( edot == eight_levels[0] )
        code |= 0;
    else if( edot == eight_levels[1] )
        code |= 1;
    else if( edot == eight_levels[2] )
        code |= 2;
    else if( edot == eight_levels[3] )
        code |= 3;
    else if( edot == eight_levels[4] )
        code |= 4;
    else if( edot == eight_levels[5] )
        code |= 5;
    else if( edot == eight_levels[6] )
        code |= 6;
    else
        code |= 7;

}
/************************************************************************
 *  Function:dpcm 
 *  Author: Christopher Smith
 *  Description: Creates the binary file for writing and then calls the
 *  dpcm_second or first functions based on command line arguments of p and q
 *  Parameters: f is the image to be encoded
 *              height and widht are image characteristics to loop over
 *              p is the order predictor to be used
 *              q is the quantizer to be used
 *              fname is the file name to use as output 
 ************************************************************************/
void dpcm( Mat f, const int &height, const int &width, const char &p, const char &q, const char *fname)
{
    ofstream fout;
    char ext[3] = {'P', 'N', 'G'};
    fout.open( fname, ios::out | ios::binary );
    write_header( fout, height, width, ext, &p, &q);
    if( p == 2)
    {
        if ( q == 4 )
            dpcm_second(f, height, width, fout,q, &four_level_second );
        else
            dpcm_second(f, height, width, fout,q, &eight_level);
    }
    else
    {
        if( q == 4 )
            dpcm_first( f, height, width, fout,q, &four_level );
        else
            dpcm_first( f, height, width, fout,q, &eight_level);
    }
}
/************************************************************************
 *  Function:dpcm_second  
 *  Author: Christopher Smith
 *  Description: Performs the differential pulse code modulation algorithm for a
 *  second order predictor 
 *  Parameters: f is the image to be encoded
 *              height and widht are image characteristics to loop over
 *              fout is the stream to write the encoding to
 *              levels is the number of levels in the quantizer
 *              q is the function pointer the corresponding quantizer
 ************************************************************************/
void dpcm_second( Mat f, const int &height, const int &width, ofstream &fout, const char& levels, double (*q)(const double&) )
{
    flag = false;
    Mat tmp_image = Mat(f.rows,f.cols, CV_8UC3, cvScalar(0)); 
    unsigned char channels = 3;
    unsigned char code;
    unsigned char xprev;
    unsigned char yprev;
    Vec3b ytmp;
    double fhat;
    double en;
    double edot;
    char count = 0;
    int r, c;
    Vec3b xtmp;
    Vec3b tmp;
    for( unsigned char chan = 0; chan < channels; chan++)
    {
        for( r = 0; r < height; r++ )
        {
            c = 0;
            tmp = f.at<Vec3b>(r,c);
            tmp_image.at<Vec3b>(r,c) = tmp;
            write_char(fout, tmp[chan] );
            fhat = tmp[chan];
            code = 0; 
            count = 0;
            for( c = 1; c < width; c++)
            {
                tmp = f.at<Vec3b>(r,c);
                tmp_image.at<Vec3b>(r,c) = tmp;
                if( r == 0)
                {
                    //write the initial row of the image since there are no
                    //pixels above them
                    write_char(fout,tmp[chan]);
                }
                else
                {
                    xtmp = tmp_image.at<Vec3b>(r-1, c);
                    ytmp = tmp_image.at<Vec3b>(r, c-1);
                    xprev = xtmp[chan];
                    yprev = ytmp[chan];
                    en = tmp[chan] - fhat;
                    edot = (*q)( en );
                    tmp_image.at<Vec3b>(r,c)[chan] = edot+fhat;
                    store_bits(code, edot, levels);
                    //if a four level quantizer is being used
                    if( levels == 4 )
                    {
                        count+=2;
                        if( count == 8)
                        {
                            write_char( fout, code);
                            count = 0;
                            code = 0;
                        }
                    }
                    else//if a 8 level quantizer is being used
                    {
                        count += 3;
                        if( count == 6)
                        {
                            write_char( fout, code );
                            count = 0;
                            code = 0;
                        }
                    }
                    fhat = second_predicter( xprev, yprev);
                }
            }
            //if the number of bits don't fill a byte write anyway and go to
            //next row in the image
            if( count != 0)
                write_char( fout, code);
        }
    }
}
/************************************************************************
 *  Function:dpcm_first
 *  Author: Christopher Smith
 *  Description: Performs the differential pulse code modulation algorithm for a
 *  first order predictor 
 *  Parameters: f is the image to be encoded
 *              height and widht are image characteristics to loop over
 *              fout is the stream to write the encoding to
 *              levels is the number of levels in the quantizer
 *              q is the function pointer the corresponding quantizer
 ************************************************************************/
void dpcm_first( Mat f, const int &height, const int &width, ofstream &fout, const char& levels, double (*q)(const double&) )
{
    flag = true;
    unsigned char channels = 3;
    unsigned char code;
    Vec3b ytmp;
    double fn;
    double fhat;
    double fdot;
    double en;
    double edot;
    char count = 0;
    int r, c;
    Vec3b xtmp;
    Vec3b tmp;
    if( f.type() == CV_8UC1 )
        channels = 1;
    for( unsigned char chan = 0; chan < channels; chan++)
    {
        for( r = 0; r < height; r++ )
        {
            c = 0;
            tmp = f.at<Vec3b>(r,c);
            fn = tmp[chan];
            write_char(fout, fn );
            fhat = fn;
            fdot = fhat;
            code = 0; 
            count = 0;
            for( c = 1; c < width; c++)
            {
                tmp = f.at<Vec3b>(r,c);
                fn = tmp[chan];
                en =  fn-fhat;
                edot = (*q)( en );
                fdot = edot + fhat;
                store_bits( code, edot, levels);
                if( levels == 4 )
                {
                    count+=2;
                    if( count == 8)
                    {

                        write_char( fout, code);
                        count = 0;
                        code = 0;
                    }
                }
                else
                {
                    count += 3;
                    if( count == 6)
                    {
                        write_char( fout, code );
                        count = 0;
                        code = 0;
                    }
                }
                fhat = first_predicter( fdot);
            }
            if( count != 0)
                write_char( fout, code);
        }
    }
}
/************************************************************************
 *  Function:dpcm_first
 *  Author: Christopher Smith
 *  Description: Performs the differential pulse code modulation algorithm for a
 *  first order predictor 
 *  Parameters: f is the image to be encoded
 *              height and widht are image characteristics to loop over
 *              fout is the stream to write the encoding to
 *              levels is the number of levels in the quantizer
 *              q is the function pointer the corresponding quantizer
 ************************************************************************/
Mat dpcm_decoder( const char *fname)
{
    ifstream fin;
    char ext[3] = {'P', 'N', 'G'};
    int height, width;
    char p, q;
    Mat f;
    fin.open( fname, ios::in | ios::binary );
    read_header( fin, height, width, ext, &p, &q);
    f = Mat(height, width, CV_8UC3, cvScalar(0));
    if( p == 2)
        dpcm_second_dec(f, height, width, fin,q);
    else
        dpcm_first_dec( f, height, width, fin,q);
    //namedWindow( "Image", CV_WINDOW_AUTOSIZE );
    //imshow( "Image", f );
    //waitKey(0);
    string name(fname);
    name.pop_back();
    name.pop_back();
    name.pop_back();
    name.pop_back();
    name += "_decoded.";
    name += "png";
    imwrite(name, f);
    return f;
}
/************************************************************************
 *  Function:dpcm_first_dec
 *  Author: Christopher Smith
 *  Description: Performs the differential pulse code modulation decoding algorithm for a
 *  first order predictor 
 *  Parameters: f is the image to be encoded
 *              height and widht are image characteristics to loop over
 *              fin is the stream to read the encoded image from and decode
 *              q is the number of levels used in the quantizer
 ************************************************************************/
void dpcm_first_dec( Mat f, const int &height, const int &width, ifstream &fin, const char &q)
{
    flag = true;
    unsigned char channels = 3;
    unsigned char bit = 0;
    unsigned char shift = 0;
    Vec3b tmp;
    Vec3b xtmp;
    char count;
    unsigned char pix = 0;
    unsigned char prev;
    unsigned char code;
    double delta;
    int r, c;
    if (q == 4)
    {
        bit = 3;
        shift = 2;
    }
    else
    {
        bit   = 7;
        shift = 3;
    }
   for( unsigned char chan = 0; chan < channels; chan++ )
   {
        for( r = 0; r < height; r++ )
        {
            tmp = f.at<Vec3b>(r, 0);
            read_char(fin,pix);
            tmp[chan] = pix;
            prev = pix;
            f.at<Vec3b>(r,0) = tmp;
            count = 0;
            read_char(fin,code);
            for( c = 1; c < width; c++ )
            {
                delta = 0;
                tmp  = f.at<Vec3b>(r,c);
                delta = get_delta(bit, shift, code, count);
                
                if( prev+delta > 255.0)
                    pix = 255;
                else if( prev+delta < 0.0 )
                    pix = 0;
                else
                    pix = round(prev + delta);  
                
                
                
                tmp[chan] = pix;
                f.at<Vec3b>(r,c) = tmp;
                count++;
                if( shift == 2 && count == 4 && c+1 != width)
                {
                    read_char(fin, code);
                    count = 0;
                }
                else if( shift == 3 &&  count == 2 && c+1 != width)
                {
                    read_char(fin, code);
                    count = 0;
                }
                prev = pix;
            }
        }
   }

}
/************************************************************************
 *  Function:dpcm_second_dec
 *  Author: Christopher Smith
 *  Description: Performs the differential pulse code modulation decoding algorithm for a
 *  second order predictor 
 *  Parameters: f is the image to be encoded
 *              height and widht are image characteristics to loop over
 *              fin is the stream to read the encoded image from and decode
 *              q is the number of levels used in the quantizer
 ************************************************************************/
void dpcm_second_dec( Mat f, const int &height, const int &width, ifstream &fin, const char &q)
{
    flag = false;
    unsigned char channels = 3;
    unsigned char bit = 0;
    unsigned char shift = 0;
    Vec3b tmp;
    Vec3b ytmp;
    Vec3b xtmp;
    char count;
    unsigned char pix = 0;
    unsigned char code;
    double tmp_pix;
    char delta;
    int r, c;
    //A four level quantizer was used to encode the image
    if (q == 4)
    {
        bit = 3;
        shift = 2;
    }
    else//A 8 level quantizer was used to encode the image
    {
        bit = 7;
        shift =3;
    }
   for( unsigned char chan = 0; chan < channels; chan++ )
   {
        for( r = 0; r < height; r++ )
        {
            tmp = f.at<Vec3b>(r, 0);
            read_char(fin,pix);
            tmp[chan] = pix;
            f.at<Vec3b>(r,0) = tmp;
            count = 0;
            read_char(fin,code);
            for( c = 1; c < width; c++ )
            {
                if( r == 0 )
                {
                    //if the r is 0 just keep reading row pixel values in until
                    //next row
                    if( c != 1)
                        read_char(fin,code);
                    tmp = f.at<Vec3b>(r, c);
                    tmp[chan] = code;
                    f.at<Vec3b>(r,c) = tmp;
                }
                else
                {
                    tmp  = f.at<Vec3b>(r,c);
                    xtmp = f.at<Vec3b>(r,c-1);
                    ytmp = f.at<Vec3b>(r-1,c);
                    
                    delta = get_delta(bit, shift, code, count);
                    
                    tmp_pix = second_predicter(xtmp[chan],ytmp[chan]) + delta;

                    if( tmp_pix > 255.0 )
                        pix = 255;
                    else if( tmp_pix < 0.0 )
                        pix = 0;
                    else
                        pix = round(tmp_pix);

                    tmp[chan] = pix;
                    f.at<Vec3b>(r,c) = tmp;
                    count++;
                    if( shift == 2 && count == 4 && c+1 != width)
                    {
                        read_char(fin, code);
                        count = 0;
                    }
                    else if( shift == 3 &&  count == 2 && c+1 != width)
                    {
                        read_char(fin, code);
                        count = 0;
                    }
                }
            }
        }
   }
}

/************************************************************************
 *  Function:get_delta
 *  Author: Christopher Smith
 *  Description: calls the corresponding function to decode the encoded data
 *  based on the level quantizer used to do the encoding
 *  Parameters: bit mask to do a bitwise and with the encoded data in pix
 *              shift tells the function what level quantizer was used
 *              pix is the encoded data that needs to be extracted
 *              count is number of reads that have been done on pix so the mask
 *              can be shifted appropriately
 ************************************************************************/

double get_delta( char bit, char shift, char pix, char count)
{
    if( shift == 2)//if a 4 level qauntizer was used
        return decode_4_level(bit, pix, count);
    return decode_8_level(bit, pix, count);
}
/************************************************************************
 *  Function:decode_4_level
 *  Author: Christopher Smith
 *  Description: This function returns the decoded 2 bits that were encoded
 *  after quantization
 *  Parameters: bit mask to do a bitwise and with the encoded data in pix
 *              pix is the encoded data that needs to be extracted
 *              count is number of reads that have been done on pix so the mask
 *              can be shifted appropriately
 ************************************************************************/
double decode_4_level( char bit, char pix, char count)
{
    char tmp = pix & (bit << (count * 2));
    tmp = tmp >> (count*2);
    if(!flag)//if a second order encoder was used
        return four_levels_second[(int)tmp];
    return four_levels[(int)tmp];
}
/************************************************************************
 *  Function:decode_8_level
 *  Author: Christopher Smith
 *  Description: This function returns the decoded 3 bits that were encoded
 *  after quantization
 *  Parameters: bit mask to do a bitwise and with the encoded data in pix
 *              pix is the encoded data that needs to be extracted
 *              count is number of reads that have been done on pix so the mask
 *              can be shifted appropriately
 ************************************************************************/
double decode_8_level( char bit, char pix, char count)
{
    char tmp = pix & (bit << (count * 3));
    tmp = tmp >> (count*3);
    return eight_levels[(int)tmp];
}

