/************************************************************************/
/* Buildings
/* ------
/* A class that contains all buildings
/************************************************************************/

#include "Buildings.h"
#include <glm\glm.hpp>
#include "../Utility/RenderUtils.h"

House::House()
	: color()
	 ,roof()
	 ,side()
{
}

House::House(float x, float y, float z, sf::Color color)
	: SceneObject(x, y, z)
	 ,roof(ImageManager::get().getImage("roof.png"))
	 ,side(ImageManager::get().getImage("side.png"))
	 ,color(color)
{
}

House::~House()
{
}

void House::draw()
{

	float size = 6.f;

	glPushMatrix();
		glTranslated(pos.x, pos.y, pos.z);

		side.Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		const float nl = -0.5f * size;
		const float pl =  0.5f * size;

		glBegin(GL_QUADS);
			glNormal3d( 0,0,1);
				glTexCoord2f(0.f,    0.f);glVertex3d(pl,pl,pl);
				glTexCoord2f(size,    0.f);glVertex3d(nl,pl,pl);
				glTexCoord2f(0.f,    size);glVertex3d(nl,nl,pl);
				glTexCoord2f(size,   size);glVertex3d(pl,nl,pl);
			glNormal3d( 0, 0, -1);
				glTexCoord2f(0.f,    0.f);glVertex3d(pl,pl, nl);
				glTexCoord2f(size,    size);glVertex3d(pl,nl, nl);
				glTexCoord2f(0.f,    size);glVertex3d(nl,nl, nl);
				glTexCoord2f(size,    0.f);glVertex3d(nl,pl, nl);
			glNormal3d( 1,0,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(pl,pl,pl);
				glTexCoord2f(size,   size);glVertex3d(pl,nl,pl);
				glTexCoord2f(0.f,    size);glVertex3d(pl,nl,nl);
				glTexCoord2f(size,   0.f);glVertex3d(pl,pl,nl);
			glNormal3d(-1,0,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(nl,pl,pl);
				glTexCoord2f(size,    0.f);glVertex3d(nl,pl,nl);
				glTexCoord2f(0.f,    size);glVertex3d(nl,nl,nl);
				glTexCoord2f(size,    size);glVertex3d(nl,nl,pl);
		glEnd();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();

	glm::vec3 tmp(pos);
	tmp.y += size/2.f;
	
	float radius = size/2.f + 1;
	float height = size/2.f;

	roof.Bind();

	glPushMatrix();
		glTranslated(tmp.x, tmp.y, tmp.z);
		glDisable(GL_TEXTURE_2D);
		glColor3f(0.f, 0.f, 0.f);
		glPushMatrix();
			glRotatef(180, 1, 0, 0);
			glBegin(GL_QUADS);
				glNormal3d(0, 1, 0);
				glVertex3d(-radius, 0, -radius);
				glVertex3d(-radius, 0, radius);
				glVertex3d(radius, 0, radius);
				glVertex3d(radius, 0, -radius);
			glEnd();
		glPopMatrix();
		glEnable(GL_TEXTURE_2D);



		glBegin(GL_TRIANGLE_FAN);
			glTexCoord2f(0.5f,    1.f);glVertex3d(0, height, 0);
			glTexCoord2f(1.f,    0.f);glVertex3d(-radius, 0, -radius);
			glTexCoord2f(0.f,    0.f);glVertex3d(-radius, 0, radius);
			glTexCoord2f(1.f,    0.f);glVertex3d(radius, 0, radius);
			glTexCoord2f(0.f,    0.f);glVertex3d(radius, 0, -radius);
			glTexCoord2f(1.f,    0.f);glVertex3d(-radius, 0, -radius);
		glEnd();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();

	posEdge = glm::vec3(tmp.x + radius, tmp.y + height, tmp.z + radius);
	negEdge = glm::vec3(tmp.x - radius, tmp.y - height, tmp.z - radius);

}