/************************************************************************/
/* ParticleEmitters
/* ----------------
/* Subclasses of ParticleEmitter 
/************************************************************************/
#include "ParticleEmitters.h"
#include "ParticleEmitter.h"
#include "ParticleAffectors.h"
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
	add(new ScaleDownAffector(this, 0.f, 10.f));
	add(new FadeOutAffector(this, 0.f, 20.f));
	add(new ForceAffector(this, vec3(0,-10,0)));

	setBlendMode(ALPHA);
	setPosition(position);
	setOneTimeEmission(true);
	setOneTimeNumParticles(maxParticles);
	setTexture(&ImageManager::get().getImage("particle-dot.png"));
}

void ExplosionEmitter::initParticle(Particle& p)
{
	Particle pp;

	pp.position     = position;
	pp.prevPosition = position;

	pp.velocity = sphericalRand(30.f);
	pp.accel    = -0.1f * pp.velocity;

	pp.color = vec4(linearRand(0.3f, 1.f)
				  , linearRand(0.3f, 1.f)
				  , linearRand(0.3f, 1.f)
				  , 1.f);

	pp.lifespan = 1.f;
	pp.scale = linearRand(0.1f, 0.4f);

	pp.active = true;

	p = pp;
}
