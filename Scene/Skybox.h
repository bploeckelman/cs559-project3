#pragma once
/************************************************************************/
/* Skybox 
/* ------
/* A collection of 6 textures to apply to the interior of a cube
/* drawn around a camera to simulate a distant environment
/************************************************************************/
#include "Camera.h"

#include <SFML/Graphics.hpp>

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
	static const std::string directory;
	static const float vertices[];
	static const float texcoords[];

	std::map<Face, GLuint> textures;

	bool init();
	void cleanup();

	void drawFace(const Face& face);

	bool buildTextureObjects(std::map<Face, std::string>& faceImages);
	bool getFilenames(std::vector<std::string>& filenames);
	bool getFaceImageMap(const std::vector<std::string>& filenames, 
						 std::map<Face, std::string>& faceImages);

};
