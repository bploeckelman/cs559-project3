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
#include "../Core/Common.h"
#include "../Core/ImageManager.h"
#include "../Particles/Particles.h"

#include <functional>
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
        , lights()
	, meshes()
	, objects()
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

	// setup opengl state
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_POINTS);
	glPointSize(5.f);
	glLineWidth(1.f);

	glShadeModel(GL_SMOOTH);
//	glEnable(GL_COLOR_MATERIAL);
//	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClearDepth(1.f);
/*
	const vec4 fogColor(0.1f, 0.1f, 0.1f, 1.f);
	const float fogDensity = 0.01f;
	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_EXP2);
	glFogfv(GL_FOG_COLOR, value_ptr(fogColor));
	glFogf(GL_FOG_DENSITY, fogDensity);
	glHint(GL_FOG_HINT, GL_NICEST);
*/
	glClearColor(0.1f, 0.1f, 0.1f, 1.f);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	setupLights();

	// setup textures
	ImageManager::get().addResourceDir("Resources/images/");
	ImageManager::get().addResourceDir("Resources/images/plants/");
	ImageManager::get().addResourceDir("Resources/images/particles/");
	ImageManager::get().addResourceDir("../");
	ImageManager::get().addResourceDir("../../Resources/images/");
	ImageManager::get().addResourceDir("../../Resources/images/plants/");
	ImageManager::get().addResourceDir("../../Resources/images/particles/");

	// setup meshes
	HeightMap *heightmap = new HeightMap("heightmap-terrain.png"); 
//	HeightMap *heightmap2 = new HeightMap("heightmap-terrain.png",256.f, 256.f); 
//	HeightMap *heightmap3 = new HeightMap("heightmap-terrain.png",256.f, 0.f);
	
	meshes.push_back(heightmap);
//	meshes.push_back(heightmap2);
//	meshes.push_back(heightmap3);

	meshOverlay = new MeshOverlay(*heightmap);

	// load models
	/*ObjModel *model = new ObjModel("./Resources/models/house/house.obj");
	if( model != nullptr )
	{
		model->setRenderMode(GLM_SMOOTH | GLM_TEXTURE);
		models.push_back(model);
	}*/

	// generate a new fluid surface
	fluid = new Fluid(
		256,   // number of vertices wide
		256,   // number of vertices high
		0.75f,  // distance between vertices
		0.03f, // time step for evaluation
		10.0f,  // wave velocity
		0.1f,  // fluid viscosity
		vec3(40.f, 5.f, 40.f)
	);
	fluid->setSkybox(&skybox);
	
	// add Scene objects
//	const vec3 housePos(100.f, heightmap->heightAt(100,100) + 10, 100.f);
//	objects.push_back(new House(housePos, sf::Color(0, 255, 0), stone, 10, 20, 10));
	objects.push_back(new Blimp(vec3(120, 50, 80), 10));
	objects.push_back(new ModelObject(vec3(15, heightmap->heightAt(15, 20) + 3.5, 20), "./Resources/models/house/house.obj", 7.f));

	const vec3 position1(60.f, heightmap->heightAt(60, 100) + 2.f, 100.f);
	FountainEmitter *fountain = new FountainEmitter(position1, 20);
	FireEmitter *fire = new FireEmitter(vec3(30, heightmap->heightAt(30, 30), 30) + 1.f, 500);
	SmokeEmitter *smoke = new SmokeEmitter(vec3(31, heightmap->heightAt(31, 31) + 1.2f, 31), 500);
	objects.push_back(new Fountain(vec3(60.f, heightmap->heightAt(60, 100) + 1.f, 100.f), 10, *fountain, &skybox));

	objects.push_back(new Fish(vec3(70, 3.5f, 75), sf::Color(255, 127, 0), *heightmap, *fluid));

	objects.push_back(new Campfire(vec3(31, heightmap->heightAt(31, 31) + 1.f, 31), *fire, *smoke, 5.f));

	// add transparent scene objects
	const unsigned int numBushes = 50;
	for(unsigned int i = 0; i < numBushes; ++i)
	{
		const float x = linearRand(5.f, 250.f); // TODO: pick in heightmap bounds
		const float z = linearRand(5.f, 250.f); // TODO: pick in heightmap bounds
		const vec3 pos(x, heightmap->heightAt(x,z), z);
		alphaObjects.push_back(new Plant(pos)); 
	}

	// add particle systems
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
	system4->add(new TestEmitter(*heightmap, vec3(64.f, 0.f, 64.f)));
	system4->start();
	particleMgr.add(system4);

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
	// setup and enable lights
	Light *light0 = new Light(
		vec4(128.f , 20.f, 128.f, 1.f),        // position
		vec4(0.1f, 0.1f, 0.1f, 1.f),  // ambient
		vec4(1.f, 1.f, 1.f, 1.f),  // diffuse
		vec4(1.f, 1.f, 1.f, 1.f)      // specular 
	);
	light0->enableAmbient();
	light0->enableDiffuse();
	light0->enableSpecular();
	light0->enable();
