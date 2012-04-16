/// GraphicsTown2006 - CS559 Sample Code
// written by Michael Gleicher - originally written Fall, 2000
// updated Fall, 2005
//
#include "GrTown_PCH.H"

#include "FlyCamera.H"
#include "DrawUtils.H"
#include <time.h>
#include <iostream>

int flyCamCount = 0;

FlyCamera::FlyCamera() : GrObject("FlyCamera",flyCamCount), 
  direction(0), pitch(0), posX(0), posY(10), posZ(0),
  lastUItime(0), buttonDown(0)
{
  // don't make this ridable, since its special
  //  ridable = true; 
}
void FlyCamera::draw(DrawingState*)
{
}

// build the transformation
// C = T Ry Rx
void FlyCamera::getCamera(Matrix camera)
{
	Matrix tmp1,  tmp2,  tmp3;

	transMatrix(tmp1, -posX, -posY, -posZ);
	rotMatrix(tmp2,'Y',-direction);
	multMatrix(tmp1, tmp2, tmp3);
	rotMatrix(tmp1,'X',-pitch);
	multMatrix(tmp3,tmp1,camera);
}


int FlyCamera::handle(int e)
{
	static int stX=0, stY=0;
	static int lX=0, lY=0;

	switch(e) {
	case FL_PUSH:
		lX = stX = Fl::event_x();
		lY = stY = Fl::event_y();
		buttonDown = Fl::event_button();
		return 1;
	case FL_RELEASE:
		buttonDown = 0;
		return 1;
	case FL_FOCUS:
		return 1;
	case FL_KEYBOARD:
		switch(Fl::event_key()) {
		case 'z':
			break;
		};
		return 1;
	case FL_DRAG:
		int mX = Fl::event_x();
		int mY = Fl::event_y();
		if (buttonDown) {
			switch(Fl::event_button()) {
	        case 1:
			case 3:
				float dx = static_cast<float>(mX - lX);
				float dy = static_cast<float>(mY - lY);
				if (fabs(dx) > fabs(dy)) {
					direction -=  0.01f * dx;												
				} else {										
					pitch += 0.01f * dy;
				}
				break;
			}
		}
		
		buttonDown = Fl::event_button();

		lX = mX; lY = mY;
		return 1;
	};
	return 0;
}

// to go forward, we need to now what the "forward" direction
// is - to find it out, look at the Z axis of the camera transform
void FlyCamera::forward(float a)
{
	Matrix cam;
	getCamera(cam);

	posX -= a*cam[0][2];
	posY -= a*cam[1][2];
	posZ -= a*cam[2][2];
}
// move the position in the LOCAL coordinates
// use the transform to know what that is
// but the transform really isn't used, so we have to get the camera
void FlyCamera::localTrans(float x,float y,float z)
{
	Matrix cam;
	getCamera(cam);

	  posX -= x*cam[0][0] + y*cam[0][1] + z*cam[0][2];
	  posY -= x*cam[1][0] + y*cam[1][1] + z*cam[1][2];
	  posZ -= x*cam[2][0] + y*cam[2][1] + z*cam[2][2];
}



bool FlyCamera::uiStep()
{
  unsigned long uitime = clock();
  unsigned long te = uitime-lastUItime;
  lastUItime = uitime;

  float u = ((float) te) / 30.f;

  bool didSomething = false;

  if (buttonDown>1) {
	forward(3 * u); didSomething = true; }

  if (Fl::get_key(FL_Left)) {
	direction += .1f * u; didSomething = true; }
  if (Fl::get_key(FL_Right)) {
	direction -= .1f * u; didSomething = true; }
  if (Fl::get_key(FL_Up)) {
	pitch -= .1f * u; didSomething = true; }
  if (Fl::get_key(FL_Down)) {
	pitch += .1f * u; didSomething = true; }
  if (Fl::get_key(' ')) {
	forward(3 * u); didSomething = true; }
  if (Fl::get_key('x')) {
	forward(-3 * u); didSomething = true; }
  if (Fl::get_key(FL_KP+'8')) {
	localTrans(0,-u,0); didSomething = true; }
  if (Fl::get_key(FL_KP+'2')) {
	localTrans(0,u,0); didSomething = true; }
  if (Fl::get_key(FL_KP+'4')) {
	localTrans(u,0,0); didSomething = true; }
  if (Fl::get_key(FL_KP+'6')) {
	localTrans(-u,0,0); didSomething = true; }
  if (Fl::get_key(FL_KP+'5')) {
	localTrans(0,0,u); didSomething = true; }
  if (Fl::get_key(FL_KP+'0')) {
	localTrans(0,0,-u); didSomething = true; }

  if (Fl::get_key('d')) {
	localTrans(-u,0,0); didSomething = true; }
  if (Fl::get_key('a')) {
	localTrans(u,0,0); didSomething = true; } 
  if (Fl::get_key('s')) {
	forward(-3 * u); didSomething = true; }
  if (Fl::get_key('w')) {
	forward(3 * u); didSomething = true; }
  

  return didSomething;
}


