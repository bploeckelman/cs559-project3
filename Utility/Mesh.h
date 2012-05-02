#pragma once
/************************************************************************/
/* Mesh 
/* ----
/* A simple 3d triangle mesh 
/************************************************************************/
#include <glm/glm.hpp>

#include <SFML/Graphics.hpp>

#include <vector>
#include <string>

namespace sf { class Image; }


class Mesh
{
protected:
	std::vector<const sf::Image*> textures;

	glm::vec4 *colors;
	glm::vec3 *vertices;
	glm::vec3 *normals;

	// TODO: store multi-texture coords
	glm::vec2 *texcoords;

	unsigned int *indices;

	unsigned int mode;
	unsigned int width;
	unsigned int height;
	unsigned int numVertices;
	unsigned int numTriangles;
	unsigned int numIndices;

	bool texture;
	bool blend;
	bool light;
	bool fill;

	// TODO: vertex buffer object management

public:

	// Create an uninitialized mesh
	Mesh();
	// Create a mesh with the specified dimension
	Mesh( const unsigned int width
		, const unsigned int height
		, const float spread = 1.f
		, const unsigned int elementMode = GL_TRIANGLES );
	// Create a mesh by loading an image with the specified filename
	Mesh( const std::string& imageFileName
		, const float spread = 1.f
		, const unsigned int elementMode = GL_TRIANGLES );
	// Create a mesh by loading displacement data from the specified image
	Mesh( const sf::Image& image
		, const float spread = 1.f
		, const unsigned int elementMode = GL_TRIANGLES );
	
	// Deallocate mesh resources
	~Mesh();

	// Render this mesh to the screen in the positive quadrant of the XZ-plane
	void render() const;

	// TODO: add texture coordinates too?
	// Add a new texture layer to this mesh
	void addTexture(const sf::Image *texture);

	// Render state toggles
	void toggleTexturing();
	void toggleBlending();
	void toggleLighting();
	void toggleWireframe();

	// Mesh details accessors
	int getWidth()        const;
	int getHeight()       const;
	int getNumIndices()   const;
	int getNumVertices()  const;
	int getNumTriangles() const;

private:
	// Non-copyable
	Mesh(const Mesh& other);
	void operator=(const Mesh& other);

	// Helper methods for the constructors
	void initialize();
	void initialize(const unsigned int width
				  , const unsigned int height
				  , const float spread
				  , const unsigned int elementMode);
	void initialize(const std::string& imageFileName
				  , const float spread
				  , const unsigned int elementMode);
	void initialize(const sf::Image& image
				  , const float spread
				  , const unsigned int elementMode);

	// Generate indices for a planar mesh
	void generateArrayIndices();
	// Generate vertices in the XZ-plane
	void regenerateArrays( const unsigned int w
						 , const unsigned int h
						 , const float s );
	
	// Clean up memory and zero out all members
	void dropMesh();

	// Set all member data to some known zero value
	void zeroMembers();

	// Enable or disable OpenGL states based on state flags
	void setRenderStates()   const;
	// Undo OpenGL state changes made in setRenderStates()
	void resetRenderStates() const;
};


inline void Mesh::addTexture(const sf::Image *texture) { textures.push_back(texture); }

inline void Mesh::toggleTexturing() { texture = !texture; }
inline void Mesh::toggleBlending()  { blend = !blend; }
inline void Mesh::toggleLighting()  { light = !light; }
inline void Mesh::toggleWireframe() { fill  = !fill; }

inline int Mesh::getWidth()        const { return width; }
inline int Mesh::getHeight()       const { return height; }
inline int Mesh::getNumIndices()   const { return numIndices; }
inline int Mesh::getNumVertices()  const { return numVertices; }
inline int Mesh::getNumTriangles() const { return numTriangles; }