/*
	Light *light1 = new Light(
		vec4(100.f , 40.f, 50.f, 1.f),     // position
		vec4(1.f, 0.5f, 0.f, 1.f),    // ambient
		vec4(0.3f, 0.3f, 0.3f, 1.f),  // diffuse
		vec4(1.f, 0.f, 0.f, 1.f)      // specular 
	);
	light1->disable();
//	light1->enable();

	Light *light2 = new Light(
		vec4(50.f, 40.f, 100.f, 1.f),      // position
		vec4(0.1f, 0.1f, 0.1f, 1.f),  // ambient
		vec4(0.f, 1.f, 0.3f, 1.f),    // diffuse
		vec4(0.f, 1.f, 0.f, 1.f)      // specular 
	);
	light2->disable();
//	light2->enable();
*/
	lights.push_back(light0);
//	lights.push_back(light1);
//	lights.push_back(light2);

	// setup and enable materials
	vec4 ambient(0.1f, 0.1f, 0.1f, 1.f);
	vec4 diffuse(1.f, 1.f, 1.f, 1.f);
	vec4 specular(1.f, 1.f, 1.f, 1.f);
	float shininess[1] = { 50.f };

	glMaterialfv(GL_FRONT, GL_AMBIENT, value_ptr(ambient));
	glMaterialfv(GL_FRONT, GL_DIFFUSE, value_ptr(diffuse));
//	glMaterialfv(GL_FRONT, GL_SPECULAR, value_ptr(specular));
//	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

//	glEnable(GL_NORMALIZE);
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

	// update scene objects
	for each(auto object in objects)
		object->update(clock);
	for each(auto object in alphaObjects)
		object->update(clock);

	// Sort alpha scene objects by distance to camera
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

	// update other things
	fluid->evaluate();
	particleMgr.update(clock.GetElapsedTime());

	static float angle = 0.f;
	angle += timer.GetElapsedTime();
	if( angle > constants::two_pi )
		angle = 0.f;
//*
	Light *light0 = lights.front();
	assert( light0 != nullptr );
	const vec4& position = light0->position();
	light0->position(position + vec4(-cos(angle), 0.f, sin(angle), 0.f));
//*/
	timer.Reset();
}

void Scene::render( const Clock& clock )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera->apply();

	skybox.render(*camera);

	for each(auto mesh in meshes)
		mesh->render();

	// TODO: remove these
	meshOverlay->render();
	fluid->render();

	glEnable(GL_LIGHTING);
	for each(auto object in objects)
		object->draw(*camera);
	glDisable(GL_LIGHTING);

	// TODO: ObjModel should be a part of a SceneObject
	// so that it has its own transform
	//glEnable(GL_LIGHTING);
//	glPolygonMode(GL_FRONT, GL_LINE);
//	glColor3f(1,1,1);
	/*glPushMatrix();
	glTranslatef(32, 15, 40);
	glScalef(3,3,3);
	for each(auto model in models)
		model->render();
	glPopMatrix();
//	glPolygonMode(GL_FRONT, GL_FILL);
	glDisable(GL_LIGHTING);*/

	for each(auto object in alphaObjects)
		object->draw(*camera);

	particleMgr.render(*camera);

	glDisable(GL_LIGHTING);
	for each(auto light in lights)
		light->render();
	glEnable(GL_LIGHTING);

	Render::basis();
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

	delete fluid;
}
