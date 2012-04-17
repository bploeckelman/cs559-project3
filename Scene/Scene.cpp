/************************************************************************/
/* Scene
/* -----
/* A scene consisting of a camera and a number of game objects
/************************************************************************/
#include "Scene.h"
#include "Camera.h"
#include "Skybox.h"
#include "../Utility/Plane.h"
#include "../Core/Common.h"
#include "../Core/ImageManager.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Input.hpp>
#include <SFML/System/Randomizer.hpp>

using namespace sf;
using glm::vec3;


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
	// setup textures
	ImageManager::get().addResourceDir("Resources/images/");
	ImageManager::get().addResourceDir("./");
	ImageManager::get().addResourceDir("../");
	ImageManager::get().addResourceDir("../../Resources/images/");

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
//	camera->lookAt(vec3(5,10,5)    // camera position
//				 , vec3(0,10,0)    // look at point
//				 , vec3(0,1,0));   // up vector

	glPushMatrix();
		skybox.render(*camera);
	glPopMatrix();

	glPushMatrix();
		static const Plane plane(vec3(0,0,10), vec3(0,0,-1));
		renderTestPlane(plane);
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

void Scene::renderTestCube(const vec3& position, const float scale)
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
	const vec3& p(plane.point());
	const vec3& n(plane.normal());
	const vec3  o(n.y - n.z, n.z - n.x, n.x - n.y); //orthogonal to normal
	const vec3  s(glm::normalize(o));
	const vec3  t(glm::normalize(glm::cross(s,n)));
	const float d = plane.distance();

	// Calculate vertices
	const vec3 v0(radius *
			 vec3(( s.x + t.x) + n.x * d
				, ( s.y + t.y) + n.y * d
				, ( s.z + t.z) + n.z * d));
	const vec3 v1(radius *
			 vec3(( s.x - t.x) + n.x * d
				, ( s.y - t.y) + n.y * d
				, ( s.z - t.z) + n.z * d));
	const vec3 v2(radius *
			 vec3((-s.x + t.x) + n.x * d
				, (-s.y + t.y) + n.y * d
				, (-s.z + t.z) + n.z * d));
	const vec3 v3(radius *
			 vec3((-s.x - t.x) + n.x * d
				, (-s.y - t.y) + n.y * d
				, (-s.z - t.z) + n.z * d));

	// Draw the plane
	glDisable(GL_CULL_FACE);

	const sf::Image& image(ImageManager::get().getImage("grid.png"));
	image.Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBegin(GL_TRIANGLE_STRIP);
		glNormal3fv(glm::value_ptr(n));
		glTexCoord2f(0.f,    0.f);    glVertex3fv(glm::value_ptr(v0));
		glTexCoord2f(radius, 0.f);    glVertex3fv(glm::value_ptr(v1));
		glTexCoord2f(0.f,    radius); glVertex3fv(glm::value_ptr(v2));
		glTexCoord2f(radius, radius); glVertex3fv(glm::value_ptr(v3));
	glEnd();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_CULL_FACE);
}
