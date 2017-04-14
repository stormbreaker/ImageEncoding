#include "bitstream.h"

/***********************************/

bitstream::bitstream()
{
    in_bit = 7;
    out_bit = 7;
    buffer = (unsigned char)0;
}

bitstream::~bitstream(){}

/***********************************/

bool bitstream::write_bit( int bit )
{
    if( !stream )
        return false;

    if( bit )
        buffer |= 1 << out_bit;
    out_bit--;
    if( out_bit < 0 )
    {
        out_bit = 7;
        stream.write( (char *) &buffer, sizeof(char) );
        buffer = (unsigned char)0;
    }
    return true;
}

bool bitstream::read_bit( int &bit )
{
    if( !stream )
        return false;

    if( in_bit == 7 )
    {
        if( !stream.read( (char *) &buffer, sizeof(char) ) )
            return false;
    }
    
    bit = buffer>>in_bit & 1;
    
    in_bit--;
    if( in_bit < 0 )
        in_bit = 7;
    
    return true;
}

bool bitstream::write_byte( unsigned char c )
{
    if( !stream )
        return false;
        
    for( int i = 7; i >= 0; i-- )
        write_bit( c>>i & 1 );
        
    return true;
}

bool bitstream::read_byte( unsigned char &c )
{
    int bit;
    
    if( !stream )
        return false;
    
    c = (unsigned char)0;    
    int i = 7;
    while( i >=0 && read_bit( bit ) )
    {
        if( bit ) c |= bit<<i;
        i--;
    }
        
    return true;
}

bool bitstream::flush_bits()
{
    if( !stream )
        return false;

    if( out_bit != 7 )
    {
        out_bit = 7;
        stream.write( (char *) &buffer, sizeof(char) );
        buffer = (unsigned char)0;
    }
        
    return true;
}

