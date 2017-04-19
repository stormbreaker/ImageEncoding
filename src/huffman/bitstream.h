#ifndef __BITSTREAM_H__
#define __BITSTREAM_H__

#include <fstream>
using namespace std;

class bitstream
{
    private:
    
    int in_bit;
    int out_bit;
    
    unsigned char out_buffer;
    unsigned char in_buffer;
    
    public:
    
    fstream stream;
    
    int bytes_written;
    
    bitstream();
    ~bitstream();
    
    ostream& write( const char* s, streamsize n );
    
    bool write_bit( int bit );
    bool read_bit( int &bit );
    bool write_byte( unsigned char c );
    bool read_byte( unsigned char &c );
    bool flush_bits();
};

#endif
