/************************************************
 *   -------   quadtree.h   -------
 *   Authors: Lucas Carpenter & Wyatt Engel
 *   Class/Program: CSC300 (Data Structures) 
 *                  Programming Assignment 2
 *   
 *   Notes: This file is a header/class file for  
 *          the quadtree functions. It will handle
 *          the struct for the quadtree data
 *          structure and how the quadtree traverses
 *          inserts, removes, and handles division.
 *     
 *          - Divide and conquer -
 *   
 *   Modifications:
 *     10/12/2015 Lucas - Created class design and some
 *                        function prototypes.
 *     10/17/2015 Lucas&Wyatt - Created more prototypes
 *                        and finished class design
 *
 **********************************************/
#ifndef __QUADTREE_H_
#define __QUADTREE_H_


//include files
#include <iostream>
#include <math.h>

typedef unsigned char byte;

class quadtree {
  
 private:
  // Structure declaration
  struct node {
    int minX = 0, minY = 0, maxX = 0, maxY = 0; // handles bounds for the node
    byte color = 0;     // the average value for color of quadtree (relies on leaf being true)
    bool leaf = false;  //boolean to handle if it is leaf or parent
    node *NE;
    node *NW;
    node *SW;
    node *SE; //sub-nodes, if is not leaf then it will have children to divide to
  };
  // private variables
  node* root;          // the root of our tree.
  byte** image;        // 2D array of bitmap image for [x][y] traversing/scanning
  byte** temp;         // 2D array of bitmap image for [x][y] quadtree
  byte* newImg;        // 1D array of bitmap image for [xy] quadtree
  byte* linesImg;      // 1D array of bitmap image for [xy] quadtree w/ lines
  int fudge = 0;       // our fudge factor
  int count = 1;       // amount of nodes
  int leafcount = 0;   // amount of leaf nodes
  // private prototypes
  void convertQuad();  // function to put temp into newImg
  void convertLines(); // function to put image into linesImg
  void deleteTree(node *&tree); // function to delete tree when finished
  
 public:
  quadtree();
  ~quadtree(); // destructor - some dynamic memory in here, will have to clean?
  void tree(byte *inImg, int nrows = 0, int ncols = 0, int f = 0); // constructor - only allows for 1D array image input
  void divide(node *&tree); // divide and conquer! - will handle splitted nodes into 4 smaller nodes
  void traverse(node *&quadrant); // uses square to traverse inImg, is called recursively.
  byte getMean( node *&quadrant);  // gets average of values inside a quadrant.
  int getCount(); // returns out number of nodes
  int getLeafs(); // returns out number of leaf nodes
  void decompress(node *tree); // replaces area in quadtree with one specific color (avg of area's colors)
  void whiteLines(node *tree); // creates white lines where quadtree leafs are around the original image
  byte* getNewImg(); // returns the quadtree image
  byte* getLinesImg(); // returns the image with white lines around the quadtree areas 
};


#endif
