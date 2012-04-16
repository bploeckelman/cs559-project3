/************************************************************************
     File:        ArcBallCam.h

	This is a camera UI for FlTk OpenGL Applications. It grew out of the
	old "MyArcBall" example code (from CS559 2001-2006).

     Author:     
                  Michael Gleicher, gleicher@cs.wisc.edu
     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu

	2008 update by gleicher
	Added the double right click and the mousewheel - to make a much
	better UI.

     2007 Update by Mike Gleicher
	  2007 Update: rather than just an arcball, this will now be a whole camera UI that
	  hopefully is a bit more useful

	  the "ball" also stores all of the other camera parameters (position, FoV)

	  the downside, is that we're now tied to FlTk

	  we take over the right mouse button - pass us your events, and we'll take the ones we 
	  want.
	  the ALT button lets you do panning
	  double right click resets the arcball if you get lost

      To use it:
      1) instantiate it in your Fl_Gl_Window class
      2) call setup in the constructor of your window class
      3) call its handle method inside of your handle method
      4) use its SetProjection method when you need to set the camera
         (like when drawing)

     Comment:    
		Written October, 2001, Michael L. Gleicher
		Updated October, 2005 for CS559 - Michael L. Gleicher
        Revised October, 2007
        2008 - put into the "Utilities" module
		 
	An arcball is a user interface for rotating an object around
	its very useful for looking around the world.

	The actual mathematics to implement it are elegant, but beyond the
	scope of CS559. This example code is provided for you to use
	without having to worry about the details of how it works.

    Note:
	1.	A quickly hacked together implementation of ArcBall, with lots
	  	of bits lifted from Ken Shoemake's original source in Graphics Gems
		made into a nice C++ thing
	2.	It is simplified a lot - the controller is always at the origin,
	3.	It has a fixed radius, it doesn't have constraints

     Platform:    Visio Studio.Net 2003 (converted to 2005)

     CVS Info: Date checked in: $Date: 2008/10/19 01:54:28 $
               Revision number: $Revision: 1.5 $

*************************************************************************/
#pragma once

#include "3DUtils.H"

// if you need to pass that to OpenGL, try...
inline float* asGlMatrix(HMatrix m) { return (float*) m; };

// the arcball must be aware of FlTk
class Fl_Gl_Window;

//**************************************************************************
//
// We need a quick and dirty Quaternion class - for ArcBall
// Note1 : If you're a 559 student, don't worry about how this works
// Note2 : If you know what a quaternion is and want to use it, you
//         probably don't want this one! it's a simple version only for the
//         simple arcball
//
//**************************************************************************
class Quat {
 public:
  enum QuatPart {X=0, Y=1, Z=2, W=3, QuatLen};

  Quat();						/* gives the identity */
  Quat(float x, float y, float z, float w);
  Quat(const Quat&);			/* copy constructor */

  // conversions
  void toMatrix(HMatrix) const;

  // operations
  Quat conjugate() const;
  // make multiply look like multiply
  Quat operator* (const Quat&) const;

  void renorm();

  // the data
  float x,y,z,w;
};


//**************************************************************************
//
// The arcball implementation
//
//**************************************************************************
class ArcBallCam {
public:
	// we now must connect the ArcBall to an FlTk Window
	// note: this initializes things to a bad state (since the window might
	// not be available when we instantiate this - be sure to use setup!
	ArcBallCam();

	// you must call setup before doing anything!
	void setup(Fl_Gl_Window* wind,
			   float fieldOfView=40.0,		// field of view, in degrees
			   float eyeZ = 20.0,				// how far away camera is from the center of the world
			   float isx=0, float isy=0, float isz=0 // initial rotation
			   );

	//	all you really need is: handle and setProjection

	// handle mimics the FlTk widget handler - have your widget call this
	// if the arcball takes the event, it will return 1, otherwise it will
	// return 0 (and something else has to take the event)
	int handle(int e);

	// this sets the projection for viewing. it clears (and sets) the
	// projection and modelview matrices
	// note: we might not want to clear out the projection matrix
	// (for example, if there is a pick matrix), so we give the option
	// of not doing the load identity
	void setProjection(bool doClear=true);

	// Reset to a basic configuration
	void reset();

	// Simplified user interface
	void multMatrix();	// multiply matrix ontop of stack


	// call this when the mouse goes down
	void down(const float x, const float y);

	// this updates the cached positions - call it when the mouse is dragged
	void computeNow(const float nowX, const float nowY);

	// this gets the global matrix (start and now)
	void getMatrix(HMatrix) const;

	// Spin the ball by some vector - if you don't understand
	// how an arcball works, you probably don't care about this
	// but: basically you give it a vector to rotate the world around
	// - the length of the vector is how much to rotate around that
	//   axis
	void spin(float x, float y, float z);

	// Internal routine
	// this gets the position of the mouse in "normalized device coordinates"
	// that is X & Y go from -1 to 1 (with zero at the center)
	// this really goes with the window, but its handy here, and we can't
	// assume the window does it
	void getMouseNDC(float& x, float& y);

private:



	// This keeps track of the rotation - the current rotation is
	// start*now
	Quat start;			// orientation at the start of drag
	Quat now;			// quaternion "now" (while dragging)

	float downX, downY;	// where the mouse went down
	enum {None, Rotate, Pan} mode;	// are we panning or rotating (or not tracking)

	// this keeps track of the panning - the current pan is 
	// in eyeX, eyeY, but this is the displacement for undoing the last
	// addition
	float panX, panY;

	float eyeX, eyeY, eyeZ;
	float initEyeZ;
	float fieldOfView;
	// save a preferred rotation to return to
	float isx,isy,isz;

	Fl_Gl_Window* wind;
};

// CVS Header - if you don't know what this is, don't worry about it
// This code tells us where the original came from in CVS
// Its a good idea to leave it as-is so we know what version of
// things you started with
// $Header: /p/course/cs559-gleicher/private/CVS/Utilities/ArcBallCam.H,v 1.5 2008/10/19 01:54:28 gleicher Exp $
