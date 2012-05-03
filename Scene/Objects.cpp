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


static float lastTime = 0.f;

Fish::Fish(glm::vec3 pos, sf::Color color, HeightMap& heightmap, Fluid& fluid)
	: SceneObject(pos)
	 ,color(color)
	 ,theta(0)
	 ,direction(sf::Randomizer::Random(0,1) * 2 -1)
	 ,fluid(fluid)
	 ,heightmap(heightmap)
{
	quadric = gluNewQuadric();
}

Fish::~Fish()
{
	gluDeleteQuadric(quadric);
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

	if( heightmap.heightAt(this->getPos().x + randx, this->getPos().z + randz)  > this->getPos().y - .1f)		//should fix for when fish out of fluid
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
			glScalef(xscale, zscale, yscale);
			gluSphere(quadric, 1.f, 10, 10);
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
		2*(size*2) + 1,   // number of vertices wide
		2*(size*4) + 1,   // number of vertices high
		0.25f,  // distance between vertices
		0.03f, // time step for evaluation
		4.f,  // wave velocity
		0.4f   // fluid viscosity
		,pos.x - (size/2.f)
		,pos.y + .25f
		,pos.z - (size)
	);
}

Fountain::~Fountain()
{
	delete fluid;
}

void Fountain::update(const sf::Clock &clock)
{

	std::for_each(emitter.getParticles().begin(), emitter.getParticles().end(),
			[&](Particle& particle)
	{
		glm::vec3 translated = glm::vec3((particle.position.x - fluid->pos.x)/fluid->getDist(), particle.position.y, 
			(particle.position.z - fluid->pos.z)/fluid->getDist());
			
		if((translated.x > fluid->getWidth()) || (translated.x < 0) || (translated.z > fluid->getHeight()) || (translated.z < 0)) //outside fluid
		{
			particle.active = false;
		}
		if(particle.position.y <= fluid->pos.y)
		{
			fluid->displace(translated.x, translated.z, 1.f/(emitter.getMaxParticles()*100), particle.velocity.y);
			particle.active = false;
		}
	});
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

		// Inside
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

		//outside
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

			//bottom
			glNormal3d( 0,1,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w,-height,-l);
				glTexCoord2f(size * 2,    0.f);glVertex3d(-w,-height,l);
				glTexCoord2f(size * 2,    size);glVertex3d(w,-height,l);
				glTexCoord2f(0.f,   size);glVertex3d(w,-height,-l);
		
			
			
		glEnd();

		//top
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

Bush::Bush(glm::vec3 pos, float size) : 
	 SceneObject(pos)
	,side(GetImage("bush-side1.png"))
	,top(GetImage("bush-top.png"))
	,size(size)
{
};

Bush::~Bush()
{

}

void Bush::draw()
{
	glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();
		glMultMatrixf(glm::value_ptr(transform));

		side.Bind();
		glEnable(GL_BLEND);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

		float l =  size;
		float w =  size;

		glBegin(GL_QUADS);
			glNormal3d(-1,0,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(w,l,0);
				glTexCoord2f(0.f, 1.f);glVertex3d(w,-l,0);
				glTexCoord2f(1.f, 1.f);glVertex3d(-w,-l,0);
				glTexCoord2f(1.f,   0.f);glVertex3d(-w,l,0);
			glNormal3d(1,0,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w,l,0);
				glTexCoord2f(0.f, 1.f);glVertex3d(-w,-l,0);
				glTexCoord2f(1.f, 1.f);glVertex3d(w,-l,0);
				glTexCoord2f(1.f,   0.f);glVertex3d(w,l,0);
		glEnd();
		glRotatef(90.f, 0, 1, 0);
		glBegin(GL_QUADS);	
			glNormal3d(-1,0,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(w,l,0);
				glTexCoord2f(0.f, 1.f);glVertex3d(w,-l,0);
				glTexCoord2f(1.f, 1.f);glVertex3d(-w,-l,0);
				glTexCoord2f(1.f,   0.f);glVertex3d(-w,l,0);
			glNormal3d(1,0,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w,l,0);
				glTexCoord2f(0.f, 1.f);glVertex3d(-w,-l,0);
				glTexCoord2f(1.f, 1.f);glVertex3d(w,-l,0);
				glTexCoord2f(1.f,   0.f);glVertex3d(w,l,0);
		glEnd();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
}


Blimp::Blimp(glm::vec3 pos, float size):
	SceneObject(pos)
	,size(size)
	,btext(GetImage("blimp2.png"))
{
	quadric = gluNewQuadric();
}


Blimp::~Blimp()
{
	gluDeleteQuadric(quadric);
}

void Blimp::update(sf::Clock &clock)
{
}
	

void Blimp::draw()
{
	glEnable(GL_TEXTURE_2D);


	glPushMatrix();
		btext.Bind();	
		glMultMatrixf(glm::value_ptr(transform));
		glRotatef(90, 1, 0, 0);
		glRotatef(90, 0, 0, 1);
	    gluQuadricDrawStyle( quadric, GLU_FILL);
	    gluQuadricNormals( quadric, GLU_SMOOTH);
	    gluQuadricOrientation( quadric, GLU_OUTSIDE);
	    gluQuadricTexture( quadric, GL_TRUE);
	    glColor3f(1.0, 1.0, 1.0);
	    glScalef(size, size*2, size);
		gluSphere(quadric, 1.f, 20, 20);
		glTranslatef(0, 0, size/16);
		gluQuadricTexture( quadric, GL_FALSE);
		glDisable(GL_BLEND);
		glColor4ub(128, 128, 128, 1.f);
		gluQuadricNormals( quadric, GLU_SMOOTH);
		gluCylinder(quadric, size/16, size/16, size/16, 20, 20);
		glTranslatef(0, 0, size/16);
		gluQuadricNormals( quadric, GLU_SMOOTH);
		gluDisk(quadric, 0, size/16, 20, 20);
	glPopMatrix();
	glEnable(GL_BLEND);
}