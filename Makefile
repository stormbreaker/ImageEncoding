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
CFLAGS = -Wall
CXXFLAGS = -Wall

#Opencv library to be linked with
OPENCV = `pkg-config opencv --cflags --libs`

LIBS = $(OPENCV)

VPATH = src

#If nothing is specified with make add the optimizer flag
all:	CFLAGS += -O
all:	CXXFLAGS += -O	
all:	image_encoder dpcm
#If debug is specified add the debugging flag
debug: CFLAGS += -g
debug: CXXFLAG += -g
debug: image_encoder dpcm
#-----------------------------------------------------------------------

# MAKE allows the use of "wildcards", to make writing compilation instructions
# a bit easier. GNU make uses $@ for the target and $^ for the dependencies.
rle: runlength.o
	$(LINK) -o $@ $^ $(LIBS)
dpcm: dpcm.o predictive.o image_io.o 
	$(LINK) -o $@ $^ $(LIBS)
image_encoder: Image_Encoder.o runlength.o huffman.o predictive.o
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
