#pragma once
/************************************************************************/
/* ParticleManager 
/* -----
/* Manages a collection of ParticleSystem objects
/************************************************************************/
#include "ParticleSystem.h"

#include <list>

typedef std::list<ParticleSystem*>      ParticleSystems;
typedef ParticleSystems::iterator       ParticleSystemsIter;
typedef ParticleSystems::const_iterator ParticleSystemsConstIter;


class ParticleManager
{
private:
	ParticleSystems systems;

public:
	ParticleManager();
	~ParticleManager();

	// Add a new particle system 
	bool add(ParticleSystem* system);
	// Remove the specified particle system
	bool del(ParticleSystem* system);

	// Update all the particle systems
	void update(const float delta);
	// Render all the particle systems
	void render();
	// Cleanup all particle systems
	void clean();
};
