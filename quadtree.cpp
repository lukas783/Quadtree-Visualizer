/************************************************
 *   -------   quadtree.cpp   -------
 *   Authors: Lucas Carpenter & Wyatt Engel
 *   Class/Program: CSC300 (Data Structures) 
 *                  Programming Assignment 2
 *   
 *   Notes: This file is a cpp file for handling
 *          the quadtree class from quadtree.h.
 *          It will provide functions defined in
 *          the quadtree class to insert nodes, 
 *          construct, destruct, traverse, and
 *          divide for the purpose of creating
 *          a region-based quadtree
 *     
 *          - Divide and conquer -
 *   
 *   Modifications:
 *      10/12/2015 Lucas - Created basic class
 *      10/17/2015 Lucas & Wyatt - Created member
 *                         functions and implemented
 *                         quadtree structure
 *      10/18/2015 Wyatt - Created divide() member
 *                         function
 *      10/21/2015 Lucas & Wyatt - Finished up final
 *                         display of quadtree and
 *                         started checking/fixing bugs
 *
 **********************************************/

#include "quadtree.h"

using namespace std;

/**
 * Constructor, currently doesn't do anything, the real construction is 
 * handled in tree()
**/
quadtree::quadtree() {

}

/**
 * Tree, is basically a constructor except we wanted to be able to call
 * it whenever we needed to rebuild the tree. This function resets everything
 * and begins to rebuild the tree.
**/
void quadtree::tree(byte *inImg, int nrows, int ncols, int f) {
  //Here we initialize and reset
  fudge = f;
  count = 0;
  leafcount = 0;
  root = new node;
  root->minX = 0;
  root->minY = 0;
  root->maxX = ncols;
  root->maxY = nrows;

  //Here we allocate nodes and arrays
  image = new byte*[ncols];            //2D image from file
  newImg = new byte[ncols*nrows];      //1D image with quad compresssion
  linesImg = new byte[ncols*nrows];      //1D original image with quad lines
  temp = new byte*[ncols];             //temp 2D for storing stuff..
  for( int i = 0 ; i < ncols ; i++ ) {
    image[i] = new byte[nrows];
    temp[i] = new byte[ncols];
  }
  if(image == nullptr || temp == nullptr || newImg == nullptr || linesImg == nullptr) {
    cout << "Unable to dynamically allocate memory.\n";
    exit(2);
  }

  //Here we create the tree and manipulate the image
  for(int i = 0; i < (nrows*ncols);i++)
    image[(i%ncols)][(nrows-(i/ncols))-1] = inImg[i]; // long but it works. puts it into 2D array
  traverse(root);
  decompress(root);
  convertQuad();
  whiteLines(root);
  convertLines();

  //And here we output our info about the tree
  cerr << count << " nodes and " << leafcount << " leaves in quadtree (" << leafcount*2 << " bytes).\n";
  cerr << "The quadtree size is about ";
  cerr << ceil( ( (float)(leafcount*2)/(float)(nrows*ncols)) * 100 );
  cerr << "% of the uncompressed image size.\n";
}

/**
 * Destructor, Deletes the tree when all is finished..
**/
quadtree::~quadtree() {
  deleteTree(root);
}

/**
 * Deletes everything in the tree by traversing to the lowest point
 * when the node has no children it will delete that node and recurse itself
 * back up to the root node
**/
void quadtree::deleteTree(node *&tree) {
  if (tree->NW != nullptr) {
    deleteTree(tree->NW);
  }
  if (tree->SW != nullptr) {
    deleteTree(tree->SW);
  }
  if (tree->NE != nullptr) {
    deleteTree(tree->NE);
  }
  if (tree->SE != nullptr) {
    deleteTree(tree->SE);
  }
  delete tree;
}

/**
 * Quadtree divide, splits a node into 4 quadrents and sets the parents 4 children
 * to those quadrants.
**/
void quadtree::divide(node *&tree) {
  count += 4;
  tree->NW = new node;
  tree->NE = new node;
  tree->SW = new node;
  tree->SE = new node;
  // NW node
  tree->NW->minX = tree->minX;
  tree->NW->maxX = ((tree->maxX+tree->minX)/2);
  tree->NW->minY = tree->minY;
  tree->NW->maxY = ((tree->maxY+tree->minY)/2);
  // NE node
  tree->NE->minX = ((tree->maxX+tree->minX)/2);
  tree->NE->maxX = tree->maxX;
  tree->NE->minY = tree->minY;
  tree->NE->maxY = ((tree->maxY+tree->minY)/2);
  // SW node
  tree->SW->minX = tree->minX;
  tree->SW->maxX = ((tree->maxX+tree->minX)/2);
  tree->SW->minY = ((tree->maxY+tree->minY)/2);
  tree->SW->maxY = tree->maxY;
  // SE node
  tree->SE->minX = ((tree->maxX+tree->minX)/2);
  tree->SE->maxX = tree->maxX;
  tree->SE->minY = ((tree->maxY+tree->minY)/2);
  tree->SE->maxY = tree->maxY;
}

