/************************************************************************/
/* Mesh 
/* ----
/* A simple 3d triangle mesh 
/************************************************************************/
#include "../Lib/glee/GLee.h"

#include "Mesh.h"
#include "Logger.h"
#include "../Core/ImageManager.h"

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_access.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Image.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <list>

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
		  , const float heightSpread
		  , const unsigned int elementMode )
{
	initialize(imageFileName, spread, heightSpread, elementMode);
}

Mesh::Mesh( const sf::Image& image
		  , const float spread
		  , const float heightSpread
		  , const unsigned int elementMode )
{
	initialize(image, spread, heightSpread, elementMode);
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

	glActiveTexture(GL_TEXTURE0);
	assert(indices != nullptr);
	glDrawElements(mode, numIndices, GL_UNSIGNED_INT, indices);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	resetRenderStates();

	if( normalsVis )
		renderNormals();
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

	for(unsigned int i = 0, x = 0, z = 0; 
		i < numVertices; ++i)
	{
		const float xx = s * x;
		const float zz = s * z;

		colors[i]    = vec4(1.f, 1.f, 1.f, 1.f);
		vertices[i]  = vec3(xx, 0.f, zz);
		normals[i]   = vec3(0.f, 1.f, 0.f);

		if( ++x >= width )
		{
			x = 0;
			++z;
		}
	}

	generateArrayIndices();

	// Calculate normals
	for(unsigned int i = 0; i < numIndices; i += 3)
	{
		const vec3 v[3] = {
			vertices[ indices[i+0] ],
			vertices[ indices[i+1] ],
			vertices[ indices[i+2] ]
		};
		const vec3 va = v[1] - v[0];
		const vec3 vb = v[2] - v[0];
		const vec3 normal = normalize( cross(va, vb) );

		for(unsigned int j = 0; j < 3; ++j)
		{
			const vec3 a = v[(j+1) % 3] - v[j];
			const vec3 b = v[(j+2) % 3] - v[j];
			const float weight = acos( dot(a, b) / (a.length() * b.length()) );
			normals[ indices[i + j] ] += weight * normal;
		}
	}

	for(unsigned int i = 0; i < numVertices; ++i)
	{
		normals[i] = normalize(normals[i]);
	}
}

void Mesh::dropMesh()
{
	if( indices   != nullptr ) delete[] indices;
	if( normals   != nullptr ) delete[] normals;
	if( vertices  != nullptr ) delete[] vertices;
	if( colors    != nullptr ) delete[] colors;
	
	for each(auto texcoord in texcoords)
		delete[] texcoord;
	texcoords.clear();
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
	zeroMembers();

	this->mode   = elementMode;
	this->spread = spread;
	regenerateArrays(width, height, spread);
}

void Mesh::initialize( const string& imageFileName
					 , const float spread
					 , const float heightSpread
					 , const unsigned int elementMode )
{
	zeroMembers();
	initialize(GetImage(imageFileName), spread, heightSpread, elementMode);
}

void Mesh::initialize( const sf::Image& image
					 , const float spread
					 , const float heightSpread
					 , const unsigned int elementMode )
{
	zeroMembers();

	this->mode   = elementMode;
	this->spread = spread;

	width  = image.GetWidth();
	height = image.GetHeight();
	numVertices  = width * height;
	numTriangles = 2 * (width - 1) * (height - 1);
	numIndices   = 3 * numTriangles;

	colors    = new vec4[numVertices];
	vertices  = new vec3[numVertices];
	normals   = new vec3[numVertices];

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
		vertices[i]  = vec3(xx, heightSpread * lum, zz);

		normals[i]   = vec3(0.f, 0.f, 0.f);

		if( ++x >= width )
		{
			x = 0;
			++z;
		}
	}

	// Smooth mesh height values
	for(int i = 0; i < 20; ++i)
		smoothHeights();

	generateArrayIndices();

	// Calculate normals
	for(unsigned int i = 0; i < numIndices; i += 3)
	{
		const vec3 v[3] = {
			vertices[ indices[i+0] ],
			vertices[ indices[i+1] ],
			vertices[ indices[i+2] ]
		};
		const vec3 va = v[1] - v[0];
		const vec3 vb = v[2] - v[0];
		const vec3 normal = normalize( cross(va, vb) );

		for(unsigned int j = 0; j < 3; ++j)
		{
			const vec3 a = v[(j+1) % 3] - v[j];
			const vec3 b = v[(j+2) % 3] - v[j];
			const float weight = acos( dot(a, b) / (a.length() * b.length()) );
			normals[ indices[i + j] ] += weight * normal;
		}
	}

	for(unsigned int i = 0; i < numVertices; ++i)
	{
		normals[i] = normalize(normals[i]);
	}
}

