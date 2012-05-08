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
// TODO: keep a container of mobile stuff in Scene
ModelObject *car = nullptr;


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

	meshOverlay = new MeshOverlay(*heightmap);//, 10, 600, -5, 5);
	BoundingBox *bb = new BoundingBox(*meshOverlay);
	bounds.push_back(bb);

	// Flatten HeightMap under MeshOverlay
	vec2 minXZ(bb->getEdges()[0].x, bb->getEdges()[0].z);
	vec2 maxXZ(bb->getEdges()[1].x, bb->getEdges()[1].z);
	heightmap->flattenArea(minXZ, maxXZ, 10.f);
	meshOverlay->regenerateVertices();

	// generate a new fluid surface ------------------------------
	fluid = new Fluid(
		heightmap->getWidth()  / 4,   // number of vertices wide
		heightmap->getHeight() / 3.5	//stupid change just to make it look nicer
		,   // number of vertices high
		2.2f,  // distance between vertices
		0.03f, // time step for evaluation
		10.0f, // wave velocity
		0.1f,  // fluid viscosity
		vec3(0.1f * heightmap->getWidth() * heightmap->getGroundScale(),
			 23.f,
			 0.1f * heightmap->getWidth() * heightmap->getGroundScale())
	);
	fluid->setSkybox(&skybox);

	bounds.push_back(new BoundingBox(*fluid));
	
	// add Scene objects -----------------------------------------
	const unsigned int numHouses = 5;
	for(unsigned int i = 0; i < numHouses; ++i)
	{
		bool boolBound = false;
		const float x = linearRand(50.f, heightmap->getWidth()*heightmap->getGroundScale());		//kinda cheating way to prevent buildings from landing on road
		const float z = linearRand(5.f, heightmap->getHeight()*heightmap->getGroundScale());
		const vec3 pos(x, heightmap->heightAt(x,z), z);
		const float house = linearRand(0.f, 4.f);
		const float roof = linearRand(0.f, 2.f);
		const float length = linearRand(5.f, 20.f);
		const float width = linearRand(5.f, 20.f);
		const float height = linearRand(5.f, 10.f);
		House* building = new House(pos, sf::Color(0, 255, 0), house, roof, length, width, height);
		BoundingBox* buildingbb = new BoundingBox(*building, vec3(pos.x - width - 1, pos.y - height, pos.z - length - 1), vec3(pos.x + width + 1, pos.y + height*2, pos.z + length + 1));
		
		//UGLY... to prevent weird edge behavior (no sides, or no flattening, etc)
		if(	buildingbb->getEdges()[0].x < 0 ||
			buildingbb->getEdges()[1].x > heightmap->getWidth()*heightmap->getGroundScale() ||
			buildingbb->getEdges()[0].z < 0 ||
			buildingbb->getEdges()[1].z > heightmap->getHeight()*heightmap->getGroundScale())
		{
			delete building;
			delete buildingbb;
			i--;
			continue;
		}

		for each(auto bound in bounds)
		{
			if(bound->intersect(buildingbb))		//TODO: fix the super crappy way I wrote this
			{
				delete building;
				delete buildingbb;
				i--;
				boolBound = true;
				break;
			}
		}
		if(boolBound) continue;
		objects.push_back(building);
		bounds.push_back(buildingbb);
		minXZ = vec2(buildingbb->getEdges()[0].x, buildingbb->getEdges()[0].z);
		maxXZ = vec2(buildingbb->getEdges()[1].x, buildingbb->getEdges()[1].z);
		heightmap->flattenArea(buildingbb->getEdges()[0].y, minXZ, maxXZ);
	}

	objects.push_back(new Blimp(vec3(120, 150, 80), 10));

	objects.push_back(new FishingRod(vec3(195, heightmap->heightAt(195, 250), 250), *heightmap, 4.f));

	ModelObject* house = new ModelObject(vec3(193.5, heightmap->heightAt(193.5, 258.5) + 4.f, 258.5), "./Resources/models/house/house.obj", *heightmap, 7.f);
	objects.push_back(house);
//	bounds.push_back(new BoundingBox(*house, vec3(15 - 7, heightmap->heightAt(15, 20) + 3.5 - 7, 20 - 7), vec3(15 + 7, heightmap->heightAt(15, 20) + 3.5 + 7, 20 + 7)));
	// Manually flatten area under house
	heightmap->flattenArea(vec2(185.f, 250.5f), vec2(201.5, 266.5), 0.1f);

	ModelObject* carModel = new ModelObject(vec3(64, heightmap->heightAt(64, 16.5)+1.f, 16.5), "./Resources/models/car/car_riviera.obj", *heightmap, 4.f);
	objects.push_back(carModel);
