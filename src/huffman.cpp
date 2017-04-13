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

struct bitstream
{
    fstream stream;
    
    int in_bit;
    int out_bit;
    
    unsigned char buffer;
    
    bitstream()
    {
        in_bit = 0;
        out_bit = 0;
        buffer = (unsigned char)0;
    }
    
    ~bitstream(){}
    
    bool bad_write_bit( int bit )
    {
        if( !stream )
            return false;
    
        /*if( bit )
            buffer |= (1<<out_bit);
        out_bit++;
        if( out_bit >= 8 )
        {
            out_bit = 0;
            stream.write( (char *) &buffer, sizeof(char) );
            buffer = (unsigned char)0;
        }*/
        //stream.write( (char *) &bit, sizeof(int) );
        stream << bit << endl;
        return true;
    }
    
    bool write_bit( int bit )
    {
        if( !stream )
            return false;
    
        if( bit )
            buffer |= (1<<out_bit);
        out_bit++;
        if( out_bit >= 8 )
        {
            out_bit = 0;
            stream.write( (char *) &buffer, sizeof(char) );
            buffer = (unsigned char)0;
        }
        /*stream.write( (char *) &bit, sizeof(int) );
        stream << endl;*/
        return true;
    }
    
    bool write_byte( unsigned char c )
    {
        if( !stream )
            return false;
            
        for( int i = 7; i >= 0; i-- )
            write_bit( c>>i & 1 );
        //stream << endl;
        //stream << (int)c << endl;
            
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
        //stream.read( (char *) &c, sizeof(char) ); // skip endl
        //stream.read( (char *) &c, sizeof(char) );
        //c = (unsigned char)bit;
            
        return true;
    }
    
    bool read_bit( int &bit )
    {
        if( !stream )
            return false;
    
        char local_buff;
        if( in_bit == 0 )
        {
            if( !stream.read( (char *) &buffer, sizeof(char) ) )
                return false;
            //buffer = (unsigned char)local_buff;
        }
        
        bit = buffer >> (7-in_bit) & 1;
        
        in_bit++;
        in_bit %= 8;
        
        //stream >> bit;
        
        return true;
    }
    
