#pragma once
/************************************************************************/
/* ParticleAffector
/* ----------------
/* Update the behavior of a collection of particles in a ParticleEmitter
/************************************************************************/
#include "Particle.h"

class ParticleEmitter;


class ParticleAffector
{
protected:
	ParticleEmitter* parentEmitter;

public:
	ParticleAffector(ParticleEmitter* parentEmitter)
		: parentEmitter(parentEmitter)
	{ }

	virtual ~ParticleAffector() { }

	virtual void update(Particle& particle, const float delta) = 0;
};
