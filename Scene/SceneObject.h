#pragma once
/************************************************************************/
/* SceneObject
/* ------
/* A simple 3d SceneObject
/************************************************************************/
#include <glm/glm.hpp>
#include <SFML/System/Clock.hpp>


class SceneObject
{
protected:
	glm::vec3	pos;
	int			ID;

public:
	SceneObject();
	SceneObject(float x, float y, float z);
	virtual ~SceneObject();

	virtual void draw();
	virtual void update(const sf::Clock &clock);

	glm::vec3 getPos() const;
};

inline glm::vec3 SceneObject::getPos() const	{ return pos;}