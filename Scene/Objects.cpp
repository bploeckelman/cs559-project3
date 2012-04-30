/************************************************************************/
/* Objects
/* ------
/* A class that contains all objects in the scene
/************************************************************************/

#include "Objects.h"
#include <glm\glm.hpp>
#include "../Utility/RenderUtils.h"
#include <SFML\Graphics.hpp>


static float lastTime = 0.f;

Fish::Fish()
	: color()
	
{
}

Fish::Fish(float x, float y, float z, sf::Color color)
	: SceneObject(x, y, z)
	 ,color(color)
{
}

Fish::~Fish()
{
}

void Fish::update(const sf::Clock &clock)
{
	
	if(lastTime < .0001)
	{
		lastTime += clock.GetElapsedTime();
		return;
	}
	else
	{
		lastTime = 0;
	}
	float randz = sf::Randomizer::Random(0.f, .05f);
	float randx = sf::Randomizer::Random(-.01f, .01f);

	this->pos = glm::vec3(this->getPos().x + randx, this->getPos().y, this->getPos().z + randz);
}

void Fish::draw()
{
	float size = .2f;
	
	float radiusx = size;
	float radiusy = size/4.f;
	float height = size;

	glDisable(GL_TEXTURE_2D);

	glColor3ub(color.r, color.g, color.b);

	glPushMatrix();
		glTranslated(pos.x, pos.y, pos.z);
		glRotatef(90, 1, 0, 0);
		glPushMatrix();
			glRotatef(180, 1, 0, 0);
			glBegin(GL_QUADS);
				glVertex3d(-radiusy, 0, -radiusx);
				glVertex3d(-radiusy, 0, radiusx);
				glVertex3d(radiusy, 0, radiusx);
				glVertex3d(radiusy, 0, -radiusx);
			glEnd();
		glPopMatrix();

		glBegin(GL_TRIANGLE_FAN);
			glVertex3d(0, height, 0);
			glVertex3d(-radiusy, 0, -radiusx);
			glVertex3d(-radiusy, 0, radiusx);
			glVertex3d(radiusy, 0, radiusx);
			glVertex3d(radiusy, 0, -radiusx);
			glVertex3d(-radiusy, 0, -radiusx);
		glEnd();

		float xscale = .1f;
		float yscale = .2f;
		float zscale = .35f;

		glPushMatrix();
			glTranslatef(0.f, height + zscale, 0.f);
			GLUquadricObj* obj = gluNewQuadric();
			glScalef(xscale, zscale, yscale);
			gluSphere(obj, 1.f, 10, 10);
			gluDeleteQuadric(obj);
		glPopMatrix();

	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	

}

Fountain::Fountain()
	: size()
	
{
}

Fountain::Fountain(float x, float y, float z, float size)
	: SceneObject(x, y, z)
	 ,size(size)
{
}

Fountain::~Fountain()
{
}

void Fountain::update(const sf::Clock &clock)
{
}

void Fountain::draw()
{

	glPushMatrix();

		glEnable(GL_TEXTURE_2D);
		glTranslated(pos.x, pos.y, pos.z);
		texture.Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		const float nl = -5.5f * size;
		const float pl =  .5f * size;

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

		glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}