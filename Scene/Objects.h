/************************************************************************/
/* Objects
/* ------
/* A class that contains all objects in the scene
/************************************************************************/

#include <SFML\Graphics.hpp>
#include "SceneObject.h"
#include "../Core/ImageManager.h"
#include "HeightMap.h"
#include "../Particles/Particles.h"
#include "Fluid.h"

static void setupObjects(HeightMap& map);

class Fish : public SceneObject{
private:
	sf::Color color;
	int posNeg;
	HeightMap& heightmap;
	float theta;
	Fluid& fluid;
	int direction;
	GLUquadricObj* quadric;

public:
	Fish();
	Fish(glm::vec3 pos, sf::Color color, HeightMap& heightmap, Fluid& fluid);

	~Fish();

	void update(const sf::Clock &clock);

	void draw(const Camera& camera);

};


class Fountain : public SceneObject{
private:
	float size;
	ParticleEmitter& emitter;
	Fluid* fluid;
	int count;
	sf::Image texture;
	GLUquadricObj* quadric;

public:
	Fountain();
	Fountain(glm::vec3 pos, float size, ParticleEmitter& emitter);

	~Fountain();

	void update(const sf::Clock &clock);

	void draw(const Camera& camera);

};

class Bush : public SceneObject{
private:
	sf::Image side;
	sf::Image top;
	float size;

public:	
	Bush();
	Bush(glm::vec3 pos, float size);

	~Bush();

	void draw(const Camera& camera);

};

class Blimp : public SceneObject{
private:
	sf::Image btext;
	float size;
	float count;
	GLUquadricObj* quadric;
	float theta;


public:
	Blimp();
	Blimp(glm::vec3 pos, float size);

	~Blimp();

	void update(const sf::Clock &clock);
	void draw(const Camera& camera);
};

class Campfire : public SceneObject{
private:
	float size;
	ParticleEmitter& fire;
	ParticleEmitter& smoke;
	GLUquadricObj* quadric;

public:
	Campfire();
	Campfire(glm::vec3 pos, ParticleEmitter& fire, ParticleEmitter& smoke, float size);

	~Campfire();

	void update(const sf::Clock &clock);
	void draw(const Camera& camera);

};