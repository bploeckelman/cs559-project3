/************************************************************************/
/* Scene
/* -----
/* A scene consisting of a camera and a number of game objects
/************************************************************************/
#include "Scene.h"
#include "Camera.h"
#include "Skybox.h"
#include "Fluid.h"
#include "Buildings.h"
#include "Objects.h"
#include "../Utility/Plane.h"
#include "../Utility/RenderUtils.h"
#include "../Core/Common.h"
#include "../Core/ImageManager.h"
#include "../Particles/Particles.h"

#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Input.hpp>
#include <SFML/System/Randomizer.hpp>

using namespace sf;
using namespace glm; 


Scene::Scene()
	: camera(nullptr)
	, cameras()
	, skybox()
	, heightmap()
	, fluid(nullptr)
	, particleMgr()
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
	glPointSize(5.f);
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

	// generate a new fluid surface
	fluid = new Fluid(
		128,   // number of vertices wide
		128,   // number of vertices high
		0.5f,  // distance between vertices
		0.03f, // time step for evaluation
		4.0f,  // wave velocity
		0.4f,	// fluid viscosity
		0.f,
		.75f,
		0.f
	);

	// add Scene objects
	objects.push_back(new House(10, 4, 10, sf::Color(0, 255, 0)));
	objects.push_back(new Fish(40, .5, 40, sf::Color(255, 127, 0), heightmap));
	objects.push_back(new Fish(30, .5, 50, sf::Color(255, 127, 0), heightmap));
	objects.push_back(new Fish(50, .5, 30, sf::Color(255, 127, 0), heightmap));
	objects.push_back(new Fish(35, .5, 50, sf::Color(255, 127, 0), heightmap));
	objects.push_back(new Fish(50, .5, 35, sf::Color(255, 127, 0), heightmap));

	// add particle systems
	/*ParticleSystem *system = new ParticleSystem();
	system->add(new FountainEmitter(vec3(25.f, 0.f, 25.f)));
	system->start();
	particleMgr.add(system);*/

	ParticleSystem *founSystem = new ParticleSystem();
	FountainEmitter *fountain = new FountainEmitter(vec3(60.f, 5.f, 60.f));
	founSystem->add(fountain);
	particleMgr.add(founSystem);


	objects.push_back(new Fountain(60.f, 1.75f, 60.f, 5, *fountain));

	// create and position cameras
	cameras.push_back(Camera(heightmap, vec3(-2.5, 25.0, -2.5)   // position
							,vec3(40.0, 135.0, 0.0)));// rotation
	camera = &cameras[0];

	
}

void Scene::setupLights()
{
	glEnable(GL_LIGHTING);

	vec4 gAmbient(0.1f, 0.1f, 0.1f, 1.f);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, value_ptr(gAmbient));

	// Light 0
	vec4 ambient(0.3f, 0.3f, 0.3f, 1.f);
	vec4 diffuse(0.3f, 0.3f, 0.3f, 1.f);
	vec4 specular(1.f, 1.f, 1.f, 1.f);
	vec4 position0(0.f, 10.f, 0.f, 1.f);
	glLightfv(GL_LIGHT0, GL_AMBIENT, value_ptr(ambient));
//	glLightfv(GL_LIGHT0, GL_DIFFUSE, value_ptr(diffuse));
	glLightfv(GL_LIGHT0, GL_SPECULAR, value_ptr(specular));
	glLightfv(GL_LIGHT0, GL_POSITION, value_ptr(position0));
	glEnable(GL_LIGHT0);

	// Light 1
	vec4 orange(1.f, 0.54f, 0.f, 1.f);
	vec4 position1(10.f, 10.f, 10.f, 1.f);
	glLightfv(GL_LIGHT1, GL_SPECULAR, value_ptr(orange));
	glLightfv(GL_LIGHT1, GL_POSITION, value_ptr(position1));
	glEnable(GL_LIGHT1);

	// Light 2
	vec4 white(1.f, 1.f, 1.f, 1.f);
	vec4 position2(-10.f, 10.f, -10.f, 1.f);
	glLightfv(GL_LIGHT2, GL_SPECULAR, value_ptr(white));
	glLightfv(GL_LIGHT2, GL_POSITION, value_ptr(position2));
	glEnable(GL_LIGHT2);
}

void Scene::update( const Clock& clock, const Input& input )
{
	// process input for the current camera
	camera->processInput(input, clock);

	// update all cameras
	std::for_each(cameras.begin(), cameras.end()
		, [&](Camera& cam){ cam.update(clock, input); }
	);
	// update scene objects
	for each(auto object in objects)
		object->update(clock);

	// update other things
	fluid->evaluate();
	particleMgr.update(clock.GetElapsedTime());
}

void Scene::render( const Clock& clock )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera->apply();

	skybox.render(*camera);
	heightmap.render(camera);

	for each(auto object in objects)
		object->draw();

	fluid->render();

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

	particleMgr.render(*camera);
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
		if( event.Key.Code == Key::Space ) 
			fluid->displace();
		if( event.Key.Code == Key::RControl )
			camera->toggleMouseLook();
		if( event.Key.Code == Key::N)
			skybox.setNight();
		if( event.Key.Code == Key::M)
			skybox.setDay();
		if( event.Key.Code == Key::Space )
		{
			vec3 position(linearRand(10.f, 40.f)
						, linearRand(10.f, 20.f)
						, linearRand(10.f, 40.f));
			ParticleSystem *ps = new ParticleSystem();
			ps->add(new ExplosionEmitter(position));
			ps->start();
			particleMgr.add(ps);
		}
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
