/************************************************************************/
/* Objects
/* ------
/* A class that contains all objects in the scene
/************************************************************************/

#include <SFML\Graphics.hpp>
#include "SceneObject.h"
#include "../Core/ImageManager.h"
#include "HeightMap.h"

class Objects{
public:
	void setup(const HeightMap& heights);
}

class Fish : public SceneObject{
private:
	sf::Color color;

public:
	Fish();
	Fish(float x, float y, float z, sf::Color color);

	~Fish();

	void update(const sf::Clock &clock);

	void draw();

};

class Fountain : public SceneObject{
private:
	float size;

	sf::Image texture;

public:
	Fountain();
	Fountain(float x, float y, float z, float size);

	~Fountain();

	void update(const sf::Clock &clock);

	void draw();

};