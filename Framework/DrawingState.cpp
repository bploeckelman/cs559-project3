// GraphicsTown2006 - CS559 Sample Code
// written by Michael Gleicher - originally written Fall, 2000
// updated Fall, 2005
//
#include "GrTown_PCH.H"

#include "DrawingState.H"

DrawingState::DrawingState()
{
  // reasonable defaults for everything
  timeOfDay = 9;				// morning
  sky(.7f,.7f,1);					// blue sky
  ground(0,84,24);				// green grass
  camera = 0;
  fieldOfView = 45;
  backCull = 0;
  drGrTex = 0;
}



// $Header: /p/course/cs559-gleicher/private/CVS/GrTown/DrawingState.cpp,v 1.2 2007/11/01 21:23:44 gleicher Exp $
