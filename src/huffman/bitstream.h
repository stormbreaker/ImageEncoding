/*
                             ***** bitstream.h *****

Contains the definition for the Bitstream class. This class is a wrapper for
a basic file stream that allows the user to read and write single bits to and
from a file stream. It accomplishes this with the use of byte buffers. This
class also keeps track of the number of bytes it has written to the file.

CSC542 Digital Image Processing - Spring 2017

Author: Scott Carda
*/

#ifndef __BITSTREAM_H__
#define __BITSTREAM_H__

/******************************************************************************/
/*                                Include Files                               */
/******************************************************************************/

#include <fstream> // For handling file streams
using namespace std;

/******************************************************************************/
/*                            Bitstream Definition                            */
/******************************************************************************/

class bitstream
{
    private:
    
    int in_bit; // Keeps track of current location in in_buffer
    int out_bit; // Keeps track of the current location in the out_buffer
    
    unsigned char in_buffer; // The bit buffer for reading in from a file
    unsigned char out_buffer; //  The bit buffer for writing out to a file
    
    public:
    
    fstream stream; // The file stream underlying the bitstream
    
	// The number of bytes written to the file with the bitstream
    int bytes_written;
    
	// Constructor and Destructor
    bitstream();
    ~bitstream();
    
	// Class Member Functions
    ostream& write( const char* s, streamsize n );
    bool write_bit( int bit );
    bool read_bit( int &bit );
    bool write_byte( unsigned char c );
    bool read_byte( unsigned char &c );
    bool flush_bits();
};

#endif
