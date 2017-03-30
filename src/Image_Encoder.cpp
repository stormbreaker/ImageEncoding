#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

void printUsage();

int main( int argc, char **argv )
{
    int aflag = 0;
    int bflag = 0;
    char *cvalue = NULL;
    int index;
    int c;

    opterr = 0;
    
    while( ( c = getopt( argc, argv, "abc:" ) ) != EOF )
        switch( c )
        {
        case 'a':
            aflag = 1;
            break;
            
        case 'b':
            bflag = 1;
            break;
            
        case 'c':
            cvalue = optarg;
            break;
            
        case '?':
            if( optopt == 'c' )
                printUsage();
            return 1;            
                /*fprintf( stderr, "Option -%c requires an argument.\n", optopt );
            else if( isprint( optopt ) )
                fprintf( stderr, "Unknown option '-%c'.\n", optopt );
            else
                fprintf( stderr, "Unknown option character '\\x%x'.\n", optopt);*/
                
        default:
            abort();
        }


    //printf( "aflag = %d, bflag = %d, cvalue = %s\n", aflag, bflag, cvalue );
    cout<< "aflag = " << aflag << ", bflag = " << bflag << ", cvalue = " << cvalue << endl;

    for( index = optind; index < argc; index++ )
        //printf( "Non-option argument %s\n", argv[index] );
        cout << "Non-option argument " << argv[index] << endl;

    return 0;
}

void printUsage()
{
    cout << "Usage" << endl;
    return;
}
