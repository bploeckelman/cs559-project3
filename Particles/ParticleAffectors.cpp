/************************************************************************/
/* ParticleAffectors
/* -----------------
/* Update the behavior of a collection of particles in a ParticleEmitter
/************************************************************************/
#include "ParticleAffectors.h"
#include "ParticleAffector.h"
#include "ParticleEmitter.h"
#include "Particle.h"

#include <glm/glm.hpp>

using namespace glm;


/************************************************************************/
/* ScaleDownAffector
/* Reduces the particle's scale amount at the specified rate
/* Note: only applies to textured quads, not points
/************************************************************************/
ScaleDownAffector::ScaleDownAffector( ParticleEmitter* parentEmitter
									, const float min  /* = 0.f */
									, const float rate /* = 1.f */ )
	: ParticleAffector(parentEmitter)
	, min(min)
	, rate(rate)
{ }

void ScaleDownAffector::update( Particle& particle, const float delta )
{
	if( (particle.scale -= delta * rate) < min )
		particle.scale = min;
}


/************************************************************************/
/* ScaleUpAffector
/* Increases the particle's scale amount at the specified rate
/* Note: only applies to textured quads, not points
/************************************************************************/
ScaleUpAffector::ScaleUpAffector( ParticleEmitter* parentEmitter
                                , const float max  /* = 1.f */
                                , const float rate /* = 1.f */ )
	: ParticleAffector(parentEmitter)
	, max(max)
	, rate(rate)
{ }

void ScaleUpAffector::update( Particle& particle, const float delta )
{
	if( (particle.scale += delta * rate) > max )
		particle.scale = max;
}


/************************************************************************/
/* FadeOutAffector
/* Reduces the particle's alpha component at the specified rate
/************************************************************************/
FadeOutAffector::FadeOutAffector( ParticleEmitter* parentEmitter
								, const float min  /* = 0.f */
								, const float rate /* = 1.f */ )
	: ParticleAffector(parentEmitter)
	, min(min)
	, rate(rate)
{ }

void FadeOutAffector::update( Particle& particle, const float delta )
{
	if( (particle.color.a -= delta * rate) < min )
		particle.color.a = min;
}


/************************************************************************/
/* ForceAffector
/* Applies the specified force vector to the particle's acceleration
/************************************************************************/
ForceAffector::ForceAffector( ParticleEmitter* parentEmitter
							, const vec3& force)
	: ParticleAffector(parentEmitter)
	, force(force)
{ }

void ForceAffector::update( Particle& particle, const float delta )
{
	particle.accel += force;
}
