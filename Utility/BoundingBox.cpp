/************************************************************************/
/* BoundingBox
/* ------
/* Box that specifies where the edges of an object are.
/************************************************************************/

#include <glm\glm.hpp>
#include "Mesh.h"
#include "../Scene/MeshOverlay.h"
#include "../Scene/Objects.h"
#include "BoundingBox.h"

BoundingBox::BoundingBox(glm::vec3 greater, glm::vec3 lesser)
{
	edges = glm::mat2x3();
	edges[0] = lesser;
	edges[1] = greater;
}

BoundingBox::BoundingBox(Mesh& mesh)
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
				edges[0][k] < mesh.vertexAt(i , j)[k] ? edges[0][k] : mesh.vertexAt(i , j)[k];
				edges[1][k] > mesh.vertexAt(i , j)[k] ? edges[0][k] : mesh.vertexAt(i , j)[k];
			}
		}
	}
}

BoundingBox::BoundingBox(MeshOverlay& mesh)
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
				edges[0][k] < mesh.vertexAt(i , j)[k] ? edges[0][k] : mesh.vertexAt(i , j)[k];
				edges[1][k] > mesh.vertexAt(i , j)[k] ? edges[0][k] : mesh.vertexAt(i , j)[k];
			}
		}
	}	
}

BoundingBox::BoundingBox(ModelObject& obj)
{
	//TODO: Figure out a good way to get vertices
	edges = glm::mat2x3();
}

BoundingBox::~BoundingBox()
{}