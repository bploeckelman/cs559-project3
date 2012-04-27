#pragma once
/************************************************************************/
/* ParticleEmitter
/* ---------------
/* Manages a collection of Particle objects
/************************************************************************/
#include "Particle.h"

#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/Image.hpp>

#include <vector>

typedef std::vector<Particle>     Particles;
typedef Particles::iterator       ParticlesIter;
typedef Particles::const_iterator ParticlesConstIter;


enum BlendMode { NONE = 0, ALPHA, ADD, MULTIPLY };


class ParticleEmitter
{
private:
	Particles particles;

	unsigned int maxParticles;
	unsigned int oneTimeNumParticles;

	glm::vec3  position;
	sf::Image *texture;
	BlendMode  blendMode;

	float emissionRate;
	float emissionCounter;
	float lifetime;
	float age;

	bool emitting;
	bool alive;
	bool paused;
	bool grayscale;
	bool oneTimeEmission;

public:
	ParticleEmitter(const unsigned int maxParticles
				  , const float lifetime = -1.f);
	virtual ~ParticleEmitter();

	// Initialize this emitter
	virtual void init();
	// Update all the particles
	virtual void update(const sf::Clock& clock);
	// Render all the particles
	virtual void render(const sf::Clock& clock);
	// Cleanup all particles
	virtual void clean();

	// Start emitting
	void start();
	// Stop emitting 
	void stop();

	bool isAlive() const;
	bool isEmitting() const;

	void setPosition(const glm::vec3& p);
	void setLifetime(const float lifetime);
	void setOneTimeEmission(bool oneTime);
	void setOneTimeNumParticles(const unsigned int num);
	void setEmissionRate(const float rate);
	void setBlendMode(const BlendMode& mode);
	void setTexture(const sf::Image* image);

protected:
	virtual void initParticle(Particle& p) = 0;
	virtual void emitParticles(const float deltaTime);

	void initGeometry();

private:
	virtual void subUpdate(const float deltaTime);
};
