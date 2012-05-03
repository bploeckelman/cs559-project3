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

	void draw();

};


class Fountain : public SceneObject{
private:
	float size;
	ParticleEmitter& emitter;
	Fluid* fluid;
	int count;
	sf::Image texture;

public:
	Fountain();
	Fountain(glm::vec3 pos, float size, ParticleEmitter& emitter);

	~Fountain();

	void update(const sf::Clock &clock);

	void draw();

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

	void draw();

};

class Blimp : public SceneObject{
private:
	sf::Image btext;
	float size;
	GLUquadricObj* quadric;
	//maybe add curve for flight path

public:
	Blimp();
	Blimp(glm::vec3 pos, float size);

	~Blimp();

	void update(sf::Clock &clock);
	void draw();
};
