/************************************************************************/
/* Objects
/* ------
/* A class that contains all objects in the scene
/************************************************************************/
#include "Objects.h"
#include "Skybox.h"
#include "../Particles/Particles.h"
#include "../Utility/RenderUtils.h"

#undef __glext_h_
#undef __glxext_h_
#undef __gl_h_
#include "../Framework/Utilities/GLee.h"

#include <SFML/Graphics.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace sf;
using namespace glm;


/************************************************************************/
/* Fountain
/* --------
/* A fountain consisting of a fluid surface, a particle emitter that 
/* disturbs the fluid surface, and some containing geometry
/************************************************************************/
Fish::Fish(vec3 pos, Color color, HeightMap& heightmap, Fluid& fluid)
	: SceneObject(pos)
	, posNeg(0)
	, direction(Randomizer::Random(0,1) * 2 -1)
	, theta(0.f)
	, fluid(fluid)
	, heightmap(heightmap)
	, quadric(gluNewQuadric())
	, color(color)
{ }

Fish::~Fish()
{
	gluDeleteQuadric(quadric);
}

void Fish::update(const Clock &clock)
{
	//TODO: make this a little more flashy (all 3 axes instead of one, rotate instead of flip, fix minor visual glitch)
	float randz = Randomizer::Random(0.f, .05f);
	float randx = Randomizer::Random(-.02f, .02f);

	if( heightmap.heightAt(this->getPos().x + randx, this->getPos().z + randz)  > this->getPos().y - .2f)		//should fix for when fish out of fluid
	{		//need to turn
		transform[0][0] = cos(theta);
		transform[2][0] = sin(theta);
		transform[0][2] = -sin(theta);
		transform[2][2] = cos(theta);
		transform[1][1] = 1;
		theta += .1f * direction;
		transform[3][0] = transform[3][0] + (randx * cos(theta)) + (randz * sin(theta));
		transform[3][2] = transform[3][2] + (randz * cos(theta)) + (randx * sin(theta));
	}
	else
	{
		direction = Randomizer::Random(0,1) * 2 - 1;
		transform[3][0] = transform[3][0] + (randx * cos(theta)) + (randz * sin(theta));
		transform[3][2] = transform[3][2] + (randz * cos(theta)) + (randx * sin(theta));
	}
}

