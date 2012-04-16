/// GraphicsTown2006 - CS559 Sample Code
// written by Michael Gleicher - originally written Fall, 2000
// updated Fall, 2005
// updated Fall, 2006 - new texture manager, improvements for behaviors

// the simple example objects - not that exciting - yours should be
// better! 

// note - because this is a precompiled header, getting the name the same
// is more important than getting the path right
#include "../GrTown_PCH.H"
#include "../DrawUtils.H"
#include "../Utilities/Texture.H"
#include "../MMatrix.H"

#include "Suburbs.H"

/***********************************************************************/
// a Really simple tree - just a cone with a stem...
// use normals to make it look rounder...
SimpleTree1::SimpleTree1(float h, float r)
  : height(h), radius(r)
{
}
void SimpleTree1::draw(DrawingState*)
{
  // cone for the body
  glFrontFace(GL_CW);
  glColor3f(0.f,.6f,.3f);
  glBegin(GL_TRIANGLE_FAN);
  glNormal3f(0,1,0);
  glVertex3f(0,height,0);
  glNormal3f(1,0,0);
  glVertex3f(radius,height/4,0);
  glNormal3f(0,0,1);
  glVertex3f(0,height/4,radius);
  glNormal3f(-1,0,0);
  glVertex3f(-radius,height/4,0);
  glNormal3f(0,0,-1);
  glVertex3f(0,height/4,-radius);
  glNormal3f(1,0,0);
  glVertex3f(radius,height/4,0);
  glEnd();
  glFrontFace(GL_CCW);
  // stem
  glColor3f(.6f,.5f,.3f);
  glBegin(GL_QUAD_STRIP);
  glNormal3f(1,0,0);
  glVertex3f(radius/5,0,0);
  glVertex3f(radius/5,height/4,0);
  glNormal3f(0,0,1);
  glVertex3f(0,0,radius/5);
  glVertex3f(0,height/4,radius/5);
  glNormal3f(-1,0,0);
  glVertex3f(-radius/5,0,0);
  glVertex3f(-radius/5,height/4,0);
  glNormal3f(0,0,-1);
  glVertex3f(0,0,-radius/5);
  glVertex3f(0,height/4,-radius/5);
  glNormal3f(1,0,0);
  glVertex3f(radius/5,0,0);
  glVertex3f(radius/5,height/4,0);
  glEnd();
}

/***********************************************************************/
// draw a patch of lawn - use a repeating texture
Lawn::Lawn(float xi1, float zi1, float xi2, float zi2)
  : x1(xi1), z1(zi1), x2(xi2), z2(zi2)
{
}

void Lawn::draw(DrawingState* d)
{
  // the catch here is that we need to use a polygon offset to draw
  // the lawn just above the ground...
  if (d->drGrTex)
	fetchTexture("grass.png",true,true);
  else
    glBindTexture(GL_TEXTURE_2D,0);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(-2.,-2.);
  glNormal3f(0,1,0);
  glColor3f(0,1,1);
  glBegin(GL_POLYGON);
  glTexCoord2f(0,(z2-z1)/4.f);            glVertex3f(x1,0,z2);
  glTexCoord2f((x2-x1)/4.f, (z2-z1)/4.f); glVertex3f(x2,0,z2);
  glTexCoord2f((x2-x1)/4.f,0);            glVertex3f(x2,0,z1);
  glTexCoord2f(0,0);                      glVertex3f(x1,0,z1);
  glEnd();
  glDisable(GL_POLYGON_OFFSET_FILL);
  glBindTexture(GL_TEXTURE_2D,0);
}


