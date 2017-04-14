#include "huffman.h"

void huffman_encode( Mat img, string outfile )
{
    int hist[256] = {0}; // Image histogram
    int height = img.rows; // Height of image
    int width = img.cols; // Width of image
    huffnode* root = nullptr; // Root of Huffman tree
    string bitstring; // Huffman bitstrings for encoded values
    vector<huffnode*> leaves; // Vector of the leaves of the Huffman tree
    
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
    sort( leaves.begin(), leaves.end(), huffnode::node_sort_val );
    
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
            for( unsigned int k = 0; k < bitstring.size(); k++ )
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
    //int hist[256] = {0};
    int height;
    int width;
    huffnode* root = nullptr;
    vector<huffnode*> leaves;
    
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
    sort( leaves.begin(), leaves.end(), huffnode::node_sort_val );
    
    for( int i = 0; i < height; i++ )
        for( int j = 0; j < width; j++ )
            img.at<uchar>( i, j ) = read_next_huff( bin, root );
            
    bin.stream.close();
    
    delete root;
    
    return;
}

unsigned char read_next_huff( bitstream &bin, huffnode* root )
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

void write_hufftree( bitstream &bout, huffnode* root )
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

void read_hufftree( bitstream &bin, huffnode* &root )
{
    int bit;
    unsigned char c;
    
    if( !bin.read_bit( bit ) )
        return;
    
    root = new huffnode();
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

void cascade_bitstring( huffnode* root )
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

void get_leaves( vector<huffnode*> &leaves, huffnode* root )
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

huffnode* huffman_tree( int* hist, int size )
{
    vector<huffnode*> huff_list;
    huffnode *first, *second;
    huffnode* node;
            
    for( int i = 0; i < size; i++ )
    {
        if( hist[i] != 0 )
        {
            huff_list.push_back( new huffnode( (char)i, hist[i] ) );
        }
    }
    sort( huff_list.begin(), huff_list.end(), huffnode::node_sort_freq );
    
    while( huff_list.size() > 1 )
    {
        first = huff_list.back();
        huff_list.pop_back();
        
        second = huff_list.back();
        huff_list.pop_back();
        
        node = new huffnode( first->frequency + second->frequency );
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


































