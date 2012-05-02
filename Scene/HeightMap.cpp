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


HeightMap::HeightMap(const unsigned int width
					, const unsigned int height
					, const float groundScale
					, const float heightScale)
	: Mesh(width, height, groundScale)
	, groundScale(groundScale)
	, heightScale(heightScale)
	, imageName("")
{ }

HeightMap::HeightMap( const std::string& imageFilename
                    , const float groundScale /* = 0.5f */
					, const float heightScale /* = 2.f */ )
	: Mesh(imageFilename, groundScale)
	, groundScale(groundScale)
	, heightScale(heightScale)
	, imageName(imageFilename)
{
	const float ts = 0.1f;

	glm::vec2 *texcoord = new glm::vec2[numVertices];
	unsigned int i = 0;

	for(unsigned int z = 0; z < height; ++z)
	for(unsigned int x = 0; x < width;  ++x)
		texcoord[i++] = glm::vec2(x * ts, z * ts);

	sf::Image *tex = &GetImage("grass_256x256.png");
	addTexture(tex, texcoord);
	texture = true;
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
