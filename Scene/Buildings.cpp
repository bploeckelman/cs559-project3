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

	//TODO: needs work, should really probably move the texturing into RenderUtils
	float size = 6.f;

	Render::cube(getPos(), size);

	glPushMatrix();
	glTranslated(pos.x+.1, pos.y, pos.z-.1);

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
			glTexCoord2f(size,    0.f);glVertex3d(pl,nl, nl);
			glTexCoord2f(0.f,    size);glVertex3d(nl,nl, nl);
			glTexCoord2f(size,    size);glVertex3d(nl,pl, nl);
		glNormal3d( 1,0,0);
			glTexCoord2f(0.f,    0.f);glVertex3d(pl,pl,pl);
			glTexCoord2f(size,    0.f);glVertex3d(pl,nl,pl);
			glTexCoord2f(0.f,    size);glVertex3d(pl,nl,nl);
			glTexCoord2f(size,   size);glVertex3d(pl,pl,nl);
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
	Render::pyramid(tmp, size/2.f + .5f, size/2.f);


}