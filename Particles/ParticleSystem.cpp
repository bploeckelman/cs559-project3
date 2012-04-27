/************************************************************************/
/* ParticleSystem
/* ---------------
/* Controls a collection of ParticleEmitter objects
/************************************************************************/
#include "ParticleSystem.h"
#include "ParticleEmitter.h"
#include "../Scene/Camera.h"

#include <cassert>


ParticleSystem::ParticleSystem()
	: visible(true)
	, emitters()
{ }

ParticleSystem::~ParticleSystem()
{
	clean();
}

void ParticleSystem::add( ParticleEmitter* emitter )
{
	assert(emitter != nullptr);
	emitters.push_back(emitter);
}

void ParticleSystem::update( const float delta )
{
	if( !visible ) return;

	for each(auto emitter in emitters)
	{
		emitter->update(delta);
	}
}

void ParticleSystem::render( const Camera& camera )
{
	for each(auto emitter in emitters)
	{
		emitter->render(camera);
	}
}

void ParticleSystem::clean()
{
	for each(auto emitter in emitters)
	{
		delete emitter;
	}
	emitters.clear();
}

void ParticleSystem::start()
{
	for each(auto emitter in emitters)
	{
		emitter->start();
	}
}

void ParticleSystem::stop()
{
	for each(auto emitter in emitters)
	{
		emitter->stop();
	}
}

bool ParticleSystem::isAlive() const
{
	bool alive = false;
	for each(auto emitter in emitters)
	{
		if( emitter->isAlive() )
		{
			alive = true;
			break;
		}
	}
	return alive;
}
