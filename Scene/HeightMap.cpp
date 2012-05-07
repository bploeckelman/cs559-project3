/************************************************************************/
/* HeightMap 
/* ---------
/* A basic height map mesh 
/************************************************************************/
#include "HeightMap.h"
#include "../Utility/Mesh.h"
#include "../Core/ImageManager.h"

#include <SFML/Graphics/Image.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

#include <sstream>
#include <limits>

using namespace glm;


HeightMap::HeightMap( const unsigned int width
					, const unsigned int height
					, const float offsetWidth
					, const float offsetHeight
					, const float groundScale
					, const float heightScale )
	// TODO: update Mesh ctor to accept heightScale
	: Mesh(width, height, groundScale)
	, offset(offsetWidth, offsetHeight)
	, groundScale(groundScale)
	, heightScale(heightScale)
	, imageName("")
{ }

HeightMap::HeightMap( const std::string& imageFilename
					, const float offsetWidth
					, const float offsetHeight
                    , const float groundScale /* = 0.5f */
					, const float heightScale /* = 2.f */ )
	: Mesh(imageFilename, groundScale, heightScale)
	, offset(offsetWidth, offsetHeight)
	, groundScale(groundScale)
	, heightScale(heightScale)
	, imageName(imageFilename)
{
	// update vertices based on offsets
	for(unsigned int z = 0; z < height; ++z)
	for(unsigned int x = 0; x < width;  ++x)
		vertexAt(x,z) += vec3(offset.x, 0.f, offset.y);

	float        ts = 0.1f;
	unsigned int i  = 0;

	// Load first texture
	glm::vec2 *texcoord = new glm::vec2[numVertices];

	i  = 0;
	ts = 0.05f;
	for(unsigned int z = 0; z < height; ++z)
	for(unsigned int x = 0; x < width;  ++x)
		texcoord[i++] = glm::vec2(x * ts, z * ts);

	sf::Image *tex = &GetImage("grass_256x256.png");
	addTexture(tex, texcoord);

	// Load second texture
	glm::vec2 *detailTexCoord = new glm::vec2[numVertices];

	i  = 0;
	ts = 0.1f;//01f;
	for(unsigned int z = 0; z < height; ++z)
	for(unsigned int x = 0; x < width;  ++x)
		detailTexCoord[i++] = glm::vec2(x * ts, z * ts);

	sf::Image *detailTex = &GetImage("grass-detail.png");
	addTexture(detailTex, detailTexCoord);

	// Setup default render states
	light        = true;
	fill         = true;
	texture      = true;
	multiTexture = true;
}

void HeightMap::randomizeGaussian()
{
	for(unsigned int row = 0; row < height; ++row)
	for(unsigned int col = 0; col < width;  ++col)
	{
		glm::vec3& vertex = vertexAt(col, row);
		vertex.y = static_cast<float>(glm::gaussRand(1.0, 0.4));
	}
}

float HeightMap::heightAt( const float col, const float row )
{
	if( col < width * groundScale && row < height * groundScale )
	{
		const vec2 mapcoords(col / groundScale, row / groundScale);

		if( mapcoords.x >= 0 && mapcoords.y >= 0
         && mapcoords.x < (width - 1) && mapcoords.y < (height - 1) )
		{
			const unsigned int x = static_cast<unsigned int>(mapcoords.x);
			const unsigned int y = static_cast<unsigned int>(mapcoords.y);

			const vec3& v0 = vertexAt(x  , y  );
			const vec3& v1 = vertexAt(x  , y+1);
			const vec3& v2 = vertexAt(x+1, y  );
			const vec3& v3 = vertexAt(x+1, y+1);

			const float dx = mapcoords.x - x;
			const float dy = mapcoords.y - y;

			// Interpolate height values between pairs 
			// of vertices on either side of the square
			const float h1 = v0.y + dy * (v1.y - v0.y);
			const float h2 = v2.y + dy * (v3.y - v2.y);

			// Interpolate the height value between the 
			// previously interpolated values
			return (h1 + dx * (h2 - h1));
		}
	}
/*
	std::stringstream ss;
	ss << "Warning: invalid bounds for HeightMap::heightAt"
       << "(" << col << " , " << row << ")";
	Log(ss);
*/
	return std::numeric_limits<float>::min();
}