/***********************************************************************/
/* simplest possible house */
SimpleHouse1::SimpleHouse1()
{
  color(.6f,.7f,.8f);
}
void SimpleHouse1::draw(DrawingState*)
{
  glColor3fv(&color.r);
  fetchTexture("simpleHouse1-front.png");
  glBegin(GL_POLYGON);
  glNormal3f(0,0,-1);
  glTexCoord2f(0,0); glVertex3i(-20, 0, -25);
  glTexCoord2f(0,1); glVertex3i(-20,30, -25);
  glTexCoord2f(.5,1.5); glVertex3i(  0,45,-25);
  glTexCoord2f(1,1); glVertex3i( 20,30,-25);
  glTexCoord2f(1,0); glVertex3i( 20, 0,-25);
  glEnd();
  glBegin(GL_POLYGON);
  glNormal3f(0,0,1);
  glTexCoord2f(0,0); glVertex3i(-20, 0,   25);
  glTexCoord2f(1,0); glVertex3i( 20, 0,   25);
  glTexCoord2f(1,1); glVertex3i( 20,30,   25);
  glTexCoord2f(.5,1.5); glVertex3i(  0,45,25);
  glTexCoord2f(0,1); glVertex3i(-20,30,   25);
  glEnd();
  glBindTexture(GL_TEXTURE_2D,0);
  //  polygoni( 5, -20,0,-25,  20,0,-25,   20, 30,-25,   0,45,-25, -20,30,-25);
  //polygoni( 5, -20,0, 25,  20,0, 25,   20, 30, 25,   0,45, 25, -20,30, 25);
  polygoni( 4, 20, 0,-25,  20, 0, 25,  20, 30, 25,  20, 30, -25);
  polygoni(-4,-20, 0,-25, -20, 0, 25, -20, 30, 25, -20, 30, -25);
  polygoni( 4, 20,30,-25,  20,30, 25,   0, 45, 25,   0, 45, -25);
  polygoni(-4, -20,30,-25, -20,30, 25,   0, 45, 25,   0, 45, -25);
}
/***********************************************************************/
/* simplest possible house */
SimpleHouse2::SimpleHouse2()
{
  color(.6f,.7f,.8f);
  rotMatrix(transform,'Y',3.14159f/2.f);
}
void SimpleHouse2::draw(DrawingState*)
{
  glColor3fv(&color.r);
  polygoni( 5, -20,0,-30, 20,0,-30, 20,30,-30, 0,45,-30, -20,30,-30);
  polygoni(-5, -20,0, 30, 20,0, 30, 20,30, 30, 0,45, 30, -20,30, 30);
  fetchTexture("simpleHouse2-front.png");
  glBegin(GL_POLYGON);
  glNormal3f(1,0,0);
  glTexCoord2f(0,1); glVertex3i( 20,30,-30);
  glTexCoord2f(1,1); glVertex3i( 20,30, 30);
  glTexCoord2f(1,0); glVertex3i( 20, 0, 30);
  glTexCoord2f(0,0); glVertex3i( 20, 0,-30);
  glEnd();
  glBegin(GL_POLYGON);
  glNormal3f(-1,0,0);
  glTexCoord2f(0,0); glVertex3i(-20, 0,-30);
  glTexCoord2f(1,0); glVertex3i(-20, 0, 30);
  glTexCoord2f(1,1); glVertex3i(-20,30, 30);
  glTexCoord2f(0,1); glVertex3i(-20,30,-30);
  glEnd();
  glBindTexture(GL_TEXTURE_2D,0);
  glBindTexture(GL_TEXTURE_2D,0);
  fetchTexture("roof3.png");
  polygoni( 4, 20,30,-30,  20,30, 30,   0, 45, 30,   0, 45, -30);
  polygoni(-4,-20,30,-30, -20,30, 30,   0, 45, 30,   0, 45, -30);
}

/***********************************************************************/
// OK, this is even simpler...
SimpleHouse3::SimpleHouse3()
{
  color(.6f,.7f,.8f);
}
void SimpleHouse3::draw(DrawingState*)
{
  glColor3fv(&color.r);
  fetchTexture("simpleHouse2-front.png");
  polygoni(4, -20, 0,-20,  20, 0,-20,  20,30,-20,  -20,30,-20);
  fetchTexture("simpleHouse3-side.png");
  polygoni(4,  20, 0,-20,  20, 0, 20,  20,30, 20,   20,30,-20);
  fetchTexture("simpleHouse2-front.png");
  polygoni(4,  20, 0, 20, -20, 0, 20, -20,30, 20,   20,30, 20);
  fetchTexture("simpleHouse3-side.png");
  polygoni(4, -20, 0, 20, -20, 0,-20, -20,30,-20,  -20,30, 20);
  fetchTexture("roof3.png");
  polygoni(3, -20,30,-20,  20,30,-20,   0,45,0);
  polygoni(3,  20,30,-20,  20,30, 20,   0,45,0);
  polygoni(3,  20,30, 20, -20,30, 20,   0,45,0);
  polygoni(3, -20,30, 20, -20,30,-20,   0,45,0);
}


