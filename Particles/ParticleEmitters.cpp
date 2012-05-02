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


/************************************************************************/
/* ExplosionEmitter
/* Emit particles once from "position" with velocities 
/* that cause them to move outward in a spherical shape.
/* Affectors: FadeOut, ScaleDown, Force (gravity) 
/************************************************************************/
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
	setTexture(&GetImage("particle-dot.png"));
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


/************************************************************************/
/* FountainEmitter 
/* Emit particles continuously from "position" with velocities 
/* mostly upwards velocities and slightly to either side. 
/* Affectors: FadeOut, ScaleDown, Force (gravity) 
/************************************************************************/
FountainEmitter::FountainEmitter( const glm::vec3& position
								, const unsigned int maxParticles
								, const float lifetime)
	: ParticleEmitter(maxParticles, lifetime)
{
	add(new ScaleDownAffector(this, 0.f, 10.f));
	add(new FadeOutAffector(this, 0.f, 20.f));
	add(new ForceAffector(this, vec3(0,-5,0)));

	setBlendMode(ALPHA);
	setPosition(position);
	setOneTimeEmission(false);
	setTexture(&GetImage("particle-droplet.png"));

	// TODO: hacking around SFML's low delta value... 
	// should probably accumulate deltas until a 
	// certain threshold has been reached, then pass 
	// the accumulated value in to ParticleManager::update()
	setEmissionRate(10000.f);
}

void FountainEmitter::initParticle( Particle& p )
{
	Particle pp;

	pp.position     = position;
	pp.prevPosition = position;

	pp.velocity = vec3(linearRand(-10.f, 10.f)
					 , linearRand(30.f, 40.f)
					 , linearRand(-10.f, 10.f));
	pp.accel = vec3(0,0,0);

	pp.color = vec4(0, 0.8f, 1, 1);

	pp.lifespan = 0.4f;
	pp.scale = linearRand(0.2f, 0.4f);

	pp.active = true;

	p = pp;
}
