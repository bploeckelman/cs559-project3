#pragma once
/************************************************************************/
/* Scene
/* -----
/* A scene consisting of a camera and a number of game objects
/************************************************************************/
#include "Light.h"
#include "Camera.h"
#include "Skybox.h"
#include "HeightMap.h"
#include "MeshOverlay.h"
#include "SceneObject.h"
#include "../Core/Common.h"
#include "../Utility/Mesh.h"
#include "../Utility/Plane.h"
#include "../Utility/ObjModel.h"
#include "../Particles/Particles.h"
#include "../Utility/BoundingBox.h"

#include <glm/glm.hpp>

#include <vector>

class Fluid;


namespace sf
{
	class Clock;
	class Input;
	class Event;
}

typedef std::vector<ObjModel*>    Models;
typedef std::vector<Mesh*>        Meshes;
typedef std::vector<Light*>       Lights;
typedef std::vector<SceneObject*> SceneObjects;
typedef std::vector<BoundingBox*> BoundingBoxes;



class Scene
{
protected:
	Camera         *camera;      // the current camera
	CameraVector    cameras;     // all the cameras in the scene
	Skybox          skybox;      // the current skybox
	Fluid          *fluid;       // a test fluid surface
	Lights          lights;      // a container of lights
	Models          models;      // container of 3d models
	Meshes          meshes;      // container of mesh objects
	SceneObjects    objects;     // scene objects
	SceneObjects    alphaObjects;// transparent scene objects
	BoundingBoxes	bounds;		//holds bounding boxes for objects
	ParticleManager particleMgr; // handler for particle systems


	MeshOverlay    *meshOverlay; // test mesh overlay
	sf::Clock timer;

public:
	Scene();
	~Scene();

	// Allocate any needed memory
	void init();
	// Update this scene, process input
	void update(const sf::Clock& clock, const sf::Input& input);

	// Render this scene's objects from the current camera perspective
	void render(const sf::Clock& clock);
	// Handle an event generated from an SFML Window
	void handle(const sf::Event& event);

	// Is mouse look on or off? 
	bool isMouseLook() const;

	// Peek at the camera position
	const glm::vec3& cameraPosition() const;

private:
	// Load resources, arrange scene and cameras
	void setup();
	// Sort transparent scene objects based on distance to camera
	void sortTransparentObjects();
	// Setup the OpenGL lighting for the scene
	void setupLights();
	// Update the OpenGL lighting for the scene
	void updateLights();
	// Free any allocated memory, called on destruction
	void cleanup();
};

inline bool Scene::isMouseLook() const { return camera->isMouseLook(); }
inline const glm::vec3& Scene::cameraPosition() const { return camera->position(); }
