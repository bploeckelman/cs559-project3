#ifndef GrObject_H
#define GrObject_H
// GraphicsTown - CS559-2000, sample solution and framework code
// 
// class GrObject: a drawable object in the word of GraphicsTown
// this is the guts, and is a good place to start. the main thing you
// do in this project is create subclasses of this class (and of
// Behaviors)
//

// originally, this code was written to avoid using STL (it used linked lists)
// in 2008, things were revised to use vectors instead

// Also in 2008: convert from C strings to C++ strings - makes memory
// management issues less likely

// things still don't release memory correctly :-(
// but since things should only be allocated at startup, there isn't an
// issue as everything is release at shutdown

// note: Objects can do many things
// Objects can sometimes serve as cameras


#include "MMatrix.H"
#include <vector>

// other classes referred to:
class DrawingState;
class Behavior;

extern int noNameCtr;
class GrObject {
 public:
  GrObject(char* nameBase = "NoName", int& nameCtr = noNameCtr, char* xn=0);
  virtual ~GrObject();

  // things to know about the object
  bool ridable;					/* can we "ride" this thing */
  std::string name;
  float rideAbove;				// in case we don't want to be stuck on the ground

  // simulation: every object is required to "update itself" to the
  // current time. no guaruntees that this clock is updated
  // uniformly (since it depends on drawing rate). The units are
  // number of milliseconds since the beginning of the simulation
  virtual void simulateUntil(unsigned long time);

  // what makes the object go - by default, this is all that
  // SimulateUntil does.
  // note - this is a linked list! All behaviors get their chance
  std::vector<Behavior*> behaviors;

  // the main thing an object does is draw itself
  // this happens in 2 phases, one where normal drawing happens, and
  // one phase that is "after" all the other objects have draw. This
  // allows for transparent objects to be drawn last. 
  // The caller will always take care of doing a
  // pushAttributes/matrix, so don't worry about that
  virtual void draw(DrawingState*);
  virtual void drawAfter(DrawingState*);

  // most objects have some way to position themselves
  // by standardizing that, we can make any object "ridable"
  // in general, most objects will have their centers at their
  // local original and use this matrix to move them around
  Matrix transform;

  // if we are "riding" this object (that means that it is the current
  // camera), it has two responsibilities:
  // 1) it must set up the camera transformation (not the projection)
  // 2) it must provide the user interface
  virtual void getCamera(Matrix);	// provide a camera matrix

  virtual bool uiStep();		// called if you're the camera before
								// drawing - lets you do key polling
  virtual int handle(int e);	// exactly what the FlTk Widgets do

  // we also have hierarchy
  GrObject* parent;				// your parent is responsible for you
  std::vector<GrObject*> children;
  GrObject* add(GrObject*, float x=0, float y=0, float z=0, float ry=0);

  // an object might be "interesting" - that is, that we want to put a
  // bookmark in the UI 
  // store the lookat and lookfrom points for these objects - in local
  // coordinates
  bool interesting;
  float laX, laY, laZ;
  float lfX, lfY, lfZ;
};

// add a new object onto the objects list
// allow for positioning in case object doesn't
// note: if you want to make a subobj, use the method of GrObj
extern std::vector<GrObject*> theObjects;
GrObject* add(GrObject*, float x=0, float y=0, float z=0, float ry=0);

// draw a list of objects
// note: this has to recurse to draw each objects children!
void drawObList(std::vector<GrObject*>& head, DrawingState* drst);
void drawAfterObList(std::vector<GrObject*>&, DrawingState* drst);


// $Header: /p/course/cs559-gleicher/private/CVS/GrTown/GrObject.H,v 1.4 2008/11/14 19:53:30 gleicher Exp $

#endif