    bool flush_bits()
    {
        if( !stream )
            return false;
    
        while( out_bit != 0 )
            write_bit(0);
            
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

int toInt(string bitString, int sLength )
{
    int tempInt;
    int num=0;
    for(int i=0; i<sLength; i++)
    {
        tempInt=bitString[i]-'0';
        num |= (1 << (sLength-1-i)) * tempInt;
    }

    return num;
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
string get_bitstring( char symbol, huff_node* root, int* hist );
void get_hist( int* hist, Mat img );
huff_node* huffman_tree( int* hist, int size );
void huff_print_bit( huff_node* root );
void huff_print( huff_node* root, int bars = 0 );
void write_hufftree( bitstream &bout, huff_node* root );
void read_hufftree( bitstream &bout, huff_node* &root );
unsigned char read_next_huff( bitstream &bout, huff_node* root );

void write_hist( bitstream &bout, int* hist, int size );
void read_hist( bitstream &bout, int* hist, int size );

void huffman_test()
{
    const unsigned int num = 6;
    int hist[num] = { 25, 25, 20, 15, 10, 5 };
    huff_node* root = nullptr;
    vector<huff_node*> leaves;
    
    root = huffman_tree( hist, num );
    cascade_bitstring( root );
    get_leaves( leaves, root );
    
    vector<huff_node*> leaves2 = leaves;
    vector<huff_node*> leaves3 = leaves;
    
    sort( leaves.begin(), leaves.end(), node_sort_freq );
    sort( leaves2.begin(), leaves2.end(), node_sort_val );
    sort( leaves3.begin(), leaves3.end(), node_sort_bit );
    
    for( unsigned int i = 0; i < leaves.size(); i++ )
    {
        cout << "[ " << setw(5) << leaves[i]->frequency << " : " << setw(3) << (int)leaves[i]->val << " : " << setw(13) << leaves[i]->bitstring << " ]";
        cout << setw( 6 ) << " ";
        cout << "[ " << setw(5) << leaves2[i]->frequency << " : " << setw(3) << (int)leaves2[i]->val << " : " << setw(13) << leaves2[i]->bitstring << " ]";
        cout << setw( 6 ) << " ";
        cout << "[ " << setw(5) << leaves3[i]->frequency << " : " << setw(3) << (int)leaves3[i]->val << " : " << setw(13) << leaves3[i]->bitstring << " ]";
        cout << endl;
    }
    cout << endl;
    huff_print( root );
    //huff_print_bit( root );
    return;
}

void huffman_encode( Mat img, string outfile )
{
    int hist[256] = {0};
    int height = img.rows;
    int width = img.cols;
    huff_node* root = nullptr;
    string bitstring = "";
    vector<huff_node*> leaves;
    
    bitstream bout;
    bout.stream.open( outfile, ofstream::out | ofstream::trunc | ofstream::binary );
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
    
    // Create the histograph for the image
    get_hist( hist, img );
    root = huffman_tree( hist, 256 );
    
    write_hist( bout, hist, 256 );
    
    cascade_bitstring( root );
    get_leaves( leaves, root );
    
    sort( leaves.begin(), leaves.end(), node_sort_val );
    
    int col = 0;
    for( int i = 0; i < height; i++ )
    {
        for( int j = 0; j < width; j++ )
        {
            bitstring = leaves[(int)img.at<uchar>( i, j )]->bitstring;
            
            for( int k = 0; k < bitstring.size(); k++ )
                bout.write_bit( (int)bitstring[k] - (int)'0' );
        
            /*bitstring += leaves[(int)img.at<uchar>( i, j )]->bitstring;

            while( bitstring.size() >= 8 )
            {
                fout << toInt( bitstring, 8 ) << " ";
                bitstring.erase( 0, 8 );
                col++;
                if( col >= width )
                {
                    col = 0;
                    fout << endl;
                }
            }*/
        }
    }
    
    bout.flush_bits();
    
    // pad to a word boundary with zeros
    /*if( bitstring.length() % 8 )
    {
        for( int i = 8 - bitstring.length() % 8; i > 0; i-- )
            bitstring += "0";
        fout << toInt( bitstring, 8 );
    }*/
    
    bout.stream.close();
    
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
    bin.stream.open( infile, fstream::in | fstream::binary );
    if( !bin.stream )
    {
        cout << "Error: could not open " << infile << " for reading." << endl;
        return;
    }
    
    // Read the height and width
    bin.stream.write( (char*) &height, sizeof(int) );
    bin.stream.write( (char*) &width, sizeof(int) );
    
    img = Mat( height, width, CV_8UC3 );
    width *= 3;
    
    read_hist( bin, hist, 256 );
    root = huffman_tree( hist, 256 );
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

void huffman_encode_test( Mat img, string outfile )
{
    int hist[256] = {0};
    //int hist[6] = { 25, 25, 20, 15, 10, 5 };
    huff_node* root = nullptr;
    vector<huff_node*> leaves;
    
    bitstream bout;
    bout.stream.open( outfile, ios::out | ios::trunc | ios::binary );
    if( !bout.stream )
    {
        cout << "Error: could not open " << outfile << " for writing." << endl;
        return;
    }
    
    // Create the histograph for the image
    get_hist( hist, img );
    root = huffman_tree( hist, 256 );
    //root = huffman_tree( hist, 6 );
    
    //write_hufftree( bout, root );
    //bout.flush_bits();
    write_hist( bout, hist, 256 );
    bout.stream << endl;
    
    bout.stream.close();

    fstream fout;
    fout.open( "precompressed.txt", ofstream::out | ofstream::trunc );
    if( !fout )
    {
        cout << "Error: could not open precompressed.txt for writing." << endl;
        delete root;
        return;
    }

    cascade_bitstring( root );
    get_leaves( leaves, root );
    sort( leaves.begin(), leaves.end(), node_sort_val );
    
    for( unsigned int i = 0; i < leaves.size(); i++ )
    {
        fout << "[ " << setw(3) << (int)leaves[i]->val << " : " << setw(13) << leaves[i]->bitstring << " ]";
        fout << endl;
    }
    
    fout.close();
    
    delete root;
    
    return;
}

void huffman_decode_test( string infile )
{
    huff_node* root = nullptr;
    vector<huff_node*> leaves;
    int hist[256] = {0};
    
    bitstream bout;
    bout.stream.open( infile, fstream::in | fstream::binary );
    if( !bout.stream )
    {
        cout << "Error: could not open " << infile << " for reading." << endl;
        return;
    }
    
    /*char local;
    local = bout.stream.get();
    if( local == std::char_traits<char>::eof() )
        cout << "Other" << endl;
    cout << local << endl;
    
    bout.stream.close();
    return;*/
    
    //read_hufftree( bout, root );
    
    read_hist( bout, hist, 256 );
    bout.stream.close();
    
    fstream fout;
    fout.open( "postcompressed.txt", ofstream::out | ofstream::trunc );
    if( !fout )
    {
        cout << "Error: could not open postcompressed.txt for writing." << endl;
        delete root;
        return;
    }
    
    root = huffman_tree( hist, 256 );
    cascade_bitstring( root );
    get_leaves( leaves, root );
    sort( leaves.begin(), leaves.end(), node_sort_val );
    
    for( unsigned int i = 0; i < leaves.size(); i++ )
    {
        fout << "[ " << setw(3) << (int)leaves[i]->val << " : " << setw(13) << leaves[i]->bitstring << " ]";
        fout << endl;
    }
    
    fout.close();
    
    delete root;
    
    return;
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

void read_hufftree( bitstream &bout, huff_node* &root )
{
    int bit;
    unsigned char c;
    
    if( !bout.read_bit( bit ) )
        return;
    
    root = new huff_node();
    if( bit == 1 )
    {
        bout.read_byte( c );
        root->val = c;
    }
    else
    {
        read_hufftree( bout, root->left );
        read_hufftree( bout, root->right );
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

/*string get_bitstring( char symbol, huff_node* root, int* hist )
{
    int frequency = hist[(int)symbol];
    huff_node* curr = root;
    string bitstring;
    
    while( !curr->is_leaf() )
    {
        if( frequency < curr->frequency )
        {
            bitstring += "0";
            curr = curr->left;
        }
        else
        {
            bitstring += "1";
            curr = curr->right;
        }
    }
    
    if( curr->val == symbol && curr->frequency == frequency )
        return bitstring;
    return string("");
}*/

void get_hist( int* hist, Mat img )
{
    //int hist[256] = {0};
    int height = img.rows;
    int width = img.cols;
    
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

void huff_print_bit( huff_node* root )
{
    if( root == nullptr )
        return;
    
    if( !root->is_leaf() )
    {
        huff_print_bit( root->left );
        huff_print_bit( root->right );
    }
    else
    {
        cout << "[ " << root->frequency << " : " << (int)root->val << " : " << root->bitstring << " ]" << endl;
    }

    return;
}

void huff_print( huff_node* root, int bars )
{
    if( root == nullptr )
        return;

    for( int i = 0; i < bars; i++ )
        cout << "|";
    
    if( !root->is_leaf() )
    {
        cout << "[ " << root->frequency << " ]" << endl;
        huff_print( root->left, bars + 1 );
        huff_print( root->right, bars + 1 );
    }
    else
    {
        cout << "[ " << root->frequency << " : " << (int)root->val << " ]" << endl;
    }
        
    return;
}

void copy_img( Mat orig )
{
    vector<int> params;
    int height = orig.rows;
    int width = orig.cols;
    
    Mat cpy( height, width, CV_8UC3 );
    width *= 3;
    for( int i = 0; i < height; i++ )
        for( int j = 0; j < width; j++ )
            cpy.at<uchar>( i, j ) = orig.at<uchar>( i, j );
    
    params.push_back( CV_IMWRITE_PXM_BINARY );
    params.push_back( 0 );
    
    imwrite( "output.ppm", cpy, params );
    return;
}

int main()
{
    vector<int> params;
    Mat in_image = imread( "images/Hummingbird_txt.ppm" );
    
    //copy_img( image );
    
    huffman_encode( in_image, "compressed2.bin" );
    //huffman_encode_test( image, "compressed2.bin" );
    //huffman_decode_test( "compressed2.bin" );
    //huffman_test();
    
    Mat out_image;
    huffman_decode( out_image, "compressed2.bin" );
    
    params.push_back( CV_IMWRITE_PXM_BINARY );
    params.push_back( 0 );
    
    imwrite( "output.ppm", out_image, params );
    
    return 0;
}


































