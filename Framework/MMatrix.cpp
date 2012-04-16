/// GraphicsTown2006 - CS559 Sample Code
// written by Michael Gleicher - originally written Fall, 2000
// updated Fall, 2005

// a matrix "class" taken from my old code. ancient and embarassing,
// but useful - beware, this stuff is circa 1990!
// all references specific to Snap-Together math have been removed tho
// this is only useful if you are building in our application

#include "GrTown_PCH.H"

/* support for SGI's matrix class */
/* typedef float Matrix[4][4]; */
/*   since it's a typedef, we can't make it a class */

// this dates back to the original Iris GL. With OpenGL, I do not
// think that Matrix still exists as an entity - instead you just pass
// a pointer to 16 floats. Probably much easier in the long run. But
// back in the old days, a matrix was a matrix!

// STMness stripped out

#include <stdio.h>

#include "MMatrix.H"

// Matrix identityMatrix = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
Matrix scrMat;

void idMatrix(Matrix m)
{
  int x,y;

  for (x=0; x<4; x++)
    for (y=0; y<4; y++)
      m[x][y] = ((x==y) ? 1.0f : 0.0f);
}

void copyMatrix(Matrix m1,Matrix m2)
{
  int x;
  float *a,*b;

  for (x=0,a=&(m1[0][0]),b=&(m2[0][0]);x<16;x++)
    *b++ = *a++;
}

void printMatrix(Matrix m)
{
  int r,c;
  for (r=0; r<4; r++,printf("\n"))
    for (c=0; c<4; c++)
      printf("%7.4f ",m[r][c]);
}

void multMatrix(Matrix a, Matrix b, Matrix f)
{
  int r,c,i;
  for(r=0; r<4; r++)
    for(c=0;c<4;c++)
      for(i=0,f[r][c]=0;i<4;i++)
	f[r][c] += a[r][i] * b[i][c];
}
void transformMatrix(Matrix m,float x,float y,float& ix,float& iy)
{
  ix = m[0][0] * x + m[1][0] * y + m[3][0];
  iy = m[0][1] * x + m[1][1] * y + m[3][1];
}
void transformMatrix(Matrix m,
		     float x,float y,float z,
		     float& ix,float& iy, float& iz)
{
  ix = m[0][0] * x + m[1][0] * y + m[2][0] *z + m[3][0];
  iy = m[0][1] * x + m[1][1] * y + m[2][1] *z + m[3][1];
  iz = m[0][2] * x + m[1][2] * y + m[2][2] *z + m[3][2];
}
void transformMatrix(Matrix m,
		     float x,float y,float z,float w,
		     float& ix,float& iy, float& iz, float& iw)
{
  ix = m[0][0] * x + m[1][0] * y + m[2][0] *z + m[3][0]*w;
  iy = m[0][1] * x + m[1][1] * y + m[2][1] *z + m[3][1]*w;
  iz = m[0][2] * x + m[1][2] * y + m[2][2] *z + m[3][2]*w;
  iw = m[0][3] * x + m[1][3] * y + m[2][3] *z + m[3][3]*w;
}
void transformMatrixNH(Matrix m,
		       float x,float y,float z,
		       float& ix,float& iy, float& iz)
{
  ix = m[0][0] * x + m[1][0] * y + m[2][0] *z;
  iy = m[0][1] * x + m[1][1] * y + m[2][1] *z;
  iz = m[0][2] * x + m[1][2] * y + m[2][2] *z;
}
void transformTransposeMatrix(Matrix m,
			      float x,float y,float z,
			      float& ix,float& iy, float& iz)
{
  ix = m[0][0] * x + m[0][1] * y + m[0][2] *z + m[0][3];
  iy = m[1][0] * x + m[1][1] * y + m[1][2] *z + m[1][3];
  iz = m[2][0] * x + m[2][1] * y + m[2][2] *z + m[2][3];
}

void rotMatrix(Matrix m, float th)
{
  idMatrix(m);
  float s = (float) sin(th);
  float c = (float) cos(th);

  m[0][0] = c;
  m[0][1] = -s;
  m[1][0] = s;
  m[1][1] = c;
}
void rotMatrix(Matrix m, char ax, float th)
{
  idMatrix(m);
  float s = (float) sin(th);
  float c = (float) cos(th);

	switch(toupper(ax)) {
		case 'Z' :
		  m[0][0] = c;
		  m[0][1] = -s;
		  m[1][0] = s;
		  m[1][1] = c;
		  break;
		case 'Y' :
		  m[0][0] = c;
		  m[0][2] = -s;
		  m[2][0] = s;
		  m[2][2] = c;
		  break;
		case 'X' :
		  m[1][1] = c;
		  m[1][2] = -s;
		  m[2][1] = s;
		  m[2][2] = c;
		  break;
		default:
		  printf("Bad axis to rotMatrix");
	}

}
void transMatrix(Matrix m,float x,float y)
{
  idMatrix(m);
  m[3][0] = (float) x;
  m[3][1] = (float) y;
}
void transMatrix(Matrix m,float x,float y, float z)
{
  idMatrix(m);
  m[3][0] = (float) x;
  m[3][1] = (float) y;
  m[3][2] = (float) z;
}

/* numerical recipes in C gauss jordan solver, re-written for C++ and */
 /* my data structures */

#define SWAP(a,b) {float temp=(a);(a)=(b);(b)=temp;}
    
/* make the dynamically allocated space a global variable so it can be */
 /* allocated exactly once */
static int *indxc=0,*indxr=0,*ipiv=0;
static float **a;