/***********************************************************************/
Church::Church() : GrObject("Church")
{
  color(.6f,.7f,.8f);
}
void Church::draw(DrawingState*)
{
  int xsz = 30;
  int zsz = 40;
  int hgh = 50;
  int top = 65;

  glColor3fv(&color.r);
  fetchTexture("church-front.png");
  glBegin(GL_POLYGON);
  glNormal3f(0,0,-1);
  glTexCoord2f(0,0); glVertex3i(-xsz, 0, -zsz);
  glTexCoord2f(0,1); glVertex3i(-xsz,hgh, -zsz);
  glTexCoord2f(.5,1.5); glVertex3i(  0,top,-zsz);
  glTexCoord2f(1,1); glVertex3i( xsz,hgh,-zsz);
  glTexCoord2f(1,0); glVertex3i( xsz, 0,-zsz);
  glEnd();
  glBegin(GL_POLYGON);
  glNormal3f(0,0,1);
  glTexCoord2f(0,0); glVertex3i(-xsz, 0,   zsz);
  glTexCoord2f(1,0); glVertex3i( xsz, 0,   zsz);
  glTexCoord2f(1,1); glVertex3i( xsz,hgh,   zsz);
  glTexCoord2f(.5,1.5); glVertex3i(  0,top,zsz);
  glTexCoord2f(0,1); glVertex3i(-xsz,hgh,   zsz);
  glEnd();
  glBindTexture(GL_TEXTURE_2D,0);

  polygoni( 4, xsz,hgh,-zsz,  xsz,hgh, zsz,   0, top, zsz,   0, top, -zsz);
  polygoni(-4, -xsz,hgh,-zsz, -xsz,hgh, zsz,   0, top, zsz,   0, top,
		   -zsz);

  fetchTexture("church-side.png");
  polygoni( 4, xsz, 0,-zsz,  xsz, 0, zsz,  xsz, hgh, zsz,  xsz, hgh, -zsz);
  polygoni(-4,-xsz, 0,-zsz, -xsz, 0, zsz, -xsz, hgh, zsz, -xsz, hgh, -zsz);


  glPushMatrix();
  glTranslatef(0,0,(float)zsz);
  int ss=10;					// steeple size
  int sh=60;					// height
  int th=120;					// top of point
  glBindTexture(GL_TEXTURE_2D,0);
  polygoni(4, -ss, 0,-ss,  ss, 0,-ss,  ss,sh,-ss,  -ss,sh,-ss);
  polygoni(4,  ss, 0,-ss,  ss, 0, ss,  ss,sh, ss,   ss,sh,-ss);
  polygoni(4,  ss, 0, ss, -ss, 0, ss, -ss,sh, ss,   ss,sh, ss);
  polygoni(4, -ss, 0, ss, -ss, 0,-ss, -ss,sh,-ss,  -ss,sh, ss);
  polygoni(3, -ss,sh,-ss,  ss,sh,-ss,   0,th,0);
  polygoni(3,  ss,sh,-ss,  ss,sh, ss,   0,th,0);
  polygoni(3,  ss,sh, ss, -ss,sh, ss,   0,th,0);
  polygoni(3, -ss,sh, ss, -ss,sh,-ss,   0,th,0);
  glPopMatrix();
}

/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
StreetLight::StreetLight(double p) : poleZ(p)
{
	radius = 10;
}
void StreetLight::draw(DrawingState*)
{
  glColor3f(.6f,.5f,.3f);
  glBegin(GL_QUAD_STRIP);
  glNormal3d( 1, 0,0);
  glVertex3d( 1, 0,poleZ);
  glVertex3d( 1,20,poleZ);
  glNormal3d( 0, 0,1);
  glVertex3d( 0, 0,poleZ+1);
  glVertex3d( 0,20,poleZ+1);
  glNormal3d(-1, 0,0);
  glVertex3d(-1, 0,poleZ);
  glVertex3d(-1,20,poleZ);
  glNormal3d( 0, 0,-1);
  glVertex3d( 0, 0,poleZ-1);
  glVertex3d( 0,20,poleZ-1);
  glNormal3d( 1, 0,0);
  glVertex3d( 1, 0,poleZ);
  glVertex3d( 1,20,poleZ);
  glEnd();
  glBegin(GL_QUAD_STRIP);
  glNormal3d(0,1,0);
  glVertex3d( 0,21, 0);
  glVertex3d( 0,21, poleZ);
  glNormal3d(-.707,-.707, 0);
  glVertex3d(-1,20, 0);
  glVertex3d(-1,20, poleZ);
  glNormal3d(.707,-.707, 0);
  glVertex3d( 1,20, 0);
  glVertex3d( 1,20, poleZ);
  glNormal3d(0,1,0);
  glVertex3d( 0,21, 0);
  glVertex3d( 0,21, poleZ);
	glEnd();
}

