#pragma once
/************************************************************************/
/* Fluid
/* -----
/* A 2d fluid surface
/*
/* Based on example from chapter 15 in:
/* "Mathematics for 3D Game Programming and Computer Graphics 3rd Ed."
/************************************************************************/
#include <SFML/System/Clock.hpp>

#include <glm/glm.hpp>


class Fluid
{
private:
	long width;
	long height;
	float dist;

	glm::vec3 *buffer[2];
	long renderBuffer;

	long numIndices;
	unsigned int *indices;

	glm::vec3 *normal;
	glm::vec3 *tangent;
	float k1, k2, k3;
	float t_step;

	sf::Clock evalTimer;

	float* getVertexBufferPtr();
	float* getNormalBufferPtr();

public:
	bool blend;  // Toggle alpha blending
	bool light;  // Toggle lighting effects

	glm::vec3 pos;

	/**
	* Fluid surface ctor
	* n,m - width, height of vertex array
	*   d - distance between adjacent vertices
	*   t - time interval between evaluations
	*   c - wave velocity (0 < c < [(d/2t)*sqrt(mu*t + 2)])
	*  mu - fluid viscosity
	**/
	Fluid( long n, long m, float d
		 , float t, float c, float mu
		 , const glm::vec3& pos=glm::vec3(0,0,0) );
	~Fluid();

	void render();

	void evaluate();
	void displace();
	void displace(float x, float z, float scale, float velocity);

	const long getWidth() const;
	const long getHeight() const;

	float getDist() const;

	
};

inline float Fluid::getDist() const { return dist; } 
inline const long Fluid::getHeight() const { return height;}
inline const long Fluid::getWidth() const { return width;}