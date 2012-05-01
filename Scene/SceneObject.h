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
	int				ID;
	glm::mat4x4		transform;

public:
	SceneObject();
	SceneObject(glm::vec3 pos);
	virtual ~SceneObject();

	virtual void draw();
	virtual void update(const sf::Clock &clock);

	glm::vec3 getPos() const;
};

inline glm::vec3 SceneObject::getPos() const	{ return glm::vec3(transform[3][0], transform[3][1], transform[3][2]) ;}