int invertMatrix(Matrix m, int printErrors)
{
  int ok = 1;
  int i,icol=0,irow=0,j,k,l,ll;
  float big,dum,pivinv;
  
  int n=4;

  if (indxc==0) {
    indxr = new int[4];
    indxc = new int[4];
    ipiv  = new int[4];
    a     = new float*[4];
    if (!a || !ipiv) printf("new fails - memory problem\n");
  }
  for (j=0;j<n;j++) a[j] = &(m[j][0]);

  for (j=0;j<n;j++) ipiv[j]=0;
  for (i=0;i<n;i++) {
    big=0.0;
    for (j=0;j<n;j++)
      if (ipiv[j] != 1)
        for (k=0;k<n;k++) {
          if (ipiv[k] == 0) {
            if (fabs(a[j][k]) >= big) {
              big=ABS(a[j][k]);
              irow=j;
              icol=k;
            }
          } else if (ipiv[k] > 1) {
          		if (printErrors) printf("GAUSSJ: Singular Matrix-1\n");
          		ok =0;
          	}
        }
    ++(ipiv[icol]);
    if (irow != icol) {
      for (l=0;l<n;l++) SWAP(a[irow][l],a[icol][l])
    }
    indxr[i]=irow;
    indxc[i]=icol;
    if (a[icol][icol] == 0.0) {
    	ok =0;
    	if (printErrors) printf("GAUSSJ: Singular Matrix-2");
    }
    pivinv=1.0f/a[icol][icol];
    a[icol][icol]=1.0;
    for (l=0;l<n;l++) a[icol][l] *= pivinv;
    for (ll=0;ll<n;ll++)
      if (ll != icol) {
        dum=a[ll][icol];
        a[ll][icol]=0.0;
        for (l=0;l<n;l++) a[ll][l] -= a[icol][l]*dum;
      }
  }
  for (l=n-1;l>=0;l--) {
    if (indxr[l] != indxc[l])
      for (k=0;k<n;k++)
        SWAP(a[k][indxr[l]],a[k][indxc[l]]);
  }
  return ok;
}

float dot4(float a[4], float b[4])
{
  float o=0;
  for (int i=0; i<4; i++)
    o+= a[i]*b[i];
  return o;
}

void postMultiply(Matrix m,
		  float   a[4],
		  float   r[4])
{
  for (int i=0; i<4; i++) {
    r[i] = 0;
    for (int j=0; j<4; j++)
      r[i] += m[i][j] * a[j];
  }
}

void preMultiply(float a[4],
		 Matrix m,
		 float r[4])
{
  for (int i=0; i<4; i++) {
    r[i] = 0;
    for (int j=0; j<4; j++)
      r[i] += m[j][i] * a[j];
  }
}

void print4(float a[4])
{
  printf("[%5.2f %5.2f %5.2f %5.2f]\n",a[0],a[1],a[2],a[3]);
}

void quaternionMatrix(float w, float x, float y, float z, Matrix m)
{
  float mag = (float) sqrt(x*x+y*y+z*z+w*w);
  float nx = (float) (x/mag);
  float ny = (float) (y/mag);
  float nz = (float) (z/mag);
  float nw = (float) (w/mag);

  m[0][0]=1- 2*ny*ny - 2*nz*nz; m[0][1]=   2*nx*ny + 2*nw*nz; m[0][2]=  2*nx*nz-2*nw*ny;
  m[1][0]=   2*nx*ny - 2*nw*nz; m[1][1]=1- 2*nx*nx - 2*nz*nz; m[1][2]=  2*ny*nz+2*nw*nx;
  m[2][0]=   2*nx*nz + 2*nw*ny; m[2][1]=   2*ny*nz - 2*nw*nx; m[2][2]=1-2*nx*nx-2*ny*ny;
  m[3][0]=m[3][1]=m[3][2]=m[0][3]=m[1][3]=m[2][3]=0;
  m[3][3]=1;
}

void cross(float ax, float ay, float az, float bx, float by, float bz,
		   float& cx, float& cy, float& cz)
{
  cx = bz * ay - by * az;
  cy = bx * az - bz * ax;
  cz = by * ax - bx * ay;
}

void normalize(float* n)
{
  float l = (float) sqrt(n[0]*n[0]+n[1]*n[1]+n[2]*n[2]);
  if (l < .00001) {
  } else
	scale(n,1/l);
}
void scale(float* n, float v)
{
  n[0] *= v;
  n[1] *= v;
  n[2] *= v;
}

void orthogonalize(Matrix m)
{
  // normalize z
  float zl = (float) sqrt(m[2][0]*m[2][0] + m[2][1]*m[2][1] +
						  m[2][2]*m[2][2]);
  if (zl < .00001) {
	m[2][0] = 0;
	m[2][1] = 0;
	m[2][2] = 1;
  } else {
	m[2][0] /= zl;
	m[2][1] /= zl;
	m[2][2] /= zl;
  }
  float xl = (float) sqrt(m[0][0]*m[0][0] + m[0][1]*m[0][1] +
						  m[0][2]*m[0][2]);
  if (xl < .00001) {
	m[0][0] = 1;
	m[0][1] = 0;
	m[0][2] = 0;
  } else {
	m[0][0] /= xl;
	m[0][1] /= xl;
	m[0][2] /= xl;
  }
  cross(m[2][0],m[2][1],m[2][2], m[0][0],m[0][1],m[0][2],
		m[1][0],m[1][1],m[1][2]);
  cross(m[1][0],m[1][1],m[1][2], m[2][0],m[2][1],m[2][2],
		m[0][0],m[0][1],m[0][2]);
}


// $Header: /p/course/cs559-gleicher/private/CVS/GrTown/MMatrix.cpp,v 1.2 2007/11/01 21:25:15 gleicher Exp $
