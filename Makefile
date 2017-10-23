# Makefile for quadtree program using OpenGL.

# Author: John M. Weiss, Ph.D.
# Class: CSC300 Data Structures, Fall 2015 (sample code for PA#2)

# Usage:  make target1 target2 ...

#-----------------------------------------------------------------------

# GNU C/C++ compiler and linker:
LINK = g++

# Preprocessor and compiler flags (turn on warnings, optimization and debugging):
# CPPFLAGS = <preprocessor flags go here>
CFLAGS = -Wall -O -g -std=c++11
CXXFLAGS = $(CFLAGS)

# OpenGL/GLUT libraries for Linux:
GL_LIBS = -lglut -lGLU -lGL -lm

#-----------------------------------------------------------------------
# Specific targets:

# MAKE allows the use of "wildcards", to make writing compilation instructions
# a bit easier. GNU make uses $@ for the target and $^ for the dependencies.

all:	quadtree

# demo BMP display program
#BMPdisplay:	BMPdisplay.o BMPload.o
#	$(LINK) -o $@ $^ $(GL_LIBS)
#

# here is an entry for your quadtree program
quadtree:	BMPload.o BMPdisplay.o quadtree.o
	$(LINK) -o $@ $^ $(GL_LIBS)

clean:
	rm -f *.o *~
