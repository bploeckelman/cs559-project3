#pragma once
/************************************************************************/
/* ParticleSystem
/* ---------------
/* Controls a collection of ParticleEmitter objects
/************************************************************************/
#include "ParticleEmitter.h"

#include <vector>

typedef std::vector<ParticleEmitter*>    ParticleEmitters;
typedef ParticleEmitters::iterator       ParticleEmittersIter;
typedef ParticleEmitters::const_iterator ParticleEmittersConstIter;


class ParticleSystem
{
private:
	ParticleEmitters emitters;
	bool visible;

public:
	ParticleSystem();
	~ParticleSystem();

	// Add a new particle emitter 
	bool add(ParticleEmitter* emitter);

	// Update all the particle emitters
	void update(const sf::Clock& clock);
	// Render all the particle emitters 
	void render(const sf::Clock& clock);
	// Cleanup all particle emitters
	void clean();

	// Start emitting
	void start();
	// Stop emitting 
	void stop();

	// Returns true if any emitter is still emitting, false otherwise
	bool isAlive() const;

	bool isVisible() const;
	void setVisible(const bool v);
};
