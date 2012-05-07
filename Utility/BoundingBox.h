/************************************************************************/
/* BoundingBox
/* ------
/* Box that specifies where the edges of an object are.
/************************************************************************/

#include <glm\glm.hpp>
#include "Mesh.h"
#include "../Scene/MeshOverlay.h"


class BoundingBox
{

private:
	glm::mat2x3 edges;

public:
	BoundingBox();
	BoundingBox(glm::vec3 greater, glm::vec3 lesser);
	BoundingBox(Mesh& mesh);
	BoundingBox(MeshOverlay& mesh);
	BoundingBox(ModelObject& obj);

	~BoundingBox();
	
	const glm::mat2x3& getEdges();
};

inline const glm::mat2x3& BoundingBox::getEdges() { return edges;}

