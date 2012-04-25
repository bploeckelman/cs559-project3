#pragma once
/************************************************************************/
/* Render 
/* ------
/* A static helper class for rendering various things 
/************************************************************************/
#include "Plane.h"

#include <glm/glm.hpp>


class Render
{
public:
	// Draw a basic cube at the specified position with the specified scale
	static void cube(const glm::vec3& position=glm::vec3(0,0,0)
					,const float scale=1.f);

	// Draw the specified plane as a quad with the specified radius
	static void plane(const Plane& plane, const float radius=10.f);

	// Draw a vector from the specified point
	static void vector(const glm::vec3& v, const glm::vec3& point
					 , const glm::vec3& color=glm::vec3(1,1,1));
};
