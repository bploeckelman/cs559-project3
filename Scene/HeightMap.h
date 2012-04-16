#pragma once
/************************************************************************/
/* HeightMap 
/* ---------
/* A basic height map 
/************************************************************************/
#include "../Utility/Matrix2d.h"

typedef Matrix2d<double> HeightMatrix;

class Camera;


class HeightMap
{
private:
	HeightMatrix heights;	

	void randomize();

public:
	HeightMap(const unsigned int width=100
			, const unsigned int height=100);

	void render(Camera *camera);
	double heightAt(const unsigned int row, const unsigned int col);

};

inline double HeightMap::heightAt(const unsigned int row, const unsigned int col)
{
	return heights(row,col);
}
