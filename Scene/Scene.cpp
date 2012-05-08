/************************************************************************/
/* Scene
/* -----
/* A scene consisting of a camera and a number of game objects
/************************************************************************/
#include "Scene.h"
#include "Light.h"
#include "Camera.h"
#include "Skybox.h"
#include "Fluid.h"
#include "Buildings.h"
#include "Objects.h"
#include "../Utility/Plane.h"
#include "../Utility/ObjModel.h"
#include "../Utility/RenderUtils.h"
#include "../Utility/BoundingBox.h"
#include "../Core/Common.h"
#include "../Core/ImageManager.h"
#include "../Particles/Particles.h"

#include <functional>
#include <algorithm>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Input.hpp>
#include <SFML/System/Randomizer.hpp>
#include <iomanip>

using namespace sf;
using namespace glm; 

// TODO: keep a container of followee's in Scene, or add followee member to camera
ParticleEmitter *followee = nullptr;


Scene::Scene()
	: camera(nullptr)
	, cameras()
	, skybox()
	, fluid(nullptr)
    , lights()
	, meshes()
	, objects()
	, bounds()
	, particleMgr()
	, meshOverlay(nullptr)
	, timer()
{ }

Scene::~Scene()
{
	cleanup();
}

void Scene::setup()
{
	timer.Reset();

	// setup opengl state ----------------------------------------
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

	const vec4 fogColor(0.1f, 0.1f, 0.1f, 1.f);
	const float fogDensity = 0.002f;
	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_EXP2);
	glFogfv(GL_FOG_COLOR, value_ptr(fogColor));
	glFogf(GL_FOG_DENSITY, fogDensity);

	// setup texture directories ---------------------------------
	ImageManager::get().addResourceDir("Resources/images/");
	ImageManager::get().addResourceDir("Resources/images/plants/");
	ImageManager::get().addResourceDir("Resources/images/particles/");
	ImageManager::get().addResourceDir("../");
	ImageManager::get().addResourceDir("../../Resources/images/");
	ImageManager::get().addResourceDir("../../Resources/images/plants/");
	ImageManager::get().addResourceDir("../../Resources/images/particles/");

	// setup meshes ----------------------------------------------
//	HeightMap *heightmap = new HeightMap(256, 256, 2.f);
	HeightMap *heightmap = new HeightMap("heightmap-terrain.png", 1.f, 100.f); 
//	HeightMap *heightmap2 = new HeightMap("heightmap-terrain.png", 2.f, 100.f, 256.f, 256.f); 
//	HeightMap *heightmap3 = new HeightMap("heightmap-terrain.png", 2.f, 100.f, 256.f, 0.f);
	
	meshes.push_back(heightmap);
//	meshes.push_back(heightmap2);
//	meshes.push_back(heightmap3);

	meshOverlay = new MeshOverlay(*heightmap);
	BoundingBox *bb = new BoundingBox(*meshOverlay);
	bounds.push_back(bb);

	// TESTING!!
	// Flatten HeightMap under MeshOverlay
	const vec2 minXZ(bb->getEdges()[0].x, bb->getEdges()[0].z);
	const vec2 maxXZ(bb->getEdges()[1].x, bb->getEdges()[1].z);
	heightmap->flattenArea(minXZ, maxXZ, 5.f);
	meshOverlay->regenerateVertices();

	// generate a new fluid surface ------------------------------
	fluid = new Fluid(
		heightmap->getWidth()  / 4,   // number of vertices wide
		heightmap->getHeight() / 4,   // number of vertices high
		2.2f,  // distance between vertices
		0.03f, // time step for evaluation
		10.0f, // wave velocity
		0.1f,  // fluid viscosity
		vec3(0.1f * heightmap->getWidth() * heightmap->getGroundScale(),
			 23.f,
			 0.1f * heightmap->getWidth() * heightmap->getGroundScale())
	);
	fluid->setSkybox(&skybox);
	
	// add Scene objects -----------------------------------------
