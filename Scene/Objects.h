/************************************************************************/
/* Objects
/* ------
/* A class that contains all objects in the scene
/************************************************************************/
#include "SceneObject.h"
#include "Fluid.h"
#include "Camera.h"
#include "HeightMap.h"
#include "../Core/ImageManager.h"
#include "../Particles/Particles.h"
#include "../Utility/ObjModel.h"

#include <SFML\Graphics.hpp>

class Skybox;


/************************************************************************/
/* Fish
/* ----
/* A fish
/************************************************************************/
class Fish : public SceneObject{
private:
	int posNeg;
	int direction;
	float theta;
	Fluid& fluid;
	HeightMap& heightmap;
	GLUquadricObj* quadric;
	sf::Color color;

public:
	Fish(glm::vec3 pos, sf::Color color, HeightMap& heightmap, Fluid& fluid);

	~Fish();

	void update(const sf::Clock &clock);

	void draw(const Camera& camera);
};


/************************************************************************/
/* Fountain
/* --------
/* A fountain consisting of a fluid surface, a particle emitter that 
/* disturbs the fluid surface, and some containing geometry
/************************************************************************/
class Fountain : public SceneObject{
private:
	int count;
	float size;
	Fluid* fluid;
	sf::Image texture;
	GLUquadricObj* quadric;
	ParticleEmitter& emitter;

public:
	Fountain( glm::vec3 pos
			, float size
			, ParticleEmitter& emitter
			, Skybox* skybox );

	~Fountain();

	void update(const sf::Clock &clock);

	void draw(const Camera& camera);
};


/************************************************************************/
/* Plant
/* -----
/* A single plant that is drawn using 4 crossed planes 
/* with a random plant texture applied
/************************************************************************/
class Plant : public SceneObject{
private:
	sf::Image texture;
	float size;

public:	
	Plant(const glm::vec3& pos);

	void draw(const Camera& camera);
};


/************************************************************************/
/* Blimp
/* -----
/* A Goodyear invasion! 
/************************************************************************/
class Blimp : public SceneObject{
private:
	float size;
	float count;
	float theta;
	sf::Image btext;
	GLUquadricObj* quadric;

public:
	Blimp(glm::vec3 pos, float size);

	~Blimp();

	void update(const sf::Clock &clock);
	void draw(const Camera& camera);
};


/************************************************************************/
/* Campfire
/* --------
/* A cozy little campfire, with some logs 
/* and some particles for atmosphere
/************************************************************************/
class Campfire : public SceneObject{
private:
	float size;
	ParticleEmitter& fire;
	ParticleEmitter& smoke;
	GLUquadricObj* quadric;
	sf::Image wood;

public:
	Campfire( glm::vec3 pos
			, ParticleEmitter& fire
			, ParticleEmitter& smoke
			, float size );

	~Campfire();

	void draw(const Camera& camera);
};


/************************************************************************/
/* Model Object
/* --------
/* Holds a ObjModel
/************************************************************************/

class ModelObject : public SceneObject{
protected:
	ObjModel*	model;
	float size;
	HeightMap& heightmap;

public:
	ModelObject( glm::vec3 pos, const std::string& filename, HeightMap& heightmap, float size);

	~ModelObject();

	void draw(const Camera& camera);
	void update(const sf::Clock &clock);
};


