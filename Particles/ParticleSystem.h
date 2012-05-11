#pragma once
/************************************************************************/
/* ParticleSystem
/* ---------------
/* Controls a collection of ParticleEmitter objects
/************************************************************************/
#include "ParticleEmitter.h"
#include "../Scene/Camera.h"

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
	void add(ParticleEmitter* emitter);

	// Update all the particle emitters
	void update(const float delta); 
	// Render all the particle emitters 
	void render(const Camera& camera);
	// Cleanup all particle emitters
	void clean();

	// Start all particle emitters
	void start();
	// Stop all particle emitters
	void stop();

	// Returns true if any emitter is still emitting, false otherwise
	bool isAlive() const;

	bool isVisible() const;
	void setVisible(const bool v);
};

bool ParticleSystem::isVisible() const { return visible; }
void ParticleSystem::setVisible(const bool v) { visible = v; }