///////////////////////////////////////////////////////////////
FollowCam::FollowCam() : GrObject("follower"), 
	following(0), followDistance(45), minY(15)
{
  ridable = 1;
  lastUItime = 0;
}

// build a transformation that looks at the object that is being
// followed, and tries to keep a sane distance away
void FollowCam::getCamera(Matrix camera)
{
  float atX, atY, atZ;		// where we're looking at

  if (following) {
	atX = following->transform[3][0];
	atY = following->transform[3][1];
	atZ = following->transform[3][2];
  } else {
	  std::cerr << "No object for followcam!";
	  return;
  }

  // get the old "from" point from the matrix
  float oldFromX = transform[3][0];
  float oldFromY = transform[3][1];
  float oldFromZ = transform[3][2];

  // compute the new from point
  float fromX, fromY, fromZ;

  // for now, make the distance be constant
  float dx = oldFromX - atX;
  float dy = oldFromY - atY;
  float dz = oldFromZ - atZ;
  float d = sqrt(dx*dx+dy*dy+dz*dz);

  if (d<1) {
	fromX = atX + 10;
	fromY = atY + 10;
	fromZ = atZ + 10;
  } else {
	  float ds = followDistance / d;
	  fromX = atX + (dx * ds);
	  fromY = atY + (dy * ds);
	  fromZ = atZ + (dz * ds);
  }
  if (fromY < minY) fromY = minY;

  lookatMatrix(fromX,fromY,fromZ, atX,atY,atZ, camera);
}

bool FollowCam::uiStep()
{  unsigned long uitime = clock();
  unsigned long te = uitime-lastUItime;
  lastUItime = uitime;

  float u = ((float) te) / 20.f;

  return true;
}

int FollowCam::handle(int e)
{
	switch(e) {
	case FL_FOCUS:
		return 1;
	case FL_KEYBOARD:
		return 1;
	};
	return 0;
}

////////////////////////////////////////////////////////////////////////
Map::Map() :  GrObject("Map"), x(0), y(0), z(-2000)
{
  ridable = 1;
}

void Map::getCamera(Matrix camera)
{
  transMatrix(camera,x,y,z);
  camera[0][0] = 1;
  camera[1][1] = 0;
  camera[1][2] = 1;
  camera[2][1] = -1;
  camera[2][2] = 0;
}

int Map::handle(int e)
{
	switch(e) {
	case FL_FOCUS:
		return 1;
	case FL_KEYBOARD:
		switch(Fl::event_key()) {
		case ' ':
			x = 0;
			y = 0;
			z = -2000;
			break;
		};
		return 1;
	}
	return 1;
}

bool Map::uiStep()
{
  unsigned long uitime = clock();
  unsigned long te = uitime-lastUItime;
  lastUItime = uitime;

  float u = ((float) te) / 30.f;

  bool didSomething = false;

  const float s = 50.0f;

  if (Fl::get_key(FL_Left)) {
	x -= s * u; didSomething = true; }
  if (Fl::get_key(FL_Right)) {
	x += s * u; didSomething = true; }
  if (Fl::get_key(FL_Up)) {
	y += s * u; didSomething = true; }
  if (Fl::get_key(FL_Down)) {
	y -= s * u; didSomething = true; }

  return didSomething;
}

///////////////////////////////////////////////////////////////////////
InterestingCam::InterestingCam() : focus(0)
{
}

void InterestingCam::getCamera(Matrix camera)
{
	float laX, laY, laZ, lfX, lfY, lfZ;

	// find the points in world coordinates
	if (focus) {
	  transformMatrix(focus->transform, focus->laX, focus->laY, focus->laZ, laX, laY, laZ); 
	  transformMatrix(focus->transform, focus->lfX, focus->lfY, focus->lfZ, lfX, lfY, lfZ);
	} else {
	  laX = laY = laZ = 0;
	  lfX = lfY = lfZ = 500;
	}
	lookatMatrix(lfX,lfY,lfZ, laX, laY, laZ, camera);
}

// $Header: /p/course/cs559-gleicher/private/CVS/GrTown/FlyCamera.cpp,v 1.7 2007/11/12 16:56:06 gleicher Exp $
