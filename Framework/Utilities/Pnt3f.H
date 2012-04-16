#pragma once
// CS559 Utility Code
// - simple 3D point class
//
// Note: for the longest time, we avoided giving out basic data
// structure classes. Everyone likes to make their own, and everyone
// makes different choices in how to use them.
//
// This one is really basic. It is evolved from the old "Pnt3"
// class that Mike wrote in grad school (circa 1993). 
// It is hard coded for floats (since we didn't want to get started
// with templates). 
//
// We've only put in a few functions. You can add more if you want.
//
// File assembled by Mike Gleicher, October 2008

class Pnt3f {
public:
	// for simplicity, we just make this public so everything can access
	// it. real software engineers would make the internal data private.
	float x;			/* isn't this obvious */
	float y;
	float z;

	// if you want to treat this thing as a C vector (point to float)
	float* v();

				/***** CREATION */
	Pnt3f();			/* if we have 1, we need the default */
	explicit Pnt3f(const float x,const float y,const float z);	/* say where */
	Pnt3f(const float*);			/* from an array */
	// Pnt3f(const Pnt3f&);		/* copy constructor created by default*/

	// some useful operators - not the complete set, but just some basic ones
	Pnt3f operator * (const Pnt3f&) const;	/* cross product */
	Pnt3f operator * (const float)	const;  /* scale by scalar */
	Pnt3f operator + (const Pnt3f&) const;	/* create a temp */

	// make sure that we're unit length - vertical in the error case (0 length)
	void normalize();

	// note - the operators above are for Pnt3f*scalar, here we have scalar*Pnt3f
	friend Pnt3f operator * (const float s, const Pnt3f& p );
};

// inline definitions
inline float* Pnt3f::v()
{
	return &x;
}
inline Pnt3f Pnt3f::operator * (const float s) const {
	return Pnt3f(x*s,y*s,z*s);
};
inline Pnt3f operator * (const float s, const Pnt3f& p)  {
	return Pnt3f(s*p.x,s*p.y,s*p.z);
};
inline Pnt3f Pnt3f::operator + (const Pnt3f& p) const {
	return Pnt3f(x+p.x, y+p.y, z+p.z);
};
// cross product
inline Pnt3f Pnt3f::operator*(const Pnt3f& p) const {
  Pnt3f q;
  q.x = p.z * y - p.y * z;
  q.y = p.x * z - p.z * x;
  q.z = p.y * x - p.x * y;
  return q;
};
 
// This code tells us where the original came from in CVS
// Its a good idea to leave it as-is so we know what version of
// things you started with
// $Header: /p/course/cs559-gleicher/private/CVS/Utilities/Pnt3f.H,v 1.5 2008/10/19 01:54:28 gleicher Exp $