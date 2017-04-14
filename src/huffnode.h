#ifndef __HUFFNODE_H__
#define __HUFFNODE_H__

#include <string>
using namespace std;

class huffnode
{
    public:
    
    unsigned char val;
    int frequency;
    string bitstring;
    huffnode* left;
    huffnode* right;
    
    huffnode();
    huffnode( int frequency );
    huffnode( char val, int frequency );
    
    ~huffnode();
    
    void delete_children();
    bool is_leaf();
    
    static bool node_sort_freq( huffnode* first, huffnode* second );
    static bool node_sort_val( huffnode* first, huffnode* second );
    static bool node_sort_bit( huffnode* first, huffnode* second );
};

#endif
