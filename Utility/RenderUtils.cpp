/************************************************************************/
/* Render 
/* ------
/* A static helper class for rendering various things 
/************************************************************************/
#include "RenderUtils.h"
#include "Plane.h"
#include "../Core/ImageManager.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SFML/Graphics.hpp>

using glm::vec3;


void Render::cube( const vec3& position, const float scale )
{
	glPushMatrix();
	glTranslated(position.x, position.y, position.z);

	const float nl = -0.5f * scale;
	const float pl =  0.5f * scale;

	glBegin(GL_QUADS);
		glNormal3d( 0,0,1);
			glVertex3d(pl,pl,pl);
			glVertex3d(nl,pl,pl);
			glVertex3d(nl,nl,pl);
			glVertex3d(pl,nl,pl);
		glNormal3d( 0, 0, -1);
			glVertex3d(pl,pl, nl);
			glVertex3d(pl,nl, nl);
			glVertex3d(nl,nl, nl);
			glVertex3d(nl,pl, nl);
		glNormal3d( 0, 1, 0);
			glVertex3d(pl,pl,pl);
			glVertex3d(pl,pl,nl);
			glVertex3d(nl,pl,nl);
			glVertex3d(nl,pl,pl);
		glNormal3d( 0,-1,0);
			glVertex3d(pl,nl,pl);
			glVertex3d(nl,nl,pl);
			glVertex3d(nl,nl,nl);
			glVertex3d(pl,nl,nl);
		glNormal3d( 1,0,0);
			glVertex3d(pl,pl,pl);
			glVertex3d(pl,nl,pl);
			glVertex3d(pl,nl,nl);
			glVertex3d(pl,pl,nl);
		glNormal3d(-1,0,0);
			glVertex3d(nl,pl,pl);
			glVertex3d(nl,pl,nl);
			glVertex3d(nl,nl,nl);
			glVertex3d(nl,nl,pl);
	glEnd();
	
	glPopMatrix(); 
}

void Render::plane( const Plane& plane, const float radius )
{
	// Calculate/get plane details
	const vec3& p(plane.point());
	const vec3& n(plane.normal());
	const vec3  o(n.y - n.z, n.z - n.x, n.x - n.y); //orthogonal to normal
	const vec3  s(glm::normalize(o));
	const vec3  t(glm::normalize(glm::cross(s,n)));
	const float d = plane.distance();

	// Calculate vertices
	const vec3 v0(radius *
			 vec3(( s.x + t.x) + n.x * d
				, ( s.y + t.y) + n.y * d
				, ( s.z + t.z) + n.z * d));
	const vec3 v1(radius *
			 vec3(( s.x - t.x) + n.x * d
				, ( s.y - t.y) + n.y * d
				, ( s.z - t.z) + n.z * d));
	const vec3 v2(radius *
			 vec3((-s.x + t.x) + n.x * d
				, (-s.y + t.y) + n.y * d
				, (-s.z + t.z) + n.z * d));
	const vec3 v3(radius *
			 vec3((-s.x - t.x) + n.x * d
				, (-s.y - t.y) + n.y * d
				, (-s.z - t.z) + n.z * d));

	// Draw the plane
	glDisable(GL_CULL_FACE);

	const sf::Image& image(ImageManager::get().getImage("grid.png"));
	image.Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBegin(GL_TRIANGLE_STRIP);
		glNormal3fv(glm::value_ptr(n));
		glTexCoord2f(0.f,    0.f);    glVertex3fv(glm::value_ptr(v0));
		glTexCoord2f(radius, 0.f);    glVertex3fv(glm::value_ptr(v1));
		glTexCoord2f(0.f,    radius); glVertex3fv(glm::value_ptr(v2));
		glTexCoord2f(radius, radius); glVertex3fv(glm::value_ptr(v3));
	glEnd();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_CULL_FACE);
}

void Render::vector( const vec3& vec, const vec3& point, const vec3& color )
{
	glBegin(GL_LINES);
		glColor4fv(glm::value_ptr(color));
		glVertex3fv(glm::value_ptr(point));
		glVertex3fv(glm::value_ptr(vec));
	glEnd();

	// Draw the arrow head
//	glPushMatrix();
//	glTranslatef(vec.x(), vec.y(), vec.z());
//	applyBasisFromTangent(normalize(pos - vec));
//	glutSolidCone(0.5f, 1.5f, 8, 4);
//	glPopMatrix();
}
