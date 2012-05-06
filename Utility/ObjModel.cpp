/************************************************************************/
/* ObjModel
/* --------
/* A .obj model loaded with GLM 
/* (the obj loading library, not glm the math library)
/************************************************************************/
#include "ObjModel.h"
#include "Logger.h"

#include "../Lib/glm-obj/glm.h"

#include <sstream>
#include <string>


ObjModel::ObjModel( const std::string& filename
                  , const bool unitize /* = true */ )
	: model(nullptr)
	, mode(GLM_NONE)
	, filename(filename)
{
	model = glmReadOBJ(filename.c_str());
	if( model == nullptr )
	{
		std::stringstream ss;
		ss <<  "{ Warning: unable to read file "
		   << "\"" << filename << "\" }";
		Log(ss);
	}
	else 
	{

		if( unitize )
			glmUnitize(model);

		// Calculate normals
//		glmFacetNormals(model);
//		glmVertexNormals(model, 90.f);
	}
}

ObjModel::~ObjModel()
{
	if( model != nullptr )
		glmDelete(model);
}
