#ifndef DRAWUTILS_H
#define DRAWUTILS_H
/// GraphicsTown2006 - CS559 Sample Code
// written by Michael Gleicher - originally written Fall, 2000
// updated Fall, 2005

// some utilities for making drawing easier
#include "MMatrix.H"

// draw a polygon given the points - automatically computes the normal
// for you, and makes sure that the handedness is correct for backface
// removal 
// negative number of polygons means clockwise, not ccw
//
// takes lots and lots of doubles...
// (yes, doubles - since thats what varargs likes)
// be very careful - varargs can't cast - so if you accidentally send
// it an int, it will barf
void polygon(int nv, ...);
// because doubles are a pain...
void polygoni(int nv, ...);

// notice this takes doubles so it is easier to use polygon
void cube(double x, double y, double z, double s);

// colors are R,G,B 0->1
// if you get RGB>1, assume that its 0-255 and convert
class Color {
public:
  Color(float R=0, float G=0, float B=0, float A=1);
  Color(Color&);

  float r,g,b,a;
  void operator() (float R=0, float G=0, float B=0, float A=1);
  void operator() (int*);
};

const float pi = 3.1415927f; // windows won't give me one of these


// sometimes we need to build a LookAt matrix. this calls
// glu and grabs the result...
void lookatMatrix(float ex, float ey, float ez, 
				  float ax, float ay, float az, Matrix r);

// $Header: /p/course/cs559-gleicher/private/CVS/GrTown/DrawUtils.H,v 1.2 2007/11/01 21:23:40 gleicher Exp $

#endif
