/************************************************************************/
/* ParticleManager 
/* -----
/* Manages a collection of ParticleSystem objects
/************************************************************************/
#include "ParticleManager.h"
#include "ParticleSystem.h"
#include "../Scene/Camera.h"

#include <SFML/System/Clock.hpp>

#include <cassert>

ParticleManager::ParticleManager()
	: systems()
	, timer()
{
	timer.Reset();
}

ParticleManager::~ParticleManager()
{
	clean();
}

void ParticleManager::add( ParticleSystem* system )
{
	assert(system != nullptr);
	systems.push_back(system);
}

void ParticleManager::remove( ParticleSystem* system )
{
	assert(system != nullptr);
	systems.remove(system);
}

void ParticleManager::update( const float delta )
{
	const float dt = timer.GetElapsedTime() * 0.01f;

	ParticleSystems deadSystems;

	for each(auto system in systems)
	{
		system->update(dt);

		if( !system->isAlive() )
			deadSystems.push_back(system);
	}

	for each(auto dead in deadSystems)
	{
		remove(dead);
		delete dead;
	}

	timer.Reset();
}

void ParticleManager::render( const Camera& camera )
{
	for each(auto system in systems)
	{
		system->render(camera);
	}
}

void ParticleManager::clean()
{
	for each(auto system in systems)
	{
		delete system;
	}
	systems.clear();
}
