#pragma once
/************************************************************************/
/* Scene
/* -----
/* A scene consisting of a camera and a number of game objects
/************************************************************************/
#include "Camera.h"
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

public:
	Scene();
	~Scene();

	void setup();
	void update(const sf::Clock& clock, const sf::Input& input);
	void render(const sf::Clock& clock);

	void renderTestCube(const glm::vec3& position=glm::vec3(0.f, 0.f, 0.f)
					  , const float scale=1.f);
	void renderTestPlane(const Plane& plane, const float radius=100.f);

	const glm::vec3& cameraPosition() const;
	
private:
	void init();
	void cleanup();
};

inline const glm::vec3& Scene::cameraPosition() const { return camera->position(); }
