#pragma once
/************************************************************************/
/* HeightMap 
/* ---------
/* A basic height map 
/************************************************************************/
#include "../Utility/Matrix2d.h"

#include <string>

typedef Matrix2d<double> HeightMatrix;

class Camera;


class HeightMap
{
private:
	HeightMatrix heights;	

	std::string  imageName;

	float heightScale;
	float groundScale;

	/**
	 * Generates random height values for this heightmap
	 * according to a Gaussian distribution
	**/
	void randomizeGaussian();

public:
	/**
	 * Creates a new heightmap of the with the specified parameters
	 * \param width  - number of vertices wide
	 * \param height - number of vertices long
	 * \param groundScale - distance between vertices in the plane
	 * \param heightScale - distance between vertices vertically
	**/
	HeightMap(const unsigned int width  = 100
			, const unsigned int height = 100
			, const float groundScale   = 0.5f
			, const float heightScale   = 2.f);

	/**
	 * Recreates this heightmap with height values 
	 * loaded from the image with the specified filename
	 * \param filename - the name of the image file containing height data
	**/
	void loadFromImage(const std::string& filename);

	/**
	 * Renders this heightmap to the screen 
	 * \param camera - the camera that is viewing this heightmap
	**/
	void render(Camera *camera);

	/**
	 * Get the height value for the given row,col vertex index
	 * \param row - the row index to lookup
	 * \param col - the column index to lookup
	 * \return - the height value at the specified indices,
	 *           or throws out of bounds exception
	**/
	double heightAt(const unsigned int row
				  , const unsigned int col) const;

	float getHeightScale() const;
	float getGroundScale() const;

	// TODO: shouldn't need this... 
	// heightAt(row,col) interface should be sufficient
	const HeightMatrix& getHeights() const;
};

inline float HeightMap::getHeightScale() const {return heightScale;}
inline float HeightMap::getGroundScale() const {return groundScale;}

inline const HeightMatrix& HeightMap::getHeights() const {return heights;}

inline double HeightMap::heightAt(const unsigned int row
								, const unsigned int col) const
{
	return heights(row,col);
}
