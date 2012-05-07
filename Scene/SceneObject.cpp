/************************************************************************/
/* SceneObject
/* ------
/* A simple 3d SceneObject
/************************************************************************/

#include "SceneObject.h"
#include "Camera.h"

static int nextID = 0;

SceneObject::SceneObject()
	: ID(nextID)
{
	nextID++;
}

SceneObject::SceneObject(glm::vec3 pos)
	: ID(nextID)
{
	transform = glm::mat4x4();
	transform[3] = glm::vec4(pos, 1.f);
	nextID++;
}

SceneObject::~SceneObject()
{
}

void SceneObject::draw(const Camera& camera)
{
}

void SceneObject::update(const sf::Clock &clock, const sf::Input& input)
{
}