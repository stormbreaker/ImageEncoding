#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <iomanip>
using namespace std;

int read_bin_int( ifstream &fin );

class Image
{

    public:
    
    int magic_number;
    int row;
    int col;
    int max_val;
    string comment;
    
    int** data;

    Image( const char* filename )
    {
        ifstream fin;
    
        fin.open( filename, ios::in );
        if ( !fin )
        {
            cout << "Cannot Open " << filename << "!" << endl;
            return;
        }
        
        streampos data_start = read_header( fin );
        
        /*string line;
        
        fin.seekg( 1 );
        fin >> magic_number;
        
        getline( fin, line );
        do
        {
            getline( fin, line );
            if ( line[0] == '#' )
            {
                comment.append( line + "\n" );
            }
            
        } while( line[0] == '#' );
        
        stringstream ss;
        ss.str( line );
        ss >> col >> row;
        getline( fin, line );
        ss.str( line );
        ss >> max_val;
        
        streampos spot = fin.tellg();*/
        
        fin.close();
        fin.clear();
        
        allocate_data();
        
        /*if ( magic_number < 4 )
        {
            //cout << "Is Text" << endl;
            fin.open( filename, ios::in );
            if ( !fin )
            {
                cout << "Bad Problem" << endl;
                return;
            }
            
            fin.seekg( data_start, ios::beg );
            read_txt_data( fin );
        }
        else
        {
            //cout << "Is Binary" << endl;
            fin.open( filename, ios::in | ios::binary );
            if ( !fin )
            {
                cout << "Bad Problem" << endl;
                return;
            }
            
            fin.seekg( data_start, ios::beg );
            
            read_bin_data( fin );
        }*/
        
            
        fin.close();
        return;
    }
    
    ~Image()
    {
        for ( int i = 0; i < row; i++ )
        {
            delete[] data[i];
        }
        delete[] data;
        return;
    }
    
    streampos read_header( ifstream &fin )
    {
        string line;
        
        fin.seekg( 1 );
        fin >> magic_number;
        
        // comments need to be handeled differently
        getline( fin, line );
        do
        {
            getline( fin, line );
            if ( line[0] == '#' )
            {
                comment.append( line + "\n" );
            }
            
        } while( line[0] == '#' );
        
        stringstream ss;
        ss.str( line );
        ss >> col >> row;
        
        if ( magic_number % 3 != 1 )
            fin >> max_val;
        else
            max_val = 1;
        
        return fin.tellg();
    }
    
    void allocate_data()
    {
        int col_max = col;
        if ( magic_number % 3 == 0 )
            col_max *= 3;
    
        data = new int*[row];
        for ( int i = 0; i < row; i++ )
        {
            data[i] = new int[col_max];
        }
        return;
    }
    
    void read_txt_data( ifstream &fin )
    {
        int col_max = col;
        if ( magic_number % 3 == 0 )
            col_max *= 3;
            
        for ( int i = 0; i < row; i++ )
        {
            for ( int j = 0; j < col_max; j++ )
            {
                //cout << "Col: " << j << endl;
                fin >> data[i][j];
            }
        }
        return;
    }
    
    void read_bin_data( ifstream &fin )
    {
        int col_max = col;
        if ( magic_number % 3 == 0 )
            col_max *= 3;
    
        if ( magic_number % 3 == 1 )
        {
        
            int val;
            for ( int i = 0; i < row; i++ )
            {
                for ( int j = 0; j < col_max; j++ )
                {
                    val = read_bin_int( fin );
                    for ( int k = 0; k < 7; k++ )
                    {
                        data[i][j+k] = ( ( val >> k ) & 1 );
                    }
                }
            }
        }
        else
        {
            for ( int i = 0; i < row; i++ )
            {
                for ( int j = 0; j < col_max; j++ )
                {
                    data[i][j] = read_bin_int( fin );
                }
            }
        }
    }
    
    void print_data( const char* filename )
    {
        ofstream fout;
        fout.open( filename, ios::out | ios::trunc );
        if ( !fout )
        {
            cout << "Cannot Open " << filename << "!" << endl;
            return;
        }
        fout << "P" << magic_number << endl;
        fout << col << " " << row;
        if ( magic_number % 3 != 1 )
            fout << " " << max_val;
        fout << endl;
        
        /*cout << "Magic Number: " << magic_number << endl;
        cout << "Row Number: " << row << endl;
        cout << "Col Number: " << col << endl;
        cout << "Max Value: " << max_val << endl;*/
        
        int max_col = col;
        if ( magic_number % 3 == 0 )
            max_col *= 3;
        
        for ( int i = 0; i < row; i++ )
        {
            for ( int j = 0; j < max_col; j++ )
            {
                //fprintf( fout, "%3d", data[i][j] );
                fout << setw(3) << data[i][j] << " ";
            }
        }
        
        fout.close();
        return;
    }
};

int read_bin_int( ifstream &fin )
{
    unsigned char symbol;
    fin.read((char*) &symbol, 1);
    return (int) symbol;
}

/*int not_main()
{
    ifstream fin;
    
    fin.open( "test.txt", ios::in | ios::binary );
    if ( !fin )
    {
        cout << "bad" << endl;
        return 1;
    }
    
    int val1 = read_bin_int( fin );
    int val2 = read_bin_int( fin );
    int val3 = read_bin_int( fin );
    
    //int val1, val2, val3;
    
    //fin >> val1 >> val2 >> val3;
    
    fin.close();
    
    cout << val1 << ", " << val2 << ", " << val3 << endl;

    return 0;
}*/

int main()
{
    //const char* doop = "test.ppm";
    Image img( "Images/test.ppm" );
    img.print_data( "Images/output.txt" );
    return 0;
}

int not_main()
{
    ifstream fin;
    
    fin.open( "test.ppm", ios::in );
    if ( !fin )
    {
        cout << "bad" << endl;
        return 1;
    }
    
    int magic_number;
    int row;
    int col;
    int max_val;
    string line;
    string comment;
    
    fin.seekg( 1 );
    fin >> magic_number;
    
    getline( fin, line );
    do
    {
        getline( fin, line );
        if ( line[0] == '#' )
        {
            comment.append( line + "\n" );
        }
        
    } while( line[0] == '#' );
    
    stringstream ss;
    ss.str( line );
    ss >> col >> row;
    getline( fin, line );
    ss.str( line );
    ss >> max_val;
    
    streampos spot = fin.tellg();
    
    fin.close();
    
    int val1, val2, val3;
    
    if ( magic_number < 4 )
    {
        cout << "Is Text" << endl;
        fin.open( "test.ppm", ios::in );
        
        fin.seekg( spot, ios::beg );
        
        fin >> val1 >> val2 >> val3;
    }
    else
    {
        cout << "Is Binary" << endl;
        fin.open( "test.ppm", ios::in | ios::binary );
        
        fin.seekg( spot, ios::beg );
        
        val1 = read_bin_int( fin );
        val2 = read_bin_int( fin );
        val3 = read_bin_int( fin );
    }
        
    fin.close();
    
    cout << row << ", " << col << endl;
    
    return 0;
}
