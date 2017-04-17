#include "bitstream.h"

/***********************************/

bitstream::bitstream() :
in_bit(7), // Start with the most significant bit
out_bit(7), // Start with the most significant bit
out_buffer((unsigned char)0), // Start the buffer with all 0 bits
in_buffer((unsigned char)0), // Start the buffer with all 0 bits
bytes_written(0) // Start with 0 bytes written
{}

// Empty Destructor
bitstream::~bitstream(){}

/***********************************/

ostream& bitstream::write( const char* s, streamsize n )
{
    // Add to total number of bytes written
    bytes_written += (int)n;
    return stream.write( s, n );
}

bool bitstream::write_bit( int bit )
{
    if( !stream )
        return false;

    // If writing a 1
    if( bit )
        out_buffer |= 1 << out_bit;
    
    out_bit--; // Move to the next bit
    
    // If the buffer is full, write the buffer
    if( out_bit < 0 )
    {
        out_bit = 7; // Reset the current bit
        bytes_written += 1; // Increment number of bytes written
        // Write the buffer
        stream.write( (char *) &out_buffer, sizeof(char) );
        out_buffer = (unsigned char)0; // Reset the buffer
    }
    
    return true;
}

bool bitstream::read_bit( int &bit )
{
    if( !stream )
        return false;

    // If it is the beginning of a new word
    if( in_bit == 7 )
    {
        // Read a word from the file into the buffer
        if( !stream.read( (char *) &in_buffer, sizeof(char) ) )
            return false;
    }
    
    // Get the appropriate bit from the buffer
    bit = in_buffer>>in_bit & 1;
    
    in_bit--; // Move to the next bit
    
    // If the buffer has been fully read, reset the current bit
    if( in_bit < 0 )
        in_bit = 7;
    
    return true;
}

bool bitstream::write_byte( unsigned char c )
{
    if( !stream )
        return false;
        
    // Write each bit of the byte
    for( int i = 7; i >= 0; i-- )
        write_bit( c>>i & 1 );
        
    return true;
}

bool bitstream::read_byte( unsigned char &c )
{
    int bit;
    
    if( !stream )
        return false;
    
    c = (unsigned char)0; // Start the byte at all 0 bits
    
    // Read 8 bits into the byte
    for( int i = 7; i >=0 && read_bit( bit ); i-- )
        if( bit ) c |= bit<<i;
        
    return true;
}

bool bitstream::flush_bits()
{
    if( !stream )
        return false;

    // If the current bit is not at a byte boundary
    if( out_bit != 7 )
    {
        // Write the buffer as-is to the file
        stream.write( (char *) &out_buffer, sizeof(char) );
        bytes_written += 1; // Increment number of bytes written
        
        out_bit = 7; // Reset the current bit
        out_buffer = (unsigned char)0; // Reset the buffer
    }
        
    return true;
}

