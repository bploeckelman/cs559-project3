/// GraphicsTown2006 - CS559 Sample Code
// written by Michael Gleicher - originally written Fall, 2000

// WARNING: the road system code is meant as an example in terms of what it
// does, not necessarily for code to read and modify. 
// Don't start here! 

// updated Fall, 2005
// updated Fall, 2006 - smoothness at boundaries, direction preferences
// updated Fall, 2008 - comments added, connection to the centralized utilities

#include "../GrTown_PCH.H"
#include "../MMatrix.H"
#include "Drive.H"

#include <iostream>
#include <assert.h>

// generic driving behavior
Drive::Drive(GrObject* car, Road* road_, float u_, int lane_) : 
  Behavior(car),
  road(road_), u(u_), lane(lane_), speed(.25), lastRoad(0)
{
}

// put the car in the right place
void Drive::setMatrix()
{
  float x,z, dx, dz;

  road->position(lane,u, x,z,dx,dz);
  
  transMatrix(owner->transform,x,0,z);
  
  // the direction vector is forward (z)
  owner->transform[2][0] = -dx;
  owner->transform[2][1] = 0;
  owner->transform[2][2] = -dz;
  // x direction must be perpindicular
  owner->transform[0][0] = -dz;
  owner->transform[0][1] = 0;
  owner->transform[0][2] = dx;
}

// we fake arc length correction by seeing how fast we go
unsigned long Drive::advanceU(unsigned long time)
{
	// need to keep these things in case we need them
	float d=0,dur=0;
	// guess at the u increment
	float uig = .02f; // 2 * dist;

	if (!lastV) {		// on the first step, do something random
		u += uig;
	} else {
		// figure out where we're starting
		float bx, bz, bdx, bdz;
		road->position(lane,u, bx,bz,bdx,bdz);

		// figure out how long the step is
		dur = static_cast<float>(time-lastV);

		// how far should we go in that amount of time
		float dist = dur * speed;


		// see where this speed would have us end up
		float fx,fz;
		road->position(lane,u + uig,fx,fz,bdx,bdz);
		// how far would we go
		d = sqrt( (fx-bx)*(fx-bx) + (fz-bz)*(fz-bz) );

		float du = uig * (dist/(d+.00001f));

		if (du > road->speedLimit) du = road->speedLimit;  

		u += du;
	}
 	lastV = time;
	if (u>1) {
		float leftOverU = u-1;
		// figure out how long that was 
		float leftOverD = leftOverU * d / uig;
		float leftOverT = leftOverD / speed;

		// printf("Duration: %g,  LeftOver: %g  lU:%g\n", dur,leftOverT,leftOverU);
		// left over time must be less than the original duration
		assert(leftOverT < dur);

		u=1;
		return static_cast<unsigned long>(leftOverT);
	}
	return 0;
}

// the simple test thing
SimpleDrive::SimpleDrive(GrObject* car, Road*  road, float u, int lane)
  : Drive(car, road, u, lane)
{
}

// make the car go around
// if we go past the end, we need to go extra far next time
void SimpleDrive::simulateUntil(unsigned long time)
{
	unsigned long leftover = 0;
	if (u>=1) u=0;
	leftover = advanceU(time);

	// if we didn't go far enough since we hit the end, we need to
	// do something
	// idea: say that our last update is not now, so next time, we'll
	// take a longer step
	if (leftover)
		lastV -= leftover;

	setMatrix();
}

////////////////////////////////////////////////////////////////////////
RandomDrive::RandomDrive(GrObject* car, Road*  road, float u, int lane, int p)
  : Drive(car, road, u, lane), prefer(p)
{
}

void RandomDrive::simulateUntil(unsigned long time)
{
  assert(road);

//  if (u<=0) {
//	printf("starting on road %s lane %d\n",road->name,lane);
//	printf("  destination %s lane %d\n",
//		   lane ? (road->endA ? road->endA->name : "(null)")
//		        : (road->endB ? road->endB->name : "(null)"),
//		   lane ? road->laneA : road->laneB
//				);
//  }

  // if we're in a legal lane, move forward - unless you hit the end
  // of a road
  if (lane >=0) {
	if (u<1) {
	  unsigned long leftover = advanceU(time);
	  // here would be a good place to check to see if we should 
	  // be stopping for a stop sign - otherwise, when we update
	  // lastU, we'll move on to the next road segment
	  lastV -= leftover;
	  setMatrix();
	} else {					// we're at the end of the road
	  lastRoad = road;			// remember before we update
	  // we're at the end of the road, so move on to the next road
	  // this would be the place to check to see if there's a stop
	  // sign and potentially pause 
	  // if we're at the end of an intersection, then the "ends" are
	  // stored differently
	  if (road->is_intersection) {
		// the next choice is stored in the lane
		int choice = lane >> 2;
		Intersection* inter = static_cast<Intersection*>(lastRoad);
		road = inter->roads[choice];
		lane = inter->lanes[choice];
		if (!road) {
			std::cerr << "Wierd - picked a disconnected road at an intersection, trying to recover\n";
			inter->pickSomething(road,lane);
		}
	  } else {
		// we've gotten to the end of a road segment
	    // note: prior to 11/20/07 this was road=road, not lastroad
		if (lane) {				// we've arrived at A
		  if (road->endA) {
			road = lastRoad->endA;
			lane = lastRoad->laneA;
		  } else {
			lane = 0;				// no place to go, turn around
		  }
		} else {					// we've arrived at B
		  if (road->endB) {
			road = lastRoad->endB;
			lane = lastRoad->laneB;
		  } else {
			lane = 1;				// no place to go, turn around
		  }
		}
	  }
	  u = 0;					// start at the begining of next seg
	}
  }

  // if we're in an illegal lane, then we had better figure out what
  // lane to be in - 
  // note - this is not an else from the above "are we in a legal
  // lane" since we might be on a different road now
  if (lane < 0) {
	// if the road is not an intersection, being in an illegal lane
	// means we've screwer up
	assert(road->is_intersection);

	// picking a lane means picking the next road
	// need to pick a next road
	int choices[4];
	int lastDirIndex;
	int nc = static_cast<Intersection*>(road)->pickDirection(lastRoad,choices,lastDirIndex, prefer);
    assert(lastDirIndex >= 0);

	// pick a next choice at random
	int c = rand() % nc;

	// the "lane" for the intersection is the incoming and outgoing
	lane = choices[c] << 2 | lastDirIndex;

//	printf("starting intersection - from %d to %d code %d\n",lastDirIndex,c,lane);
//	printf("   [%c%c%c%c] -> [",static_cast<Intersection*>(road)->roads[0] ? '+' : '-',
//								static_cast<Intersection*>(road)->roads[1] ? '+' : '-',
//								static_cast<Intersection*>(road)->roads[2] ? '+' : '-',
//								static_cast<Intersection*>(road)->roads[3] ? '+' : '-');
//	for(int i=0; i<nc; i++) printf("%d ",choices[i]);
//	printf("]\n");
  } 

  setMatrix();
}


// $Header: /p/course/cs559-gleicher/private/CVS/GrTown/Roads/Drive.cpp,v 1.4 2008/11/11 03:48:23 gleicher Exp $
