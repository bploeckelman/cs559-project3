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
	const float groundScale;
	const float heightScale;


	void randomize();

public:
	HeightMap(const unsigned int width=100
			, const unsigned int height=100
			, const float groundScale = .25f
			, const float heightScale = 2.f);	
	void loadFromImage(const std::string& filename);

	void render(Camera *camera);

	double heightAt(const unsigned int row, const unsigned int col);
	float getGroundScale();
	float getHeightScale();
	HeightMatrix* getHeights();
};

inline HeightMatrix* HeightMap::getHeights()		{ return &heights;}
inline float HeightMap::getGroundScale()	{ return groundScale;}
inline float HeightMap::getHeightScale()	{ return heightScale;}

inline double HeightMap::heightAt(const unsigned int row
								, const unsigned int col)
{
	return heights(row,col);
}