//	bounds.push_back(new BoundingBox(*car, vec3(35 - 4, heightmap->heightAt(35, 20)+1.f - 4, 20 - 4), vec3(35 + 4, heightmap->heightAt(35, 20)+1.f + 4, 20 + 4)));
	car = carModel;

	const unsigned int numWindmills = 3;
	for(unsigned int i = 0; i < numWindmills; ++i)
	{
		bool boolBound = false;
		const float x = linearRand(5.f, heightmap->getWidth()*heightmap->getGroundScale());
		const float z = linearRand(5.f, heightmap->getHeight()*heightmap->getGroundScale());
		const vec3 pos(x, heightmap->heightAt(x,z), z);

		Windmill* windmill = new Windmill(pos, *heightmap, 50.f);
		BoundingBox* windbb = new BoundingBox(*windmill, vec3(pos.x - 10, pos.y, pos.z - 10), vec3(pos.x + 10, pos.y + 60, pos.z + 10));
		for each(auto bound in bounds)
		{
			if(bound->intersect(windbb))		//TODO: fix the super crappy way I wrote this
			{
				delete windmill;
				delete windbb;
				i--;
				boolBound = true;
				break;
			}
		}
		if(boolBound) continue;
		objects.push_back(windmill);
		bounds.push_back(windbb);
		vec2 minXZ = vec2(windbb->getEdges()[0].x, windbb->getEdges()[0].z);
		vec2 maxXZ = vec2(windbb->getEdges()[1].x, windbb->getEdges()[1].z);
		heightmap->flattenArea(windbb->getEdges()[0].y - .1f, minXZ, maxXZ);
	}
	

	const unsigned int numFish = 15;
	for(unsigned int i = 0; i < numFish; ++i)
	{
		bool boolBound = false;
		const float x = linearRand(fluid->pos.x * fluid->getDist(), fluid->getWidth() * fluid->getDist());
		const float z = linearRand(fluid->pos.z * fluid->getDist(), fluid->getHeight() * fluid->getDist());
		vec3 pos(x, heightmap->heightAt(x,z), z);
		if(pos[1] > fluid->pos.y)
		{
			i--;
			continue;
		}
		pos.y = pos.y + 1.f >= fluid->pos.y - 1.f ? fluid->pos.y - 1.f : pos.y + 1.f;
		objects.push_back(new Fish(pos, sf::Color(255, 127, 0), *heightmap, *fluid));
	}
	

	/*const vec3 firePosition(40.f, heightmap->heightAt(40, 30) + 1.f, 30.f);
	const vec3 smokePosition(firePosition + vec3(0,1.f,0));
	FireEmitter  *fire  = new FireEmitter(firePosition);
	SmokeEmitter *smoke = new SmokeEmitter(smokePosition);
	
	Campfire* campfire = new Campfire(firePosition, *fire, *smoke, 5.f);
	objects.push_back(campfire);
	bounds.push_back(new BoundingBox(*campfire, glm::vec3(firePosition.x - 5, firePosition.y - 5, firePosition.z - 5) , glm::vec3(firePosition.x + 5, firePosition.y + 5, firePosition.z + 5)));*/

	
	ParticleSystem  *system3 = new ParticleSystem();

	const unsigned int numFires = 6;
	for(unsigned int i = 0; i < numFires; ++i)
	{
		bool boolBound = false;
		const float x = linearRand(5.f, heightmap->getWidth()*heightmap->getGroundScale());
		const float z = linearRand(5.f, heightmap->getHeight()*heightmap->getGroundScale());
		const vec3 pos(x, heightmap->heightAt(x,z)  + 2.f, z);
		
		const vec3 firePosition(pos.x, pos.y + 1.f, pos.z);
		const vec3 smokePosition(firePosition + vec3(0,1.f,0));
		FireEmitter  *fire  = new FireEmitter(firePosition);
		SmokeEmitter *smoke = new SmokeEmitter(smokePosition);
		Campfire* campfire = new Campfire(firePosition, *fire, *smoke, 5.f);
		BoundingBox* firebb = new BoundingBox(*campfire,glm::vec3(pos.x - 5, pos.y + 1, pos.z - 5) , 
														glm::vec3(pos.x + 5, pos.y + 5, pos.z + 5));

		for each(auto bound in bounds)
		{
			if(bound->intersect(firebb))		//TODO: fix the super crappy way I wrote this
			{
				delete fire;
				delete smoke;
				delete campfire;
				delete firebb;
				i--;
				boolBound = true;
				break;
			}
		}
		if(boolBound) continue;

		objects.push_back(campfire);
		bounds.push_back(firebb);
		system3->add(fire);
		system3->add(smoke);
		minXZ = vec2(firebb->getEdges()[0].x, firebb->getEdges()[0].z);
		maxXZ = vec2(firebb->getEdges()[1].x, firebb->getEdges()[1].z);
		heightmap->flattenArea(firebb->getEdges()[0].y - .1f, minXZ, maxXZ);
	}

	
	
	
	//moved to accomodate multiple fountains
	ParticleSystem  *system1 = new ParticleSystem();

	const unsigned int numFountains = 4;
	for(unsigned int i = 0; i < numFountains; ++i)
	{
		bool boolBound = false;
		const float x = linearRand(5.f, heightmap->getWidth()*heightmap->getGroundScale());
		const float z = linearRand(5.f, heightmap->getHeight()*heightmap->getGroundScale());
		const vec3 pos(x, heightmap->heightAt(x,z)  + 2.f, z);
		
		//const vec3 fountainPosition(40.f, heightmap->heightAt(40, 100) + 2.f, 100.f);
		FountainEmitter *fountain = new FountainEmitter(pos, 20);
		Fountain* foun = new Fountain(pos - vec3(0,1.f,0), 10, *fountain, &skybox);
		BoundingBox* founbb = new BoundingBox(*foun,	glm::vec3(pos.x - 6, pos.y - 1.5, pos.z - 11) , 
														glm::vec3(pos.x + 6, pos.y + 1, pos.z + 11));

		for each(auto bound in bounds)
		{
			if(bound->intersect(founbb))		//TODO: fix the super crappy way I wrote this
			{
				delete fountain;
				delete founbb;
				delete foun;
				i--;
				boolBound = true;
				break;
			}
		}
		if(boolBound) continue;

		objects.push_back(foun);
		bounds.push_back(founbb);
		system1->add(fountain);
		minXZ = vec2(founbb->getEdges()[0].x, founbb->getEdges()[0].z);
		maxXZ = vec2(founbb->getEdges()[1].x, founbb->getEdges()[1].z);
		heightmap->flattenArea(founbb->getEdges()[0].y - .1f, minXZ, maxXZ);
	}

	

	// add transparent scene objects -----------------------------
	const unsigned int numBushes = 50;
	for(unsigned int i = 0; i < numBushes; ++i)
	{
		bool boolBound = false;
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
	
	system1->start();
	particleMgr.add(system1);

	ParticleSystem *system2 = new ParticleSystem();
	vec3 fountainPos(128, heightmap->heightAt(128, 128) + 2.f, 128);
	system2->add(new FountainEmitter(fountainPos));
	system2->start();
	particleMgr.add(system2);

	system3->start();
	particleMgr.add(system3);

	ParticleSystem *system4 = new ParticleSystem();
	const float heightmapMax = (heightmap->getHeight() - 5.f) * heightmap->getGroundScale();
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
	light1->position(vec4(64.f , h->heightAt(64,64) + 8.f, 64.f, 1.f));
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

	// TODO: fixme
	// move the car
/*
	car->setPos(car->getPos() + vec3(0, timer.GetElapsedTime(), 0));
	const vec3 p(car->getPos());
	if( p.z > h->getHeight() * h->getGroundScale() )
		car->setPos(vec3(p.x, p.y, 0.f));
	car->setPos(vec3(p.x, p.z, h->heightAt(p.x, p.z) + 1.f));//, p.z));
//*/

	// randomly displace fluid every 'limit' seconds
	static const float limit = 4.f; // in seconds
	static float accum = 0.f;
	if( (accum += timer.GetElapsedTime()) > limit )
	{
		float x = linearRand(0.f, (float)fluid->getWidth());
		float z = linearRand(0.f, (float)fluid->getHeight());
		if(h->heightAt(x*fluid->getDist() + fluid->pos.x, z*fluid->getDist() + fluid->pos.z) > fluid->pos.y)	return;
		fluid->displace( x
                       , z
					   , 1.f, linearRand(0.2f, 2.2f));
		accum = 0.f;
	}
	fluid->evaluate();

//	std::cout << "timer: " << std::setw(10) << timer.GetElapsedTime() << "   "
//		      << "clock: " << std::setw(10) << clock.GetElapsedTime() << std::endl; 
//	const vec3 p(camera->position());
//	std::cout << "cam: (" << p.x << " , " << p.y << " , " << p.z << ")" << std::endl;
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

//	for each(auto light in lights)
//		light->render();

	glDisable(GL_LIGHTING);
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
	}
	const vec4& pos0 = lights[0]->position();
	const vec4& pos1 = lights[1]->position();
	lights[0]->position(pos0 + 0.5f * vec4(cos(accum), 0.f, sin(accum), 0.f));
	lights[1]->position(pos1 + 0.1f * vec4(cos(accum), 0.f, sin(accum), 0.f));
}