//	const vec3 housePos(100.f, heightmap->heightAt(100,100) + 10, 100.f);
//	objects.push_back(new House(housePos, sf::Color(0, 255, 0), stone, 10, 20, 10));

	objects.push_back(new Blimp(vec3(120, 150, 80), 10));

	ModelObject* house = new ModelObject(vec3(15, heightmap->heightAt(15, 20) + 3.5, 20), "./Resources/models/house/house.obj", *heightmap, 7.f);
	objects.push_back(house);
	bounds.push_back(new BoundingBox(*house, vec3(15 - 7, heightmap->heightAt(15, 20) + 3.5 - 7, 20 - 7), vec3(15 + 7, heightmap->heightAt(15, 20) + 3.5 + 7, 20 + 7)));
	ModelObject* car = new ModelObject(vec3(35, heightmap->heightAt(35, 20)+1.f, 20), "./Resources/models/car/car_riviera.obj", *heightmap, 4.f);
	objects.push_back(car);
	bounds.push_back(new BoundingBox(*car, vec3(35 - 4, heightmap->heightAt(35, 20)+1.f - 4, 20 - 4), vec3(35 + 4, heightmap->heightAt(35, 20)+1.f + 4, 20 + 4)));

	//objects.push_back(new FishingRod(vec3(120, heightmap->heightAt(120, 75), 75), *heightmap, 4.f));
	Windmill* windmill = new Windmill(vec3(120, heightmap->heightAt(120, 75), 75), *heightmap, 50.f);
	objects.push_back(windmill);
	bounds.push_back(new BoundingBox(*windmill, vec3(120 - 10, heightmap->heightAt(120, 75) - 10, 75 - 10), vec3(120 + 10, heightmap->heightAt(120, 75) + 60, 75 + 10)));

	//objects.push_back(new Fish(vec3(70, 3.5f, 75), sf::Color(255, 127, 0), *heightmap, *fluid));

	const vec3 firePosition(40.f, heightmap->heightAt(40, 30) + 1.f, 30.f);
	const vec3 smokePosition(firePosition + vec3(0,1.f,0));
	FireEmitter  *fire  = new FireEmitter(firePosition);
	SmokeEmitter *smoke = new SmokeEmitter(smokePosition);
	
	Campfire* campfire = new Campfire(firePosition, *fire, *smoke, 5.f);
	objects.push_back(campfire);
	bounds.push_back(new BoundingBox(*campfire, glm::vec3(firePosition.x - 5, firePosition.y - 5, firePosition.z - 5) , glm::vec3(firePosition.x + 5, firePosition.y + 5, firePosition.z + 5)));

	const vec3 fountainPosition(60.f, heightmap->heightAt(60, 100) + 2.f, 100.f);
	FountainEmitter *fountain = new FountainEmitter(fountainPosition, 20);
	Fountain* foun = new Fountain(fountainPosition - vec3(0,1.f,0), 10, *fountain, &skybox);
	objects.push_back(foun);
	bounds.push_back(new BoundingBox(*foun, glm::vec3(fountainPosition.x - 10, fountainPosition.y - 10, fountainPosition.z - 11) , glm::vec3(fountainPosition.x + 10, fountainPosition.y + 10, fountainPosition.z + 11)));

	// add transparent scene objects -----------------------------
	const unsigned int numBushes = 50;
	for(unsigned int i = 0; i < numBushes; ++i)
	{
		bool boolBound = false;
		// TODO: don't add where objects already are
		const float x = linearRand(5.f, 512.f);
		const float z = linearRand(5.f, 512.f);
		const vec3 pos(x, heightmap->heightAt(x,z), z);
		for each(auto bound in bounds)
		{
			if(bound->inBox(pos))		//TODO: fix the super crappy way I wrote this
			{
				i--;
				boolBound = true;
				break;
			}
		}
		if(boolBound) continue;
		alphaObjects.push_back(new Plant(pos)); 
	}

	// add particle systems --------------------------------------
	ParticleSystem  *system1 = new ParticleSystem();
	system1->add(fountain);
	system1->start();
	particleMgr.add(system1);

	ParticleSystem *system2 = new ParticleSystem();
	vec3 fountainPos(128, heightmap->heightAt(128, 128) + 2.f, 128);
	system2->add(new FountainEmitter(fountainPos));
	system2->start();
	particleMgr.add(system2);

	ParticleSystem *system3 = new ParticleSystem();
	system3->add(smoke);
	system3->add(fire);
	system3->start();
	particleMgr.add(system3);

	ParticleSystem *system4 = new ParticleSystem();
	const float heightmapMax = heightmap->getHeight() * heightmap->getGroundScale();
	const vec3 p1(linearRand(5.f, heightmapMax), 0.f, linearRand(5.f, heightmapMax));
	const vec3 p2(linearRand(5.f, heightmapMax), 0.f, linearRand(5.f, heightmapMax));
	const vec3 p3(linearRand(5.f, heightmapMax), 0.f, linearRand(5.f, heightmapMax));
	TestEmitter *followEmitter = new TestEmitter(*heightmap, p1);
	system4->add(followEmitter);
	system4->add(new TestEmitter(*heightmap, p2));
	system4->add(new TestEmitter(*heightmap, p3));
	system4->start();
	particleMgr.add(system4);
	
	// set followee for camera
	followee = followEmitter;

	// create and position cameras -------------------------------
	// TODO: add more cameras and be able to switch between them
	cameras.push_back(Camera( *heightmap 
							, vec3(10.0, 20.0, 10.0)    // position
							, vec3(40.0, 135.0, 0.0) ));// rotation
	cameras.push_back(Camera( *heightmap
							, vec3(0,50,0)
							, vec3(40.f, 0.f, 0.f)) );
	camera = &cameras[0];

	Camera* cam = &cameras[1];

	// setup lighting --------------------------------------------
	setupLights();

	// perform perspective projection ----------------------------
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(value_ptr(camera->projection()));
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// log setup duration ----------------------------------------
	const float delta = timer.GetElapsedTime();
	std::stringstream ss;
	ss << "[Scene setup completed in " << delta << " seconds]";
	Log(ss);
	timer.Reset();
}

