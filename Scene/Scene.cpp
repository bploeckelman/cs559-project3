/************************************************************************/
/* Scene
/* -----
/* A scene consisting of a camera and a number of game objects
/************************************************************************/
#include "Scene.h"
#include "Camera.h"
#include "Skybox.h"
#include "../Utility/Plane.h"
#include "../Utility/RenderUtils.h"
#include "../Core/Common.h"
#include "../Core/ImageManager.h"

#include <glm/glm.hpp>

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
	ImageManager::get().addResourceDir("../");
	ImageManager::get().addResourceDir("../../Resources/images/");

	// setup meshes
	heightmap.loadFromImage("heightmap-of-disapproval.png");

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
//		Render::plane(Plane(vec3(0,0,0), vec3(0,1,0)), 50.f);
	glPopMatrix();

	glPushMatrix();
		heightmap.render(camera);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	Render::vector(vec3(1,0,0), vec3(0,0,0), vec3(1,0,0));
	Render::vector(vec3(0,1,0), vec3(0,0,0), vec3(0,1,0));
	Render::vector(vec3(0,0,1), vec3(0,0,0), vec3(0,0,1));

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
