/************************************************************************/
/* ParticleEmitters
/* ----------------
/* Subclasses of ParticleEmitter 
/************************************************************************/
#include "ParticleEmitters.h"
#include "ParticleEmitter.h"
#include "Particle.h"
#include "../Core/ImageManager.h"

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

using namespace glm;


ExplosionEmitter::ExplosionEmitter(const vec3& position
								 , const unsigned int maxParticles
								 , const float lifetime)
	: ParticleEmitter(maxParticles, lifetime)
{
	setPosition(position);
	setBlendMode(ALPHA);
	setOneTimeEmission(true);
	setOneTimeNumParticles(maxParticles);
	setTexture(&ImageManager::get().getImage("particle-point.png"));
}

void ExplosionEmitter::initParticle(Particle& p)
{
	Particle pp;

	pp.position     = position;
	pp.prevPosition = position;

	pp.velocity = vec3(linearRand(-15.f, 15.f)
					 , linearRand(-15.f, 15.f)
					 , linearRand(-15.f, 15.f));
	pp.accel    = vec3(linearRand(-1.f, 1.f)
					 , linearRand(-1.f, 1.f)
					 , linearRand(-1.f, 1.f));

	pp.color = vec4(linearRand(0.1f, 1.f)
				  , linearRand(0.1f, 1.f)
				  , linearRand(0.1f, 1.f)
				  , 1.f);

	pp.lifespan = 1.f;
	pp.scale = linearRand(0.1f, 0.4f);

	pp.active = true;

	p = pp;
}
