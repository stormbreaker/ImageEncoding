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
    
    // Loop through img, encoding its values to file
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
    //int hist[256] = {0};  // Image histogram
    int height; // Height of image
    int width; // Width of image
    huffnode* root = nullptr; // Root of Huffman tree
    vector<huffnode*> leaves; // Vector of the leaves of the Huffman tree
    
    // Bitstream for the compressed input file
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
    
    // Create new image Mat object
    img = Mat( height, width, CV_8UC3 );
    
    // Multiply for each channel
    width *= 3;
    
    read_hufftree( bin, root ); // Read Huffman tree from file
    //read_hist( bin, hist, 256 ); // Read histogram from file
    //root = huffman_tree( hist, 256 ); // Recreate tree from histogram
    
    cascade_bitstring( root ); // Computes the bitstrings for the leaves
    get_leaves( leaves, root ); // Extracts a list of the leaves
    // Sorts the leaves by the symbol they represent
    sort( leaves.begin(), leaves.end(), huffnode::node_sort_val );
    
    // Loop through img, decoding its values from file
    for( int i = 0; i < height; i++ )
        for( int j = 0; j < width; j++ )
            img.at<uchar>( i, j ) = read_next_huff( bin, root );
            
    // Close the stream
    bin.stream.close();
    
    // Deallocate the Huffman tree
    delete root;
    
    return;
}

/********************************************************************/

unsigned char read_next_huff( bitstream &bin, huffnode* root )
{
    int bit;
    
    // Traverse huff tree
    while( !root->is_leaf() )
    {
        // Read a bit from file to determine left or right child
        bin.read_bit( bit );
        
        if( bit )
            root = root->right;
        else
            root = root->left;
    }
    
    return root->val;
}

/***************************************************************/

void write_hist( bitstream &bout, int* hist, int size )
{
    // Store the data from the hist int array to file
    bout.stream.write( (char*)hist, sizeof(int) * size );

    return;
}

void read_hist( bitstream &bout, int* hist, int size )
{
    // Read in the data for the hist int array from file
    bout.stream.read( (char*)hist, sizeof(int) * size );

    return;
}

void write_hufftree( bitstream &bout, huffnode* root )
{
    // Store a bit to tell if the current root node is a leaf
    if( root->is_leaf() )
    {
        // If node is a leaf, store a 1 followed by the character value
        bout.write_bit(1);
        bout.write_byte( root->val );
    }
    else
    {
        // If node is not a leaf, store a 0
        bout.write_bit(0);
        // Then store the structure of the left subtree
        write_hufftree( bout, root->left );
        // Followed by the structure of the right subtree
        write_hufftree( bout, root->right );
    }

    return;
}

void read_hufftree( bitstream &bin, huffnode* &root )
{
    int bit;
    unsigned char c;
    
    // Read a bit to determine if the current root node should be a leaf
    if( !bin.read_bit( bit ) )
        return;
    
    root = new huffnode(); // Allocate a new node
    
    if( bit == 1 ) // If the scanned bit is 1, root should be a leaf
    {
        // The following byte is the node's character value
        bin.read_byte( c );
        root->val = c;
    }
    else // If the scanned bit is 0, root should not be a leaf
    {
        // The following data is the structure of the left subtree
        read_hufftree( bin, root->left );
        // Followed by the structure of the right subtree
        read_hufftree( bin, root->right );
    }

    return;
}

/**********************************************************/

// Create the histograph for the image
void get_hist( int* hist, Mat img )
{
    int height = img.rows;
    int width = img.cols * 3; // Multiply for each channel
    
    // Loop through img, counting the number of times each value occures
    for( int i = 0; i < height; i++ )
        for( int j = 0; j < width; j++ )
            hist[(int)img.at<uchar>( i, j )]++;
            
    return;
}

// Create the Huffman tree based on a histogram
huffnode* huffman_tree( int* hist, int size )
{
    vector<huffnode*> huff_list; // List of available root nodes
    huffnode *left, *right; // pointers for handling node children
    huffnode* node; // pointer for handling newly created nodes

    // Create a new node for each non-zero histograph value
    for( int i = 0; i < size; i++ )
        if( hist[i] != 0 )
            huff_list.push_back( new huffnode( (char)i, hist[i] ) );
    
    // Order the nodes by their frequency values, the most frequent being last
    sort( huff_list.begin(), huff_list.end(), huffnode::node_sort_freq );
    
    // While there is more than one available root node
    while( huff_list.size() > 1 )
    {
        // Get the two most frequent nodes
        left = huff_list.back();
        huff_list.pop_back();
        right = huff_list.back();
        huff_list.pop_back();
        
        // Create a new node whose frequency is the sum of the left and right nodes'
        node = new huffnode( left->frequency + right->frequency );
        // Assign the children of the new node
        node->left = left;
        node->right = right;
        
        if( huff_list.size() > 0 )
        {
            // Insert the new node into the node list, keeping the list sorted
            int i = huff_list.size() - 1;
            while( i > -1 && huff_list[i]->frequency < node->frequency ) i--;
            huff_list.insert( huff_list.begin() + i + 1, node );
        }
        else
            huff_list.push_back( node );
    }
    
    // Return the remaining root node
    return huff_list[0];
}

void cascade_bitstring( huffnode* root )
{
    if( root->left != nullptr )
    {
        // Append a 0 to root's bitstring for the left child
        root->left->bitstring = root->bitstring + "0";
        // Apply logic to left subtree
        cascade_bitstring( root->left );
    }
    if( root->right != nullptr )
    {
        // Append a 1 to root's bitstring for the right child
        root->right->bitstring = root->bitstring + "1";
        // Apply logic to right subtree
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
        // Aquire leaves from the left subtree
        get_leaves( leaves, root->left );
        // Aquire leaves from the right subtree
        get_leaves( leaves, root->right );
    }
    else
    {
        // This node is a leaf, so put it in the list
        leaves.push_back( root );
    }
    
    return;
}

/****************************************************************/

void Usage()
{
    cout << "Usage Statement!" << endl;
    return;
}

int main( int argc, char** argv )
{
    if( argc != 4 )
    {
        Usage();
        return 1;
    }
    
    if( strcmp( argv[1], "-e" ) == 0 )
    {
        Mat in_image = imread( argv[2] );
        if( in_image.data == NULL )
        {
            cout << "Error: could not open " << argv[2] << " for reading." << endl;
            return 2;
        }
        huffman_encode( in_image, argv[3] );
    }
    else if( strcmp( argv[1], "-d" ) == 0 )
    {
        Mat out_image;
        huffman_decode( out_image, argv[2] );
        
        vector<int> params;
        params.push_back( CV_IMWRITE_PXM_BINARY );
        params.push_back( 1 );
        
        imwrite( argv[3], out_image, params );
    }
    else
    {
        Usage();
        return 1;
    }
    
    return 0;
}


