/**
 * getMean is used while traversing to get epsilon of the area in question.
 * Once the mean is found it will return it out and traverse will check for
 * anything in the quadrant that is more or less than what is allowed
**/
byte quadtree::getMean(node *&quadrant) {
  double avg = 0;
  for(int i = quadrant->minY; i < quadrant->maxY; i++ ) {
    for(int j = quadrant->minX; j < quadrant->maxX; j++) {
      avg += image[j][i];
    }
  }
  avg = avg/(double)((quadrant->maxX - quadrant->minX) * (quadrant->maxY - quadrant->minY));
  return ceil(avg);
}

/**
 * Traverse will have to search the quadrant looking for anything too different
 * based on the 'fudge factor' given. If anything is too different, divide, if not
 * set the node to a leaf and give it getMean's value in node->color
 **/
void quadtree::traverse(node *&quadrant) {
  int avg = getMean(quadrant);
  for(int y = quadrant->minY; y < quadrant->maxY;y++) {
    for(int x = quadrant->minX; x < quadrant->maxX;x++) {
      if(abs(image[x][y] - avg) > fudge) {
	divide(quadrant);
	traverse(quadrant->NW);
	traverse(quadrant->NE);
	traverse(quadrant->SE);
	traverse(quadrant->SW);
	return;
      }
    }
  }
  quadrant->leaf = true;
  leafcount++;
  quadrant->color = avg;
}

/** 
 * getCount, returns an int that contains the total
 * nodes in the quadtree
**/
int quadtree::getCount() {
  return count;
}

/**
 * getLeafs, returns an int that contains the total
 * of leaf nodes in the quadtree
**/
int quadtree::getLeafs() {
  return leafcount;
}

/**
 * decompress, traverses to leaf nodes assigning
 * each area in the quadtree leaf one specific color
 * based on the average of all the areas
**/
void quadtree::decompress(node *tree) {
  if(tree->leaf == true) {
    for(int y = tree->minY; y < tree->maxY; y++ ) {
      for(int x = tree->minX; x< tree->maxX; x++ ) {
	temp[x][y] = tree->color;
      }
    }
  } else {
    decompress(tree->NW);
    decompress(tree->NE);
    decompress(tree->SE);
    decompress(tree->SW);
  }
}

/**
 * whiteLines, creates the white lines for our second
 * image. Traverses to each leaf node and adds a white
 * border around the image.
**/
void quadtree::whiteLines(node *tree) {
  if(tree->leaf == true) {
    for(int x = tree->minX; x < tree->maxX; x++ ) {
      image[x][tree->minY] = 255; // 255 = white
      //Exception so we don't over-step our bounds
      if(tree->maxY != root->maxY)
	image[x][tree->maxY] = 255;
      else
	image[x][tree->maxY-1] = 255;
    }
    for(int y = tree->minY; y < tree->maxY; y++ ) {
      image[tree->minX][y] = 255; // 255 = white
      //Exception so we don't over-step our bounds
      if(tree->maxX != root->maxX)
	image[tree->maxX][y] = 255;
      else
	image[tree->maxX-1][y] = 255;
    }
  } else {
    whiteLines(tree->NW);
    whiteLines(tree->NE);
    whiteLines(tree->SE);
    whiteLines(tree->SW);
  }
}

/** 
 * getNewImg, returns the byte* storing newImg, the
 * 1D array with our quadtree decoding
**/
byte* quadtree::getNewImg() {
  return newImg;
}

/**
 * getLinesImg, returns the byte* storing linesImg, the
 * 1D array with a normal image but white lines around 
 * the quadtree leafs.
**/
byte* quadtree::getLinesImg() {
  return linesImg;
}

/**
 * convertQuad, takes our 2D array and inserts it into
 * a reversed 1D array (how it was given to us)
**/
void quadtree::convertQuad() {
  for(int i = 0; i < (root->maxX * root->maxY);i++) {
    newImg[i] = temp[(i%root->maxX)][(root->maxY-(i/root->maxX))-1];
  }
}

/**
 * convertLines, takes our 2D array and inserts it into
 * a reversed 1D array (how it was given to us)
**/
void quadtree::convertLines() {
  for(int i = 0; i < (root->maxX * root->maxY);i++) {
    linesImg[i] = image[(i%root->maxX)][(root->maxY-(i/root->maxX))-1];
  }
}
