#pragma once
/************************************************************************/
/* Skybox 
/* ------
/* A collection of 6 textures to apply to the interior of a cube
/* drawn around a camera to simulate a distant environment
/************************************************************************/
#include "Camera.h"

#include <glm/glm.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/System/Clock.hpp>

#include <string>
#include <map>

extern const float faceSize;
extern const std::string face_names[];


class Skybox
{
public:
	enum Face { front, back, left, right, top, bottom };
	typedef std::map<Skybox::Face, sf::Image*> FaceImageMap;

	Skybox();
	~Skybox();

	void render(const Camera& camera);
	
	void setDay();
	void setNight();

	float getDayNightCycleDelta() const;
	const sf::Image& getTexture(const Face& face, const bool day);

private:
	static const std::string dayDir;
	static const std::string nightDir;
	static std::string directory;

	static const glm::vec3 vertices[]; 
	static const glm::vec2 texcoords[]; 
	static const unsigned char indices[];

	FaceImageMap *textures;
	FaceImageMap  dayTextures;
	FaceImageMap  nightTextures;

	sf::Clock timer;
	bool toggleDayNight;  // true = day, false = night
	float dayNightCycleDelta;

	bool init();
	void cleanup();

	void drawFace(const Face& face);
	void drawSkybox(const Camera& camera);

	bool getTextures();

	bool buildTextureObjects(std::map<Face, std::string>& faceImages);
	bool getFilenames(std::vector<std::string>& filenames);
	bool getFaceImageMap(const std::vector<std::string>& filenames, 
						 std::map<Face, std::string>& faceImages);
};


void Skybox::setDay() 
{
	toggleDayNight = true;
	textures = &dayTextures;
}

void Skybox::setNight()
{
	toggleDayNight = false;
	textures = &nightTextures;
}
