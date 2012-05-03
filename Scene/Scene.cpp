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
#include <sstream>

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
	, fluid(nullptr)
	, meshes()
	, objects()
	, particleMgr()
	, timer()
{ }

Scene::~Scene()
{
	cleanup();
}

void Scene::setup()
{
	timer.Reset();

	// setup opengl state
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_POINTS);
	glPointSize(5.f);
	glLineWidth(1.f);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClearDepth(1.f);

	glClearColor(0.1f, 0.1f, 0.1f, 1.f);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	setupLights();

	// setup textures
	ImageManager::get().addResourceDir("Resources/images/");
	ImageManager::get().addResourceDir("../");
	ImageManager::get().addResourceDir("../../Resources/images/");

	// setup meshes
	HeightMap* heightmap = new HeightMap("heightmap-terrain.png"); 
	
	meshes.push_back(heightmap);

	// generate a new fluid surface
	fluid = new Fluid(
		128,   // number of vertices wide
		128,   // number of vertices high
		0.5f,  // distance between vertices
		0.03f, // time step for evaluation
		4.0f,  // wave velocity
		0.4f,  // fluid viscosity
		0.f,
		.75f,
		0.f
	);

	
	// add Scene objects
	objects.push_back(new House(glm::vec3(100, heightmap->heightAt(100, 100) + 10, 100), sf::Color(0, 255, 0), stone, 10, 20, 10));
	objects.push_back(new Bush(glm::vec3(30, heightmap->heightAt(30, 30) + 5, 30), 5));
	objects.push_back(new Bush(glm::vec3(50, heightmap->heightAt(50, 100) + 3, 100), 3));
	objects.push_back(new Bush(glm::vec3(37, heightmap->heightAt(37, 82) + 3, 82), 3));
	objects.push_back(new Bush(glm::vec3(58, heightmap->heightAt(58, 15) + 7, 15), 7));
	objects.push_back(new Blimp(glm::vec3(120, 50, 80), 10));
	/*objects.push_back(new Fish(glm::vec3(40, .5, 40), sf::Color(255, 127, 0), *heightmap, *fluid)); */

	// add particle systems
	ParticleSystem *system = new ParticleSystem();
	vec3 position1(128.f, 30.f, 128.f);
	system->add(new FountainEmitter(position1));
	FountainEmitter *fountain = new FountainEmitter(vec3(60.f, heightmap->heightAt(60, 100) + 2.f, 100.f), 20);
	system->add(fountain);
	system->add(new FireEmitter(glm::vec3(40, heightmap->heightAt(40, 20), 20)));
	system->start();
	particleMgr.add(system);



	objects.push_back(new Fountain(glm::vec3(60.f, heightmap->heightAt(60, 100) + .5f, 100.f), 10, *fountain));

	// create and position cameras
	cameras.push_back(Camera( *heightmap 
							, vec3(-2.5, 25.0, -2.5)    // position
							, vec3(40.0, 135.0, 0.0) ));// rotation
	camera = &cameras[0];

	// Log setup duration
	const float delta = timer.GetElapsedTime();
	std::stringstream ss;
	ss << "[Scene setup completed in " << delta << " seconds]";
	Log(ss);
	timer.Reset();
}

void Scene::setupLights()
{
	glEnable(GL_LIGHTING);

	vec4 gAmbient(0.3f, 0.2f, 0.3f, 1.f);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, value_ptr(gAmbient));

	// Light 0
	vec4 ambient(0.2f, 0.2f, 0.2f, 1.f);
	vec4 diffuse(0.3f, 0.3f, 0.3f, 1.f);
	vec4 specular(1.f, 1.f, 1.f, 1.f);
	vec4 position0(128.f, 30.f, 128.f, 1.f);
	glLightfv(GL_LIGHT0, GL_AMBIENT, value_ptr(ambient));
	glLightfv(GL_LIGHT0, GL_DIFFUSE, value_ptr(diffuse));
	glLightfv(GL_LIGHT0, GL_SPECULAR, value_ptr(specular));
	glLightfv(GL_LIGHT0, GL_POSITION, value_ptr(position0));
	glEnable(GL_LIGHT0);

	// Light 1
	vec4 orange(1.f, 0.54f, 0.f, 1.f);
	vec4 position1(256.f, 40.f, 256.f, 1.f);
	glLightfv(GL_LIGHT1, GL_SPECULAR, value_ptr(orange));
	glLightfv(GL_LIGHT1, GL_POSITION, value_ptr(position1));
	glEnable(GL_LIGHT1);

	// Light 2
	vec4 magenta(1.f, 0.f, 1.f, 1.f);
	vec4 position2(64.f, 15.f, 64.f, 1.f);
	glLightfv(GL_LIGHT2, GL_SPECULAR, value_ptr(magenta));
	glLightfv(GL_LIGHT2, GL_POSITION, value_ptr(position2));
	glEnable(GL_LIGHT2);

//	glEnable(GL_NORMALIZE);
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
	//fluid->evaluate();
	particleMgr.update(clock.GetElapsedTime());

	timer.Reset();
}

void Scene::render( const Clock& clock )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera->apply();

	skybox.render(*camera);

	for each(auto mesh in meshes)
		mesh->render();

	for each(auto object in objects)
		object->draw(*camera);

	//fluid->render();

	Render::basis();

	particleMgr.render(*camera);
}

void Scene::handle(const Event& event)
{
	switch(event.Type)
	{
	case Event::KeyPressed:
		// Fluid interface
		if( event.Key.Code == Key::Num1 )
			fluid->light = !fluid->light;
		if( event.Key.Code == Key::Num2 )
			fluid->blend = !fluid->blend;
		if( event.Key.Code == Key::Space ) 
			fluid->displace();
		// Mesh interface
		if( event.Key.Code == Key::Num3 )
			(*meshes.front()).toggleWireframe();
		if( event.Key.Code == Key::Num4 )
			(*meshes.front()).toggleLighting();
		if( event.Key.Code == Key::Num5 )
			(*meshes.front()).toggleTexturing();
		if( event.Key.Code == Key::Num6 )
			(*meshes.front()).toggleNormalsVis();
		// Mouse look toggle
		if( event.Key.Code == Key::RControl )
			camera->toggleMouseLook();
		// Skybox toggle
		if( event.Key.Code == Key::N)
			skybox.setNight();
		if( event.Key.Code == Key::M)
			skybox.setDay();
		break;
	case Event::KeyReleased:
		break;
	case Event::MouseButtonPressed:
		if( event.MouseButton.Button == sf::Mouse::Left )
		{
			const vec3 campos(camera->position());
			vec3 position(linearRand(10.f, 40.f)
						, linearRand(-5.f,  5.f)
						, linearRand(10.f, 40.f));
			ParticleSystem *ps = new ParticleSystem();
			ps->add(new ExplosionEmitter(campos + position));
			ps->start();
			particleMgr.add(ps);
		}
		break;
	case Event::MouseButtonReleased:
		break;
	default: break;
	};
}

void Scene::init()
{
	Log("\n[Initializing scene...]");
	setup();
}

void Scene::cleanup()
{
	Log("\n[Cleaning up scene...]");

	for each(auto mesh in meshes)
		delete mesh;
	meshes.clear();

	for each(auto obj in objects)
		delete obj;
	objects.clear();

	delete fluid;
}
