/************************************************************************/
/* Scene
/* -----
/* A scene consisting of a camera and a number of game objects
/************************************************************************/
#include "Scene.h"
#include "Camera.h"
#include "../Core/Common.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Input.hpp>
#include <SFML/System/Randomizer.hpp>

using namespace sf;


Scene::Scene()
	: camera(nullptr)
	, cameras()
{
	init();
}

Scene::~Scene()
{
	cleanup();
}

void Scene::setup()
{
	// load textures

	// setup meshes

	// create and position cameras
	cameras.push_back(Camera());
	camera = &(cameras.back());
}

void Scene::update( const Clock& clock, const Input& input )
{
	camera->processInput(input, clock);
}

void Scene::render( const Clock& clock )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	camera->apply();
//	camera->lookAt(glm::vec3(5,10,5)    // camera position
//				 , glm::vec3(0,10,0)    // look at point
//				 , glm::vec3(0,1,0));   // up vector

	glColor3f(1.f, 0.f, 1.f);
	renderTestCube(glm::vec3(0.f, 0.f, 10.f));
}

void Scene::init()
{
	Log("Initializing scene...");
}

void Scene::cleanup()
{
	Log("Cleaning up scene...");
}

void Scene::renderTestCube(const glm::vec3& position, const float scale)
{
	glPushMatrix();

	glTranslated(position.x, position.y, position.z);

	const float nl = -0.5f * scale;
	const float pl =  0.5f * scale;

	glBegin(GL_QUADS);

	glNormal3d( 0,0,1);
	glVertex3d(pl,pl,pl);
	glVertex3d(nl,pl,pl);
	glVertex3d(nl,nl,pl);
	glVertex3d(pl,nl,pl);

	glNormal3d( 0, 0, -1);
	glVertex3d(pl,pl, nl);
	glVertex3d(pl,nl, nl);
	glVertex3d(nl,nl, nl);
	glVertex3d(nl,pl, nl);

	glNormal3d( 0, 1, 0);
	glVertex3d(pl,pl,pl);
	glVertex3d(pl,pl,nl);
	glVertex3d(nl,pl,nl);
	glVertex3d(nl,pl,pl);

	glNormal3d( 0,-1,0);
	glVertex3d(pl,nl,pl);
	glVertex3d(nl,nl,pl);
	glVertex3d(nl,nl,nl);
	glVertex3d(pl,nl,nl);

	glNormal3d( 1,0,0);
	glVertex3d(pl,pl,pl);
	glVertex3d(pl,nl,pl);
	glVertex3d(pl,nl,nl);
	glVertex3d(pl,pl,nl);

	glNormal3d(-1,0,0);
	glVertex3d(nl,pl,pl);
	glVertex3d(nl,pl,nl);
	glVertex3d(nl,nl,nl);
	glVertex3d(nl,nl,pl);

	glEnd();

	glPopMatrix(); 
}
