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
	float size;

public:
	House();
	House(glm::vec3 pos, sf::Color color, float size);

	~House();

	void draw();

};