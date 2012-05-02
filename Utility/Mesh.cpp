/************************************************************************/
/* Mesh 
/* ----
/* A simple 3d triangle mesh 
/************************************************************************/
#include "Mesh.h"
#include "../Core/ImageManager.h"

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
{
	initialize();
}

Mesh::Mesh( const unsigned int width
		  , const unsigned int height
		  , const float spread
		  , const unsigned int elementMode )
{
	initialize(width, height, spread, elementMode);
}

Mesh::Mesh( const string& imageFileName
		  , const float spread
		  , const unsigned int elementMode )
{
	initialize(imageFileName, spread, elementMode);
}

Mesh::Mesh( const sf::Image& image
		  , const float spread
		  , const unsigned int elementMode )
{
	initialize(image, spread, elementMode);
}

Mesh::~Mesh()
{
	dropMesh();
}

void Mesh::render() const
{
	setRenderStates();

	// Draw the mesh ------------------------------
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	assert(colors   != nullptr);
	assert(vertices != nullptr);
	glColorPointer (4, GL_FLOAT, 0, value_ptr(colors[0]));
	glVertexPointer(3, GL_FLOAT, 0, value_ptr(vertices[0]));

	assert(indices != nullptr);
	glDrawElements(mode, numIndices, GL_UNSIGNED_INT, indices);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	resetRenderStates();
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

void Mesh::regenerateArrays( const unsigned int w
						   , const unsigned int h
						   , const float s )
{
	width  = w;
	height = h;
	numVertices  = width * height;
	numTriangles = 2 * (width - 1) * (height - 1);
	numIndices   = 3 * numTriangles;

	colors    = new vec4[numVertices];
	vertices  = new vec3[numVertices];
	normals   = new vec3[numVertices];
	texcoords = new vec2[numVertices];

	for(unsigned int i = 0, x = 0, z = 0; 
		i < numVertices; ++i)
	{
		const float xx = s * x;
		const float zz = s * z;

		colors[i]    = vec4(1.f, 1.f, 1.f, 1.f);
		vertices[i]  = vec3(xx, 0.f, zz);
		normals[i]   = vec3(0.f, 1.f, 0.f);
		texcoords[i] = vec2(x / width, z / height);

		if( ++x >= width )
		{
			x = 0;
			++z;
		}
	}

	// Calculate normals
	for(unsigned int j = 1; j < height - 1; ++j)
	{
		const vec3 *next = vertices + j * width;
		vec3 *nrml = normals  + j * width;

		for(unsigned int i = 1; i < width - 1; ++i)
		{
			nrml[i].x = next[i - 1].z - next[i + 1].z;
			nrml[i].y = next[i - width].z - next[i + width].z;
			nrml[i].z = next[i + 1].z - next[i - 1].z;
//			nrml[i] = glm::normalize(nrml[i]);
		}
	}
}

void Mesh::dropMesh()
{
	if( indices   != nullptr ) delete[] indices;
	if( texcoords != nullptr ) delete[] texcoords;
	if( normals   != nullptr ) delete[] normals;
	if( vertices  != nullptr ) delete[] vertices;
	if( colors    != nullptr ) delete[] colors;
	textures.clear();

	zeroMembers();
}


void Mesh::initialize()
{
	zeroMembers();
}

void Mesh::initialize( const unsigned int width
					 , const unsigned int height
					 , const float spread
					 , const unsigned int elementMode )
{
	mode = elementMode;

	zeroMembers();
	regenerateArrays(width, height, spread);
	generateArrayIndices();
}

void Mesh::initialize( const string& imageFileName
					 , const float spread
					 , const unsigned int elementMode )
{
	zeroMembers();
	initialize(ImageManager::get().getImage(imageFileName), spread, elementMode);
}

void Mesh::initialize( const sf::Image& image
					 , const float spread
					 , const unsigned int elementMode )
{
	zeroMembers();

	mode = elementMode;

	width  = image.GetWidth();
	height = image.GetHeight();
	numVertices  = width * height;
	numTriangles = 2 * (width - 1) * (height - 1);
	numIndices   = 3 * numTriangles;

	colors    = new vec4[numVertices];
	vertices  = new vec3[numVertices];
	normals   = new vec3[numVertices];
	texcoords = new vec2[numVertices];

	for(unsigned int i = 0, x = 0, z = 0; 
		i < numVertices; ++i)
	{
		// Use pixel color value for base vertex color
		const sf::Color pixel  = image.GetPixel(x,z);
		const float r = pixel.r / 255.f;
		const float g = pixel.g / 255.f;
		const float b = pixel.b / 255.f;
		const float a = pixel.a / 255.f;

		colors[i] = vec4(r,g,b,a);

		// Use luminance value to determine height in [0,1]
		const float xx = spread * x;
		const float zz = spread * z;
//		const float lum = 0.299f * r + 0.587f * g + 0.114f * b; 
		const float lum = (r + g + b) / 3.f; 

		// TODO: expose this scaling factor
		vertices[i]  = vec3(xx, 20.f * lum, zz);

		// TODO: calculate normals based on neighboring verts
		normals[i]   = vec3(0.f, 1.f, 0.f);

		texcoords[i] = vec2(x / width, z / height);

		if( ++x >= width )
		{
			x = 0;
			++z;
		}
	}

	generateArrayIndices();

	// Calculate normals
	// TODO: not really working
	for(unsigned int j = 1; j < height - 1; ++j)
	{
		const vec3 *next = vertices + j * width;
		vec3 *nrml = normals  + j * width;

		for(unsigned int i = 1; i < width - 1; ++i)
		{
			nrml[i].x = next[i - 1].z - next[i + 1].z;
			nrml[i].y = next[i - width].z - next[i + width].z;
			nrml[i].z = next[i + 1].z - next[i - 1].z;
			nrml[i] = glm::normalize(nrml[i]);
		}
	}
}

void Mesh::zeroMembers()
{
	colors       = nullptr;
	vertices     = nullptr;
	normals      = nullptr;
	texcoords    = nullptr;
	indices      = nullptr;
	mode         = 0;
	width        = 0;
	height       = 0;
	numVertices  = 0;
	numTriangles = 0;
	numIndices   = 0;
	blend        = false;
	light        = false;
	fill         = false;
}

void Mesh::setRenderStates() const
{
	// Set texturing state ----------------------
	// TODO: handle multitexturing 
	if( texture ) 
	{
		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		assert(texcoords != nullptr);
		glTexCoordPointer(2, GL_FLOAT, 0, value_ptr(texcoords[0]));
	}
	else
		glDisable(GL_TEXTURE_2D);

	// Set lighting state -------------------------
	if( light )
	{
		glEnable(GL_LIGHTING);
		glEnableClientState(GL_NORMAL_ARRAY);
		assert(normals != nullptr);
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
}

void Mesh::resetRenderStates() const
{
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

	// Reset texturing state -----------------------
	// TODO: handle multitexuring 
	if( texture )
	{
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
	}
	else 
		glEnable(GL_TEXTURE_2D);
}
