/// GraphicsTown2006 - CS559 Sample Code
// written by Michael Gleicher - originally written Fall, 2000
// updated Fall, 2005 - major rewrite
// updated Fall, 2006 - smoothness at boundaries, direction preferences
// updated Fall, 2008 - comments added, connection to the centralized utilities

// WARNING: the road system code is meant as an example in terms of what it
// does, not necessarily for code to read and modify. 
// Don't start here! 

#include "../GrTown_PCH.H"
#include "Roads.H"
#include "../Utilities/Texture.H"
#include "../DrawUtils.H"
#include <assert.h>

#include <cmath>
#include <iostream>

// the road width (the width of a lane) is a constant
const float roadWidth  = 15.0f;
const float roadWidth2 =  7.5f;

Roads theRoads;

// Roads
Road::Road(float _ax, float _az, float _bx, float _bz,
		   char* name, float sl) :
  GrObject(name),
  ax(_ax), az(_az), bx(_bx), bz(_bz),
  endA(0), endB(0), laneA(-1), laneB(-1),
  is_intersection(false),
  speedLimit(sl)
{
  theRoads.push_back(this);
}

// note - we use a polygon offset to move road a little above ground
// to avoid Z-Fighting
void Road::draw(DrawingState*)
{
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(-1.,-2.);
  glColor3f(.6f,.6f,.6f);
  glNormal3f(0,1,0);
  fetchTexture("asphalt.png",true,true);
  roadDraw();
  glDisable(GL_POLYGON_OFFSET_FILL);

  // add yourself to the list of roads
  theRoads.push_back(this);
}

////////////////////////////////////////////////////////////////////////
StraightRoad::StraightRoad(float ax, float az, float bx, float bz,
						   char* name)
  : Road(ax,az,bx,bz,name)
{
  ux = bx-ax;
  uz = bz-az;
  length = sqrt(ux*ux+uz*uz);
  ux /= length;
  uz /= length;
}
// make the lanes 15 units wide - remember the endpoints are the
// centers of the rectangle
void StraightRoad::roadDraw()
{
  glBegin(GL_POLYGON);
  glTexCoord2f(0,2);
  glVertex3f(ax - roadWidth*uz, 0, az + roadWidth*ux);
  glTexCoord2f(length/roadWidth,2);
  glVertex3f(bx - roadWidth*uz, 0, bz + roadWidth*ux);
  glTexCoord2f(length/roadWidth,0);
  glVertex3f(bx + roadWidth*uz, 0, bz - roadWidth*ux);
  glTexCoord2f(0,0);
  glVertex3f(ax + roadWidth*uz, 0, az - roadWidth*ux);
  glEnd();
}

void StraightRoad::position(int lane, float u,
							float& x, float& z, float& dx, float& dz)
{
  if (lane) {
	dx = -ux;
	dz = -uz;
	x = u * ax + (1-u) * bx + uz * roadWidth2;
	z = u * az + (1-u) * bz - ux * roadWidth2;
  } else {
	dx = ux;
	dz = uz;
	x = (1-u) * ax + u * bx - uz * roadWidth2;
	z = (1-u) * az + u * bz + ux * roadWidth2;
  }
}

RoundRoad::RoundRoad(float cx_, float cz_, float r_) :
	Road(cx+r,cz,cx+r,cz),
	cx(cx_), cz(cz_), r(r_)
{
}

void RoundRoad::roadDraw()
{
	glBegin(GL_QUAD_STRIP);
	for(float u=0; u<=1.01; u+=.1f) {
		float s = sin(2*pi*u);
		float c = cos(2*pi*u);

		glVertex3f(cx + c * (r+roadWidth), 0, cz + s * (r+roadWidth));
		glVertex3f(cx + c * (r-roadWidth), 0, cz + s * (r-roadWidth));
	}
	glEnd();
}

void RoundRoad::position(int lane, float u, float& x, float& z, float& dx, float& dz)
{
	float lr = r + static_cast<float>((lane) ? roadWidth2 : -roadWidth2);
	float th = u * 2 * pi;
	if (lane) th *= -1;

	float s = sin(th);
	float c = cos(th);

	x = cx + c * lr;
	z = cz + s * lr;
	if (lane) {
		dx = s;
		dz = -c;
	} else {
		dx = -s;
		dz = c;
	}
}


