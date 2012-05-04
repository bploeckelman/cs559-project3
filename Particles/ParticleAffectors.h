#pragma once
/************************************************************************/
/* ParticleAffectors
/* -----------------
/* Update the behavior of a collection of particles in a ParticleEmitter
/************************************************************************/
#include "ParticleAffector.h"
#include "ParticleEmitter.h"
#include "Particle.h"


/************************************************************************/
/* ScaleDownAffector
/* Reduces the particle's scale amount at the specified rate
/* Note: only applies to textured quads, not points
/************************************************************************/
class ScaleDownAffector : public ParticleAffector
{
protected:
	float min;
	float rate;

public:
	ScaleDownAffector(ParticleEmitter* parentEmitter
					, const float min  = 0.f
					, const float rate = 1.f);

	virtual void update(Particle& particle, const float delta);
};


/************************************************************************/
/* ScaleUpAffector
/* Increases the particle's scale amount at the specified rate
/* Note: only applies to textured quads, not points
/************************************************************************/
class ScaleUpAffector : public ParticleAffector
{
protected:
	float max;
	float rate;

public:
	ScaleUpAffector( ParticleEmitter* parentEmitter
                   , const float max  = 1.f
                   , const float rate = 1.f );

	virtual void update(Particle& particle, const float delta);
};


/************************************************************************/
/* FadeOutAffector
/* Reduces the particle's alpha component at the specified rate
/************************************************************************/
class FadeOutAffector : public ParticleAffector
{
protected:
	float min;
	float rate;

public:
	FadeOutAffector(ParticleEmitter* parentEmitter
				  , const float min  = 0.f
				  , const float rate = 1.f);

	virtual void update(Particle& particle, const float delta);
};


/************************************************************************/
/* ForceAffector
/* Applies the specified force vector to the particle's acceleration
/************************************************************************/
class ForceAffector : public ParticleAffector
{
protected:
	glm::vec3 force;

public:
	ForceAffector(ParticleEmitter* parentEmitter
				, const glm::vec3& force);

	virtual void update(Particle& particle, const float delta);
};
