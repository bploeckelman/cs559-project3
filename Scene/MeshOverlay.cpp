/************************************************************************/
/* MeshOverlay 
/* -----------
/* A mesh that sits on top of another mesh 
/************************************************************************/
#include "MeshOverlay.h"
#include "../Utility/Mesh.h"
#include "../Core/ImageManager.h"

#include <glm/glm.hpp>

using namespace glm;


MeshOverlay::MeshOverlay( Mesh& parent
                        , const unsigned int width   /* = 60  */
                        , const unsigned int height  /* = 500 */
                        , const unsigned int offsetw /* = 60  */
                        , const unsigned int offseth /* = 5   */ )
	: Mesh(width, height, parent.getSpread())
	, parent(parent)
	, offsetw(offsetw)
	, offseth(offseth)
{
	// initialize vertices based on offset and parent
	regenerateVertices();

	// initialize texture
	glm::vec2 *texcoord = new glm::vec2[numVertices];

	unsigned int i  = 0;
	float ts = 0.1f;
	for(unsigned int z = 0; z < height; ++z)
	for(unsigned int x = 0; x < width;  ++x)
		texcoord[i++] = glm::vec2(x * 1.f / (width-1), z * ts);

	sf::Image *tex = &GetImage("road.png");
	addTexture(tex, texcoord);

	fill = true;
	texture = true;
	light = true;
//	normalsVis = true;
}

void MeshOverlay::regenerateVertices()
{
	for(unsigned int z = 0; z < height; ++z)
	for(unsigned int x = 0; x < width;  ++x)
	{
		vec3& vertex = vertexAt(x,z);
		const unsigned int offsetx = x + offsetw;
		const unsigned int offsetz = z + offseth;
		if( offsetx < parent.getWidth() && offsetz < parent.getHeight() )
		{
			vertex = parent.vertexAt(offsetx, offsetz);
			vertex.y += 0.01f;
		}
		else
		{
			const float s = parent.getSpread();
			vertex = vec3(offsetx * s, 0, offsetz * s);
		}
	}

	regenerateNormals();
}
