#ifndef GrTown_PCH
#define GrTown_PCH
/// GraphicsTown2006 - CS559 Sample Code
// written by Michael Gleicher - originally written Fall, 2000
// updated Fall, 2005
// precompiled header file so that things can compile faster...
// But precompiled headers aren't actually enabled in the sample version
// so this is just a convenience to include one file to get all kinds of
// generally useful stuff

// updated Fall, 2006
// turn off FlTk warnings

// note: the include path is hacked so that Utilities is in it
// make sure to include glee before opengl
#include "Utilities/glee.h"
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdarg.h>
#include <stdio.h>
#include <math.h>

// include the useful FlTk stuff too
// FLTk gives some annoying warnings - turn them off
#pragma warning(push)
#pragma warning(disable:4312)
#pragma warning(disable:4311)
#include <FL/Fl_Widget.h>
#include <FL/Fl.h>
#include <FL/FL_Window.h>
#include <FL/FL_Gl_Window.h>
#pragma warning(pop)

// use some of STL
#include <vector>

// $Header: /p/course/cs559-gleicher/private/CVS/GrTown/GrTown_PCH.H,v 1.4 2008/11/14 19:53:30 gleicher Exp $

#endif
