/// GraphicsTown2006 - CS559 Sample Code
// written by Michael Gleicher - originally written Fall, 2000
// updated Fall, 2005

#include "GrTown_PCH.H"

#include "TownViewWidget.H"
#include "DrawingState.H"
#include "GrWorld.H"
#include "GraphicsTownUI.H"
#include "GrObject.H"
#include "DrawUtils.H"
#include "Utilities/Texture.H"
#include <time.h>
#include "FlyCamera.H"

#include <iostream>

using std::vector;

// at idle time, this gets called
// it must advance time to the current time step by simulating
// all of the objects
// it also gives the camera a chance to do its user interface
void tvIdler(void* v)
{
  TownViewWidget* tv = (TownViewWidget*) v;
  if (tv->getCamera()->uiStep())
	tv->damage(1);

  unsigned long t = clock();
  unsigned long dt = t- tv->lastClock;

  tv->lastClock = t;

  float speedup = static_cast<float>(tv->ui->speedup->value());

  unsigned long ti = static_cast<unsigned long>(static_cast<float>(dt) * speedup);

  tv->time +=  ti;

  if (ti>0)
	  for(vector<GrObject*>::iterator g = theObjects.begin(); g != theObjects.end(); ++g)
		(*g)->simulateUntil(tv->time);
  tv->damage(1);
}

////////////////////////////////////////////////////////////////////////////
TownViewWidget::TownViewWidget(int x, int y, int w, int h, 
							   const char* l)
  : Fl_Gl_Window(x,y,w,h,l),
	time(0),	// start time at the beginning
	lastClock(clock())
{
  // we will probably want them all...
  mode(FL_RGB | FL_DOUBLE | FL_DEPTH | FL_ALPHA /*| FL_STENCIL*/);
  Fl::add_idle(tvIdler,this);
  followCamera = new FollowCam();
  interestingCamera = new InterestingCam();
}
  
unsigned long lastDrawDone = 0;

void TownViewWidget::draw()
{
  // figure out how to draw
  DrawingState drst;
  getStateFromUI(&drst);
  glEnable(GL_TEXTURE_2D);

  if (drst.backCull) {
	  glEnable(GL_CULL_FACE);
	  glCullFace(GL_BACK);
  } else
	  glDisable(GL_CULL_FACE);

  glFrontFace(GL_CCW);

	// set up the camera for drawing!

  glEnable( GL_DEPTH_TEST );

  glMatrixMode(GL_PROJECTION);
  glViewport(0,0,w(),h());
  glLoadIdentity();

  // compute the aspect ratio so we don't distort things
  double aspect = ((double) w()) / ((double) h());
  gluPerspective(drst.fieldOfView, aspect, 1, 60000);

  // put the camera where we want it to be
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  setupLights(&drst);

  Matrix camera;
  drst.camera->getCamera(camera);
  glMultMatrixf(&camera[0][0]);

  glClearStencil(0);

  // the actual clearing goes on in the sky routine since its the only
  // thing that knows what color to make the sky
  drawSky(&drst);
  drawEarth(&drst);

  //  GrObject* g;
  drawObList(theObjects,&drst);
  drawAfterObList(theObjects, &drst);

  if (lastDrawDone) {
	  double ifr = ((double)CLOCKS_PER_SEC) / (double) (clock()-lastDrawDone+1);
	  ui->rate->value(ifr);
  }
  lastDrawDone = clock();
}

void TownViewWidget::getStateFromUI(DrawingState* st)
{
	st->timeOfDay = (int) ui->time->value();
	st->fieldOfView = (float) ui->fov->value();
	st->camera = getCamera();
	st->backCull = ui->cull->value();
	st->drGrTex = ui->lgTex->value();
}

GrObject* TownViewWidget::getCamera()
{
  int p = ui->pickCamera->value();
  if (p) {
	if (ui->follower->value()) {
	  followCamera->following =  (GrObject*) ui->pickCamera->data(p);
	  return followCamera;
	}
	return (GrObject*) ui->pickCamera->data(p);
  } else {
	p = ui->pickInteresting->value();
	if (p) {
	  interestingCamera->focus = (GrObject*) ui->pickInteresting->data(p);
	  return interestingCamera;
	} else
	  return defaultCamera;
  } 
}

int TownViewWidget::handle(int e)
{
	switch(e) {
	case FL_SHOW:
		show();
		return 1;
	case FL_FOCUS:
		return 1;
	case FL_KEYBOARD:
		return 1;
	default:
		int r = getCamera()->handle(e);
		if (r) damage(1);
		return r;
	};
}

// $Header: /p/course/cs559-gleicher/private/CVS/GrTown/TownViewWidget.cpp,v 1.5 2009/11/10 22:40:03 gleicher Exp $
