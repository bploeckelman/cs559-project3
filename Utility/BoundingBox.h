#pragma once
/************************************************************************/
/* BoundingBox
/* ------
/* Box that specifies where the edges of an object are.
/************************************************************************/
#include "Mesh.h"
#include "../Scene/Objects.h"
#include "../Scene/SceneObject.h"
#include "../Scene/MeshOverlay.h"

#include <glm\glm.hpp>


class BoundingBox : public SceneObject
{

private:
	glm::mat2x3 edges;

public:

	BoundingBox(glm::vec3 pos, glm::vec3 greater, glm::vec3 lesser);
	BoundingBox(MeshOverlay& mesh);//, glm::vec3 lesser, glm::vec3 greater);
	BoundingBox(ModelObject& obj, glm::vec3 lesser, glm::vec3 greater);
	BoundingBox(SceneObject& obj, glm::vec3 lesser, glm::vec3 greater);
	BoundingBox(Fluid& fluid);

	~BoundingBox();
	
	const glm::mat2x3& getEdges();
	const bool inBox(glm::vec3 vec);
	const bool intersect(BoundingBox* box);
	void draw();
};

const glm::mat2x3& BoundingBox::getEdges() { return edges;}

const bool BoundingBox::inBox(glm::vec3 vec)
{
	if(	vec.x >= edges[0].x && vec.x <= edges[1].x &&
		vec.y >= edges[0].y && vec.y <= edges[1].y &&
		vec.z >= edges[0].z && vec.z <= edges[1].z)
	{
		return true;
	}
	return false;
}

const bool BoundingBox::intersect(BoundingBox* box)
{
	//only uses corners, so its super imprecise and fairly inaccurate
	float step = 50;
	for(int i = 0; i < 2; ++i)
	for(int j = 0; j < 2; ++j)
	for(int k = 0; k < step; ++k)
	if(this->inBox(glm::vec3(box->getEdges()[i].x, box->getEdges()[j].y, box->getEdges()[0].z) + 
		(glm::length((glm::vec3(box->getEdges()[i].x, box->getEdges()[j].y, box->getEdges()[1].z) - glm::vec3(box->getEdges()[i].x, box->getEdges()[j].y, box->getEdges()[0].z))) / step) * k))
		return true;

	for(int i = 0; i < 2; ++i)
	for(int j = 0; j < 2; ++j)
	for(int k = 0; k < step; ++k)
	if(this->inBox(glm::vec3(box->getEdges()[0].x, box->getEdges()[j].y, box->getEdges()[i].z) + 
		(glm::length((glm::vec3(box->getEdges()[1].x, box->getEdges()[j].y, box->getEdges()[i].z) - glm::vec3(box->getEdges()[0].x, box->getEdges()[j].y, box->getEdges()[i].z))) / step) * k))
		return true;

	for(int i = 0; i < 2; ++i)
	for(int j = 0; j < 2; ++j)
	for(int k = 0; k < step; ++k)
	if(this->inBox(glm::vec3(box->getEdges()[i].x, box->getEdges()[0].y, box->getEdges()[j].z) + 
		(glm::length((glm::vec3(box->getEdges()[i].x, box->getEdges()[1].y, box->getEdges()[j].z) - glm::vec3(box->getEdges()[i].x, box->getEdges()[0].y, box->getEdges()[j].z))) / step) * k))
		return true;

	for(int i = 0; i < 2; ++i)
	for(int j = 0; j < 2; ++j)
	for(int k = 0; k < step; ++k)
	if(box->inBox(glm::vec3(this->getEdges()[i].x, this->getEdges()[j].y, this->getEdges()[0].z) + 
		(glm::length((glm::vec3(this->getEdges()[i].x, this->getEdges()[j].y, this->getEdges()[1].z) - glm::vec3(this->getEdges()[i].x, this->getEdges()[j].y, this->getEdges()[0].z))) / step) * k))
		return true;

	for(int i = 0; i < 2; ++i)
	for(int j = 0; j < 2; ++j)
	for(int k = 0; k < step; ++k)
	if(box->inBox(glm::vec3(this->getEdges()[0].x, this->getEdges()[j].y, this->getEdges()[i].z) + 
		(glm::length((glm::vec3(this->getEdges()[1].x, this->getEdges()[j].y, this->getEdges()[i].z) - glm::vec3(this->getEdges()[0].x, this->getEdges()[j].y, this->getEdges()[i].z))) / step) * k))
		return true;

	for(int i = 0; i < 2; ++i)
	for(int j = 0; j < 2; ++j)
	for(int k = 0; k < step; ++k)
	if(box->inBox(glm::vec3(this->getEdges()[i].x, this->getEdges()[0].y, this->getEdges()[0].z) + 
		(glm::length((glm::vec3(this->getEdges()[i].x, this->getEdges()[1].y, this->getEdges()[1].z) - glm::vec3(this->getEdges()[i].x, this->getEdges()[0].y, this->getEdges()[0].z))) / step) * k))
		return true;

	/*for(int i = 0; i < 2; ++i)
	for(int j = 0; j < 2; ++j)
	for(int k = 0; k < 2; ++k)
	if(box->inBox(glm::vec3(this->getEdges()[i].x, this->getEdges()[j].y, this->getEdges()[k].z)))
		return true;*/

	return false;
}
