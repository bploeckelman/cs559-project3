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

	sf::Image texture;

public:
	Fountain();
	Fountain(glm::vec3 pos, float size, ParticleEmitter& emitter);

	~Fountain();

	void update(const sf::Clock &clock);

	void draw();

};