void Scene::setupLights()
{
	Light *light0 = new Light();
	light0->position(vec4(128.f , 15.f, 128.f, 1.f));
	light0->ambient(vec4(1.f, 1.f, 1.f, 1.f));
	light0->diffuse(vec4(1.f, 1.f, 1.f, 1.f));
	light0->enable();
//	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.75f);

	// MOAR HACK !!!
	HeightMap *h = reinterpret_cast<HeightMap*>(meshes.front());
	Light *light1 = new Light();
	light1->position(vec4(32.f , h->heightAt(32,32) + 4.f, 32.f, 1.f));
	light1->ambient(vec4(1.f, 1.f, 1.f, 1.f));
	light1->diffuse(vec4(1.f, 1.f, 1.f, 1.f));
	light1->enable();

	lights.push_back(light0);
	lights.push_back(light1);

	vec4 ambient(0.2f, 0.2f, 0.2f, 1.f);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, value_ptr(ambient));

	glEnable(GL_LIGHTING);
}

void Scene::update( const Clock& clock, const Input& input )
{
	// process input for the current camera
	camera->processInput(input, clock);

	// update all cameras
	std::for_each(cameras.begin(), cameras.end()
		, [&](Camera& cam){ cam.update(clock, input); }
	);

	// Update following camera
	// TODO: clean this up and make it spin around the followee
	HeightMap *h = reinterpret_cast<HeightMap*>(meshes.front());
	static const vec3 up(0,1,0);
	const vec3 followPos = followee->getPos();
	const vec3 offsetPos = followPos + vec3(-30, 8, -30);
	const float y = h->heightAt(offsetPos.x, offsetPos.z);
	cameras[1].position(offsetPos.x, y + 10.f, offsetPos.z);
	cameras[1].lookAt(cameras[1].position(), followPos, up); 

	// update scene objects
	for each(auto object in objects)
		object->update(clock, input);
	for each(auto object in alphaObjects)
		object->update(clock, input);
	sortTransparentObjects();

	// update other things
	updateLights();
	particleMgr.update(clock.GetElapsedTime());

	// randomly displace fluid every 'limit' seconds
	static const float limit = 2.f; // in seconds
	static float accum = 0.f;
	if( (accum += timer.GetElapsedTime()) > limit )
	{
		fluid->displace( linearRand(0.f, (float)fluid->getWidth())
                       , linearRand(0.f, (float)fluid->getHeight())
					   , 1.f, linearRand(0.2f, 2.2f));
		accum = 0.f;
	}
	fluid->evaluate();

//	std::cout << "timer: " << std::setw(10) << timer.GetElapsedTime() << "   "
//		      << "clock: " << std::setw(10) << clock.GetElapsedTime() << std::endl; 
	timer.Reset();
}

