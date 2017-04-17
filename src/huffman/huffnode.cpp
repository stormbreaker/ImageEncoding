#include "huffnode.h"

// Default Constructor
huffnode::huffnode() :
frequency(-1),
bitstring(string("")),
left(nullptr),
right(nullptr)
{}

// Frequency Constructor
huffnode::huffnode( int frequency ) :
frequency(frequency),
bitstring(string("")),
left(nullptr),
right(nullptr)
{}

// Value and Frequency Constructor
huffnode::huffnode( char val, int frequency ) :
val(val),
frequency(frequency),
bitstring(string("")),
left(nullptr),
right(nullptr)
{}

// Destructor
huffnode::~huffnode()
{
    delete_children();
}

// Recursively deallocate the children under a node
void huffnode::delete_children()
{
    if( left != nullptr )
    {
        // Delete the left subtree
        left->delete_children();
        delete left;
        left = nullptr;
    }    
    if( right != nullptr )
    {
        // Delete the right subtree
        right->delete_children();
        delete right;
        right = nullptr;
    }
        
    return;
}

// Determines if the node is a leaf
bool huffnode::is_leaf()
{
    // If node has no children, it is a leaf
    return left == nullptr && right == nullptr;
}

// Static function for telling how to sort nodes pointers by their frequencies
bool huffnode::node_sort_freq( huffnode* first, huffnode* second )
{
    if( first->frequency <= second->frequency )
        return false;
    return true;
}

// Static function for telling how to sort nodes pointers by their values
bool huffnode::node_sort_val( huffnode* first, huffnode* second )
{
    if( first->val > second->val )
        return false;
    return true;
}

// Static function for telling how to sort nodes pointers by their bitstrings
bool huffnode::node_sort_bit( huffnode* first, huffnode* second )
{
    // First compare with string length
    if( first->bitstring.size() < second->bitstring.size() )
        return true;
    if( first->bitstring.size() > second->bitstring.size() )
        return false;
    
    // If equal length, compare with string.compare
    return first->bitstring.compare( second->bitstring ) > 0;
}

