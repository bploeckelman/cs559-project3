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
				edges[0][k] = edges[0][k] < mesh.vertexAt(i , j)[k] ? edges[0][k] : mesh.vertexAt(i , j)[k];
				edges[1][k] = edges[1][k] > mesh.vertexAt(i , j)[k] ? edges[1][k] : mesh.vertexAt(i , j)[k];
			}
		}
	}	

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

BoundingBox::BoundingBox(Fluid& obj)
	: SceneObject(obj.pos)
{
	//TODO: Figure out a good way to get vertices
	edges = glm::mat2x3();
	edges[0] = glm::vec3(obj.pos.x, -FLT_MIN, obj.pos.z);
	edges[1] = glm::vec3(obj.getHeight()*obj.getDist() + obj.pos.x, obj.pos.y, obj.getWidth()*obj.getDist() + obj.pos.z);
}

BoundingBox::~BoundingBox()
{}

void BoundingBox::draw()
{
	glColor3f(0, 0, 0);
	glBegin(GL_QUADS);
		glVertex3f(this->getEdges()[0].x, this->getEdges()[0].y, this->getEdges()[0].z);
		glVertex3f(this->getEdges()[0].x, this->getEdges()[0].y, this->getEdges()[1].z);
		glVertex3f(this->getEdges()[0].x, this->getEdges()[1].y, this->getEdges()[1].z);
		glVertex3f(this->getEdges()[0].x, this->getEdges()[1].y, this->getEdges()[0].z);
	glEnd();

	glBegin(GL_QUADS);
		glVertex3f(this->getEdges()[1].x, this->getEdges()[0].y, this->getEdges()[1].z);
		glVertex3f(this->getEdges()[1].x, this->getEdges()[0].y, this->getEdges()[0].z);
		glVertex3f(this->getEdges()[1].x, this->getEdges()[1].y, this->getEdges()[0].z);
		glVertex3f(this->getEdges()[1].x, this->getEdges()[1].y, this->getEdges()[1].z);
	glEnd();

	glBegin(GL_QUADS);
		glVertex3f(this->getEdges()[0].x, this->getEdges()[0].y, this->getEdges()[1].z);
		glVertex3f(this->getEdges()[1].x, this->getEdges()[0].y, this->getEdges()[1].z);
		glVertex3f(this->getEdges()[1].x, this->getEdges()[1].y, this->getEdges()[1].z);
		glVertex3f(this->getEdges()[0].x, this->getEdges()[1].y, this->getEdges()[1].z);
	glEnd();

	glBegin(GL_QUADS);
		glVertex3f(this->getEdges()[1].x, this->getEdges()[0].y, this->getEdges()[0].z);
		glVertex3f(this->getEdges()[0].x, this->getEdges()[0].y, this->getEdges()[0].z);
		glVertex3f(this->getEdges()[0].x, this->getEdges()[1].y, this->getEdges()[0].z);
		glVertex3f(this->getEdges()[1].x, this->getEdges()[1].y, this->getEdges()[0].z);
	glEnd();

	glBegin(GL_QUADS);
		glVertex3f(this->getEdges()[0].x, this->getEdges()[1].y, this->getEdges()[0].z);
		glVertex3f(this->getEdges()[0].x, this->getEdges()[1].y, this->getEdges()[1].z);
		glVertex3f(this->getEdges()[1].x, this->getEdges()[1].y, this->getEdges()[1].z);
		glVertex3f(this->getEdges()[1].x, this->getEdges()[1].y, this->getEdges()[0].z);
	glEnd();

	glBegin(GL_QUADS);
		glVertex3f(this->getEdges()[1].x, this->getEdges()[0].y, this->getEdges()[1].z);
		glVertex3f(this->getEdges()[1].x, this->getEdges()[0].y, this->getEdges()[0].z);
		glVertex3f(this->getEdges()[0].x, this->getEdges()[0].y, this->getEdges()[0].z);
		glVertex3f(this->getEdges()[0].x, this->getEdges()[0].y, this->getEdges()[1].z);
	glEnd();
}