void Mesh::zeroMembers()
{
	colors       = nullptr;
	vertices     = nullptr;
	normals      = nullptr;
	indices      = nullptr;
	texcoords.clear();
	textures.clear();
	mode         = 0;
	width        = 0;
	height       = 0;
	numVertices  = 0;
	numTriangles = 0;
	numIndices   = 0;
	spread       = 0.f;
	texture      = false;
	blend        = false;
	light        = false;
	fill         = false;
	normalsVis   = false;
}

void Mesh::setRenderStates() const
{
	// Set texturing state ----------------------
	if( texture ) 
	{
		if( !textures.empty() && !texcoords.empty() )
		{
			assert(textures.size() == texcoords.size());
			const sf::Image *texture  = textures.front();
			const glm::vec2 *texcoord = texcoords.front();

			// Enable texture unit 0
			glActiveTexture(GL_TEXTURE0);
			glClientActiveTexture(GL_TEXTURE0);
			glEnable(GL_TEXTURE_2D);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			texture->Bind();

			// Set texture to wrap in both dimensions
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			// Set the texture environment to modulate
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

			glTexCoordPointer(2, GL_FLOAT, 0, value_ptr(*texcoord));

			// Handle multitexturing
			if( textures.size() > 1 && multiTexture )
			{
				// Just one more for now...
				const sf::Image *mtexture  = textures[1];
				const glm::vec2 *mtexcoord = texcoords[1];

				glActiveTexture(GL_TEXTURE1); // GL_TETXTURE0 + ith_texture
				glClientActiveTexture(GL_TEXTURE1);
				glEnable(GL_TEXTURE_2D);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);

				mtexture->Bind();

				glEnable(GL_TEXTURE_2D);

				// Generate mipmaps
//				glGenerateMipmap(GL_TEXTURE_2D);
				static bool once = false;
				if( !once )
				{
					glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
					once = true;
				}
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

				// Set texture to wrap in both dimensions
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

				// Combine the texture with the previous texture colors 
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
				glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);

				glTexCoordPointer(2, GL_FLOAT, 0, value_ptr(*mtexcoord));
			}
		}
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
		if( !textures.empty() && !texcoords.empty() )
		{
			glActiveTexture(GL_TEXTURE0);
			glClientActiveTexture(GL_TEXTURE0);
			glDisable(GL_TEXTURE_2D);

			textures.front()->Bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

			glBindTexture(GL_TEXTURE_2D, 0);
			glTexCoordPointer(2, GL_FLOAT, 0, nullptr); 
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

			//Switch off the TU 1
			if( multiTexture )
			{
				glActiveTexture(GL_TEXTURE1);
				glClientActiveTexture(GL_TEXTURE1);

				glDisable(GL_TEXTURE_2D);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);

				glActiveTexture(GL_TEXTURE0);
				glClientActiveTexture(GL_TEXTURE0);
			}
		}
	}
	else 
		glEnable(GL_TEXTURE_2D);
}

