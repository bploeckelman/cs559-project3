#pragma once
/************************************************************************/
/* Scene
/* -----
/* A scene consisting of a camera and a number of game objects
/************************************************************************/
#include "Camera.h"
#include "Skybox.h"
#include "HeightMap.h"
#include "../Core/Common.h"
#include "../Utility/Plane.h"

#include <glm/glm.hpp>

class Fluid;

namespace sf
{
	class Clock;
	class Input;
}


class Scene
{
protected:
	Camera		*camera;	// the current camera
	CameraVector cameras;	// all the cameras in the scene
	Skybox       skybox;    // the current skybox
	HeightMap    heightmap; // a test heightmap
	Fluid*       fluid;     // a test fluid surface
	bool		 mouseView;

public:
	Scene();
	~Scene();

	// Load resources, arrange scene and cameras
	void setup();
	// Update this scene, process input
	void update(const sf::Clock& clock, const sf::Input& input);
	// Render this scene's objects from the current camera perspective
	void render(const sf::Clock& clock);

	void setMouseView(bool val);

	// Peek at the camera position
	const glm::vec3& cameraPosition() const;

private:
	// Allocate any needed memory, called on creation
	void init();
	// Free any allocated memory, called on destruction
	void cleanup();
};

inline void Scene::setMouseView(bool val) { camera->setMouseView(val); }

inline const glm::vec3& Scene::cameraPosition() const { return camera->position(); }
