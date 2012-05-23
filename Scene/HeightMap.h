#pragma once
/************************************************************************/
/* HeightMap 
/* ---------
/* A basic height map mesh
/************************************************************************/
#include "../Utility/Mesh.h"
#include "../Utility/Logger.h"

#include <glm/glm.hpp>


class HeightMap : public Mesh
{
private:
	std::string  imageName;
	glm::vec2 offset;
	float heightScale;
	float groundScale;

public:
	/**
	 * Creates a new heightmap with the specified parameters
	 * by using the diamond-square algorithm to generate 
	 * height values procedurally
	 * \param width  - number of vertices wide
	 * \param height - number of vertices long
	 * \param groundScale - distance between vertices in the plane
	 * \param heightScale - distance between vertices vertically
	**/
	HeightMap(const unsigned int width  = 100
			, const unsigned int height = 100
			, const float groundScale   = 1.f
			, const float heightScale   = 20.f
			, const float offsetWidth   = 0.f
			, const float offsetHeight  = 0.f);

	/**
	 * Creates a new heightmap by loading height data from an image 
	 * \param imageFilename - the name of the image file that has height data 
	 * \param groundScale - distance between vertices in the plane
	 * \param heightScale - distance between vertices vertically
	**/
	HeightMap(const std::string& imageFilename
			, const float groundScale   = 0.5f
			, const float heightScale   = 20.f
			, const float offsetWidth   = 0.f
			, const float offsetHeight  = 0.f);

	/**
	 * Get the height value for the given column and row 
	 * using bilinear interpolation to get a value between 
	 * neighboring vertices.
	 * \param col - the column index to lookup
	 * \param row - the row index to lookup
	 * \return - the height value at the specified indices,
	 *           or the smallest possible float 
	**/
	float heightAt(const float col  
                 , const float row);

	float getHeightScale() const;
	float getGroundScale() const;

	void flattenArea(const glm::vec2& minXZ, const glm::vec2& maxXZ, float tolerance);
	void flattenArea(float height, const glm::vec2& minXZ, const glm::vec2& maxXZ);

private:
	void diamondSquare(bool smooth = true);
	void randomizeGaussian();
	void updateVerticesByOffsets();
	void setupTextures();
	void zeroHeightValues();
};


inline float HeightMap::getHeightScale() const {return heightScale;}
inline float HeightMap::getGroundScale() const {return groundScale;}
