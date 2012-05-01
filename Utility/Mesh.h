#pragma once
/************************************************************************/
/* Mesh 
/* ----
/* A simple 3d triangle mesh 
/************************************************************************/
#include <glm/glm.hpp>

#include <vector>
#include <string>

namespace sf { class Image; }


class Mesh
{
private:
	std::vector<sf::Image*> textures;

	glm::vec4 *colors;
	glm::vec3 *vertices;
	glm::vec3 *normals;

	// TODO: store multi-texture coords in a more general way
	glm::vec2 *texcoords0;
	glm::vec2 *texcoords1;
	glm::vec2 *texcoords2;

	unsigned int *indices;

	unsigned int width;
	unsigned int height;
	unsigned int numVertices;
	unsigned int numTriangles;
	unsigned int numIndices;

	bool blend;
	bool light;
	bool fill;

	// TODO: opengl draw mode, default GL_TRIANGLES
	// TODO: vertex buffer object management

public:

	// Create an uninitialized mesh
	Mesh();
	// Create a mesh with the specified dimension
	Mesh( const unsigned int width
		, const unsigned int height
		, const float spread );
	// Create a mesh by loading an image with the specified filename
	Mesh( const std::string& imageFileName );
	// Create a mesh by loading displacement data from the specified image
	Mesh( const sf::Image& image );
	
	~Mesh();

	// Render this mesh to the screen in the positive quadrant of the XZ-plane
	void render() const;

	void addTexture(sf::Image *texture);

private:
	// Non-copyable
	Mesh(const Mesh& other);
	void operator=(const Mesh& other);

	void generateArrayIndices();
};

inline void Mesh::addTexture(sf::Image *texture) { textures.push_back(texture); }
