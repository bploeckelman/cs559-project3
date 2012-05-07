#pragma once
/************************************************************************/
/* SceneObject
/* ------
/* A simple 3d SceneObject
/************************************************************************/
#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SFML/System/Clock.hpp>


class SceneObject
{
protected:
	int       ID;
	glm::mat4 transform;

public:
	SceneObject();
	SceneObject(glm::vec3 pos);
	virtual ~SceneObject();

	virtual void draw(const Camera& camera);
	virtual void update(const sf::Clock &clock, const sf::Input& input);

	void setPos(const glm::vec3& pos);
	glm::vec3 getPos() const;
};

inline void SceneObject::setPos(const glm::vec3& pos) 
{
	transform = glm::translate(transform, -getPos());
	transform = glm::translate(transform, pos);
}

inline glm::vec3 SceneObject::getPos() const
{ 
	return glm::vec3(transform[3][0], transform[3][1], transform[3][2]);
}

