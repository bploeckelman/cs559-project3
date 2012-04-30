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

#include <string>
#include <map>

extern const float faceSize;
extern const std::string face_names[];


class Skybox
{
public:
	enum Face { front, back, left, right, top, bottom };
	
	Skybox();
	~Skybox();

	void render(const Camera& camera);

private:
	static const std::string dayDir;
	static const std::string nightDir;
	static const std::string directory;

	static const glm::vec3 vertices[]; 
	static const glm::vec2 texcoords[]; 
	static const unsigned char indices[];

	std::map<Face, sf::Image*> textures;

	bool init();
	void cleanup();

	void drawFace(const Face& face);

	bool buildTextureObjects(std::map<Face, std::string>& faceImages);
	bool getFilenames(std::vector<std::string>& filenames);
	bool getFaceImageMap(const std::vector<std::string>& filenames, 
						 std::map<Face, std::string>& faceImages);

};
