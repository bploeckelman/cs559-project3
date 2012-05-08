#pragma once
/************************************************************************/
/* MeshOverlay 
/* -----------
/* A mesh that sits on top of another mesh 
/************************************************************************/
#include "../Utility/Mesh.h"

#include <glm/glm.hpp>


class MeshOverlay : public Mesh 
{
protected:
	Mesh& parent;
	unsigned int offsetw;
	unsigned int offseth;

public:
	MeshOverlay( Mesh& parent
               , const unsigned int width   = 16
               , const unsigned int height  = 600 
               , const unsigned int offsetw = 60
               , const unsigned int offseth = 5 );

	const glm::vec2& getOffset() const;
};


inline const glm::vec2& MeshOverlay::getOffset() const {return glm::vec2(offsetw, offseth);}
