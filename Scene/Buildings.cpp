/************************************************************************/
/* Buildings
/* ------
/* A class that contains all buildings
/************************************************************************/
#include "../Lib/glee/GLee.h"

#include "Buildings.h"
#include "../Utility/RenderUtils.h"

#include <SFML/Graphics.hpp>

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>


std::string HouseTypeNames[] =
{
	"brick",
	"stone",
	"wood",
	"cement",
	"mossy"
};

std::string RoofTypeNames[] =
{
	"roof",
	"oldTile",
	"smallTile"
};

House::House()
	: color()
	 ,roof()
	 ,side()
{
}

House::House(glm::vec3 pos, sf::Color color, int houseType, int roofType, float length, float width, float height = 10.f)
	: SceneObject(pos)
	 ,roof(GetImage( std::string(RoofTypeNames[roofType]) + ".png"))
	 ,side(GetImage( std::string(HouseTypeNames[houseType]) + ".png"))
	 ,color(color)
	 ,length(length)
	 ,width(width)
	 ,height(height)
{
}

House::~House()
{
}

void House::draw(const Camera& camera)
{
	glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();
		glMultMatrixf(glm::value_ptr(transform));

		side.Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		float l = length;
		float w = width;

		glColor4f(1.f, 1.f, 1.f, 1.f);

		//sides
		glBegin(GL_QUADS);
			glNormal3d(0,0,1);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w,-height,l);
				glTexCoord2f(w, 0.f);glVertex3d(w,-height,l);
				glTexCoord2f(w, height);glVertex3d(w,height,l);
				glTexCoord2f(0.f,   height);glVertex3d(-w,height,l);

			glNormal3d( 0,0,-1);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w,-height,-l);
				glTexCoord2f(0.f,  height);glVertex3d(-w,height,-l);
				glTexCoord2f(w, height);glVertex3d(w,height,-l);
				glTexCoord2f(w,   0.f);glVertex3d(w,-height,-l);

			glNormal3d( 1, 0, 0);
				glTexCoord2f(l,    0.f);glVertex3d(w,-height, -l);
				glTexCoord2f(l,    height);glVertex3d(w,height, -l);
				glTexCoord2f(0.f,    height);glVertex3d(w,height, l);
				glTexCoord2f(0.f,   0.f);glVertex3d(w,-height,l);


			glNormal3d( -1,0,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w,-height,-l);
				glTexCoord2f(l,    0.f);glVertex3d(-w,-height,l);
				glTexCoord2f(l,    height);glVertex3d(-w,height,l);
				glTexCoord2f(0.f,   height);glVertex3d(-w,height,-l);

			//bottom
			glNormal3d( 0,1,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w,-height,-l);
				glTexCoord2f(0.f,    l);glVertex3d(-w,-height,l);
				glTexCoord2f(w,    l);glVertex3d(w,-height,l);
				glTexCoord2f(w,  0.f);glVertex3d(w,-height,-l);

			glNormal3d( 0,-1,0);
				glTexCoord2f(w,    l);glVertex3d(-w,-height,-l);
				glTexCoord2f(0.f,   l);glVertex3d(w,-height,-l);
				glTexCoord2f(0.f,    0.f);glVertex3d(w,-height,l);
				glTexCoord2f(w,    0.f);glVertex3d(-w,-height,l);
		glEnd();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();

	glm::vec3 tmp(transform[3]);
	tmp.y += height;

	l += 1;
	w += 1;
	
	roof.Bind();

	//roof
	glPushMatrix();
		glTranslatef(tmp.x, tmp.y, tmp.z);
		glDisable(GL_TEXTURE_2D);
		glColor3f(1.f, 1.f, 1.f);
		glPushMatrix();
			glRotatef(180, 1, 0, 0);
			glBegin(GL_QUADS);
				glNormal3d(0, 1, 0);
				glVertex3d(-w, 0, -l);
				glVertex3d(-w, 0, l);
				glVertex3d(w, 0, l);
				glVertex3d(w, 0, -l);
			glEnd();
		glPopMatrix();
		glEnable(GL_TEXTURE_2D);

		/*glm::vec3 norm = glm::vec3((glm::vec3(-w, 0, -l).z - glm::vec3(0, height, 0).z)*(glm::vec3(-w, 0, l).x - glm::vec3(0, height, 0).x)
			- (glm::vec3(-w, 0, -l).x - glm::vec3(0, height, 0).x)*(glm::vec3(-w, 0, l).z - glm::vec3(0, height, 0).z));*/

		glBegin(GL_TRIANGLE_FAN);
			//glNormal3f(norm.x, norm.y, norm.z);
				glTexCoord2f(0.5f,    1.f);glVertex3d(0, height, 0);
				glTexCoord2f(1.f,    0.f);glVertex3d(-w, 0, -l);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w, 0, l);
				glTexCoord2f(1.f,    0.f);glVertex3d(w, 0, l);
				glTexCoord2f(0.f,    0.f);glVertex3d(w, 0, -l);
				glTexCoord2f(1.f,    0.f);glVertex3d(-w, 0, -l);
		glEnd();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();


}