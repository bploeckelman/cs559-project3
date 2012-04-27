/************************************************************************/
/* Scene
/* -----
/* A scene consisting of a camera and a number of game objects
/************************************************************************/
#include "Scene.h"
#include "Camera.h"
#include "Skybox.h"
#include "Fluid.h"          // *TESTING*
#include "Buildings.h"
#include "../Utility/Plane.h"
#include "../Utility/RenderUtils.h"
#include "../Core/Common.h"
#include "../Core/ImageManager.h"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
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
	glLineWidth(3.f);

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

	objects.push_back(new House(10, 4, 10, sf::Color(0, 255, 0)));

	// create and position cameras
	cameras.push_back(Camera(glm::vec3(-2.5, 25.0, -2.5)   // position
							,glm::vec3(40.0, 135.0, 0.0)));// rotation
	camera = &cameras[0];
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
	CameraVectorIter it  = cameras.begin();
	CameraVectorIter end = cameras.end();
	for(; it != end; ++it)
	{
		Camera& cam = *it;
		cam.update(clock, input);
	}
//	for each(auto cam in cameras)
//		cam.update(clock, input);

	if( input.IsKeyDown(Key::Space) )
		fluid->displace();

	fluid->evaluate();

	for(unsigned int i = 0; i < objects.size(); ++i){
		objects[i]->update(clock);
		objects[i]->draw();
	}
}

void Scene::render( const Clock& clock )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera->apply();

	skybox.render(*camera);
	heightmap.render(camera);
	fluid->render();


	for(unsigned int i = 0; i < objects.size(); ++i){
		objects[i]->draw();
	}

	glm::vec3 campos = camera->position();

	for(unsigned int i = 0; i < objects.size(); ++i){
		if((campos.x > objects[i]->getNegEdge().x - 1.5f && campos.x < objects[i]->getPos().x) && campos.z > objects[i]->getNegEdge().z && campos.z < objects[i]->getPosEdge().z)
			camera->position(glm::vec3(objects[i]->getNegEdge().x - 1.5f, campos.y, campos.z));
		if((campos.x < objects[i]->getPosEdge().x + 1.5f && campos.x > objects[i]->getPos().x) && campos.z > objects[i]->getNegEdge().z && campos.z < objects[i]->getPosEdge().z)
			camera->position(glm::vec3(objects[i]->getPosEdge().x + 1.5f, campos.y, campos.z));
		if((campos.z > objects[i]->getNegEdge().z - 1.5f && campos.z < objects[i]->getPos().z) && campos.x > objects[i]->getNegEdge().x && campos.x < objects[i]->getPosEdge().x)
			camera->position(glm::vec3(campos.x, campos.y, objects[i]->getNegEdge().z - 1.5f));
		if((campos.z < objects[i]->getPosEdge().z + 1.5f && campos.z > objects[i]->getPos().z) && campos.x > objects[i]->getNegEdge().x && campos.x < objects[i]->getPosEdge().x)
			camera->position(glm::vec3(campos.x, campos.y, objects[i]->getPosEdge().z + 1.5f));
	}


	Render::basis();
}

void Scene::handle(const Event& event)
{
	switch(event.Type)
	{
	case Event::KeyPressed:
		if( event.Key.Code == Key::Num1 )
			fluid->light = !fluid->light;
		if( event.Key.Code == Key::Num2 )
			fluid->blend = !fluid->blend;
		if( event.Key.Code == Key::RControl )
			camera->toggleMouseLook();
		break;
	case Event::KeyReleased:
		break;
	case Event::MouseButtonPressed:
		break;
	case Event::MouseButtonReleased:
		break;
	default: break;
	};
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
