#pragma once
/************************************************************************/
/* ParticleManager 
/* -----
/* Manages a collection of ParticleSystem objects
/************************************************************************/
#include "ParticleSystem.h"

#include <list>

namespace sf { class Clock; }

typedef std::list<ParticleSystem*>      ParticleSystems;
typedef ParticleSystems::iterator       ParticleSystemsIter;
typedef ParticleSystems::const_iterator ParticleSystemsConstIter;


class ParticleManager
{
private:
	ParticleSystems systems;
	sf::Clock       timer;

public:
	ParticleManager();
	~ParticleManager();

	// Add a new particle system 
	void add(ParticleSystem* system);
	// Remove the specified particle system
	void remove(ParticleSystem* system);

	// Update all the particle systems
	void update(const float delta);
	// Render all the particle systems
	void render(const Camera& camera);
	// Cleanup all particle systems
	void clean();

	const ParticleSystems& getSystems();
};

const ParticleSystems& ParticleManager::getSystems()	{return systems;}
