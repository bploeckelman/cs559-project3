#pragma once
/************************************************************************/
/* Scene
/* -----
/* A scene consisting of a camera and a number of game objects
/************************************************************************/
#include "Camera.h"
#include "Skybox.h"
#include "../Core/Common.h"
#include "../Utility/Plane.h"

#include <glm/glm.hpp>

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

public:
	Scene();
	~Scene();

	// Load resources, arrange scene and cameras
	void setup();
	// Update this scene, process input
	void update(const sf::Clock& clock, const sf::Input& input);
	// Render this scene's objects from the current camera perspective
	void render(const sf::Clock& clock);

	// Draw a basic cube at the specified position with the specified scale
	void renderTestCube(const glm::vec3& position=glm::vec3(0.f, 0.f, 0.f)
					  , const float scale=1.f);
	// Draw the specified plane as a quad with the specified radius
	void renderTestPlane(const Plane& plane, const float radius=100.f);

	// Peek at the camera position
	const glm::vec3& cameraPosition() const;
	
private:
	// Allocate any needed memory, called on creation
	void init();
	// Free any allocated memory, called on destruction
	void cleanup();
};

inline const glm::vec3& Scene::cameraPosition() const { return camera->position(); }