void StreetLight::drawAfter(DrawingState* d)
{
  int tod = d->timeOfDay;
  bool daytime;
  if (tod >=6 && tod <= 19)
	daytime = true;
  else 
	daytime = false;

  if (!daytime) {

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_DST_ALPHA);
	float cone_length = 20;
	float radius = 10;
	float theta,dtheta = pi/16;
	int c;

	float norm[3];
	float sum;
	// Beam One
	glPushMatrix();
			
	glFrontFace(GL_CW);
			
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(1,1,0,.8f);
	glVertex3f(0,cone_length,0);
	for ( c=0,theta = 0.0 ; c <= 32  ; theta += dtheta, c++ ) {
	  float z = sin(theta) * radius;
	  float x = cos(theta) * radius;
	  glColor4f(1,1,0,.2f);
	
	  norm[0] = 0; norm[1] = x; norm[2] = z; sum = norm[0] + norm[1] + norm[2];
	  norm[0] /= sum; norm[1] /= sum; norm[2] /= sum;				
	  glNormal3f( norm[0],norm[1],norm[2]);
	
	  glVertex3f( x, cone_length/6, z );
	}  
			
	glEnd();

			
	glPopMatrix();

	glDisable(GL_BLEND);
	glClearColor(1,1,1,1);

  }
}

/**********************************************************************/
Sign::Sign(float sxi, float syi, float hi, char* tex, 
		   SignShape ss)
  : sx(sxi), sy(syi), h(hi), texName(tex), shape(ss)
{
}
void Sign::draw(DrawingState*)
{
  glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
  fetchTexture(texName);
  glBegin(GL_POLYGON);
  glNormal3f(0,0,1);
  glColor3f(1,1,1);
  drawShape(0);
  glEnd();
  // back of the sign
  glBindTexture(GL_TEXTURE_2D,0);
  glNormal3f(0,0,-1);
  drawShape(-.1f);
  // post
  glBegin(GL_POLYGON);
    glNormal3f(0,0,1);
	glVertex3f(-.1f,h,0);
	glVertex3f( .1f,h,0);
	glVertex3f( .1f,0,0);
	glVertex3f(-.1f,0,0);
  glEnd();
  glFrontFace(GL_CW);
  glBegin(GL_POLYGON);
    glNormal3f(0,0,1);
	glVertex3f(-.1f,h,-.05f);
	glVertex3f( .1f,h,-.05f);
	glVertex3f( .1f,0,-.05f);
	glVertex3f(-.1f,0,-.05f);
  glEnd();
  glFrontFace(GL_CCW);	
}
void Sign::drawShape(float z)
{
  glFrontFace(GL_CW);
  switch(shape) {
  case rectangle:
	glBegin(GL_POLYGON);
	glTexCoord2f(0.,0.); glVertex3f(-sx/2,h,z);
	glTexCoord2f(1.,0.); glVertex3f( sx/2,h,z);
	glTexCoord2f(1.,1.); glVertex3f( sx/2,h+sy,z);
	glTexCoord2f(0.,1.); glVertex3f(-sx/2,h+sy,z);
	glEnd();
	break;
  case octagon:
	glBegin(GL_POLYGON);
	glTexCoord2f(0.25f,0.f); glVertex3f(-sx/4,h,z);
	glTexCoord2f(0.75f,0.f); glVertex3f( sx/4,h,z);
	glTexCoord2f(1.f,.25f); glVertex3f( sx/2.f,h+    sy/4.f,z);
	glTexCoord2f(1.f,.75f); glVertex3f( sx/2.f,h+3.f*sy/4.f,z);
	glTexCoord2f(.75f,1); glVertex3f( sx/4,h+sy,z);
	glTexCoord2f(.25f,1); glVertex3f(-sx/4,h+sy,z);
	glTexCoord2f(0,.75f); glVertex3f(-sx/2.f,h+3.f*sy/4.f,z);
	glTexCoord2f(0,.25f); glVertex3f(-sx/2.f,h+sy/4.f,z);
	glEnd();
	break;
  case diamond:
	glBegin(GL_POLYGON);
	glTexCoord2f(0.45f, 0.f); glVertex3f(-sx/10,h,z);
	glTexCoord2f(0.55f, 0.f); glVertex3f( sx/10,h,z);
	glTexCoord2f(   1 ,.45f); glVertex3f( sx/2,h+9*sy/20.f,z);
	glTexCoord2f(   1 ,.55f); glVertex3f( sx/2,h+11*sy/20.f,z);
	glTexCoord2f(.55f, 1   ); glVertex3f( sx/10,h+sy,z);
	glTexCoord2f(.45f, 1   ); glVertex3f(-sx/10,h+sy,z);
	glTexCoord2f(   0 ,.55f); glVertex3f(-sx/2,h+11*sy/20.f,z);
	glTexCoord2f(   0 ,.45f); glVertex3f(-sx/2,h+ 9*sy/20.f,z);
	glEnd();
	break;
  };
  glFrontFace(GL_CCW);
}

