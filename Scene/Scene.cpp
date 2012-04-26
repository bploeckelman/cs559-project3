/************************************************************************/
/* Scene
/* -----
/* A scene consisting of a camera and a number of game objects
/************************************************************************/
#include "Scene.h"
#include "Camera.h"
#include "Skybox.h"
#include "Fluid.h"          // *TESTING*
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
	, fluid(nullptr)
{ }

Scene::~Scene()
{
	cleanup();
}

void Scene::setup()
{
	// setup opengl state
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_POINTS);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(10.f);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClearDepth(1.f);

	glClearColor(0.1f, 0.1f, 0.1f, 1.f);

	setupLights();

	// setup textures
	ImageManager::get().addResourceDir("Resources/images/");
	ImageManager::get().addResourceDir("../");
	ImageManager::get().addResourceDir("../../Resources/images/");

	// setup meshes
	heightmap.loadFromImage("heightmap-test.png");

	// Generate a new fluid surface
	// width,height
	// distance between verts
	// time step for evaluation
	// wave velocity
	// viscosity
	//                 w   h    d      t     c    mu
	fluid = new Fluid(128, 128, 0.5f, 0.03f, 4.0f, 0.4f);

	// create and position cameras
	cameras.push_back(Camera());
	camera = &(cameras.back());
}

void Scene::setupLights()
{
	glEnable(GL_LIGHTING);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);

	GLfloat gAmbient[] = { 0.1f, 0.1f, 0.1f, 1.f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gAmbient);

	GLfloat ambient[] = { 0.3f, 0.3f, 0.3f, 1.f };
//	GLfloat diffuse[] = {.3f, .3f, .3f, 1.f }; //diffuse is gray
	GLfloat specular[] = { 1.f, 1.f, 1.f, 1.f };
	GLfloat position1[] = { 0.f, 10.f, 0.f, 1.f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
//	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, position1);

	GLfloat lightPosition2[] = {10.f, 10.f, 10.f, 1.f};
	GLfloat lightPosition3[] = {-10.f, 10.f, -10.f, 1.f};
	GLfloat orangeLight[] = {1.0f, 0.54f, 0.0f, 1.f};
	GLfloat whiteLight[] = {1.0f, 1.0f, 1.0f, 1.f};

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_SPECULAR, orangeLight);

	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	glLightfv(GL_LIGHT2, GL_SPECULAR, whiteLight);
}

void Scene::update( const Clock& clock, const Input& input )
{
	camera->processInput(input, clock);

	if( input.IsKeyDown(Key::Space) )
		fluid->displace();

	fluid->evaluate();
}

void Scene::render( const Clock& clock )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera->apply();

	skybox.render(*camera);
	heightmap.render(camera);
	fluid->render();

	glDisable(GL_TEXTURE_2D);
	Render::vector(vec3(1,0,0), vec3(0,0,0), vec3(1,0,0));
	Render::vector(vec3(0,1,0), vec3(0,0,0), vec3(0,1,0));
	Render::vector(vec3(0,0,1), vec3(0,0,0), vec3(0,0,1));
	glEnable(GL_TEXTURE_2D);
}

void Scene::init()
{
	Log("\nInitializing scene...");
	setup();
}

void Scene::cleanup()
{
	Log("\nCleaning up scene...");
	delete fluid;
}
