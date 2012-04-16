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
// a new "Roads" system - since the old one was way too complicated
//
// idea: 
// - have objects that are "pieces of road"
// - connect these objects into a graph at intersections
//
// WARNING: Roads ignore hierarchy! - they must be placed in absolute
// world coords!
//
#include "../GrObject.H"

// forward declarations of the base types
class Road;
class Intersection;

// a "road" is a little piece of road from A to B (both point
// positions) 
// roads have 2 "lanes" - lane 0 goes from point A to point B, lane 1
// goes from B to A
class Road : public GrObject {
public:
  Road(float ax, float az, float bx, float bz,
	   char* name="Road", float speedLimit = .33);

  // put the "road specific" drawing code into roadDraw
  // the "draw" function generically sets up the texture
  virtual void draw(DrawingState*);
  virtual void roadDraw()=0;		/* internal - assumes everything
									   is set */

  // find a vehicle's position on the road
  // give a parametric value between 0-1, return a position and a
  // direction (unit vector)
  // you give it a lane
  virtual void position(int lane, float u, 
						float& x, float& z, float& dx, float& dz) = 0;

  // store the endpoints
  float ax, az, bx, bz;

  // a speed limit IN PARAMETER SPACE!
  // note: this is parameter units per step - so the main thing
  // it is useful for is unforcing some degree of smoothness/sampling
  // no matter how fast things go, we want to have a few points along
  // the road when we go around a turn
  float speedLimit;

  // at each end, there might be an intersection
  // or another road
  Road* endA;
  Road* endB;
  // for each of those next roads, we need to store what lane of the
  // road to go to
  // if the next road is an intersection, then the next lane should be
  // -1 (illegal lane) since the intersection must figure out what the
  // real lane is
  int laneA;
  int laneB;

  // is this an intersection (if so, a lot of stuff makes no sense)
  bool is_intersection;
};

// collect a list of roads
typedef std::vector<Road*> Roads;
extern Roads theRoads;

// a straight segment of road from a to b
class StraightRoad : public Road {
public:
  StraightRoad(float ax, float az, float bx, float bz,
			   char* name="StraightRoad");

  virtual void roadDraw();
  virtual void position(int lane, float u, 
						float& x, float& z, float& dx, float& dz);

  // cache the a->b direction vector and the road length
  float ux, uz;
  float length;
};

// a round road - mainly useful for testing - its pretty boring
// endpoints don't mean much
class RoundRoad : public Road
{
public:
  RoundRoad(float cx, float cz, float r);

  virtual void roadDraw();
  virtual void position(int lane, float u, 
						float& x, float& z, float& dx, float& dz);

  float cx, cz, r;
};


////////////////////////////////////////////////////////////////////////
// an intersection
// assumes a "4-way" (although some ways might be missing)
// assume that things are aligned in compass direction
// so, each intersection might have 4 things coming in
// directions:
// 0 = +x
// 1 = +z
// 2 = -x
// 3 = -z
// note: we assume that the incoming lane is the opposite of the
// outgoing lane
class Intersection : public Road
{
public:
  // creation will automatically connect to all existing roads
  Intersection(float cx, float cy);

  // for each direction, a road - with the OUTGOING lane
  Road* roads[4];
  int   lanes[4];

  virtual void roadDraw();

  // note that the lane must be different because we need to deal with
  // many different "lanes" - effectively every combination of input
  // directions 
  // we assume that the output direction is different than the "other
  // lane" than the input direction, so we only need to store 4
  // choices for input and 4 choices for output - 16 choices
  // altogether
  virtual void position(int lane, float u, 
						float& x, float& z, float& dx, float& dz);

  // pick a new direction
  // try not to pick the direction you came in on, but use it as a
  // last resort
  // returns the number of choices, fills in the array with the
  // indecies
  // we also give back the direction that we came from (the index of
  // last road) in case you need it
  // for testing, its nice to have a car that will always turn left/right
  // for this set prefer to be +/- 1
  int pickDirection(Road* lastRoad, int c[4], int& lri, int prefer=0);

  // in a moment of desperation, when something goes horribly wrong, randomly
  // pick some valid connected road instead of crashing
  void pickSomething(Road*& roadp, int& lanep);

  // for debugging
  void print();
};

// $Header: /p/course/cs559-gleicher/private/CVS/GrTown/Roads/Roads.H,v 1.4 2008/11/11 03:48:23 gleicher Exp $