vec4& Mesh::colorAt( const unsigned int col, const unsigned int row )
{
	if( col < width && row < height )
	{
		assert(colors != nullptr);
		return colors[row * width + col];
	}
	else 
	{
		stringstream ss;
		ss << "Bad indices: Mesh::colorAt(" << col << "," << row << ")";
		Log(ss);

		assert(colors != nullptr);
		return colors[0]; 
	}
}

vec3& Mesh::vertexAt( const unsigned int col, const unsigned int row )
{
	if( col < width && row < height )
	{
		assert(vertices != nullptr);
		return vertices[row * width + col];
	}
	else 
	{
		stringstream ss;
		ss << "Bad indices: Mesh::vertexAt(" << col << "," << row << ")";
		Log(ss);

		assert(vertices != nullptr);
		return vertices[0]; 
	}
}

vec3& Mesh::normalAt( const unsigned int col, const unsigned int row )
{
	if( col < width && row < height )
	{
		assert(normals != nullptr);
		return normals[row * width + col];
	}
	else 
	{
		stringstream ss;
		ss << "Bad indices: Mesh::normalAt(" << col << "," << row << ")";
		Log(ss);

		assert(normals != nullptr);
		return normals[0]; 
	}
}

vec2& Mesh::texcoordAt( const unsigned int col
                      , const unsigned int row
                      , const unsigned int layer)
{
	if( col < width && row < height && layer < textures.size() )
	{
		assert(texcoords[layer] != nullptr);
		return texcoords[layer][row * width + col];
	}
	else 
	{
		stringstream ss;
		ss << "Bad indices: Mesh::texcoordAt(" << col << "," << row << ")";
		Log(ss);

		assert(texcoords[0] != nullptr);
		return texcoords[0][0]; 
	}
}

void Mesh::smoothHeights()
{
	assert(vertices != nullptr);
	for(unsigned int z = 0; z < height; ++z)
	for(unsigned int x = 0; x < width;  ++x)
	{
		vec3& v0 = vertexAt(x,z);

		const vec3& v1 = vertexAt(x-1, z-1);
		const vec3& v2 = vertexAt(x-1, z+1);
		const vec3& v3 = vertexAt(x+1, z-1);
		const vec3& v4 = vertexAt(x+1, z+1);

		const vec3& v5 = vertexAt(x+0, z-1);
		const vec3& v6 = vertexAt(x+0, z+1);
		const vec3& v7 = vertexAt(x-1, z+0);
		const vec3& v8 = vertexAt(x+1, z+0);

		v0.y += v1.y + v2.y + v3.y + v4.y 
			  + v5.y + v6.y + v7.y + v8.y;
		v0.y /= 9.f;
	}
}

void Mesh::addTexture( const sf::Image *texture , glm::vec2 *texcoord )
{
	if( texture == nullptr || texcoord == nullptr )
	{
		Log("Warning: tried to add texture to mesh with null pointers");
		return;
	}

	texcoords.push_back(texcoord);
	textures.push_back(texture);		
}

void Mesh::renderNormals() const
{
	if( normals == nullptr ) 
		return;

	static const unsigned int step = 1;
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glColor4fv(value_ptr(vec3(0.75f,0,0)));
	glBegin(GL_LINES);
	for(unsigned int z = 0; z < height; z += step)
	for(unsigned int x = 0; x < width;  x += step)
	{
		const vec3& vertex(vertices[z * width + x]);
		const vec3& normal( normals[z * width + x]);

		glVertex3fv(value_ptr(vertex));
		glVertex3fv(value_ptr(vertex + normal));
	}
	glEnd();

	glColor4fv(value_ptr(vec3(1,0,0)));
	glPointSize(4.25f);
	glBegin(GL_POINTS);
	for(unsigned int z = 0; z < height; z += step)
	for(unsigned int x = 0; x < width;  x += step)
	{
		const vec3& vertex(vertices[z * width + x]);
		const vec3& normal( normals[z * width + x]);
		glVertex3fv(value_ptr(vertex + normal));
	}
	glEnd();

	glEnable(GL_TEXTURE_2D);
}
