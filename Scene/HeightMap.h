#pragma once
/************************************************************************/
/* HeightMap 
/* ---------
/* A basic height map mesh
/************************************************************************/
#include "../Utility/Mesh.h"

#include <string>
#include <limits>


class HeightMap : public Mesh
{
private:
	std::string  imageName;
	float heightScale;
	float groundScale;

public:
	/**
	 * Creates a new heightmap with the specified parameters
	 * \param width  - number of vertices wide
	 * \param height - number of vertices long
	 * \param groundScale - distance between vertices in the plane
	 * \param heightScale - distance between vertices vertically
	**/
	HeightMap(const unsigned int width  = 100
			, const unsigned int height = 100
			, const float groundScale   = 1.f
			, const float heightScale   = 1.f);

	/**
	 * Creates a new heightmap by loading height data from an image 
	 * \param imageFilename - the name of the image file that has height data 
	 * \param groundScale - distance between vertices in the plane
	 * \param heightScale - distance between vertices vertically
	**/
	HeightMap(const std::string& imageFilename
			, const float groundScale   = 0.5f
			, const float heightScale   = 2.f);

	/**
	 * Get the height value for the given row,col vertex index
	 * \param col - the column index to lookup
	 * \param row - the row index to lookup
	 * \return - the height value at the specified indices,
	 *           or the smallest possible float 
	**/
	float heightAt(const float col  
                 , const float row);

	float getHeightScale() const;
	float getGroundScale() const;

private:
	void randomizeGaussian();
};


inline float HeightMap::getHeightScale() const {return heightScale;}
inline float HeightMap::getGroundScale() const {return groundScale;}

inline float HeightMap::heightAt( const float col 
								, const float row )
{
	if( col < width && row < height ){
			glm::vec2 mapcoords(col / getGroundScale(), row / getGroundScale());
		if( mapcoords.x >= 0 && mapcoords.y >= 0
			&& mapcoords.x < (height - 1) && mapcoords.y < (width - 1) )
		{
		const unsigned int x = static_cast<unsigned int>(mapcoords.x);
		double influenceX = mapcoords.x - x;
		const unsigned int y = static_cast<unsigned int>(mapcoords.y);
		double influenceY = mapcoords.y - y;

		double yHeight = (vertexAt(x, y).y * (1 - influenceY) + vertexAt(x, y + 1).y * influenceY);
		double xHeight = (vertexAt(x + 1, y).y * (1 - influenceY) +vertexAt(x + 1, y + 1).y * influenceY);
		const double height = (yHeight * (1 - influenceX) + xHeight * influenceX) * getHeightScale();

		return height;
		}
	}
	else
		return std::numeric_limits<float>::min();
}
