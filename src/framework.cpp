#include "framework.h"
const char * const errors[] = { "Invalid number of arguments.",
                                "Invalid encoding arguments.",
                                "Failed to read image: image not found.",
                                "Invalid method type specified."
};

int valid_enc_args( char **argv, int argc, cv::Mat& image)
{
    int method;
    if ( argc <  3)
        return -1;
    method = get_encode_method( argv[2] );

    if(!valid_method_args(argv, argc, method))
        return -2;

    image = cv::imread( argv[1], -1);
    if (image.empty())
        return -3;

    return method;
}

int get_encode_method( char* arg )
{
    return !strcmp( arg, "rle-r") ? 0 : !strcmp( arg, "rle-b") ? 1 : !strcmp( arg, "huff" ) ? 2 : !strcmp( arg, "pp") ? 3 : 6;
        
}

bool valid_method_args( char **argv, int argc, int &method )
{
   /* std::cout << argv[3] << std::endl;
    for (int i = 3; i < argc; i++)
    {
        if( !is_digit(argv[i]) );
        {
            method = -4;
            return false;
        }
    }
    */
    //std::cout<< "SWITCH\n";
    switch(method)
    {
        case 0: return argc == 4 ? true: false;
        case 1: return argc <= 8 ? true: false;
        case 2: 
        case 3: 
        case 4: return argc > 3 ? false : true;
        case 5: return argc == 5 ? true: false;
        default:
                method = -4;
    }
    return false;

}

int valid_dec_args( char **argv, int argc)
{
   return 0; 
}

void print_errors( int error_code)
{
    error_code = (error_code*-1) -1;
    std::cout << errors[error_code] << std::endl << std::endl;
    print_help();
}

void print_help()
{
    std::cout << "Program Usage: " << std::endl << "\t./encode <Input Image> <Encode Type> <Encode Args>" << std::endl << std::endl;
    std::cout << "Encode Types and number of integer arguments:" << std::endl << "\trle-r {1}" << std::endl;
    std::cout << "\thuff {0}" << std::endl << "\tpp {0}" << std::endl;
}

bool is_digit( char *arg)
{
    for( int i = 0; arg[i] != '\0'; i++)
    {
        if(arg[i] != ' ' && arg[i] != '\n')
            if( arg[i] < '0' || arg[i] > '9' )     
                return false;
    }
    return true;
}