////////////////////////////////////////////////////////////////////////
// see if two floats are equal
static inline bool feq(const float a, const float b)
{
  if (abs(a-b) < .0001) return true;
  else return false;
}

// Intersections
Intersection::Intersection(float cx, float cz)
  : Road(cx,cz,cx,cz,"Intersection")
{
	is_intersection = true;
  for(int i=0; i<4; i++) {
	roads[i] = 0;
	lanes[i] = -1;
  }

  // check all of the roads, see if any of them connect
  for(Roads::iterator r = theRoads.begin();
	  r != theRoads.end();
	  r++) {
		  if (feq(cx,(*r)->ax)) {
			  if (feq(cz + roadWidth, (*r)->az)) {
				  // we're connected to point A (lane 0) on side Z+
				  (*r)->endA = this;
				  roads[1] = *r;
				  lanes[1] = 0;
			  } else if (feq(cz - roadWidth, (*r)->az)) {
				  // we're connected to point A (lane 0) on side Z-
				  (*r)->endA = this;
				  roads[3] = *r;
				  lanes[3] = 0;
			  }
		  } else if (feq(cz,(*r)->az)) {
			  if (feq(cx + roadWidth, (*r)->ax)) {
				  // we're connected to point A (lane 0) on side X+
				  (*r)->endA = this;
				  roads[0] = *r;
				  lanes[0] = 0;
			  } else if (feq(cx - roadWidth, (*r)->ax)) {
				  // we're connected to point A (lane 0) on side X-
				  (*r)->endA = this;
				  roads[2] = *r;
				  lanes[2] = 0;
			  }
		  }

		  if (feq(cx,(*r)->bx)) {
			  if (feq(cz + roadWidth, (*r)->bz)) {
				  // we're connected to point B (lane 1) on side Z+
				  (*r)->endB = this;
				  roads[1] = *r;
				  lanes[1] = 1;
			  } else if (feq(cz - roadWidth, (*r)->bz)) {
				  // we're connected to point B (lane 1) on side Z-
				  (*r)->endB = this;
				  roads[3] = *r;
				  lanes[3] = 1;
			  }
		  } else if (feq(cz,(*r)->az)) {
			  if (feq(cx + roadWidth, (*r)->bx)) {
				  // we're connected to point B (lane 1) on side X+
				  (*r)->endB = this;
				  roads[0] = *r;
				  lanes[0] = 1;
			  } else if (feq(cx - roadWidth, (*r)->bx)) {
				  // we're connected to point B (lane 1) on side X-
				  (*r)->endB = this;
				  roads[2] = *r;
				  lanes[2] = 1;
			  }
		  }
  }
	  for(int i=0; i<4; i++)
		  if (lanes[i] > -1) assert(roads[i]);
//	  printf("@ (%g %g) lanes [%d %d %d %d]\n",cx,cz,lanes[0],lanes[1],lanes[2],lanes[3]);
}
void Intersection::roadDraw()
{
  glPushMatrix();
  glTranslatef(ax,0,az);
  glBegin(GL_POLYGON);
  glVertex3f(-roadWidth,0, roadWidth);
  glVertex3f( roadWidth,0, roadWidth);
  glVertex3f( roadWidth,0,-roadWidth);
  glVertex3f(-roadWidth,0,-roadWidth);
  glEnd();
  glPopMatrix();
}

