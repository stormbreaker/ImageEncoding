# csc442final


#Program Compilation:

make - to make all executables

make rle - makes runlength executable
make huffman - Compiles huffman executable
make dpcm - Compiles differential pulse code modulation executable
make delta - Compiles delta modulation executable
make prev - Compiles previous pixel executable

#Program Usages:

#Predictive executables:

Will auto decompress the images as well and save it with the compressed_name.png

DPCM: ./dpcm input_image predictor_order( 1 or 2) quantizer_levels( 4 or 8 ) compressed_name.bin
delta:./delta input_image delta_value compressed_name
