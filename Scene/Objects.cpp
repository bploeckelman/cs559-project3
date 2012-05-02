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
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>


static float lastTime = 0.f;

Fish::Fish(glm::vec3 pos, sf::Color color, HeightMap& heightmap, Fluid& fluid)
	: SceneObject(pos)
	 ,color(color)
	 ,theta(0)
	 ,direction(sf::Randomizer::Random(0,1) * 2 -1)
	 ,fluid(fluid)
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
	float randx = sf::Randomizer::Random(-.02f, .02f);

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

		if( height > this->getPos().y - .1f)		//should fix for when fish out of fluid
		{		//need to turn
			transform[0][0] = glm::cos(theta);
			transform[2][0] = glm::sin(theta);
			transform[0][2] = -glm::sin(theta);
			transform[2][2] = glm::cos(theta);
			transform[1][1] = 1;
			theta += .1f * direction;
			transform[3][0] = transform[3][0] + (randx * glm::cos(theta)) + (randz * glm::sin(theta));
			transform[3][2] = transform[3][2] + (randz * glm::cos(theta)) + (randx * glm::sin(theta));
		}
		else
		{
			direction = sf::Randomizer::Random(0,1) * 2 - 1;
			transform[3][0] = transform[3][0] + (randx * glm::cos(theta)) + (randz * glm::sin(theta));
			transform[3][2] = transform[3][2] + (randz * glm::cos(theta)) + (randx * glm::sin(theta));
		}

	}
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
		glMultMatrixf(glm::value_ptr(transform));
		glRotatef(90.f, 1.f, 0.f, 0.f);
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

Fountain::Fountain(glm::vec3 pos, float size, ParticleEmitter& emitter)
	: SceneObject(pos)
	 ,texture(GetImage("fountain.png"))
	 ,emitter(emitter)
	 ,size(size)
	 ,count(0)
{
	fluid = new Fluid(
		(size*2) + 1,   // number of vertices wide
		(size*4) + 1,   // number of vertices high
		0.5f,  // distance between vertices
		0.03f, // time step for evaluation
		4.f,  // wave velocity
		0.4f   // fluid viscosity
		,pos.x - (size)
		,pos.y + .25f
		,pos.z - (size/2.f)
	);
}

Fountain::~Fountain()
{
	delete fluid;
}

void Fountain::update(const sf::Clock &clock)
{
	for each(auto particle in emitter.getParticles())
	{
		//std::cout << "POS:" << particle.position.z << std::endl;
		//std::cout << "EPOS" <<  emitter.getPos().z  << std::endl;
		//std::cout << "Flu" <<  fluid->pos.y  << std::endl;
		//std::cout << "FluidW:" <<  fluid->getHeight() << std::endl;
		//std::cout << "ALL:" << particle.position.z - emitter.getPos().z + (fluid->getHeight()/2.f)*fluid->getDist() << std::endl;
		glm::vec3 translated = glm::vec3(particle.position.x - emitter.getPos().x + (fluid->getWidth()/2.f)*fluid->getDist(), particle.position.y, 
			particle.position.z - emitter.getPos().z + (fluid->getHeight()/2.f)*fluid->getDist());

		if((translated.x > fluid->getWidth()) || (translated.x < 0) || (translated.z > fluid->getHeight()) || (translated.z < 0) || !particle.active) //outside fluid
		{
			//std::cout << "HERE" << std::endl;
			particle.active = false;
		}
		else if(particle.position.y + emitter.getPos().y <= fluid->pos.y)
		{
			//std::cout << "EPOS:" <<  particle.position.y + emitter.getPos().y  << std::endl;
			//std::cout << "Flu:" <<  fluid->pos.y  << std::endl;
			//std::cout << "X:" << translated.x << std::endl;
			//std::cout << "Z:" << translated.z << std::endl;
			//std::cout << "HIT" << std::endl;
			//std::cout << translated.x << std::endl;
			//std::cout << translated.z << std::endl;
			if(count == 100)	//TODO: SERIEUSLY THIS NEEDS TO GET FIXED
			{
				fluid->displace();
				count = 0;
			}
			else
				count++;
				
			//fluid->displace(translated.x, translated.z);
			particle.lifespan = 0;
			particle.active = false;
		}
	}
	fluid->evaluate();
}

void Fountain::draw()
{
	glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();
		glMultMatrixf(glm::value_ptr(transform));
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
