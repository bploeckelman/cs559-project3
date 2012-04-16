#pragma once
/// GraphicsTown2006 - CS559 Sample Code
// written by Michael Gleicher - originally written Fall, 2000
// updated Fall, 2005
// updated Fall, 2006 - Behaviors don't complete in a cycle
// updated Fall, 2008 - turn linked lists into vectors
//
// Behaviors move objects around. Each object has a list of
// behaviors. The behaviors can change the internals of the objects.
// This is one of the classes that you will need to subclass for the project.
//
// periodically, every object is told to "simulate" - to do this, it
// calls all of its behaviors. the behavior is told how long to simulate
// for (or, to be more precise, what time to simulate up until)
// time is measured in milliseconds
class GrObject;

// an attempt to decouple how we decide where something is with what
// the thing is
class Behavior {
public:
  Behavior(GrObject* owner);

  GrObject* owner;				// who to make go

  // what time did the last step end - after you do something, be sure to set
  // this to the time at the end of the step!
  unsigned long lastV;

  // make it go!
  // be sure to update lastV! (that's the job of simulateUntil)
  virtual void simulateUntil(unsigned long);
};



// $Header: /p/course/cs559-gleicher/private/CVS/GrTown/Behavior.H,v 1.3 2008/11/11 03:48:23 gleicher Exp $






