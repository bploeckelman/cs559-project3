#ifndef TOWNVIEWWIDGET_H
#define TOWNVIEWWIDGET_H
/// GraphicsTown2006 - CS559 Sample Code
// written by Michael Gleicher - originally written Fall, 2000
// updated Fall, 2005
//
// class TownViewWidget - an FlTk widget that allows the town to be
// viewed
// in some sense, this class is the main body of the program since it
// is responsible for the UI and the drawing

#include "Fl/Fl_Gl_Window.H"

class GraphicsTownUI;
class DrawingState;
class GrObject;
class FollowCam;
class FlyCamera;
class InterestingCam;

class TownViewWidget : public Fl_Gl_Window
{
 public:
  // need this constructor for fluid
  TownViewWidget(int x, int y, int w, int h, const char* l=0);

  // the "UI" is the bigger, surrounding widget
  GraphicsTownUI* ui;

  void getStateFromUI(DrawingState*);

  GrObject* getCamera();

  FlyCamera* defaultCamera;
  FollowCam* followCamera;
  InterestingCam* interestingCamera;

  // point the defaultCamera at an object that is interesting
  void lookat(GrObject*);

  // for keeping track of time
  // remember each time step (so we can scale time if we want to)
  unsigned long lastClock;
  unsigned long time;

 private:
  virtual void draw();
  virtual int handle(int);
};

// $Header: /p/course/cs559-gleicher/private/CVS/GrTown/TownViewWidget.H,v 1.2 2007/11/01 21:25:20 gleicher Exp $

#endif
