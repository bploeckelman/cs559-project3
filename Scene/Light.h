#pragma once
/************************************************************************/
/* Light
/* -----
/* An OpenGL light source
/************************************************************************/
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SFML/Graphics.hpp>


class Light
{
private:
	glm::vec4 _position;
	glm::vec4 _ambient;
	glm::vec4 _diffuse;
	glm::vec4 _specular;

	bool enabled;
	bool useAmbient;
	bool useDiffuse;
	bool useSpecular;

	unsigned int glId;

	void reset();

public:
	static unsigned int maxLights;
	static unsigned int nextGLId;

	Light(const glm::vec4& position = glm::vec4(0,0,0,1)
		, const glm::vec4& ambient  = glm::vec4(1,1,1,1)
		, const glm::vec4& diffuse  = glm::vec4(1,1,1,1)
		, const glm::vec4& specular = glm::vec4(1,1,1,1))
		: _position(position)
		, _ambient(ambient)
		, _diffuse(diffuse)
		, _specular(specular)
		, enabled(false)
		, useAmbient(false)
		, useDiffuse(false)
		, useSpecular(false)
		, glId(nextGLId++)
	{
		reset();
	}

	void render();

	void enable();
	void disable();

	void enableAmbient();
	void disableAmbient();
	void enableDiffuse();
	void disableDiffuse();
	void enableSpecular();
	void disableSpecular();

	void position(const glm::vec4& p);
	void ambient (const glm::vec4& a);
	void diffuse (const glm::vec4& d);
	void specular(const glm::vec4& s);

	bool isEnabled() const;

	glm::vec4 position() const;
	glm::vec4 ambient()  const;
	glm::vec4 diffuse()  const;
	glm::vec4 specular() const;

	unsigned int getGLId() const;
};

void Light::reset()
{
	position(_position);
	if( useAmbient )  ambient(_ambient);
	if( useDiffuse )  diffuse(_diffuse);
	if( useSpecular ) specular(_specular);
}

unsigned int Light::getGLId() const { return glId; }

void Light::enable()
{
	glEnable(GL_LIGHT0 + glId);
	enabled = true; 
}

void Light::disable()
{ 
	glDisable(GL_LIGHT0 + glId);
	enabled = false; 
}

void Light::enableAmbient()   { useAmbient  = true;  }
void Light::disableAmbient()  { useAmbient  = false; }
void Light::enableDiffuse()   { useDiffuse  = true;  }
void Light::disableDiffuse()  { useDiffuse  = false; }
void Light::enableSpecular()  { useSpecular = true;  }
void Light::disableSpecular() { useSpecular = false; }

void Light::position(const glm::vec4& p) 
{ 
	_position = p; 
	glLightfv(GL_LIGHT0 + glId, GL_POSITION, glm::value_ptr(_position));
}

void Light::ambient(const glm::vec4& a)
{ 
	_ambient  = a; 
	if( useAmbient )
		glLightfv(GL_LIGHT0 + glId, GL_AMBIENT, glm::value_ptr(_ambient));
}

void Light::diffuse(const glm::vec4& d)
{ 
	_diffuse  = d; 
	if( useDiffuse )
		glLightfv(GL_LIGHT0 + glId, GL_DIFFUSE, glm::value_ptr(_diffuse));
}

void Light::specular(const glm::vec4& s)
{ 
	_specular = s;
	if( useSpecular )
		glLightfv(GL_LIGHT0 + glId, GL_SPECULAR, glm::value_ptr(_specular));
}

bool Light::isEnabled() const { return enabled; }

glm::vec4 Light::position() const { return _position; }
glm::vec4 Light::ambient()  const { return _ambient; }
glm::vec4 Light::diffuse()  const { return _diffuse; }
glm::vec4 Light::specular() const { return _specular; }
