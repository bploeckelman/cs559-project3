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

	void randomize();

public:
	HeightMap(const unsigned int width=100
			, const unsigned int height=100
			, const float groundScale=.5f
			, const float heightScale=2.f);
	void loadFromImage(const std::string& filename);

	void render(Camera *camera);

	double heightAt(const unsigned int row
				  , const unsigned int col) const;

	float getHeightScale();
	float getGroundScale();
	HeightMatrix getHeights();

};

inline float HeightMap::getHeightScale()	{return heightScale;}
inline float HeightMap::getGroundScale()	{return groundScale;}
inline HeightMatrix HeightMap::getHeights()	{return heights;}

inline double HeightMap::heightAt(const unsigned int row
								, const unsigned int col) const
{
	return heights(row,col);
}
