/************************************************************************/
/* Fluid
/* -----
/* A 2d fluid surface
/*
/* Based on example from chapter 15 in:
/* "Mathematics for 3D Game Programming and Computer Graphics 3rd Ed."
/************************************************************************/
#include "Fluid.h"
#include "Skybox.h"

#undef __glext_h_
#undef __glxext_h_
#undef __gl_h_
#include "../Framework/Utilities/GLee.h"

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
Fluid::Fluid( long n, long m, float d
            , float t, float c, float mu
			, const vec3& pos /* = vec3(0,0,0) */ ) 
	: pos(pos)
	, skybox(nullptr)
{
	width  = n;
	height = m;
	dist = d;

	long count = n * m;

	buffer[0] = new vec3[count];
	buffer[1] = new vec3[count];
	renderBuffer = 0;

	long numTris = 2 * (width - 1) * (height - 1);
	numIndices   = 3 * numTris;
	indices = new unsigned int[numIndices];

	normal  = new vec3[count];
	tangent = new vec3[count];

	// Precompute constants for equation 15.25
	float f1 = c * c * t * t / (d * d);
	float f2 = 1.f / (mu * t + 2);
	k1 = (4.f - 8.f * f1) * f2;
	k2 = (mu * t - 2) * f2;
	k3 = 2.f * f1 * f2;

	// Initialize vertex, normal, tangent buffers
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

	// Initialize index buffer
	a = 0;
	for(long j = 0; j < (m - 1); ++j)
	for(long i = 0; i < (n - 1); ++i)
	{
		const long a0 = width *  j    +  i;
		const long a1 = width *  j    + (i+1);
		const long a2 = width * (j+1) + (i+1);
		const long a3 = width * (j+1) +  i;

		// First triangle
		indices[a++] = a0;
		indices[a++] = a3;
		indices[a++] = a2;

		// Second triangle
		indices[a++] = a0;
		indices[a++] = a2;
		indices[a++] = a1;
	}

	evalTimer.Reset();
	t_step = t;

	blend = true;
	light = false;

	skyboxEnvTexture = 0;
}

Fluid::~Fluid()
{
	delete[] tangent;
	delete[] normal;
	delete[] indices;
	delete[] buffer[1];
	delete[] buffer[0];

	if( skyboxEnvTexture != 0 )
		glDeleteTextures(1, &skyboxEnvTexture);
}

void Fluid::render()
{
	// Setup environment mapping
	if( skybox != nullptr )
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxEnvTexture);
		glEnable(GL_TEXTURE_CUBE_MAP);
			
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);

		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);
	}
	else
		glDisable(GL_TEXTURE_2D);

	if( light )
	{
		glEnable(GL_LIGHTING);
	}
	else
	{
		glDisable(GL_LIGHTING);
	}

	if( blend )
	{
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else 
	{
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	float *vertexArray = getVertexBufferPtr();
	float *normalArray = getNormalBufferPtr();
	glVertexPointer(3, GL_FLOAT, 0, vertexArray);
	glNormalPointer(GL_FLOAT, 0, normalArray);

	glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z); 
		glRotatef(90.f, 1.f, 0.f, 0.f);

//		glColor4f(0.0f, 0.8f, 1.0f, 0.5f);
		glColor4f(0.1f, 1.f, 1.f, 0.5f);
		glDrawElements(GL_TRIANGLES
					 , numIndices
					 , GL_UNSIGNED_INT
					 , indices);
	glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	if( blend )
	{
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}

	if( light )
	{
		glDisable(GL_LIGHTING);
	}

	if( skybox != nullptr )
	{
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);

		glDisable(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
	else
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

	// The edge vertices can get out of whack after
	// some disturbances, this forces them to stay put,
	// but it is pretty hacky, and could probably be
	// worked into the loops above as special cases,
	// if they went from 0 to < width or height,
	// instead of 1 to < (width or height - 1)
	for(long j = 0; j < height; j += (height - 1))
	for(long i = 0; i < width; ++i)
	{
		vec3& v = *(buffer[renderBuffer] + j * width + i);
		v.z = 0.f;
	}
	for(long j = 0; j < height; ++j)
	for(long i = 0; i < width; i += (width - 1))
	{
		vec3& v = *(buffer[renderBuffer] + j * width + i);
		v.z = 0.f;
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
	vec3& v = *(buffer[1 - renderBuffer] + j * width + i);
	v.z -= d * scale;
}

void Fluid::displace(float x, float z, float scale = 1.f, float velocity = 1.f)
{
	const float d = velocity;
	const int i = static_cast<int>(x);
	const int j = static_cast<int>(z);
	vec3& v = *(buffer[1 - renderBuffer] + j * width + i);
	v.z -= d * scale;
}

float* Fluid::getVertexBufferPtr()
{
	return glm::value_ptr(buffer[renderBuffer][0]);
}

float* Fluid::getNormalBufferPtr()
{
	return glm::value_ptr(*normal);
}

void Fluid::setSkybox( Skybox *box )
{
	if( box != nullptr )
	{
		skybox = box;

		// Generate cube map textures based on skybox textures
		// Note: this is sort of hacky because we can't directly 
		// control how SFML generates its textures and 
		// in order to use as cube maps, we have to gen textures 
		// with GL_TEXTURE_CUBE_MAP_dir_axis as a target 
		// instead of GL_TEXTURE_2D
		// So we need to generate a new texture just for this
		// using the pixel data from each skybox texture

		// TODO: figure out how to blend between night/day textures

		glGenTextures(1, &skyboxEnvTexture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxEnvTexture);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		sf::Image& tFront  = skybox->getTexture(Skybox::front);
		sf::Image& tBack   = skybox->getTexture(Skybox::back);
		sf::Image& tLeft   = skybox->getTexture(Skybox::left);
		sf::Image& tRight  = skybox->getTexture(Skybox::right);
		sf::Image& tBottom = skybox->getTexture(Skybox::bottom);
		sf::Image& tTop    = skybox->getTexture(Skybox::top);

		// Textures have uniform size, so can just grab this once
		const unsigned int width  = tFront.GetWidth();
		const unsigned int height = tFront.GetHeight();

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tRight.GetPixelsPtr());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tLeft.GetPixelsPtr());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tTop.GetPixelsPtr());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tBottom.GetPixelsPtr());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tFront.GetPixelsPtr());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tBack.GetPixelsPtr());
	}
}
