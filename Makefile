# Makefile for Image encoding project
#
# Usage: make
# 		 make debug
# 		 make clean
# 		 make x.cpp x2.c x3.cpp
# -----------------------------------

# GNU C/C++ compiler and linker:
CC = gcc
CXX = g++
LINK = g++
# Turn on optimization and warnings (add -g for debugging with gdb):
# CPPFLAGS = 		# preprocessor flags
CFLAGS = -g -Wall
CXXFLAGS = -g -std=c++11 -Wall

#Opencv library to be linked with
OPENCV = `pkg-config opencv --cflags --libs`

LIBS = $(OPENCV)

VPATH = src:src/huffman

#If nothing is specified with make add the optimizer flag
all:	CFLAGS += -g -O
all:	CXXFLAGS += -g -O	
all:	encode
#If debug is specified add the debugging flag
debug: CFLAGS += -g
debug: CXXFLAGS += -g
debug: all
#-----------------------------------------------------------------------

# MAKE allows the use of "wildcards", to make writing compilation instructions
# a bit easier. GNU make uses $@ for the target and $^ for the dependencies.
rle: runlength.o rlemain.o statistics.o
	$(LINK) -o $@ $^ $(LIBS)
dpcm: dpcm.o predictive.o image_io.o
	$(LINK) -o $@ $^ $(LIBS)
encode: encode.o framework.o runlength.o predictive.o #huffman.o
	$(LINK) -o $@ $^ $(LIBS)
huff: huffman.o bitstream.o huffnode.o
	$(LINK) -o $@ $^ $(LIBS)
# generic C and C++ targets for OpenGL programs consisting of only one file
# type "make filename" (no extension) to build
.c:
	$(CC) -o $@ $@.c

.cpp:
	$(CXX) -o $@ $@.cpp $(LIBS)
# utility targets
clean:
	rm -f *.o *~ core
