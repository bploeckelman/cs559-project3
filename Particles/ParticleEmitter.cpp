/************************************************************************/
/* ParticleEmitter
/* ---------------
/* Manages a collection of Particle objects
/************************************************************************/
#include "ParticleEmitter.h"
#include "ParticleAffector.h"
#include "Particle.h"
#include "../Scene/Camera.h"
#include "../Utility/Logger.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <cassert>

using namespace glm;

// Setup static geometry shared by all particles
const vec3 ParticleEmitter::vertices[4] = 
{
	vec3(0,0,0),
	vec3(1,0,0),
	vec3(1,1,0),
	vec3(0,1,0)
};
const vec2 ParticleEmitter::texcoords[4] = 
{
	vec2(0,0),
	vec2(1,0),
	vec2(1,1),
	vec2(0,1)
};
const unsigned char ParticleEmitter::indices[6] = 
{
	0, 2, 3, 
	0, 1, 2
};


ParticleEmitter::ParticleEmitter(const unsigned int maxParticles
							   , const float lifetime)
	: particles()
	, affectors()
	, maxParticles(maxParticles)
	, oneTimeNumParticles(maxParticles)
	, position(0,0,0)
	, texture(nullptr)
	, blendMode(ADD)
	, emissionRate(1.f)
	, emissionCounter(0.f)
	, lifetime(lifetime)
	, age(0.f)
	, emitting(false)
	, alive(true)
	, paused(false)
	, grayscale(false)
	, oneTimeEmission(true)
{
	init();
}

ParticleEmitter::~ParticleEmitter()
{
	clean();
}

void ParticleEmitter::init()
{
	particles.clear();
	particles.reserve(maxParticles);

	for(unsigned int i = 0; i < maxParticles; ++i)
	{
		Particle p;
		p.active = false;
		particles.push_back(p);
	}

	emissionCounter = 0.f;
	emitting = true;
}

void ParticleEmitter::update(const float delta) 
{
	subUpdate(delta);

	// Stop the emitter if its time is up
	if( lifetime != -1.f && !paused )
	{
		if( age < lifetime )
		{
			age += delta;
			emitParticles(delta);
		} 
		else 
		{
			stop();
		}
	}
	else 
	{
		emitParticles(delta); 
	}

	// Track whether all particles are inactive
	bool allInactive = true;

	// Run the default update on each particle
	std::for_each(particles.begin(), particles.end()
		, [&](Particle& p) {
			if( p.active )
			{
				allInactive = false;
				p.update(delta);

				// Run each affector for this particle
				for each(auto a in affectors)
					a->update(p, delta);
			}
		}
	);

	// If all particles are inactive, 
	// and more aren't being emitted, 
	// mark this emitter as dead
	if( allInactive && !emitting )
	{
		alive = false;
	}
}

void ParticleEmitter::render(const Camera& camera)
{
	if( blendMode != NONE )
	{
		glDisable(GL_LIGHTING);
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		switch(blendMode)
		{
		case ALPHA:    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
		case ADD:      glBlendFunc(GL_ONE, GL_ONE); break;
		case MULTIPLY: glBlendFunc(GL_DST_COLOR, GL_ZERO); break;
		}
	}
	else
	{
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}

	glEnable(GL_TEXTURE_2D);
	assert(texture != nullptr);
	texture->Bind();

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, value_ptr(vertices[0]));
	glTexCoordPointer(2, GL_FLOAT, 0, value_ptr(texcoords[0]));

	glPushMatrix();

	for each(const Particle& p in particles)
	{
		// Don't draw inactive particles
		if( !p.active ) continue;

		glPushMatrix();
			// Move the particle into position and scale it  
			const mat4 particleTransform(
				scale( translate( mat4(1.0), p.position )
					 , vec3(p.scale, p.scale, p.scale) )
			);
			glMultMatrixf(value_ptr(particleTransform));

			// Undo the camera translation and get the inverse rotation
			const mat4 inverseCameraRotation(
				inverse( translate( camera.view(), camera.position() ) )
			);

			const float halfScale = -p.scale * 0.5f;
			const vec3  originCentered(halfScale, halfScale, 0.f);
			// Move the origin to the center of the particle
			const mat4 mat(	translate( inverseCameraRotation, originCentered ) );

			// Apply the final matrix for the billboarded particle
			glMultMatrixf(value_ptr(mat));

			// Set the particle's color
			if( grayscale )
				glColor4f(1,1,1,p.color.a);
			else
				glColor4fv(value_ptr(p.color));

			// Draw the triangles
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
		glPopMatrix();
	}

	glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	if( blendMode != NONE )
	{
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		glEnable(GL_LIGHTING);
	}
}

void ParticleEmitter::clean()
{
	texture = nullptr;
	particles.clear();
	
	for each(auto affector in affectors)
		delete affector;
	affectors.clear();
}

void ParticleEmitter::add( ParticleAffector* affector )
{
	assert(affector != nullptr);
	affectors.push_back(affector);
}

void ParticleEmitter::emitParticles(const float delta)
{
	if( !emitting ) return;

	// Figure out how many particles to emit this time
	int numParticlesToEmit = 0;
	if( oneTimeEmission )
	{
		numParticlesToEmit = oneTimeNumParticles;
		stop();
	}
	else
	{
		emissionCounter += delta;

		if( emissionCounter > 0.f )
		{
			// Prevent div-0
			if( emissionRate > 0.f ) 
			{
				int diff = static_cast<int>(std::ceil(emissionRate * emissionCounter + 0.5f));
				numParticlesToEmit = diff;
				emissionCounter -= numParticlesToEmit / emissionRate;
			}
			else
			{
				emissionCounter = 0.f;
			}
		}
	}

	// Emit numParticlesToEmit new particles
	ParticlesIter it  = particles.begin();
	ParticlesIter end = particles.end();

	int numEmitted = 0;
	while( numEmitted < numParticlesToEmit )
	{
		// Stop emitting if there aren't any particles left
		if( it == end ) break;

		Particle& p = *it;
		if( !p.active )
		{
			initParticle(p);
			++numEmitted;
		}
		++it;
	}
}
