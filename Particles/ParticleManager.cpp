/************************************************************************/
/* ParticleManager 
/* -----
/* Manages a collection of ParticleSystem objects
/************************************************************************/
#include "ParticleManager.h"
#include "ParticleSystem.h"
#include "../Scene/Camera.h"

#include <cassert>


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
	ParticleSystems deadSystems;

	for each(auto system in systems)
	{
		system->update(delta);

		if( !system->isAlive() )
			deadSystems.push_back(system);
	}

	for each(auto dead in deadSystems)
	{
		remove(dead);
		delete dead;
	}
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