// make the houses we create be from a set of colors
/////////////////////////////////////////
// stuff for all houses
static int houseColors[][3] = {
	{240,240,240}, {180,175,100}, 
	{200,100,100}, {147,144,244},
	{250,249,157}, {199,144,186}
};
int nHouseColors = 6;


////////////////////////////////////////////////////////////////////////
// draw a little suburban dream...
// a 100x200 lot...
SimpleLot::SimpleLot(int ht, int hc)  
{
  // make the things we want
  add(new Lawn(0,0,100,200),0,0,0,0);
  switch(ht % 3) {
  case 0: {
	  SimpleHouse1* h1 = new SimpleHouse1();
	  h1->color(houseColors[hc % nHouseColors]);
	  add(h1,50.,0,60.,0);
	  break; }
  case 1: {
	  SimpleHouse2* h2 = new SimpleHouse2();
	  h2->color(houseColors[hc% nHouseColors]);
	  add(h2,50.,0,60.,0);
	  break; }
  case 2: {
	  SimpleHouse3* h3 = new SimpleHouse3();
	  h3->color(houseColors[hc% nHouseColors]);
	  add(h3,50.,0,60.,0);
	  break; }
  } 
  add(new SimpleTree1(15,5), 10,0, 10,0);
  add(new SimpleTree1(15,5), 10,0,190,0);
  add(new SimpleTree1(15,5), 30,0,190,0);
  add(new SimpleTree1(15,5), 50,0,190,0);
  add(new SimpleTree1(15,5), 70,0,190,0);
}

/***********************************************************************/
// a set of houses going down the street
// the street is along the X axis from (0,0) to ( (nh+1)*100, 0)
SimpleSubdivision::SimpleSubdivision(int nh) 
{
	// street lights in front of every other house
	// alternate sides of the street
	for(int sl=0; sl<nh; sl ++) {
		add(new StreetLight(sl%2 ? 9 : -9), static_cast<float>(sl*100 + 25), 0, (float)((sl%2) ? 8 : -9));
	}
	
    // add houses - one on each side of the street
	for (int hc=0; hc<nh; hc++) {
		GrObject* g1 = new SimpleLot(rand(), rand());
		GrObject* g2 = new SimpleLot(rand(), rand());

		add(g1, (float) (hc*100)    ,0, 15, 0);
		add(g2, (float) (hc*100+100),0,-15,pi);
	}

  add(new Sign(4,4,4,"stop.png",octagon),(float)(100*nh-2),0,17,-(3.141592f/2.f));
  add(new Sign(4,4,4,"stop.png",octagon),  2,0,-17 ,(3.141592f/2.f));
}


// $Header: /p/course/cs559-gleicher/private/CVS/GrTown/Examples/Suburbs.cpp,v 1.3 2009/11/10 22:40:03 gleicher Exp $
