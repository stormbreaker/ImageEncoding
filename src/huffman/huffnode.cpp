#include "huffnode.h"

huffnode::huffnode() :
frequency(-1),
bitstring(string("")),
left(nullptr),
right(nullptr)
{}

huffnode::huffnode( int frequency ) :
frequency(frequency),
bitstring(string("")),
left(nullptr),
right(nullptr)
{}

huffnode::huffnode( char val, int frequency ) :
val(val),
frequency(frequency),
bitstring(string("")),
left(nullptr),
right(nullptr)
{}

huffnode::~huffnode()
{
    delete_children();
}

void huffnode::delete_children()
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

bool huffnode::is_leaf()
{
    return left == nullptr && right == nullptr;
}

bool huffnode::node_sort_freq( huffnode* first, huffnode* second )
{
    if( first->frequency <= second->frequency )
        return false;
    return true;
}

bool huffnode::node_sort_val( huffnode* first, huffnode* second )
{
    if( first->val > second->val )
        return false;
    return true;
}

bool huffnode::node_sort_bit( huffnode* first, huffnode* second )
{
    if( first->bitstring.size() < second->bitstring.size() )
        return true;
    if( first->bitstring.size() > second->bitstring.size() )
        return false;
    
    return first->bitstring.compare( second->bitstring ) > 0;
}

