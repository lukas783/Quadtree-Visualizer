/*
	***** BMPdisplay.cpp*****

Read in and display a BMP image file using OpenGL graphics, in 24-bit color and 8-bit monochrome.

Author: John M. Weiss, Ph.D.
Class: CSC300 Data Structures, Fall 2015 (sample code for PA#2)

To compile:	g++ BMPdisplay.cpp BMPload.cpp -lglut -lGLU -lGL

Modifications: 
10/14/2015 - Small modifications including some more output and initializing of the quadtree class
10/21/2015 - Added in mouseclick function and some global variables. Fudge factor can now be changed
             on the fly using +/- or the mouse scroll wheel.


*/

// include files
#include "quadtree.h"

using namespace std;

// the GLUT header automatically includes gl.h and glu.h
#include <GL/glut.h>

// symbolic constants
const int EscapeKey = 27;

// typedefs
typedef unsigned char byte;

// global vars
int ScreenWidth  = 1024;
int ScreenHeight =  768;

byte* BMPimage;		// array of RGB pixel values (range 0 to 255)
byte* image;        // array of bytes to store monochrome image (for quadtree encoding)
int nrows, ncols;   // image dimensions

// quadtree class declaration
quadtree q;

// boolean for whether spacebar has been pressed
bool space = false;
byte fudge;

// OpenGL callback function prototypes
void display( void );
void reshape( int w, int h );
void keyboard( unsigned char key, int x, int y );
void mouse(int button, int state, int x, int y);

// other function prototypes
void initOpenGL( const char *filename, int nrows, int ncols );
bool LoadBmpFile( const char* filename, int &nrows, int &ncols, byte* &image );
void displayColor( int x, int y, int w, int h, byte *image );
void displayMonochrome( int x, int y, int w, int h, byte *image );
void drawText(char *string, int x, int y);
void drawLine(float x1, float y1, float x2, float y2);

/******************************************************************************/

// main() function
int main( int argc, char *argv[] )
{
    if ( argc < 3 )
    {
        cerr << "Usage: quadtree image.bmp range\n";
        return -1;
    }

    // read image file
    if ( !LoadBmpFile( argv[1], nrows, ncols, BMPimage ) )
    {
        cerr << "Error: unable to load " << argv[1] << endl;
        return -1;
    }
    cerr << "reading " << argv[1] << ": " << nrows << " x " << ncols << endl;

    // convert 24-bit color BMP image to 8-bit monochrome image
    image = new byte [nrows * ncols ];
    byte* BMPptr = BMPimage, *imageptr = image;
    cerr << (nrows*ncols) << " 8-bit pixels in image (" <<(nrows*ncols)<< " bytes).\n";
    for ( int row = 1; row <= nrows; row++ )
        for ( int col = 0; col < ncols; col++ )
        {
	  *imageptr++ = 0.30 * BMPptr[0] + 0.59 * BMPptr[1] + 0.11 * BMPptr[2] + 0.5;
	  BMPptr += 3;
        }
    // Since the image array is now populated we can create the quadtree
    fudge = stoi(argv[2]);
    q.tree(image, nrows, ncols, fudge);

    // perform various OpenGL initializations
    glutInit( &argc, argv );
    initOpenGL( argv[1], nrows, ncols );

    // go into OpenGL/GLUT main loop, never to return
    glutMainLoop();

    // yeah I know, but it keeps compilers from bitching
    return 0;
}

/******************************************************************************/

// various commands to initialize OpenGL and GLUT
void initOpenGL( const char *filename, int nrows, int ncols )
{
    glutInitDisplayMode( GLUT_RGBA | GLUT_SINGLE );	        // 32-bit graphics and single buffering

    ScreenWidth = 2 * ncols;
    ScreenHeight = nrows+40; // we add 30 to add in text above images
    glutInitWindowSize( ScreenWidth, ScreenHeight );	    // initial window size
    glutInitWindowPosition( 100, 50 );			            // initial window  position
    glutCreateWindow( filename );			                // window title

    glClearColor( 0.0, 0.0, 0.0, 0.0 );			            // use black for glClear command

    // callback routines
    glutDisplayFunc( display );				// how to redisplay window
    glutReshapeFunc( reshape );				// how to resize window
    glutKeyboardFunc( keyboard );			// how to handle key presses
    glutMouseFunc( mouse );
}

