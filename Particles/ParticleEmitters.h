#pragma once
/************************************************************************/
/* ParticleEmitters
/* ----------------
/* Subclasses of ParticleEmitter 
/************************************************************************/
#include "ParticleEmitter.h"
#include "Particle.h"

#include <glm/glm.hpp>


class ExplosionEmitter : public ParticleEmitter
{
public:
	ExplosionEmitter( const glm::vec3& position
					, const unsigned int maxParticles = 100
					, const float lifetime            = 1.f );
protected:
	virtual void initParticle(Particle& p);
};
