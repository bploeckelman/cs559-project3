/************************************************************************/
/* SceneObject
/* ------
/* A simple 3d SceneObject
/************************************************************************/

#include "SceneObject.h"


static int nextID = 0;

SceneObject::SceneObject()
	: pos()
	 ,posEdge()
	 ,negEdge()
	 ,ID(nextID)
{
	nextID++;
}

SceneObject::SceneObject(float x, float y, float z)
	: pos(x, y, z)
	 ,ID(nextID)
{
	nextID++;
}

SceneObject::~SceneObject()
{
}

void SceneObject::draw()
{
}

void SceneObject::update(const sf::Clock &clock)
{
}