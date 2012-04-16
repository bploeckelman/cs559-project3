#pragma once
/// GraphicsTown2006 - CS559 Sample Code
// written by Michael Gleicher - originally written Fall, 2000
// updated Fall, 2005
// updated Fall, 2006 - smoothness at boundaries, direction preferences
// updated Fall, 2008 - comments added, connection to the centralized utilities

// WARNING: the road system code is meant as an example in terms of what it
// does, not necessarily for code to read and modify. 
// Don't start here! 

//
// new driving behaviors for the new Road System
//
// the thing you want to use here is randomdrive

#include "../GrObject.H"
#include "../Behavior.H"
#include "Roads.H"

// generic base class of all driving behaviors
// keeps track of basic stuff common to all driving behaviors
// this is a base class - don't use it
class Drive : public Behavior
{
public:
  Drive(GrObject* car, Road* road, float u, int lane);

  // every object that is driving must be on some road in some place
  Road* road;
  float u;
  int lane;

  // put the car in the right position
  void setMatrix();

  // have a speed that we try to achieve (roughly units/timestep)
  float speed;

  // advance u to try to simulate driving at a speed for an amount
  // of time - give it the ending time (like simulate until)
  // approximates arc length stuff
  // returns the actual amount of driving time it did not do
  // (so this will be zero if it drove the whole time, but non-zero
  //  if it stopped early - and needs to run for another 
  unsigned long advanceU(unsigned long until);

  // remember the last road you were on
  Road* lastRoad;
};


// a simple test driving behavior: just do a lane over and over
class SimpleDrive : public Drive
{
public:
  SimpleDrive(GrObject* car, Road* road, float u, int lane);

  virtual void simulateUntil(unsigned long time);
};


// a more complex driving behavior - randomly drive around
// when you get to a dead end, turn around
// when you get to an intersection, make a random choice
class RandomDrive : public Drive
{
public:
  RandomDrive(GrObject* car, Road* road, float u, int lane, int prefer=0);

  virtual void simulateUntil(unsigned long time);

  int prefer;	// is there a preferred direction to turn?
};

// $Header: /p/course/cs559-gleicher/private/CVS/GrTown/Roads/Drive.H,v 1.4 2008/11/11 03:48:23 gleicher Exp $
