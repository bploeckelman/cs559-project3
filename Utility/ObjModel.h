#pragma once
/************************************************************************/
/* ObjModel
/* --------
/* A .obj model loaded with GLM 
/* (the obj loading library, not glm the math library)
/************************************************************************/
#include "../Lib/glm-obj/glm.h"

#include <string>


class ObjModel
{
private:
	std::string  filename;
	GLMmodel    *model;
	GLuint       mode;

public:
	ObjModel( const std::string& filename
			, const bool unitize = true );
	~ObjModel();

	void render();
	void setRenderMode(unsigned int renderMode);
};


inline void ObjModel::render()
{
	if( model != nullptr )	glmDraw(model, mode);
}

inline void ObjModel::setRenderMode(unsigned int renderMode)
{
	mode = renderMode;
}