void Fish::draw(const Camera& camera)
{
	float size = .2f;
	
	float radiusx = size;
	float radiusy = size/4.f;
	float height = size;

	glDisable(GL_TEXTURE_2D);

	glColor3ub(color.r, color.g, color.b);

	glPushMatrix();
		glMultMatrixf(value_ptr(transform));
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


/************************************************************************/
/* Fountain
/* --------
/* A fountain consisting of a fluid surface, a particle emitter that 
/* disturbs the fluid surface, and some containing geometry
/************************************************************************/
Fountain::Fountain(vec3 pos, float size, ParticleEmitter& emitter, Skybox *skybox)
	: SceneObject(pos)
	, count(0)
	, size(size)
	, fluid(nullptr)
	, texture(GetImage("fountain.png"))
	, emitter(emitter)
{
	const unsigned int sz = static_cast<unsigned int>(size);
	fluid = new Fluid(
		2*(sz*2) + 1,   // number of vertices wide
		2*(sz*4) + 1,   // number of vertices high
		0.25f,  // distance between vertices
		0.03f, // time step for evaluation
		4.f,  // wave velocity
		0.4f,  // fluid viscosity
		vec3(pos.x - (size/2.f), pos.y + .25f, pos.z - (size))
	);
	fluid->setSkybox(skybox);
}

Fountain::~Fountain()
{
	delete fluid;
}

void Fountain::update(const Clock &clock)
{
	std::for_each(emitter.getParticles().begin(), emitter.getParticles().end(),
			[&](Particle& particle)
	{
		vec3 translated(
			(particle.position.x - fluid->pos.x) / fluid->getDist(),
			particle.position.y,
			(particle.position.z - fluid->pos.z) / fluid->getDist()
		);

		if( translated.x < 0 || translated.z < 0
		 || translated.x > fluid->getWidth() 
		 || translated.z > fluid->getHeight() )
		{
			particle.active = false;
		}
		if(particle.position.y <= fluid->pos.y)
		{
			fluid->displace( translated.x
						   , translated.z
						   , 1.f / (emitter.getMaxParticles() * 100)
						   , particle.velocity.y * 2.5f);
			particle.active = false;
		}
	});
	fluid->evaluate();
}

void Fountain::draw(const Camera& camera)
{
	glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();
		glMultMatrixf(value_ptr(transform));
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


/************************************************************************/
/* Plant
/* -----
/* A single plant that is drawn using 4 crossed planes 
/* with a random plant texture applied
/************************************************************************/
const unsigned int NumPlantNames = 17;
const std::string PlantTextureNames[NumPlantNames] =
{
	"bush-banana.png",
	"bush-bougainvillier.png",
	"bush-cornus.png",
	"bush-dracena.png",
	"bush-dypsis.png",
	"bush-ginger.png",
	"tree-basic.png",
	"tree-beech.png",
	"tree-benjamina.png",
	"tree-ficus.png",
	"tree-lime1.png",
	"tree-lime2.png",
	"tree-oak.png",
	"tree-palm.png",
	"tree-pine.png",
	"tree-plane.png",
	"tree-sophora.png"
};

Plant::Plant(const vec3& pos)
	: SceneObject(pos)
{
	// Get a random plant texture
	const unsigned int r = Randomizer::Random(0, NumPlantNames - 1);
	const std::string name(PlantTextureNames[r]);
	texture = GetImage(name);

	// This is all a little hacky...
	static const float treeModifier = 2.f;
	static const float bushModifier = 0.5f;

	// Size trees differently than bushes
	if( name.compare(0, 4, "tree") == 0 )
		size = linearRand(7.f, 10.f);
	else
		size = linearRand(2.f, 4.f);

	// Offset the height by the random size
	setPos(vec3(pos) + vec3(0,size,0));
}

void Plant::draw(const Camera& camera)
{
	// This prevents the alpha of one cutout
	// from clipping against another cutout
	// TODO: ideally this should be set once for all Bushes
	// instead of toggled on and off for each
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL, 0.5f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	texture.Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	glPushMatrix();
		// Move the plant into position
		const mat4 newTransform(
			translate( mat4(1.0)
				, vec3(transform[3][0], transform[3][1], transform[3][2]) )
		);
		glMultMatrixf(value_ptr(newTransform));

		// TODO: this can be made cleaner by setting up a static vertex array
		// for one quad, and just rotating it between each draw call

		const float l =  size;
		const float w =  size;

		glColor3f(1.0, 1.0, 1.0);
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

		glRotatef(45.f, 0, 1, 0);
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

		glRotatef(45.f, 0, 1, 0);
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

		glRotatef(45.f, 0, 1, 0);
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
	glPopMatrix();

	// Reset OpenGL state
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}


/************************************************************************/
/* Blimp
/* -----
/* A Goodyear invasion! 
/************************************************************************/
Blimp::Blimp(vec3 pos, float size)
	: SceneObject(pos)
	, size(size)
	, count(0)
	, theta(0)
	, btext(GetImage("blimp2.png"))
	, quadric(gluNewQuadric())
{ }

Blimp::~Blimp()
{
	gluDeleteQuadric(quadric);
}

void Blimp::update(const Clock &clock)
{
	transform[0][0] = cos(theta);
	transform[2][0] = sin(theta);
	transform[0][2] = -sin(theta);
	transform[2][2] = cos(theta);
	transform[1][1] = 1;
	transform[3][0] = 100 + sin(theta) * 40;
	transform[3][2] = 100 + cos(theta) * 40;
	theta -= .001f;
}
	
void Blimp::draw(const Camera& camera)
{
	glEnable(GL_TEXTURE_2D);
	btext.Bind();	

	glPushMatrix();
		glMultMatrixf(value_ptr(transform));
		glPushMatrix();
			glRotatef(90.f, 1, 0, 0);
			glRotatef(90.f, 0, 0, 1);

			gluQuadricDrawStyle( quadric, GLU_FILL);
			gluQuadricNormals( quadric, GLU_SMOOTH);
			gluQuadricOrientation( quadric, GLU_OUTSIDE);
			gluQuadricTexture( quadric, GL_TRUE);

			glColor4f(1.f, 1.f, 1.f, 1.f);
			glScalef(size, size*2, size);
			gluSphere(quadric, 1.f, 20, 20);

//			glDisable(GL_BLEND);
			gluQuadricTexture( quadric, GL_FALSE);
			gluQuadricNormals( quadric, GLU_SMOOTH);

			glColor4f(0.5f, 0.5f, 0.5f, 1.f);;
			glTranslatef(0, 0, size/16);
			gluCylinder(quadric, size/16, size/16, size/16, 20, 20);

			glTranslatef(0, 0, size/16);
			gluQuadricNormals( quadric, GLU_SMOOTH);
			gluDisk(quadric, 0, size/16, 20, 20);
		glPopMatrix();
	glPopMatrix();

//	glEnable(GL_BLEND);
}


/************************************************************************/
/* Campfire
/* --------
/* A cozy little campfire, with some logs 
/* and some particles for atmosphere
/************************************************************************/
Campfire::Campfire(vec3 pos, ParticleEmitter& fire, ParticleEmitter& smoke, float size = 4.f) 
	: SceneObject(pos)
	, size(size)
	, fire(fire)
	, smoke(smoke)
	, quadric(gluNewQuadric())
	, wood(GetImage("cedar.png"))
{ }

Campfire::~Campfire()
{
	gluDeleteQuadric(quadric);
}

void Campfire::draw(const Camera& camera)
{
	glEnable(GL_TEXTURE_2D);
	wood.Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

	gluQuadricDrawStyle( quadric, GLU_FILL);
	gluQuadricNormals( quadric, GLU_SMOOTH);
	gluQuadricOrientation( quadric, GLU_OUTSIDE);
	gluQuadricTexture( quadric, GL_TRUE);

//	glColor4ub(150, 75, 0, 255);
	glColor3f(0.7f, 0.7f, 0.7f);

	glPushMatrix();
		glMultMatrixf(value_ptr(transform));
		glPushMatrix();
			glRotatef(30, 0, 1, 0);
			gluCylinder(quadric, .5, .5, 2, 15, 15);
			glTranslatef(0, 0, 2);
			gluDisk(quadric, 0, .5, 15, 15);
		glPopMatrix();
		glPushMatrix();
			glRotatef(-30, 0, 1, 0);
			gluCylinder(quadric, .5, .5, 2, 15, 15);
			glTranslatef(0, 0, 2);
			gluDisk(quadric, 0, .5, 15, 15);
		glPopMatrix();
		glPushMatrix();
			glRotatef(180, 1, 0, 0);
			glPushMatrix();
				glRotatef(30, 0, 1, 0);
				gluCylinder(quadric, .5, .5, 2, 15, 15);
				glTranslatef(0, 0, 2);
				gluDisk(quadric, 0, .5, 15, 15);
			glPopMatrix();
			glPushMatrix();
				glRotatef(-30, 0, 1, 0);
				gluCylinder(quadric, .5, .5, 2, 15, 15);
				glTranslatef(0, 0, 2);
				gluDisk(quadric, 0, .5, 15, 15);
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}


/************************************************************************/
/* Model Object
/* --------
/* Holds a ObjModel
/************************************************************************/

ModelObject::ModelObject( glm::vec3 pos, const std::string& filename, HeightMap& heightmap, float size =1.f)
	: SceneObject(pos)
	, size(size)
	, heightmap(heightmap)
	, model(new ObjModel(filename))
{
	transform[0][0] = size;
	transform[1][1] = size;
	transform[2][2] = size;
	if( model != nullptr )
	{
		model->setRenderMode(GLM_SMOOTH | GLM_TEXTURE);
	}
}

ModelObject::~ModelObject()
{
	delete model;
}

void ModelObject::draw(const Camera& camera)
{
	if( model != nullptr )
	{
		glPushMatrix();
			glMultMatrixf(glm::value_ptr(transform));
			model->render();
		glPopMatrix();
	}
}

void ModelObject::update(const sf::Clock &clock)
{
}
