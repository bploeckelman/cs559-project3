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

inline void Light::reset()
{
	position(_position);
	if( useAmbient )  ambient(_ambient);
	if( useDiffuse )  diffuse(_diffuse);
	if( useSpecular ) specular(_specular);
}

inline unsigned int Light::getGLId() const { return glId; }

inline void Light::enable()
{
	glEnable(GL_LIGHT0 + glId);
	enabled = true; 
}

inline void Light::disable()
{ 
	glDisable(GL_LIGHT0 + glId);
	enabled = false; 
}

inline void Light::enableAmbient()   { useAmbient  = true;  }
inline void Light::disableAmbient()  { useAmbient  = false; }
inline void Light::enableDiffuse()   { useDiffuse  = true;  }
inline void Light::disableDiffuse()  { useDiffuse  = false; }
inline void Light::enableSpecular()  { useSpecular = true;  }
inline void Light::disableSpecular() { useSpecular = false; }

inline void Light::position(const glm::vec4& p) 
{ 
	_position = p; 
	glLightfv(GL_LIGHT0 + glId, GL_POSITION, glm::value_ptr(_position));
}

inline void Light::ambient(const glm::vec4& a)
{ 
	_ambient  = a; 
	if( useAmbient )
		glLightfv(GL_LIGHT0 + glId, GL_AMBIENT, glm::value_ptr(_ambient));
}

inline void Light::diffuse(const glm::vec4& d)
{ 
	_diffuse  = d; 
	if( useDiffuse )
		glLightfv(GL_LIGHT0 + glId, GL_DIFFUSE, glm::value_ptr(_diffuse));
}

inline void Light::specular(const glm::vec4& s)
{ 
	_specular = s;
	if( useSpecular )
		glLightfv(GL_LIGHT0 + glId, GL_SPECULAR, glm::value_ptr(_specular));
}

inline bool Light::isEnabled() const { return enabled; }

inline glm::vec4 Light::position() const { return _position; }
inline glm::vec4 Light::ambient()  const { return _ambient; }
inline glm::vec4 Light::diffuse()  const { return _diffuse; }
inline glm::vec4 Light::specular() const { return _specular; }