void Scene::render( const Clock& clock )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera->apply();

	skybox.render(*camera);

	for each(auto mesh in meshes)
		mesh->render();

	meshOverlay->render();

	for each(auto object in objects)
		object->draw(*camera);

	fluid->render(*camera);

	glDisable(GL_LIGHTING);
	for each(auto object in alphaObjects)
		object->draw(*camera);

	particleMgr.render(*camera);

	glDisable(GL_LIGHTING);
	for each(auto light in lights)
		light->render();

	Render::basis();

//	for each(auto bound in bounds)
//		bound->draw();
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
		if( event.Key.Code == Key::Num7 )
			(*meshes.front()).toggleMultiTexturing();
		// Mouse look toggle
		if( event.Key.Code == Key::RControl )
			camera->toggleMouseLook();
		// Switch cameras
		if( event.Key.Code == Key::N)
			camera = &cameras[0];
		if( event.Key.Code == Key::M)
			camera = &cameras[1];
		break;
	case Event::KeyReleased:
		break;
	case Event::MouseButtonPressed:
		if( event.MouseButton.Button == sf::Mouse::Left )
		{
			static const float distance = 25.f;

			const vec3 campos(camera->position());
			const vec3 viewdir(camera->getViewDir());
			const vec3 offset(distance * viewdir);

			// Spawn a particle system in front of the camera
			ParticleSystem *ps = new ParticleSystem();
			ps->add(new ExplosionEmitter(campos + offset));
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

	delete meshOverlay;

	for each(auto bound in bounds)
		delete bound;
	bounds.clear();

	for each(auto obj in alphaObjects)
		delete obj;
	alphaObjects.clear();

	for each(auto obj in objects)
		delete obj;
	objects.clear();

	for each(auto mesh in meshes)
		delete mesh;
	meshes.clear();

	for each(auto model in models)
		delete model;
	models.clear();
	
	for each(auto light in lights)
		delete light;
	lights.clear();	

	delete fluid;
}

void Scene::sortTransparentObjects()
{
	std::sort(alphaObjects.begin(), alphaObjects.end()
		, [&]( const SceneObject* obj1
             , const SceneObject* obj2 ) -> bool
		{
			const vec3& pos1 = obj1->getPos();
			const vec3& pos2 = obj2->getPos();
			const vec3& cpos = camera->position();
			const float dist1 = length(pos1 - cpos);
			const float dist2 = length(pos2 - cpos);
			return dist1 > dist2;
		}
	);
}

void Scene::updateLights()
{
	float delta = skybox.getDayNightCycleDelta();
	if( delta < 0.3f ) delta = 0.3f; // keep it from pitch black
	if( delta > 0.9f ) delta = 0.9f; // keep it from full bright
	vec4 ambient(delta, delta, delta, 1.f);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, value_ptr(ambient));

	// move lights around 
	static float limit = constants::two_pi;
	static float accum = 0.f;
	if( (accum += timer.GetElapsedTime()) > limit )
	{
		accum = 0.f;

		const vec4& pos0 = lights[0]->position();
		const vec4& pos1 = lights[1]->position();
		lights[0]->position(pos0 + 0.5f * vec4(cos(accum), 0.f, sin(accum), 0.f));
		lights[1]->position(pos1 + 0.1f * vec4(cos(accum), 0.f, sin(accum), 0.f));
	}
}
