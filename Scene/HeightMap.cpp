/************************************************************************/
/* HeightMap 
/* ---------
/* A basic height map 
/************************************************************************/
#include "HeightMap.h"
#include "Camera.h"
#include "../Core/Common.h"
#include "../Core/ImageManager.h"
#include "../Utility/Matrix2d.h"

#include <SFML/Graphics.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <string>
#include <cmath>

using std::vector;


HeightMap::HeightMap(const unsigned int width, const unsigned int height)
	: heights(height, width)
{
	randomize();
}

void HeightMap::render(Camera *camera)
{
	const float groundScale = 1.f;
	const float heightScale = 5.f;

	// Force the camera to stay above the heightmap
	/*
	if( camera != nullptr )
	{
		try {
		glm::vec3 campos(camera->position());
		glm::vec2 mapcoords(-campos.x / groundScale, -campos.z / groundScale);
		if( mapcoords.x >= 0 && mapcoords.y >= 0 
		 && mapcoords.x < heights.cols() && mapcoords.y < heights.rows() )
		{
			// TODO: lerp between surrounding height values
			const unsigned int x = static_cast<unsigned int>(mapcoords.x);
			const unsigned int y = static_cast<unsigned int>(mapcoords.y);
			const double height = heightAt(x, y);
			if( campos.y < height+0.5f ) 
				camera->position(glm::vec3(campos.x, height+0.5f, campos.z));
		}
		} catch(HeightMatrix::BoundsViolation& e) {
			int i = 1;
		}
	}
	*/
	glm::mat4 m;
	m = glm::translate(m, glm::vec3(0,-heightScale,0));
	glPushMatrix();
	glMultMatrixf(glm::value_ptr(m));

	// TODO: vertex arrays or vertex buffer objects
	for(unsigned int row = 0; row < (heights.rows() - 1); ++row)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for(unsigned int col = 0; col < (heights.cols() - 1); ++col)
		{
			const float r = static_cast<float>(row) / static_cast<float>(heights.rows());
			const float g = static_cast<float>(col) / static_cast<float>(heights.cols());
			const float b = static_cast<float>(row) / static_cast<float>(col);
			glColor3f(r,g,b);
			// row, col
			glVertex3d(groundScale * col
				, heightScale * heights(row,col)
				, groundScale * row);
			// row+1,col
			glVertex3d(groundScale * col
					 , heightScale * heights(row+1,col)
					 , groundScale * (row + 1));
			// row, col+1
			glVertex3d(groundScale * (col + 1)
				, heightScale * heights(row,col+1)
				, groundScale * row);
			// row+1, col+1
			glVertex3d(groundScale * (col + 1)
					 , heightScale * heights(row+1,col+1)
					 , groundScale * (row + 1));
		}
		glEnd();
	}

	glPopMatrix();
}

void HeightMap::randomize()
{
	for(unsigned int row = 0; row < heights.rows(); ++row)
	for(unsigned int col = 0; col < heights.cols(); ++col)
	{
		heights(row,col) = glm::gaussRand(1.0, 0.4);
	}
}

void HeightMap::loadFromImage( const std::string& filename )
{
	const sf::Image& image = ImageManager::get().getImage(filename);
	const unsigned int width  = image.GetWidth();
	const unsigned int height = image.GetHeight();
	heights = HeightMatrix(height, width);

	for(unsigned int y = 0; y < height; ++y)
	for(unsigned int x = 0; x < width;  ++x)
	{
		const sf::Color pixel  = image.GetPixel(x,y);
		const double grey = static_cast<double>(pixel.r) / 255.0;
		heights(y,x) = grey;
/*
		const double lum = 0.299f * pixel.r
						 + 0.587f * pixel.g
						 + 0.114f * pixel.b;
*/
	}
}
