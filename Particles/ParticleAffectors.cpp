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
#include <glm/gtc/random.hpp>

#include <SFML/System/Clock.hpp>

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


/************************************************************************/
/* HeightMapWalkAffector 
/* ---------------------
/* Moves an emitter around above a HeightMap
/************************************************************************/
HeightMapWalkAffector::HeightMapWalkAffector( ParticleEmitter* parentEmitter
											, HeightMap& heightmap
											, const vec3& initialPosition/*=vec3(0,0,0)*/ )
	: ParticleAffector(parentEmitter)
	, heightmap(heightmap)
	, position(initialPosition)
	, direction(0.f,1.f)
{
	position.y = heightmap.heightAt(position.x, position.z) + 0.1f;
}

void HeightMapWalkAffector::update( Particle& particle, const float delta )
{
	static sf::Clock timer;
	static const float limit = 1.f; // seconds

	// Update direction every 'limit' seconds
	if( timer.GetElapsedTime() > limit )
	{
		timer.Reset();

		static const float step = 1.f;
		const int r = static_cast<int>(linearRand(0.f, 9999.f)) % 8;
		switch(r)
		{
			case 0: direction = vec2( step,     0); break;
			case 1: direction = vec2(-step,     0); break;
			case 2: direction = vec2(    0,  step); break;
			case 3: direction = vec2(    0, -step); break;
			case 4: direction = vec2( step,  step); break;
			case 5: direction = vec2(-step, -step); break;
			case 6: direction = vec2( step, -step); break;
			case 7: direction = vec2(-step,  step); break;
		};
	}

	const vec3 pos = position + vec3(direction.x, 0.f, direction.y);
	
	if( pos.x > 0.f && pos.x <= (heightmap.getWidth()  * heightmap.getGroundScale())
	&& pos.z > 0.f && pos.z <= (heightmap.getHeight() * heightmap.getGroundScale()) )
	{
		position += delta * (pos - position);
	}

	position.y = heightmap.heightAt(position.x, position.z) + 0.1f;

	parentEmitter->setPosition(position);
}
