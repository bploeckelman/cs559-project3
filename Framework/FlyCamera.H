#ifndef FLYCAMERA_H
#define FLYCAMERA_H
/// GraphicsTown2006 - CS559 Sample Code
// written by Michael Gleicher - originally written Fall, 2000
// updated Fall, 2005
//
#include "GrObject.H"

// the main kind of camera that flies around under user control

class FlyCamera : public GrObject {
public:
  FlyCamera();

  // if we're not looking out of this camera, we should draw it
  virtual void draw(DrawingState*);

  virtual int handle(int);
  virtual bool uiStep();

  // computes the transform from the angles
  void getCamera(Matrix); 

  // store the direction - use it to build a matrix
  float direction;
  float pitch;
  float posX, posY, posZ;

  void forward(float);
  void localTrans(float,float,float);

  unsigned long lastUItime;

  // remember mouse state
  int buttonDown;
};

// this camera "follows" another object - its kindof like being pulled
// along by the object as if you we on a spring (or rigid rod)
// because its not too much fun to be pulled along the ground, there
// is a special hack to stay above the ground
class FollowCam : public GrObject {
public:
  unsigned long lastUItime;
  FollowCam();

  virtual bool uiStep();
  virtual int handle(int);

  void getCamera(Matrix); 

  // the object that we're following
  GrObject* following;

  // the distance to follow at
  float followDistance;

  // the minimum height for the camera (don't bang on the ground)
  float minY;
};

// a "plan" or map view - but not really orthographic
class Map : public GrObject {
public:
  Map();
  void getCamera(Matrix); 

  virtual bool uiStep();
  virtual int handle(int);
  unsigned long lastUItime;

  float x,y,z;
};

///////////////////////
// a camera that looks at interesting objects
class InterestingCam : public GrObject {
public:
  InterestingCam();

  void getCamera(Matrix);

  GrObject* focus;
};

// $Header: /p/course/cs559-gleicher/private/CVS/GrTown/FlyCamera.H,v 1.3 2007/11/12 16:56:06 gleicher Exp $

#endif