// the position in the intersection is tricky since the begining and
// end are packed inside of lane
void Intersection::position(int lane, float u, 
							float& x, float& z, float& dx, float& dz)
{
  int endDir = lane >> 2;
  int begDir = lane & 3;

  // compute the begining and end - for positions, we can assume that
  // we come in and leave on the right hand side
  // also, remember the incoming and outgoing directions so we can
  // interpolate
  float inx, inz, idx, idz;
  switch(begDir) {
  case 0: inx =  roadWidth ; inz = -roadWidth2; idx=-1; idz=0;  break;
  case 1: inx =  roadWidth2; inz =  roadWidth ; idx= 0; idz=-1; break;
  case 2: inx = -roadWidth;  inz =  roadWidth2; idx= 1; idz=0;  break;
  case 3: inx = -roadWidth2; inz = -roadWidth ; idx= 0; idz=1;  break;
  };
  float outx, outz, odx, odz;
  switch(endDir) {
  case 0: outx =  roadWidth ; outz =  roadWidth2; odx=1; odz=0; break;
  case 1: outx = -roadWidth2; outz =  roadWidth;  odx=0; odz=1; break;
  case 2: outx = -roadWidth;  outz = -roadWidth2; odx=-1;odz=0; break;
  case 3: outx =  roadWidth2; outz = -roadWidth;  odx=0; odz=-1;break;
  };

  // FIXED!
  // going in a straight line looked silly so now the cars curve
  // fixed in 2007 by yoh suzuki

  int dist = static_cast<int>(abs(abs(idz)<0.1?outz-inz:outx-inx));
  if(dist) {

	  // find the corner of the intersection to rotate around
	  int pivotx = static_cast<int>(abs(idz)<0.1 ? inx : outx);
	  int pivotz = static_cast<int>(abs(idx)<0.1 ? inz : outz);

	  // find the start and end angles (from the pivot point)
	  float inangle = 180 * atan2((pivotz - inz),(pivotx - inx)) / pi;
	  float outangle = 180 * atan2((pivotz - outz),(pivotx - outx)) / pi;

	  // some bounds checking stuff... make sure they rotate the right way
	  if(abs(outangle-inangle)>95) {
		if(outangle<inangle)
			outangle+=360;
		else
			inangle+=360;
	  }

	  // find the current angle
	  float nowangle = (1-u) * inangle + (u) * outangle;

	  // find the world position
	  x = ax + pivotx - dist*cos(pi*nowangle/180);
	  z = az + pivotz - dist*sin(pi*nowangle/180);

  }
  else {

	  // no need to turn... just cross the intersection
	  x = ax + (1-u) * inx + u * outx;
	  z = az + (1-u) * inz + u * outz;

  }

  // linearly interpolate the directions - at least there's no discontinuity
  // it does look stupid
  dx = (1-u) * idx + u * odx;
  dz = (1-u) * idz + u * odz;
  float length = sqrt(dx*dx+dz*dz);
  // problem: the direction might vanish (if we are making a u-turn)
  if (length < .001) {
	dx = outx - inx;
	dz = outz - inz;
	length = sqrt(dx*dx+dz*dz);
  }
  dx /= length;
  dz /= length;
}

// pick a new direction
int Intersection::pickDirection(Road* r, int choices[4], int &fin, int prefer)
{
  int choice[4];				// three possible choices
  int nc = 0;					// number of choices
  fin = -1;					// did we find where we came from?

  // check each direction - if there's an edge, either make it a
  // choice or note that its where we came from
  for(int i=0; i<4; i++) {
	if (lanes[i] > -1) {
		assert(roads[i]);
	  if (roads[i] == r) fin = i;
	  else choice[nc++] = i;
	}
  }

  // if there is a direction preference, take it (unless we cannot go that way)
  // note - we had to do this AFTER figuring out which way we came from
  if (prefer) {
	  int pd = (fin + prefer + 4) % 4;
	  if (lanes[pd] > -1) {	// there is a choice
		  choices[0] = pd;		// make it the only one
		  return 1;
	  }
  }

  
  //  printf("%d choices (%d was inbound)\n",nc,fin);
  assert(fin>-1);				// there had better be an in direction

  if (nc) {
	for(int i=0; i<nc; i++) 
	  choices[i] = choice[i];
	return nc;
  } else {
	choice[0] = fin;
	return 1;					// last resort
  }
}

void Intersection::print()
{
	std::cerr << "Intersection(" << this << ")" << std::endl;
	if (this) {
		for(int i=0; i<4; i++) {
			std::cerr << "   " << roads[i] << " " << lanes[i] << std::endl;
		}
	}
}

void Intersection::pickSomething(Road *&roadp, int &lanep)
{
	for(int i=0; i<4; i++) {
		if (roads[i]) {
			roadp = roads[i];
			lanep = lanes[i];
			return;
		}
	}
	std::cerr << "Couldn't pick from disconnected intersection\n";
	roadp = 0;
	lanep = -1;
}

// $Header: /p/course/cs559-gleicher/private/CVS/GrTown/Roads/Roads.cpp,v 1.7 2009/11/10 22:40:03 gleicher Exp $
