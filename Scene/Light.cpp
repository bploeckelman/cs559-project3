#include "Light.h"
#include "../Utility/RenderUtils.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SFML/Graphics.hpp>


// TODO: get this value from glGetIntegerv(GL_MAX_LIGHTS, Light::maxLights);
unsigned int Light::maxLights = 8;
unsigned int Light::nextGLId  = 0;

void Light::render()
{
//	glColor3fv(glm::value_ptr(_ambient));
	glColor3fv(glm::value_ptr(glm::vec4(1,1,1,1)));
	Render::cube(glm::vec3(_position), .5f);
}
