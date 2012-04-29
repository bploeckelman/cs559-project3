#pragma once
/************************************************************************/
/* Particle
/* --------
/* An individual particle 
/************************************************************************/
#include <glm/glm.hpp>


class Particle
{
public:
	glm::vec3 position;
	glm::vec3 prevPosition;
	glm::vec3 velocity;
	glm::vec3 accel;
	glm::vec4 color;

	float rotation;
	float scale;
	float lifespan;
	float age;

	bool active;
	bool immortal;

	Particle( const glm::vec3& position     = glm::vec3(0,0,0)
			, const glm::vec3& prevPosition = glm::vec3(0,0,0)
			, const glm::vec3& velocity     = glm::vec3(0,0,0)
			, const glm::vec3& accel        = glm::vec3(0,0,0)
			, const glm::vec4& color        = glm::vec4(1,1,1,1)
			, const float rotation          = 0.f
			, const float scale             = 1.f
			, const float lifespan          = 0.f
			, const float age               = 0.f
			, const bool immortal           = false )
		: position     (position)
		, prevPosition (prevPosition)
		, velocity     (velocity)
		, accel        (accel)
		, color        (color)
		, rotation     (rotation)
		, scale        (scale)
		, lifespan     (lifespan)
		, age          (age)
		, active       (false)
		, immortal     (immortal)
	{ }

	void update(const float delta);
};


inline void Particle::update(const float delta)
{
	if( !active ) return;

	// SFML has a fairly short delta between frames
	const float dt = delta * 10.f;

	if( !immortal )
	{
		// Kill particle if appropriate
		if( age >= lifespan )
		{
			active = false;
			age    = 0.f;
			return;
		}
		else
		{
			age += dt;
		}
	}

	prevPosition = position;

	velocity += dt * accel;
	position += dt * velocity;
}
