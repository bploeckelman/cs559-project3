/************************************************************************/
/* Buildings
/* ------
/* A class that contains all buildings
/************************************************************************/

#include <SFML\Graphics.hpp>
#include "SceneObject.h"
#include "../Core/ImageManager.h"

class House : public SceneObject{
private:
	sf::Color color;
	sf::Image roof; 
	sf::Image side;
	float length;
	float width;

public:
	House();
	House(glm::vec3 pos, sf::Color color, float length, float width);

	~House();

	void draw();

};