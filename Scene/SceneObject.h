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
	glm::vec3	posEdge;
	glm::vec3	negEdge;
	int			ID;

public:
	SceneObject();
	SceneObject(float x, float y, float z);
	virtual ~SceneObject();

	virtual void draw();
	virtual void update(const sf::Clock &clock);

	glm::vec3 getPos() const;
	glm::vec3 getPosEdge() const;
	glm::vec3 getNegEdge() const;
};

inline glm::vec3 SceneObject::getPos() const	{ return pos;}
inline glm::vec3 SceneObject::getPosEdge() const	{ return posEdge;}
inline glm::vec3 SceneObject::getNegEdge() const	{ return negEdge;}