/************************************************************************/
/* Mesh 
/* ----
/* A simple 3d triangle mesh 
/************************************************************************/
#include "Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_access.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Image.hpp>

#include <string>

using namespace std;
using namespace glm;


Mesh::Mesh()
	: textures     ()
	, colors       (nullptr)
	, vertices     (nullptr)
	, normals      (nullptr)
	, texcoords0   (nullptr)
	, texcoords1   (nullptr)
	, texcoords2   (nullptr)
	, indices      (nullptr)
	, width        (0)
	, height       (0)
	, numVertices  (0)
	, numTriangles (0)
	, numIndices   (0)
	, blend        (false)
	, light        (false)
	, fill         (false)
{ }

Mesh::Mesh( const unsigned int width
		  , const unsigned int height
		  , const float spread )
	: textures()
	, indices (nullptr)
	, width   (width)
	, height  (height)
	, blend   (false)
	, light   (false)
	, fill    (false)
{
	numVertices  = width * height;
	numTriangles = 2 * (width - 1) * (height - 1);
	numIndices   = 3 * numTriangles;

	colors     = new vec4[numVertices];
	vertices   = new vec3[numVertices];
	normals    = new vec3[numVertices];
	texcoords0 = new vec2[numVertices];
	texcoords1 = nullptr;
	texcoords2 = nullptr;
//	texcoords1 = new vec2[numVertices];
//	texcoords2 = new vec2[numVertices];

	for(unsigned int i = 0, x = 0, z = 0; 
		i < numVertices; ++i)
	{
		const float xx = spread * x;// / width;
		const float zz = spread * z;// / height;

		colors[i]     = vec4(1.f, 1.f, 1.f, 1.f);
		vertices[i]   = vec3(xx, 0.f, zz);
		normals[i]    = vec3(0.f, 1.f, 0.f);
		texcoords0[i] = vec2(0.f, 0.f);
//		texcoords1[i] = vec2(0.f, 0.f);
//		texcoords2[i] = vec2(0.f, 0.f);

		if( ++x >= width )
		{
			x = 0;
			++z;
		}
	}

	generateArrayIndices();
}

Mesh::Mesh( const string& imageFileName )
	: textures     ()
	, colors       (nullptr)
	, vertices     (nullptr)
	, normals      (nullptr)
	, texcoords0   (nullptr)
	, texcoords1   (nullptr)
	, texcoords2   (nullptr)
	, indices      (nullptr)
	, width        (0)
	, height       (0)
	, numVertices  (0)
	, numTriangles (0)
	, numIndices   (0)
	, blend        (false)
	, light        (false)
	, fill         (false)
{
	// TODO
//  const sf::Image& image = ImageManager::get().getImage(imageFileName);
//	loadFromImage(image);
}

Mesh::Mesh( const sf::Image& image )
	: textures     ()
	, colors       (nullptr)
	, vertices     (nullptr)
	, normals      (nullptr)
	, texcoords0   (nullptr)
	, texcoords1   (nullptr)
	, texcoords2   (nullptr)
	, indices      (nullptr)
	, width        (0)
	, height       (0)
	, numVertices  (0)
	, numTriangles (0)
	, numIndices   (0)
	, blend        (false)
	, light        (false)
	, fill         (false)
{
	// TODO
//	loadFromImage(image);
}

Mesh::~Mesh()
{
	delete[] indices;
	delete[] texcoords2;
	delete[] texcoords1;
	delete[] texcoords0;
	delete[] normals;
	delete[] vertices;
	delete[] colors;
	textures.clear();
}

void Mesh::render() const
{
	assert(colors != nullptr);
	assert(vertices != nullptr);
	assert(normals != nullptr);
//	assert(texcoords0 != nullptr);
	assert(indices != nullptr);

	// TODO: texture toggle
	glDisable(GL_TEXTURE_2D);

	// Set lighting state -------------------------
	if( light )
	{
		glEnable(GL_LIGHTING);
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, value_ptr(normals[0]));
	}
	else
		glDisable(GL_LIGHTING);

	// Set blending state --------------------------
	if( blend )
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else 
		glDisable(GL_BLEND);

	// Set polygon filling state ------------------
	if( fill )
		glPolygonMode(GL_FRONT, GL_FILL);
	else
		glPolygonMode(GL_FRONT, GL_LINE);

	// Draw the mesh ------------------------------
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	glColorPointer (4, GL_FLOAT, 0, value_ptr(colors[0]));
	glVertexPointer(3, GL_FLOAT, 0, value_ptr(vertices[0]));

	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, indices);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	// Reset polygon filling state -----------------
	if( !fill )
		glPolygonMode(GL_FRONT, GL_FILL);

	// Reset blending state ------------------------
	if( blend )
		glDisable(GL_BLEND);

	// Reset lighting state ------------------------
	if( light )
	{
		glDisable(GL_LIGHTING);
		glDisableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, nullptr);
	}
	else
		glEnable(GL_LIGHTING);

	glEnable(GL_TEXTURE_2D);
}

void Mesh::generateArrayIndices()
{
	if( indices != nullptr ) 
		delete[] indices;

	indices = new unsigned int[numIndices];

	unsigned int i = 0;

	for(unsigned int z = 0; z < (height - 1); ++z)
	for(unsigned int x = 0; x < (width  - 1); ++x)
	{
		const unsigned int i0 = width *  z    +  x;
		const unsigned int i1 = width * (z+1) +  x;
		const unsigned int i2 = width * (z+1) + (x+1);
		const unsigned int i3 = width *  z    + (x+1);

		// First triangle
		indices[i++] = i0;
		indices[i++] = i1;
		indices[i++] = i3;

		// Second triangle
		indices[i++] = i1;
		indices[i++] = i2;
		indices[i++] = i3;
	}
}
