//#include <fstream>
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

struct huff_node
{
    unsigned char val;
    int frequency;
    string bitstring;
    huff_node* left;
    huff_node* right;
    
    /*huff_node() :
    left(nullptr),
    right(nullptr)
    {}*/
    
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
    //if( toInt( first->bitstring, first->bitstring.size() ) <= toInt( second->bitstring, second->bitstring.size() ) )
    if( first->bitstring.size() < second->bitstring.size() )
        return true;
    if( first->bitstring.size() > second->bitstring.size() )
        return false;
    
    return first->bitstring.compare( second->bitstring ) > 0;
}

/***********************************************************/

void huffman_encode( Mat img );
void cascade_bitstring( huff_node* root );
void get_leaves( vector<huff_node*> &leaves, huff_node* root );
string get_bitstring( char symbol, huff_node* root, int* hist );
void get_hist( int* hist, Mat img );
huff_node* huffman_tree( int* hist, int size );
void huff_print_bit( huff_node* root );
void huff_print( huff_node* root, int bars = 0 );

void huffman_test()
{
    const unsigned int num = 6;
    int hist[num] = { 25, 25, 20, 15, 10, 5 };
    huff_node* root;
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

void huffman_encode( Mat img )
{
    int hist[256] = {0};
    int height = img.rows;
    int width = img.cols * 3;
    huff_node* root;
    string bitstring = "";
    vector<huff_node*> leaves;
    
    // Create the histograph for the image
    get_hist( hist, img );
    root = huffman_tree( hist, 256 );
    cascade_bitstring( root );
    get_leaves( leaves, root );
    
    vector<huff_node*> leaves2 = leaves;
    vector<huff_node*> leaves3 = leaves;
    
    cout << "Height: " << height << ", Width: " << width << endl;
    
    
    /*cout << (int)img.at<uchar>( 0, 0 ) << ", ";
    cout << (int)img.at<uchar>( 0, 1 ) << ", ";
    cout << (int)img.at<uchar>( 0, 2 ) << endl;
    cout << (int)img.at<uchar>( 0, 3 ) << ", ";
    cout << (int)img.at<uchar>( 0, 4 ) << ", ";
    cout << (int)img.at<uchar>( 0, 5 ) << endl;*/
    
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
    
    int total = 0;
    int sizes = 0;
    for( unsigned int i = 0; i < leaves.size(); i++ )
    {
        total += leaves[i]->frequency;
        sizes += leaves[i]->frequency * leaves[i]->bitstring.size();
    }
    cout << "Average Compressed Length: " << (double)sizes / total << endl;
    
    bool in_order = true;
    for( unsigned i = 1; i < leaves3.size(); i++ )
    {
        if( leaves3[i-1]->frequency < leaves3[i]->frequency )
        {
            in_order = false;
            break;
        }
    }
    cout << "Leaves are " << (in_order?"":"not ") << "in order." << endl;
    
    //huff_print( root );
    //huff_print_bit( root );
    return;
    
    int put_i = 0;
    int put_j = 0;
    string big = "";
    for( int i = 0; i < height; i++ )
    {
        for( int j = 0; j < width; j++ )
        {
            //cout << "row: " << i << ", col: " << j << endl;
            bitstring += leaves2[(int)img.at<uchar>( i, j )]->bitstring;
            big += leaves2[(int)img.at<uchar>( i, j )]->bitstring;
            //cout << "Before: " << bitstring << endl;
            while( bitstring.size() >= 8 && ( put_i < i || ( put_i == i && put_j <= j ) ) )
            {
                img.at<uchar>( put_i, put_j ) = (unsigned char)toInt( bitstring, 8 );
                bitstring.erase( 0, 8 );
                put_j++;
                if( put_j >= width )
                {
                    put_i++;
                    put_j = 0;
                }
            }
            //cout << "After: " << bitstring << endl;
        }
    }
            //bitstring += get_bitstring( img.at<uchar>( i, j ), root, hist );
    
    //cout << "After bitstring built" << endl;
    cout << "Total compressed bits: " << big.size() << endl;
    cout << "Remaining bits: " << bitstring.size() << endl;
    
    // pad to a word boundary with zeros
    if( bitstring.length() % 8 )
    {
        for( int i = 8 - bitstring.length() % 8; i > 0; i-- )
            bitstring += "0";
        img.at<uchar>( put_i, put_j ) = (unsigned char)toInt( bitstring, 8 );
        put_j++;
        if( put_j >= width )
        {
            put_i++;
            put_j = 0;
        }
    }
        
    /*int temp;
    int k = 0;
    while( !bitstring.empty() )
    {
        cout << bitstring.size() << endl;
        temp = toInt( bitstring, 8 );
        img.at<uchar>( k/width, k%width ) = (unsigned char)temp;
        k++;
        bitstring.erase( bitstring.begin(), bitstring.begin() + 8 );
    }*/

    cout << "Get here" << endl;
    
    if( put_i < height )
    {
        for(; put_j < width; put_j++ )
            img.at<uchar>( put_i, put_j ) = (unsigned char)0;
            
        for( put_i++; put_i < height; put_i++ )
            for( put_j = 0; put_j < width; put_j++ )
                img.at<uchar>( put_i, put_j ) = (unsigned char)0;
    }
    
    /*int temp;
    int k = 0;
    while( !bitstring.empty() )
    {
        temp = 0;
        for( int i = 0; i < 8; i++ )
        {
            if( bitstring.front() == '1' )
                temp += 1;
            bitstring.erase( bitstring.begin() );
            temp *= 2;
        }
        img.at<uchar>( k/width, k%width ) = (unsigned char)temp;
        k++;
    }*/
    
    /*for( int j = k%width; j < width; j++ )
        img.at<uchar>( k/width, j ) = (char)0;
    
    for( int i = k/width + 1; i < height; i++ )
        for( int j = 0; j < width; j++ )
            img.at<uchar>( i, j ) = (char)0;*/
    
    delete root;
    
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

int main()
{
    vector<int> params;
    Mat image = imread( "Images/Hummingbird_txt.ppm" );
    
    huffman_encode( image );
    //huffman_test();
    
    return 0;
    
    params.push_back( CV_IMWRITE_PXM_BINARY );
    params.push_back( 0 );
    
    imwrite( "Images/output.bpm", params );
    
    return 0;
}


































