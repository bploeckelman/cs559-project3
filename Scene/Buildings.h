/************************************************************************/
/* Buildings
/* ------
/* A class that contains all buildings
/************************************************************************/

#include <SFML\Graphics.hpp>
#include "SceneObject.h"
#include "../Core/ImageManager.h"


enum HouseType {
	brick = 0,
	stone,
	wood,
	cement,
	mossy
};

enum RoofType { 
	roof = 0,
	oldTile,
	smallTile
};

extern std::string HouseTypeNames[];
extern std::string RoofTypeNames[];

class House : public SceneObject{
private:
	sf::Color color;
	sf::Image roof; 
	sf::Image side;
	float length;
	float width;
	float height;

public:
	House();
	House(glm::vec3 pos, sf::Color color, int houseType, int roofType, float length, float width, float height);

	~House();

	void draw(const Camera& camera);

};