/******************************************************************************/
/*                          OpenGL callback functions                         */
/******************************************************************************/

// callback function that receives input from the mouse
void mouse(int button, int state, int x, int y) {
  
  // button 3 is mouse scroll up
  if(button == 3 && state == 1) {
    if(fudge < 255) {
      fudge++;
      q.tree(image, nrows, ncols, fudge);
      glutPostRedisplay();
    }
    // button 4 is mouse scroll down
  } else if(button == 4 && state == 1) {
    if(fudge > 0) {
      fudge--;
      q.tree(image, nrows, ncols, fudge);
      glutPostRedisplay();
    }
    //button 1 is mouse scroll click
  } else if(button == 1 && state == 1) {
    space = !space;
    glutPostRedisplay();
  }
  
}


// callback function that tells OpenGL how to redraw window
void display( void )
{
    // clear the display
    glClear( GL_COLOR_BUFFER_BIT );

    // display normal image in B&W
    displayMonochrome( 0, 0, ncols, nrows, image );
    // and image with quadtree compression
    if(space == true) 
      displayMonochrome( ncols, 0, ncols, nrows, q.getLinesImg() );
    else {
      displayMonochrome( ncols, 0, ncols, nrows, q.getNewImg() );
      drawLine(ncols, 0, ncols, nrows);
    }
      
    //Here we add lines and text and whatnot to tell user what each item is
    //and give seperation between image and image
    drawText((char*)"Original Image", (ncols/3)+20, (nrows+8));
    drawText((char*)"Quadtree Image (spacebar toggles quads)", ((ncols/3) + ncols)-20, (nrows + 20));
    drawText((char*)"Mouse scroll or +/- to change epsilon.", ((ncols/3) + ncols)-10, (nrows + 5));
    

    // flush graphical output
    glFlush();
}

/******************************************************************************/

// callback function that tells OpenGL how to resize window
// note that this is called when the window is first created
void reshape( int w, int h )
{
    // store new window dimensions globally
    ScreenWidth = w;
    ScreenHeight = h;

    // how to project 3-D scene onto 2-D
    glMatrixMode( GL_PROJECTION );		// use an orthographic projection
    glLoadIdentity();				// initialize transformation matrix
    gluOrtho2D( 0.0, w, 0.0, h );
    glViewport( 0, 0, w, h );			// adjust viewport to new window
}

/******************************************************************************/

// callback function that tells OpenGL how to handle keystrokes
void keyboard( unsigned char key, int x, int y )
{
    switch ( key )
      {
	// Escape quits program
      case EscapeKey:
	exit( 0 );
	break;
	// Spacebar switches between quadtree and normal
      case 32: 
	space = !space;
	glutPostRedisplay();
	break;
	// + will inc fudge factor and re-build the quadtree
      case 43:
	if(fudge < 255) {
	  fudge++;
	  q.tree(image, nrows, ncols, fudge);
	  glutPostRedisplay();
	}
	break;
	// - will dec fudge factor and re-build the quadtree
      case 45:
	if(fudge > 0) {
	  fudge--;
	  q.tree(image, nrows, ncols, fudge);
	  glutPostRedisplay();
	}
	break;
	// anything else redraws window
      default:
	glutPostRedisplay();
	break;
    }
}

/******************************************************************************/

// display 24-bit color image
void displayColor( int x, int y, int w, int h, byte* image )
{
    glRasterPos2i( x, y );
    glDrawPixels( w, h, GL_RGB, GL_UNSIGNED_BYTE, BMPimage );
}

// display 8-bit monochrome image
void displayMonochrome( int x, int y, int w, int h, byte* image )
{
    glRasterPos2i( x, y );
    glDrawPixels( w, h, GL_LUMINANCE, GL_UNSIGNED_BYTE, image );
}

// Draws a text string onto our GUI
void drawText( char *str, int x, int y ) {
  glColor3f(1.0, 1.0, 1.0);
  glRasterPos2i( x, y );
  while ( *str )
    glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, *str++ );
}

// Draws a line onto our GUI
void drawLine(float x1, float y1, float x2, float y2) {
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_LINES);
  glVertex2f(x1, y1);
  glVertex2f(x2, y2);
  glEnd();
  glFlush();
}
