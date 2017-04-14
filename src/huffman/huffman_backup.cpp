#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <iomanip>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

class bitstream
{
    public:
    
    fstream stream;
    
    int in_bit;
    int out_bit;
    
    unsigned char buffer;
    
    bitstream()
    {
        in_bit = 7;
        out_bit = 7;
        buffer = (unsigned char)0;
    }
    
    ~bitstream(){}
    
    bool write_bit( int bit )
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
    
    bool write_byte( unsigned char c )
    {
        if( !stream )
            return false;
            
        for( int i = 7; i >= 0; i-- )
            write_bit( c>>i & 1 );
            
        return true;
    }
    
    bool read_byte( unsigned char &c )
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
    
    bool read_bit( int &bit )
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
    
    bool flush_bits()
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
};

/**************************************************/

struct huff_node
{
    unsigned char val;
    int frequency;
    string bitstring;
    huff_node* left;
    huff_node* right;
    
    huff_node( int frequency ) :
    frequency(frequency),
    bitstring(string("")),
    left(nullptr),
    right(nullptr)
    {}

    huff_node( char val, int frequency ) :
    val(val),
    frequency(frequency),
    bitstring(string("")),
    left(nullptr),
    right(nullptr)
    {}
    
    huff_node() :
    frequency(-1),
    bitstring(string("")),
    left(nullptr),
    right(nullptr)
    {}
    
    ~huff_node();
    
    void delete_children();
    bool is_leaf();
    
};

huff_node::~huff_node()
{
    delete_children();
}

void huff_node::delete_children()
    {
        if( left != nullptr )
        {
            left->delete_children();
            delete left;
            left = nullptr;
        }    
        if( right != nullptr )
        {
            right->delete_children();
            delete right;
            right = nullptr;
        }
            
        return;
    }

bool huff_node::is_leaf()
{
    return left == nullptr && right == nullptr;
}

bool node_sort_freq( huff_node* first, huff_node* second )
{
    if( first->frequency <= second->frequency )
        return false;
    return true;
}

bool node_sort_val( huff_node* first, huff_node* second )
{
    if( first->val > second->val )
        return false;
    return true;
}

bool node_sort_bit( huff_node* first, huff_node* second )
{
    if( first->bitstring.size() < second->bitstring.size() )
        return true;
    if( first->bitstring.size() > second->bitstring.size() )
        return false;
    
    return first->bitstring.compare( second->bitstring ) > 0;
}

/***********************************************************/

void huffman_encode( Mat img, string outfile );
void cascade_bitstring( huff_node* root );
void get_leaves( vector<huff_node*> &leaves, huff_node* root );
void get_hist( int* hist, Mat img );
huff_node* huffman_tree( int* hist, int size );
void huff_print_bit( huff_node* root );
void huff_print( huff_node* root, int bars = 0 );
unsigned char read_next_huff( bitstream &bout, huff_node* root );

void write_hist( bitstream &bout, int* hist, int size );
void read_hist( bitstream &bout, int* hist, int size );
void write_hufftree( bitstream &bout, huff_node* root );
void read_hufftree( bitstream &bout, huff_node* &root );

void huffman_encode( Mat img, string outfile )
{
    int hist[256] = {0}; // Image histogram
    int height = img.rows; // Height of image
    int width = img.cols; // Width of image
    huff_node* root = nullptr; // Root of Huffman tree
    string bitstring; // Huffman bitstrings for encoded values
    vector<huff_node*> leaves; // Vector of the leaves of the Huffman tree
    
    // Bitstream for the compressed output file
    bitstream bout;
    bout.stream.open( outfile, ios::out | ios::trunc | ios::binary );
    if( !bout.stream )
    {
        cout << "Error: could not open " << outfile << " for writing." << endl;
        return;
    }
    
    // Write the height and width
    bout.stream.write( (char*) &height, sizeof(int) );
    bout.stream.write( (char*) &width, sizeof(int) );
    
    // Multiply for each channel
    width *= 3;
    
    get_hist( hist, img ); // Create the histograph for the image
    root = huffman_tree( hist, 256 ); // Create the Huffman tree
    cascade_bitstring( root ); // Computes the bitstrings for the leaves
    get_leaves( leaves, root ); // Extracts a list of the leaves
    // Sorts the leaves by the symbol they represent
    sort( leaves.begin(), leaves.end(), node_sort_val );
    
    //write_hist( bout, hist, 256 ); // Store histogram to file
    write_hufftree( bout, root ); // Store Huffman tree to file
    
    // Loop through img, compressing it
    for( int i = 0; i < height; i++ )
    {
        for( int j = 0; j < width; j++ )
        {
            // Gets the Huffman bitstring for the current symbol
            bitstring = leaves[(int)img.at<uchar>( i, j )]->bitstring;
            
            // Writes each bit of the bitstring to the compressed file
            for( int k = 0; k < bitstring.size(); k++ )
                bout.write_bit( (int)bitstring[k] - (int)'0' );
        }
    }
    
    // Flush and close the stream
    bout.flush_bits(); // Ensures that all bits have been written
    bout.stream.close();
    
    // Deallocate the Huffman tree
    delete root;
    
    return;
}

