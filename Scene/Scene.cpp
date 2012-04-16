/************************************************************************/
/* Scene
/* -----
/* A scene consisting of a camera and a number of game objects
/************************************************************************/
#include "Scene.h"
#include "Camera.h"
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
	renderTestCube(glm::vec3(0.f, -0.6f, 0.f));

	// XZ plane
	static Plane plane(glm::vec3(0,0,0), glm::vec3(0,1,0));
	glColor3f(0.f, 0.3f, 0.f);
	renderTestPlane(plane);
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
	const glm::vec3  s(n.y - n.z, n.z - n.x, n.x - n.y); // orthogonal to n
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
