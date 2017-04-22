/*
                             ***** huffnode.h *****

Contains the definition for the Huffnode class. This class is used to organize
values, known as symbols, into a Huffman tree data structure based on the
frequency of occurrence in a source file. A single symbol is represented by
a single node, although a node might not represent a symbol, but just occupy a
position in the Huffman tree.

CSC542 Digital Image Processing - Spring 2017

Author: Scott Carda
*/

#ifndef __HUFFNODE_H__
#define __HUFFNODE_H__

/******************************************************************************/
/*                                Include Files                               */
/******************************************************************************/

#include <string> // The bitstring data members are strings
using namespace std;

/******************************************************************************/
/*                             Huffnode Definition                            */
/******************************************************************************/

class huffnode
{
    public:
    
    unsigned char val; // The symbol that the node represents
    int frequency; // The number of times the symbol appears
    string bitstring; // The Huffman encoding of the symbol
	// Left and right children of the node
    huffnode* left;
    huffnode* right;
    
	// Constructors and Destructors
    huffnode();
    huffnode( int frequency );
    huffnode( char val, int frequency );
    ~huffnode();
    
	// Class Member Functions
    void delete_children();
    bool is_leaf();
    
	// Static Comparison Functions
    static bool node_sort_freq( huffnode* first, huffnode* second );
    static bool node_sort_val( huffnode* first, huffnode* second );
    static bool node_sort_bit( huffnode* first, huffnode* second );
};

#endif
