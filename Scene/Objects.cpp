/************************************************************************/
/* Objects
/* ------
/* A class that contains all objects in the scene
/************************************************************************/

#include "Objects.h"
#include <glm\glm.hpp>
#include "../Utility/RenderUtils.h"
#include <SFML\Graphics.hpp>
#include "../Particles/Particles.h"

static float lastTime = 0.f;

Fish::Fish(float x, float y, float z, sf::Color color, HeightMap& heightmap)
	: SceneObject(x, y, z)
	 ,posNeg(1)
	 ,color(color)
	 ,heightmap(heightmap)
{
}

Fish::~Fish()
{
}


void Fish::update(const sf::Clock &clock)
{
	//TODO: make this a little more flashy (all 3 axes instead of one, rotate instead of flip, fix minor visual glitch)
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

	glm::vec3 newPos = glm::vec3(this->getPos().x + randx, this->getPos().y, this->getPos().z + randz);
	glm::vec2 mapcoords(newPos.z / heightmap.getGroundScale(), newPos.x / heightmap.getGroundScale());
	if( mapcoords.x >= 0 && mapcoords.y >= 0
		&& mapcoords.x < (heightmap.getHeights().rows() - 1) && mapcoords.y < (heightmap.getHeights().cols() - 1) )
	{
		const unsigned int x = static_cast<unsigned int>(mapcoords.x);
		double influenceX = mapcoords.x - x;
		const unsigned int y = static_cast<unsigned int>(mapcoords.y);
		double influenceY = mapcoords.y - y;

		double yHeight = (heightmap.heightAt(x, y) * (1 - influenceY) + heightmap.heightAt(x, y + 1) * influenceY);
		double xHeight = (heightmap.heightAt(x + 1, y) * (1 - influenceY) + heightmap.heightAt(x + 1, y + 1) * influenceY);
		const double height = (yHeight * (1 - influenceX) + xHeight * influenceX) * heightmap.getHeightScale();

		if( height > .5f )		//need to reverse direction
			posNeg *= -1;

	}
	this->pos = glm::vec3(this->getPos().x + randx, this->getPos().y, this->getPos().z + (randz * posNeg));
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
		glRotatef(90*posNeg, 1, 0, 0);
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

Fountain::Fountain(float x, float y, float z, float size, ParticleEmitter& emitter)
	: SceneObject(x, y, z)
	 ,texture(ImageManager::get().getImage("fountain.png"))
	 ,emitter(emitter)
	 ,size(size)
{
	fluid = new Fluid(
		size*2 + 1,   // number of vertices wide
		size*4 + 1,   // number of vertices high
		0.5f,  // distance between vertices
		0.03f, // time step for evaluation
		4.0f,  // wave velocity
		0.4f   // fluid viscosity
		,x - (size)
		,y + .5f
		,z - (size/2.f)
	);
}

Fountain::~Fountain()
{
}

void Fountain::update(const sf::Clock &clock)
{
	fluid->evaluate();
}

void Fountain::draw()
{

	glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);

		texture.Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		float l =  1.f * size;
		float w =  0.5f * size;
		const float height = .5f;

		glBegin(GL_QUADS);
			glNormal3d( 0,0,1);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w,-height,-l);
				glTexCoord2f(size,    0.f);glVertex3d(w,-height,-l);
				glTexCoord2f(size,    1.f);glVertex3d(w,height,-l);
				glTexCoord2f(0.f,   1.f);glVertex3d(-w,height,-l);
			glNormal3d( 0, 0, -1);
				glTexCoord2f(0.f,    0.f);glVertex3d(w,-height, -l);
				glTexCoord2f(size * 2,    0.f);glVertex3d(w,-height, l);
				glTexCoord2f(size * 2,    1.f);glVertex3d(w,height, l);
				glTexCoord2f(0.f,   1.f);glVertex3d(w,height, -l);
			glNormal3d( 1,0,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(w,-height,l);
				glTexCoord2f(size, 0.f);glVertex3d(-w,-height,l);
				glTexCoord2f(size, 1.f);glVertex3d(-w,height,l);
				glTexCoord2f(0.f,   1.f);glVertex3d(w,height,l);
			glNormal3d(-1,0,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w,-height,l);
				glTexCoord2f(size * 2, 0.f);glVertex3d(-w,-height,-l);
				glTexCoord2f(size * 2, 1.f);glVertex3d(-w,height,-l);
				glTexCoord2f(0.f,   1.f);glVertex3d(-w,height,l);
		glEnd();
		

		l += 1;
		w += 1;

		glBegin(GL_QUADS);
			glNormal3d(-1,0,0);
				glTexCoord2f(size * 2,    0.f);glVertex3d(-w,-height,l);
				glTexCoord2f(0.f, 0.f);glVertex3d(w,-height,l);
				glTexCoord2f(0.f, 1.f);glVertex3d(w,height,l);
				glTexCoord2f(size * 2,   1.f);glVertex3d(-w,height,l);

			glNormal3d( 1,0,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w,-height,-l);
				glTexCoord2f(0.f,  1.f);glVertex3d(-w,height,-l);
				glTexCoord2f(size, 1.f);glVertex3d(w,height,-l);
				glTexCoord2f(size,   0.f);glVertex3d(w,-height,-l);

			glNormal3d( 0, 0, -1);
				glTexCoord2f(0.f,    0.f);glVertex3d(w,-height, -l);
				glTexCoord2f(0.f,    1.f);glVertex3d(w,height, -l);
				glTexCoord2f(size * 2,    1.f);glVertex3d(w,height, l);
				glTexCoord2f(size * 2,   0.f);glVertex3d(w,-height,l);


			glNormal3d( 0,0,-1);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w,-height,-l);
				glTexCoord2f(size * 2,    0.f);glVertex3d(-w,-height,l);
				glTexCoord2f(size * 2,    1.f);glVertex3d(-w,height,l);
				glTexCoord2f(0.f,   1.f);glVertex3d(-w,height,-l);

			glNormal3d( 0,1,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w,-height,-l);
				glTexCoord2f(size * 2,    0.f);glVertex3d(-w,-height,l);
				glTexCoord2f(size * 2,    size);glVertex3d(w,-height,l);
				glTexCoord2f(0.f,   size);glVertex3d(w,-height,-l);
		
			
			
		glEnd();

		glBegin(GL_QUADS);

			glNormal3d( 0,1,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w,height,-l);
				glTexCoord2f(0.f,    1.f);glVertex3d(-w,height,-l+ 1);
				glTexCoord2f(size,   1.f);glVertex3d(w,height,-l+ 1);
				glTexCoord2f(size,   0.f);glVertex3d(w,height,-l);

			glNormal3d( 0,1,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w,height,l-1);
				glTexCoord2f(0.f,    1.f);glVertex3d(-w,height,l);
				glTexCoord2f(size,   1.f);glVertex3d(w,height,l);
				glTexCoord2f(size,   0.f);glVertex3d(w,height,l-1);

			glNormal3d( 0,1,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w+1,height,-l+1);
				glTexCoord2f(0.f,    1.f);glVertex3d(-w,height,-l+1);
				glTexCoord2f(size*2,   1.f);glVertex3d(-w,height,l-1);
				glTexCoord2f(size*2,   0.f);glVertex3d(-w+1,height,l-1);

			glNormal3d( 0,1,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(w-1,height,l-1);
				glTexCoord2f(0.f,    1.f);glVertex3d(w,height,l-1);
				glTexCoord2f(size*2,   1.f);glVertex3d(w,height,-l+1);
				glTexCoord2f(size*2,   0.f);glVertex3d(w-1,height,-l+1);
		
			
			
		glEnd();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();

	fluid->render();
}
