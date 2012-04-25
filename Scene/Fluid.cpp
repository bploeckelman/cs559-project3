/************************************************************************/
/* Fluid
/* -----
/* A 2d fluid surface
/*
/* Based on example from chapter 15 in:
/* "Mathematics for 3D Game Programming and Computer Graphics 3rd Ed."
/************************************************************************/
#include "Fluid.h"

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>

using glm::vec3;

/**
* Fluid surface ctor
* n,m - width, height of vertex array
*   d - distance between adjacent vertices
*   t - time interval between evaluations
*   c - wave velocity (0 < c < [(d/2t)*sqrt(mu*t + 2)])
*  mu - fluid viscosity
**/
Fluid::Fluid( long n, long m, float d, float t, float c, float mu )
{
	width  = n;
	height = m;

	long count = n * m;

	buffer[0] = new vec3[count];
	buffer[1] = new vec3[count];
	renderBuffer = 0;

	normal  = new vec3[count];
	tangent = new vec3[count];

	// Precompute constants for equation 15.25
	float f1 = c * c * t * t / (d * d);
	float f2 = 1.f / (mu * t + 2);
	k1 = (4.f - 8.f * f1) * f2;
	k2 = (mu * t - 2) * f2;
	k3 = 2.f * f1 * f2;

	// Initialize buffers
	long a = 0;
	for(long j = 0; j < m; ++j)
	{
		float y = d * j;
		for(long i = 0; i < n; ++i)
		{
			buffer[0][a] = vec3(d * i, y, 0.f);
			buffer[1][a] = buffer[0][a];

			normal[a]  = vec3(0.f, 0.f, 2.f * d);
			tangent[a] = vec3(2.f * d, 0.f, 0.f);

			++a;
		}
	}

	evalTimer.Reset();
	t_step = t;
}

Fluid::~Fluid()
{
	delete[] tangent;
	delete[] normal;
	delete[] buffer[1];
	delete[] buffer[0];
}

void Fluid::render()
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);
	glPointSize(5.f);

	glColor3d(0.0, 0.0, 1.0);

	glPushMatrix();
	glRotatef(90.f, 1.f, 0.f, 0.f);
		float *vertexArray = getVertexBufferPtr();
		float *normalArray = getNormalBufferPtr();
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, vertexArray);
			glNormalPointer(GL_FLOAT, 0, normalArray);
			// TODO: generate index buffer so fluid
			// can be drawn as triangles
			glDrawArrays(GL_POINTS, 0, width * height);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
	glPopMatrix();

	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
}

void Fluid::evaluate()
{
	if( evalTimer.GetElapsedTime() < t_step )
		return;
	else
		evalTimer.Reset();

	// Apply equation 15.25
	for(long j = 1; j < height - 1; ++j)
	{
		const vec3 *crnt = buffer[renderBuffer] + j * width;
		vec3 *prev = buffer[1 - renderBuffer] + j * width;

		for(long i = 1; i < width - 1; ++i)
		{
			prev[i].z = k1 * crnt[i].z
					  + k2 * prev[i].z
					  +	k3 * (crnt[i + 1].z
							+ crnt[i - 1].z
						    + crnt[i + width].z
							+ crnt[i - width].z);
		}
	}

	// Swap buffers
	renderBuffer = 1 - renderBuffer;

	// Calculate normals and tangents
	for(long j = 1; j < height - 1; ++j)
	{

		const vec3 *next = buffer[renderBuffer] + j * width;
		vec3 *nrml = normal  + j * width;
		vec3 *tang = tangent + j * width;

		for(long i = 1; i < width - 1; ++i)
		{
			nrml[i].x = next[i - 1].z - next[i + 1].z;
			nrml[i].y = next[i - width].z - next[i + width].z;
			nrml[i].z = next[i + 1].z - next[i - 1].z;
		}
	}
}

void Fluid::displace()
{
	const float scale = 1.f;
	const float d = glm::linearRand(0.f, 1.f);
	const int i = static_cast<int>(glm::linearRand(0.f, (float)width));
	const int j = static_cast<int>(glm::linearRand(0.f, (float)height));
	vec3& v = *(buffer[renderBuffer] + j * width + i);
	v.z += d * scale;
}

float* Fluid::getVertexBufferPtr()
{
	return glm::value_ptr(buffer[renderBuffer][0]);
}

float* Fluid::getNormalBufferPtr()
{
	return glm::value_ptr(*normal);
}
