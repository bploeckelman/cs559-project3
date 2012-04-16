/************************************************************************/
/* Scene
/* -----
/* A scene consisting of a camera and a number of game objects
/************************************************************************/
#include "Scene.h"
#include "Camera.h"
#include "Skybox.h"
#include "../Core/Common.h"
#include "../Utility/Plane.h"

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
	, skybox()
	, heightmap()
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

	glPushMatrix();
		skybox.render(*camera);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
		heightmap.render(camera);
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
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

void Scene::renderTestPlane(const Plane& plane, const float radius)
{
	// Calculate/get plane details
	const glm::vec3& p(plane.point());
	const glm::vec3& n(plane.normal());
	const glm::vec3  o(n.y - n.z, n.z - n.x, n.x - n.y); //orthogonal to normal
	const glm::vec3  s(glm::normalize(o));
	const glm::vec3  t(glm::normalize(glm::cross(s,n)));
	const float d = plane.distance();

	// Calculate vertices
	const glm::vec3 v0(radius *
		glm::vec3(( s.x + t.x) + n.x * d
				, ( s.y + t.y) + n.y * d
				, ( s.z + t.z) + n.z * d));
	const glm::vec3 v1(radius *
		glm::vec3(( s.x - t.x) + n.x * d
				, ( s.y - t.y) + n.y * d
				, ( s.z - t.z) + n.z * d));
	const glm::vec3 v2(radius *
		glm::vec3((-s.x + t.x) + n.x * d
				, (-s.y + t.y) + n.y * d
				, (-s.z + t.z) + n.z * d));
	const glm::vec3 v3(radius *
		glm::vec3((-s.x - t.x) + n.x * d
				, (-s.y - t.y) + n.y * d
				, (-s.z - t.z) + n.z * d));

	// Draw the plane
	glDisable(GL_CULL_FACE);
	glBegin(GL_TRIANGLE_STRIP);
		glNormal3fv(glm::value_ptr(n));
		glVertex3fv(glm::value_ptr(v0));
		glVertex3fv(glm::value_ptr(v1));
		glVertex3fv(glm::value_ptr(v2));
		glVertex3fv(glm::value_ptr(v3));
	glEnd();
	glEnable(GL_CULL_FACE);
}
