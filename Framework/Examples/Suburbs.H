#pragma once

/// GraphicsTown2006 - CS559 Sample Code
// written by Michael Gleicher - originally written Fall, 2000
// updated Fall, 2005
// updated Fall, 2006 - new texture manager, improvements for behaviors

// Suburbia.H - A set set of sample objects for Graphics Town to make
// something that looks like a suburb

// These are all simple -you should make objects that look nicer

#include "../GrObject.H"
#include "../DrawingState.H"

// a simple tree - a cone like a pine
class SimpleTree1: public GrObject {
public:
  SimpleTree1(float height, float radius);
  virtual void draw(DrawingState*);
  float height, radius;
};

// a rectangular patch of lawn - wish mine grew so well
// it is a flat rectangle that sits on the flat ground
class Lawn : public GrObject {
public:
  Lawn(float x1, float z1, float x2, float z2);
  float x1,z1,x2,z2;
  virtual void draw(DrawingState*);
};


// three very basic houses.
// only thing that makes them at all interesting is that they have
// windows slapped onto them using texture
class SimpleHouse1 : public GrObject {
public:
  SimpleHouse1();
  Color color;
  virtual void draw(DrawingState*);
};
class SimpleHouse2 : public GrObject {
public:
  SimpleHouse2();
  Color color;
  virtual void draw(DrawingState*);
};
class SimpleHouse3 : public GrObject {
public:
  SimpleHouse3();
  Color color;
  virtual void draw(DrawingState*);
};

//
// a sortof a church: a bigger house with a steeple
class Church : public GrObject {
public:
  Church();
  Color color;
  virtual void draw(DrawingState*);
};

//**********************************************************************
// a street light - what is interesting about this is that it draws a
// transparent cone to fake a spotlight as its drawafter
// The light is actually positioned at the center of its lightcone
// (where it hits the ground) - the "poleZ" specifies how far behind
// this the pole is placed (along the Z axis)
class StreetLight : public GrObject {
 public:
	 double radius;
	 double poleZ;
  StreetLight(double poleZ);
	virtual void draw(DrawingState*);
	virtual void drawAfter(DrawingState*);
};


//**********************************************************************
// a traffic sign - this is actually an object that you might make use
// of
// the sign can be of multiple shapes (depending on the type of sign)
// and use any texture specified.
typedef enum {rectangle, diamond, octagon} SignShape;

class Sign : public GrObject {
 public:
  Sign(float sx=3,float sy=3,float h=5,char* texture = "stop",
	   SignShape ss=octagon);

  float sx,sy,h;
  virtual void draw(DrawingState*);
  char* texName;
  SignShape shape;
  void drawShape(float z);
};


////////////////////////////////////////////////////////////////////////
// basic element of suburbia - the lot
// has a house, a yard, some trees, a driveway, ...
// this object doesn't do much - it just creates a bunch of
// subobjects - so its a good example of hierarchy and instancing
class SimpleLot : public GrObject {
 public:
  SimpleLot::SimpleLot(int houseType=1, int houseColor=1);
};

/***********************************************************************/
// a subdivision plot - a straight road with houses on either side
// note: this doesn't actually make the piece of road (since I don't
// want this code to know about the roads code)
// again, all it does is instance n*2 subdivision lots, some
// streetlights, some stop signs, ...
class SimpleSubdivision : public GrObject {
 public:
  SimpleSubdivision(int nHouses);
};

// $Header: /p/course/cs559-gleicher/private/CVS/GrTown/Examples/Suburbs.H,v 1.2 2007/11/01 21:25:59 gleicher Exp $
