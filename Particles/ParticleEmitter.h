#pragma once
/************************************************************************/
/* ParticleEmitter
/* ---------------
/* Manages a collection of Particle objects
/************************************************************************/
#include "Particle.h"
#include "../Scene/Camera.h"

#include <SFML/Graphics/Image.hpp>

#include <vector>

typedef std::vector<Particle>     Particles;
typedef Particles::iterator       ParticlesIter;
typedef Particles::const_iterator ParticlesConstIter;


enum BlendMode { NONE = 0, ALPHA, ADD, MULTIPLY };


class ParticleEmitter
{
protected:
	static const glm::vec3 vertices[];
	static const glm::vec2 texcoords[];
	static const unsigned char indices[];

	Particles particles;

	unsigned int maxParticles;
	unsigned int oneTimeNumParticles;

	glm::vec3  position;
	const sf::Image *texture;
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
	virtual void update(const float delta);
	// Render all the particles
	virtual void render(const Camera& camera);
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

private:
	virtual void subUpdate(const float deltaTime) { }
};

inline void ParticleEmitter::start() { emitting = true; }
inline void ParticleEmitter::stop()  { emitting = false; }

inline bool ParticleEmitter::isAlive() const { return alive; }
inline bool ParticleEmitter::isEmitting() const { return emitting; }

inline void ParticleEmitter::setPosition(const glm::vec3& p) { position = p; }
inline void ParticleEmitter::setLifetime(const float l) { lifetime = l; }
inline void ParticleEmitter::setOneTimeEmission(const bool o) { oneTimeEmission = o; }
inline void ParticleEmitter::setOneTimeNumParticles(const unsigned int n) { oneTimeNumParticles = n; }
inline void ParticleEmitter::setEmissionRate(const float r) { emissionRate = r; }
inline void ParticleEmitter::setBlendMode(const BlendMode& m) { blendMode = m; }
inline void ParticleEmitter::setTexture(const sf::Image* t) { texture = t; }
