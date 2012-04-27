/************************************************************************/
/* ParticleEmitter
/* ---------------
/* Manages a collection of Particle objects
/************************************************************************/
#include "ParticleEmitter.h"
#include "Particle.h"
#include "../Utility/Logger.h"

#include <glm/glm.hpp>

#include <SFML/Graphics/Image.hpp>

using namespace glm;


ParticleEmitter::ParticleEmitter(const unsigned int maxParticles
							   , const float lifetime)
	: particles()
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
{ }

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

	// TODO: static geometry?

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
	for each(auto particle in particles)
	{
		if( particle.active )
		{
			allInactive = false;
			particle.update(delta);
		}
	}

	// If all particles are inactive, 
	// and more aren't being emitted, 
	// mark this emitter as dead
	if( allInactive && !emitting )
	{
		alive = false;
	}
}

void ParticleEmitter::render()
{
	// TODO: setup geometry once
}

void ParticleEmitter::clean()
{
	texture = nullptr;
	particles.clear();
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
				int diff = static_cast<int>(
					std::floor(emissionRate * emissionCounter + 0.5f));
				numParticlesToEmit -= diff;
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

		Particle p = *it;
		if( !p.active )
		{
			initParticle(p);
			++numEmitted;
		}
	}
}
