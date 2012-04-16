#ifndef MMATRIX_H
#define MMATRIX_H

/// GraphicsTown2006 - CS559 Sample Code
// written by Michael Gleicher - originally written Fall, 2000
// updated Fall, 2005

//C// Copyright (c) 1992, 1994, 1997, 2000, 2005 Michael Gleicher

// GraphicsTown - CS559-2000, sample solution
//
// a matrix "class" taken from my old code. ancient and embarassing,
// but useful.
// all references specific to Snap-Together math have been removed tho

// this began as....
/* support for SGI's matrix class */
/* typedef float Matrix[4][4]; */
/*   since it's a typedef, we can't make it a class */

// this dates back to the original Iris GL. With OpenGL, I do not
// think that Matrix still exists as an entity - instead you just pass
// a pointer to 16 floats. Probably much easier in the long run. But
// back in the old days, a matrix was a matrix!

// this line is straight out of the original GL (RIP)
typedef float Matrix[4][4];

 void idMatrix(Matrix);
 void copyMatrix(Matrix,Matrix);
 void multMatrix(Matrix,Matrix,Matrix);

// transformMatrix means transform point (2D, 3D, 4D) by a Matrix
 void transformMatrix(Matrix,float,float,float&,float&); 
 void transformMatrix(Matrix,float,float,float,float&,float&,float&); 
 void transformMatrix(Matrix,float,float,float,float,float&,float&,float&,float&); 
 void transformMatrixNH(Matrix,float,float,float,float&,float&,float&); 
 void transformTransposeMatrix(Matrix,float,float,float,float&,float&,float&); 

// matrix utils
 int invertMatrix(Matrix, int printErrors=1);
 void printMatrix(Matrix);

// build various different matrices useful in graphics
 void rotMatrix(Matrix,float);
 void rotMatrix(Matrix,char,float);
 void transMatrix(Matrix,float,float);
 void transMatrix(Matrix,float,float,float);

extern  Matrix scrMat;

 void postMultiply(Matrix,float[4],float[4]);
 void preMultiply(float[4],Matrix,float[4]);
 float dot4(float[4],float[4]);
 void print4(float[4]);

 void quaternionMatrix(float,float,float,float,Matrix);

 // another useful thing
 inline float ABS(const float x) {  return( ((x) >= 0) ? (x) : -(x)); }

// since we didn't have 3 vectors..
void cross(float ax, float ay, float az, float bx, float by, float bz,
		   float& cx, float& cy, float& cz);
void normalize(float* v);
void scale(float*, float);
// a hack...
void orthogonalize(Matrix);

// $Header: /p/course/cs559-gleicher/private/CVS/GrTown/MMatrix.H,v 1.3 2008/11/11 03:48:23 gleicher Exp $

#endif /* MATRIX_H */