void huffman_decode( Mat &img, string infile )
{
    int hist[256] = {0};
    int height;
    int width;
    huff_node* root = nullptr;
    vector<huff_node*> leaves;
    
    bitstream bin;
    bin.stream.open( infile, ios::in | ios::binary );
    if( !bin.stream )
    {
        cout << "Error: could not open " << infile << " for reading." << endl;
        return;
    }
    
    // Read the height and width
    bin.stream.read( (char*) &height, sizeof(int) );
    bin.stream.read( (char*) &width, sizeof(int) );
    
    img = Mat( height, width, CV_8UC3 );
    width *= 3;
    
    read_hufftree( bin, root );
    //read_hist( bin, hist, 256 );
    //root = huffman_tree( hist, 256 );
    cascade_bitstring( root );
    get_leaves( leaves, root );
    sort( leaves.begin(), leaves.end(), node_sort_val );
    
    for( int i = 0; i < height; i++ )
        for( int j = 0; j < width; j++ )
            img.at<uchar>( i, j ) = read_next_huff( bin, root );
            
    bin.stream.close();
    
    delete root;
    
    return;
}

unsigned char read_next_huff( bitstream &bin, huff_node* root )
{
    int bit;
    
    // Traverse huff tree
    while( !root->is_leaf() )
    {
        bin.read_bit( bit );
        if( bit )
            root = root->right;
        else
            root = root->left;
    }
    
    return root->val;
}

void write_hist( bitstream &bout, int* hist, int size )
{
    bout.stream.write( (char*)hist, sizeof(int) * size );

    return;
}

void read_hist( bitstream &bout, int* hist, int size )
{
    bout.stream.read( (char*)hist, sizeof(int) * size );

    return;
}

void write_hufftree( bitstream &bout, huff_node* root )
{
    if( root->is_leaf() )
    {
        bout.write_bit(1);
        bout.write_byte( root->val );
    }
    else
    {
        bout.write_bit(0);
        write_hufftree( bout, root->left );
        write_hufftree( bout, root->right );
    }

    return;
}

void read_hufftree( bitstream &bin, huff_node* &root )
{
    int bit;
    unsigned char c;
    
    if( !bin.read_bit( bit ) )
        return;
    
    root = new huff_node();
    if( bit == 1 )
    {
        bin.read_byte( c );
        root->val = c;
    }
    else
    {
        read_hufftree( bin, root->left );
        read_hufftree( bin, root->right );
    }

    return;
}

void cascade_bitstring( huff_node* root )
{
    if( root->left != nullptr )
    {
        root->left->bitstring = root->bitstring + "0";
        cascade_bitstring( root->left );
    }
    if( root->right != nullptr )
    {
        root->right->bitstring = root->bitstring + "1";
        cascade_bitstring( root->right );
    }

    return;
}

void get_leaves( vector<huff_node*> &leaves, huff_node* root )
{
    if( root == nullptr )
        return;
    
    if( !root->is_leaf() )
    {
        get_leaves( leaves, root->left );
        get_leaves( leaves, root->right );
    }
    else
    {
        leaves.push_back( root );
    }
    
    return;
}

void get_hist( int* hist, Mat img )
{
    int height = img.rows;
    int width = img.cols * 3;
    
    // Create the histograph for the image
    for( int i = 0; i < height; i++ )
        for( int j = 0; j < width; j++ )
            hist[(int)img.at<uchar>( i, j )]++;
            
    return;
}

huff_node* huffman_tree( int* hist, int size )
{
    vector<huff_node*> huff_list;
    huff_node *first, *second;
    huff_node* node;
            
    for( int i = 0; i < size; i++ )
    {
        if( hist[i] != 0 )
        {
            huff_list.push_back( new huff_node( (char)i, hist[i] ) );
        }
    }
    sort( huff_list.begin(), huff_list.end(), node_sort_freq );
    
    while( huff_list.size() > 1 )
    {
        first = huff_list.back();
        huff_list.pop_back();
        
        second = huff_list.back();
        huff_list.pop_back();
        
        node = new huff_node( first->frequency + second->frequency );
        node->left = first;
        node->right = second;
        
        if( huff_list.size() > 0 )
        {
            int i = huff_list.size() - 1;
            while( i > -1 && huff_list[i]->frequency < node->frequency ) i--;
            huff_list.insert( huff_list.begin() + i + 1, node );
        }
        else
            huff_list.push_back( node );
    }
    
    return huff_list[0];
}

int main()
{
    vector<int> params;
    Mat in_image = imread( "images/Hummingbird_bin.ppm" );
    
    huffman_encode( in_image, "compressed2.bin" );
    
    Mat out_image;
    huffman_decode( out_image, "compressed2.bin" );
    
    params.push_back( CV_IMWRITE_PXM_BINARY );
    params.push_back( 1 );
    
    imwrite( "output.ppm", out_image, params );
    
    return 0;
}


































