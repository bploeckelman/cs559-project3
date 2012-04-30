#pragma once
/************************************************************************/
/* ParticleEmitters
/* ----------------
/* Subclasses of ParticleEmitter 
/************************************************************************/
#include "ParticleEmitter.h"
#include "Particle.h"

#include <glm/glm.hpp>


/************************************************************************/
/* ExplosionEmitter
/* Emit particles once from "position" with velocities 
/* that cause them to move outward in a spherical shape.
/* Affectors: FadeOut, ScaleDown, Force (gravity) 
/************************************************************************/
class ExplosionEmitter : public ParticleEmitter
{
public:
	ExplosionEmitter( const glm::vec3& position
					, const unsigned int maxParticles = 100
					, const float lifetime            = 1.f );
protected:
	virtual void initParticle(Particle& p);
};


/************************************************************************/
/* FountainEmitter 
/* Emit particles continuously from "position" with velocities 
/* mostly upwards velocities and slightly to either side. 
/* Affectors: FadeOut, ScaleDown, Force (gravity) 
/************************************************************************/
class FountainEmitter : public ParticleEmitter
{
public:
	FountainEmitter( const glm::vec3& position
		, const unsigned int maxParticles = 500
		, const float lifetime            = -1.f);

protected:
	virtual void initParticle(Particle& p);
};
