/************************************************************************/
/* BoundingBox
/* ------
/* Box that specifies where the edges of an object are.
/************************************************************************/

#include "BoundingBox.h"
#include <iostream>


BoundingBox::BoundingBox(glm::vec3 pos, glm::vec3 greater, glm::vec3 lesser)
	: SceneObject(pos)
{
	edges = glm::mat2x3();
	edges[0] = lesser;
	edges[1] = greater;
}

BoundingBox::BoundingBox(MeshOverlay& mesh)//, glm::vec3 lesser, glm::vec3 greater)
{
	

	edges = glm::mat2x3();
	edges[0] = mesh.vertexAt(0,0);
	edges[1] = mesh.vertexAt(0,0);

	for(unsigned int i = 0; i < mesh.getWidth() ; ++i)
	{
		for(unsigned int j = 0; j < mesh.getHeight(); ++j)
		{
			for(int k = 0; k < 3; ++k)
			{
				edges[0][k] = edges[0][k] < mesh.vertexAt(i , j)[k] ? edges[0][k] : mesh.vertexAt(i , j)[k];
				edges[1][k] = edges[1][k] > mesh.vertexAt(i , j)[k] ? edges[0][k] : mesh.vertexAt(i , j)[k];
			}
		}
	}	

	//make local coords
	edges[0][0] -= mesh.getOffset().x;
	edges[1][0] -= mesh.getOffset().x;
	edges[0][2] -= mesh.getOffset().y;
	edges[1][2] -= mesh.getOffset().y;
	std::cout << edges[0][0] << std::endl;
	std::cout << edges[0][1] << std::endl;
	std::cout << edges[0][2] << std::endl;
	std::cout << edges[1][0] << std::endl;
	std::cout << edges[1][1] << std::endl;
	std::cout << edges[1][2] << std::endl;
}

BoundingBox::BoundingBox(ModelObject& obj, glm::vec3 lesser, glm::vec3 greater)
	: SceneObject(obj.getPos())
{
	//TODO: Figure out a good way to get vertices
	edges = glm::mat2x3();
	edges[0] = lesser;
	edges[1] = greater;
}

BoundingBox::BoundingBox(SceneObject& obj, glm::vec3 lesser, glm::vec3 greater)
	: SceneObject(obj.getPos())
{
	//TODO: Figure out a good way to get vertices
	edges = glm::mat2x3();
	edges[0] = lesser;
	edges[1] = greater;
}

BoundingBox::~BoundingBox()
{}