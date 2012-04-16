/// GraphicsTown2006 - CS559 Sample Code
// written by Michael Gleicher - originally written Fall, 2000
// updated Fall, 2005
//
#include "GrTown_PCH.H"

#include "DrawUtils.H"
#include "MMatrix.H"

// texture coords...
static float tco[4][2] = {
  {0,0}, {1,0}, {1,1}, {0,1}
};

// Use this for debugging - it is helpful to see if your polys are
// oriented correctly
// we give it texture coordinate, but no promises. basically, they are
// only right if you are drawing squares. That are clockwise
// #define DRAW_POLYGON_NORMALS
void polygon(int nv, ...)
{
#ifdef DRAW_POLYGON_NORMALS
  double cx=0,cy=0,cz=0;
#endif
  
  float ccw = 1;
  if (nv < 0) {
	nv = -nv;
	ccw = -1;
  } else glFrontFace(GL_CW);


  if (nv<3) {
	printf("too few vertices in polygon!\n");
	return;
  }
	
  va_list ap;
  va_start(ap,nv);

  // first 3 points are special...
  double p1[3],p2[3],p3[3];
  p1[0] = va_arg(ap,double); 
  p1[1] = va_arg(ap,double);
  p1[2] = va_arg(ap,double);
  p2[0] = va_arg(ap,double); 
  p2[1] = va_arg(ap,double);
  p2[2] = va_arg(ap,double);
  p3[0] = va_arg(ap,double); 
  p3[1] = va_arg(ap,double);
  p3[2] = va_arg(ap,double);

#ifdef DRAW_POLYGON_NORMALS
  cx = p1[0] + p2[0] + p3[0];
  cy = p1[1] + p2[1] + p3[1];
  cz = p1[2] + p2[2] + p3[2];
#endif

  // compute the normal
  float d1[3], d2[3];
  float n[3];
  d1[0] = (float) (p2[0] - p1[0]);
  d1[1] = (float) (p2[1] - p1[1]);
  d1[2] = (float) (p2[2] - p1[2]);
  d2[0] = (float) (p2[0] - p3[0]);
  d2[1] = (float) (p2[1] - p3[1]);
  d2[2] = (float) (p2[2] - p3[2]);
  cross(d1[0],d1[1],d1[2],d2[0],d2[1],d2[2],n[0],n[1],n[2]);
  normalize(n);
  if (ccw<0) 
	scale(n,-1);

  glNormal3f( n[0], n[1], n[2] );

  /*  printf("(%g %g %g) (%g %g %g) (%g %g %g) -> (%g %g %g)\n",
	  p1[0], p1[1], p1[2],
	  p2[0], p2[1], p2[2],
	  p3[0], p3[1], p3[2],
	  n[0],n[1],n[2]);
  */
  int tv = 0;
  glBegin(GL_POLYGON);
  glTexCoord2fv(tco[(tv++)%4]);
  glVertex3dv(p1);
  glTexCoord2fv(tco[(tv++)%4]);
  glVertex3dv(p2);
  glTexCoord2fv(tco[(tv++)%4]);
  glVertex3dv(p3);

  for(int i=3; i<nv; i++) {
	p3[0] = va_arg(ap,double); 
	p3[1] = va_arg(ap,double);
	p3[2] = va_arg(ap,double);
#ifdef DRAW_POLYGON_NORMALS
	cx += p3[0];
	cy += p3[1];
	cz += p3[2];
#endif
	glTexCoord2fv(tco[(tv++)%4]);
	glVertex3dv(p3);
  }
  glEnd();


#ifdef DRAW_POLYGON_NORMALS
  // draw a little normal vector
  cx /= ((double) nv);
  cy/= ((double) nv);
  cz/= ((double) nv);
  glColor3f(1,1,1);
  glLineWidth(3);
  glBegin(GL_LINES);
  glVertex3d(cx,cy,cz);
  glVertex3d(cx+8*n[0],cy+8*n[1],cz+8*n[2]);
  glEnd();
#endif

  va_end(ap);
  glFrontFace(GL_CCW);
}
void polygoni(int nv, ...)
{
#ifdef DRAW_POLYGON_NORMALS
  double cx=0,cy=0,cz=0;
#endif
  float ccw = 1;
  if (nv < 0) {
	nv = -nv;
	ccw = -1;
  } else glFrontFace(GL_CW);


  if (nv<3) {
	printf("too few vertices in polygon!\n");
	return;
  }
	
  va_list ap;
  va_start(ap,nv);

  // first 3 points are special...
  double p1[3],p2[3],p3[3];
  p1[0] = va_arg(ap,int); 
  p1[1] = va_arg(ap,int);
  p1[2] = va_arg(ap,int);
  p2[0] = va_arg(ap,int); 
  p2[1] = va_arg(ap,int);
  p2[2] = va_arg(ap,int);
  p3[0] = va_arg(ap,int); 
  p3[1] = va_arg(ap,int);
  p3[2] = va_arg(ap,int);

  // compute the normal
  float d1[3], d2[3];
  float n[3];
  int tv = 0;
  d1[0] = (float) (p2[0] - p1[0]);
  d1[1] = (float) (p2[1] - p1[1]);
  d1[2] = (float) (p2[2] - p1[2]);
  d2[0] = (float) (p2[0] - p3[0]);
  d2[1] = (float) (p2[1] - p3[1]);
  d2[2] = (float) (p2[2] - p3[2]);
  cross(d1[0],d1[1],d1[2],d2[0],d2[1],d2[2],n[0],n[1],n[2]);
  normalize(n);
  if (ccw<0) 
	scale(n,-1);

#ifdef DRAW_POLYGON_NORMALS
  cx = p1[0] + p2[0] + p3[0];
  cy = p1[1] + p2[1] + p3[1];
  cz = p1[2] + p2[2] + p3[2];
#endif
  glNormal3f( n[0], n[1], n[2] );

  /*  printf("(%g %g %g) (%g %g %g) (%g %g %g) -> (%g %g %g)\n",
	  p1[0], p1[1], p1[2],
	  p2[0], p2[1], p2[2],
	  p3[0], p3[1], p3[2],
	  n[0],n[1],n[2]);
  */
  glBegin(GL_POLYGON);
  glTexCoord2fv(tco[(tv++)%4]);
  glVertex3dv(p1);
  glTexCoord2fv(tco[(tv++)%4]);
  glVertex3dv(p2);
  glTexCoord2fv(tco[(tv++)%4]);
  glVertex3dv(p3);

  for(int i=3; i<nv; i++) {
	p3[0] = va_arg(ap,int); 
	p3[1] = va_arg(ap,int);
	p3[2] = va_arg(ap,int);
#ifdef DRAW_POLYGON_NORMALS
	cx += p3[0];
	cy += p3[1];
	cz += p3[2];
#endif
	glTexCoord2fv(tco[(tv++)%4]);
	glVertex3dv(p3);
  }
  glEnd();

#ifdef DRAW_POLYGON_NORMALS
  // draw a little normal vector
  glColor3f(1,1,1);
  glLineWidth(3);
  glBegin(GL_LINES);
  cx /= ((double) nv);
  cy/= ((double) nv);
  cz/= ((double) nv);
  glVertex3d(cx,cy,cz);
  glVertex3d(cx+8*n[0],cy+8*n[1],cz+8*n[2]);
  glEnd();
#endif
  va_end(ap);
  glFrontFace(GL_CCW);
}

