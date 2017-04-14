//#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
using namespace std;

typedef struct
{
    char val;
    int frequency;
    huff_node* left;
    huff_node* right;
    
    huff_node( char val, int frequency ) :
    val(val),
    frequency(frequency),
    left(nullptr),
    right(nullptr)
    {}
    
    huff_node( int frequency ) :
    frequency(frequency),
    left(nullptr),
    right(nullptr)
    {}
    
    ~huff_node()
    {
        delete_children();
    }
    
    void delete_children()
    {
        if( left != nullptr )
        {
            left.delete_children();
            delete left;
            left = nullptr;
        }    
        if( right != nullptr )
        {
            right.delete_children();
            delete right;
            right = nullptr;
        }
            
        return;
    }
    
    bool is_leaf()
    {
        return left == nullptr && right == nullptr;
    }
    
} huff_node;

bool node_comp( huff_node first, huff_node second )
{
    if( first.frequency <= second.frequency )
        return false;
    return true;
}

void huffman_encode( char** img, int height, int width )
{
    int* hist;
    huff_node* root;
    string bitstring = "";
    
    // Create the histograph for the image
    hist = get_hist( img, height, width );
    root = huffman_tree( hist, 256 );
    
    for( int i = 0; i < height; i++ )
    {
        for( int i = 0; i < height; i++ )
        {
            bitstring += get_bitstring( img[i][j], root, hist );
        }
    }
    
    // pad to a word boundary with zeros
    if( bitstring.length() % 8 )
        for( int i = 8 - bitstring.length() % 8; i > 0; i-- )
            bitstring += "0"
        
    int temp;
    int k = 0;
    while( !binstring.empty() )
    {
        temp = 0;
        for( int i = 0; i < 8; i++ )
        {
            if( bitstring.front() == "1" )
                temp += 1;
            bitstring.erase( bitstring.begin() );
            temp *= 2;
        }
        img[k/width][k%width] = (char)temp;
        k++;
    }
    
    for( int j = k%width; j < width; j++ )
        img[k/width][j] = (char)0;
    
    for( int i = k/width + 1; i < height; i++ )
        for( int j = 0; j < width; j++ )
            img[i][j] = (char)0;
    
    return;
}

string get_bitstring( char symbol, huff_node* root, int* hist )
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
}

int* get_hist( char** img, int height, int width )
{
    int hist[256] = {0};
    
    // Create the histograph for the image
    for( int i = 0; i < height; i++ )
        for( int j = 0; j < width; j++ )
            hist[(int)img[i][j]]++;
            
    return hist;
}

huff_node* huffman_tree( int* hist, int size )
{
    vector<huff_node> huff_list;
    huff_node* first, second;
            
    for( int i = 0; i < size; i++ )
    {
        if( hist[i] != 0 )
        {
            temp_node = new huff_node( (char)i, hist[i] );
            huff_list.append( temp_node );
        }
    }
    sort( huff_list.begin(), huff_list.end(), node_comp );
    
    while( huff_list.size() > 1 )
    {
        first = huff_list.back();
        huff_list.pop_back();
        
        second = huff_list.back();
        huff_list.pop_back();
        
        node = new huff_node( first.frequency + second.frequency );
        node.left = first;
        node.right = second;
        
        if( huff_list.size() > 0 )
        {
            int i = huff_list.size() - 1;
            while( i > 0 && huff_list[i].frequency < node.frequency ) i--;
            huff_list.insert( huff_list.begin() + i, node );
        }
        else
            huff_list.push_back( node );
    }
    
    return &huff_list[0];
}


































