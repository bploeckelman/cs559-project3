#pragma once
/************************************************************************/
/* Mesh 
/* ----
/* A simple 3d triangle mesh 
/************************************************************************/
#include <glm/glm.hpp>

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

namespace sf { class Image; }


class Mesh
{
protected:
	glm::vec4 *colors;
	glm::vec3 *vertices;
	glm::vec3 *normals;

	std::vector<glm::vec2*>       texcoords;
	std::vector<const sf::Image*> textures;

	unsigned int *indices;

	unsigned int mode;
	unsigned int width;
	unsigned int height;
	unsigned int numVertices;
	unsigned int numTriangles;
	unsigned int numIndices;

	float spread;

	bool blend;
	bool light;
	bool fill;
	bool normalsVis;
	bool texture;
	bool multiTexture;

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
		, const float heightSpread = 20.f
		, const unsigned int elementMode = GL_TRIANGLES );
	// Create a mesh by loading displacement data from the specified image
	Mesh( const sf::Image& image
		, const float spread = 1.f
		, const float heightSpread = 20.f
		, const unsigned int elementMode = GL_TRIANGLES );
	
	// Deallocate mesh resources
	virtual ~Mesh();

	// Render this mesh to the screen in the positive quadrant of the XZ-plane
	virtual void render() const;

	// Add a new texture layer to this mesh
	void addTexture(const sf::Image *texture
				  , glm::vec2 *texcoord);

	// Render state toggles
	void toggleBlending();
	void toggleLighting();
	void toggleWireframe();
	void toggleNormalsVis();
	void toggleTexturing();
	void toggleMultiTexturing();

	// Mesh details accessors
	float        getSpread()       const;
	unsigned int getWidth()        const;
	unsigned int getHeight()       const;
	unsigned int getNumIndices()   const;
	unsigned int getNumVertices()  const;
	unsigned int getNumTriangles() const;
	
	// Returns a reference to the color value at the specified grid indices
	glm::vec4& colorAt   (const unsigned int col, const unsigned int row);
	// Returns reference to the vertex at the specified grid indices
	glm::vec3& vertexAt  (const unsigned int col, const unsigned int row);
	// Returns a reference to the normal value at the specified grid indices
	glm::vec3& normalAt  (const unsigned int col, const unsigned int row);
	// Returns a reference to the texture coord value at specified grid indices
	glm::vec2& texcoordAt(const unsigned int col
                        , const unsigned int row
                        , const unsigned int layer);

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
				  , const float heightSpread
				  , const unsigned int elementMode);
	void initialize(const sf::Image& image
				  , const float spread
				  , const float heightSpread
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

	// Render normal vectors for visualization
	void renderNormals() const;

protected:
	// Apply a box filter over the y-values of each vertex
	void smoothHeights();
	// Regenerate vertex normals
	void regenerateNormals();
};


inline void Mesh::toggleBlending()  { blend      = !blend; }
inline void Mesh::toggleLighting()  { light      = !light; }
inline void Mesh::toggleWireframe() { fill       = !fill; }
inline void Mesh::toggleNormalsVis(){ normalsVis = !normalsVis; }
inline void Mesh::toggleTexturing() { texture    = !texture; }
inline void Mesh::toggleMultiTexturing() { multiTexture = !multiTexture; }

inline float Mesh::getSpread()              const { return spread; }
inline unsigned int Mesh::getWidth()        const { return width; }
inline unsigned int Mesh::getHeight()       const { return height; }
inline unsigned int Mesh::getNumIndices()   const { return numIndices; }
inline unsigned int Mesh::getNumVertices()  const { return numVertices; }
inline unsigned int Mesh::getNumTriangles() const { return numTriangles; }