// draw a little cube
void cube(double x, double y, double z, double e)
{

	double s = e/2;
	polygon( 4,x-s,y-s,z-s, x+s,y-s,z-s, x+s,y+s,z-s, x-s,y+s,z-s);
	polygon(-4,x-s,y-s,z+s, x+s,y-s,z+s, x+s,y+s,z+s, x-s,y+s,z+s);

	polygon(-4,x-s,y-s,z-s, x-s,y-s,z+s, x-s,y+s,z+s, x-s,y+s,z-s);
	polygon( 4,x+s,y-s,z-s, x+s,y-s,z+s, x+s,y+s,z+s, x+s,y+s,z-s);

	polygon( 4,x-s,y-s,z-s, x-s,y-s,z+s, x+s,y-s,z+s, x+s,y-s,z-s);
	polygon(-4,x-s,y+s,z-s, x-s,y+s,z+s, x+s,y+s,z+s, x+s,y+s,z-s);

}

//**********************************************************************//
Color::Color(float R, float G, float B, float A)
{
  if (R>1) r = R/255.0f; else r=R;
  if (G>1) g = G/255.0f; else g=G;
  if (B>1) b = B/255.0f; else b=B;
  if (A>1) a = A/255.0f; else a=A;
}
Color::Color(Color& color)
{
  r = color.r;
  g = color.g;
  b = color.b;
  a = color.a;
}
void Color::operator() (float R, float G, float B, float A)
{
  if (R>1) r = R/255.0f; else r=R;
  if (G>1) g = G/255.0f; else g=G;
  if (B>1) b = B/255.0f; else b=B;
  if (A>1) a = A/255.0f; else a=A;
}
void Color::operator ()(int* c)
{
	r = ((float)c[0])/255.f;
	g = ((float)c[1])/255.f;
	b = ((float)c[2])/255.f;
}

//////////////////////////////////////////////////////////////
void lookatMatrix(float ex, float ey, float ez, 
				  float ax, float ay, float az, Matrix r)
{
  glPushMatrix();
  glLoadIdentity();
  gluLookAt(ex,ey,ez,ax,ay,az,0,1,0);
  glGetFloatv(GL_MODELVIEW_MATRIX,(float*) &r[0][0]);
  glPopMatrix();
}


// $Header: /p/course/cs559-gleicher/private/CVS/GrTown/DrawUtils.cpp,v 1.2 2007/11/01 21:23:41 gleicher Exp $
