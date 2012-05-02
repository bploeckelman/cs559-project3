/************************************************************************/
/* Render 
/* ------
/* A static helper class for rendering various things 
/************************************************************************/
#include "RenderUtils.h"
#include "Plane.h"
#include "../Scene/Camera.h"
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

void Render::pyramid( const vec3& pos, const float radius, const float height)
{
	glPushMatrix();
	glTranslated(pos.x, pos.y, pos.z);

	glPushMatrix();
		glRotatef(180, 1, 0, 0);
		glBegin(GL_QUADS);
			glVertex3d(-radius, 0, -radius);
			glVertex3d(-radius, 0, radius);
			glVertex3d(radius, 0, radius);
			glVertex3d(radius, 0, -radius);
		glEnd();
	glPopMatrix();

	glBegin(GL_TRIANGLE_FAN);
		glVertex3d(0, height, 0);
		glVertex3d(-radius, 0, -radius);
		glVertex3d(-radius, 0, radius);
		glVertex3d(radius, 0, radius);
		glVertex3d(radius, 0, -radius);
		glVertex3d(-radius, 0, -radius);
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
	glColor4fv(glm::value_ptr(color));

	glBegin(GL_LINES);
		glVertex3fv(glm::value_ptr(point));
		glVertex3fv(glm::value_ptr(point + vec));
	glEnd();

	glBegin(GL_POINTS);
		glVertex3fv(glm::value_ptr(point + vec));
	glEnd();
}

void Render::frustum( const Camera& camera )
{
	const float *proj = glm::value_ptr(camera.projection());
	const float *view = glm::value_ptr(camera.view());

    // Get near and far from the Projection matrix.
    const double _near = proj[11] / (proj[10] - 1.0);
    const double _far  = proj[11] / (1.0 + proj[10]);

    // Get the sides of the _near plane.
    const double nLeft   = _near * (proj[2] - 1.0) / proj[0];
    const double nRight  = _near * (1.0 + proj[2]) / proj[0];
    const double nTop    = _near * (1.0 + proj[6]) / proj[5];
    const double nBottom = _near * (proj[6] - 1.0) / proj[5];

    // Get the sides of the _far plane.
    const double fLeft   = _far * (proj[2] - 1.0) / proj[0];
    const double fRight  = _far * (1.0 + proj[2]) / proj[0];
    const double fTop    = _far * (1.0 + proj[6]) / proj[5];
    const double fBottom = _far * (proj[6] - 1.0) / proj[5];

    /*
     0  glVertex3f(0.0f, 0.0f, 0.0f);
     1  glVertex3f(nLeft, nBottom, -near);
     2  glVertex3f(nRight, nBottom, -near);
     3  glVertex3f(nRight, nTop, -near);
     4  glVertex3f(nLeft, nTop, -near);
     5  glVertex3f(fLeft, fBottom, -far);
     6  glVertex3f(fRight, fBottom, -far);
     7  glVertex3f(fRight, fTop, -far);
     8  glVertex3f(fLeft, fTop, -far);
     */

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    //glLoadIdentity ();

        // TODO - need to invert the view before multiplying it with the current view!

    glMultMatrixf(view);

    glLineWidth(2);
    glBegin(GL_LINES);

    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(fLeft, fBottom, -_far);

    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(fRight, fBottom, -_far);

    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(fRight, fTop, -_far);

    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(fLeft, fTop, -_far);

    //_far
    glVertex3d(fLeft, fBottom, -_far);
    glVertex3d(fRight, fBottom, -_far);

    glVertex3d(fRight, fTop, -_far);
    glVertex3d(fLeft, fTop, -_far);

    glVertex3d(fRight, fTop, -_far);
    glVertex3d(fRight, fBottom, -_far);

    glVertex3d(fLeft, fTop, -_far);
    glVertex3d(fLeft, fBottom, -_far);

    //_near
    glVertex3d(nLeft, nBottom, -_near);
    glVertex3d(nRight, nBottom, -_near);

    glVertex3d(nRight, nTop, -_near);
    glVertex3d(nLeft, nTop, -_near);

    glVertex3d(nLeft, nTop, -_near);
    glVertex3d(nLeft, nBottom, -_near);

    glVertex3d(nRight, nTop, -_near);
    glVertex3d(nRight, nBottom, -_near);

    glEnd();
    glLineWidth(1);
    glPopMatrix();
}

void Render::basis(const glm::vec3& x
				 , const glm::vec3& y
				 , const glm::vec3& z)
{
	glDisable(GL_TEXTURE_2D);

	static const glm::vec3 origin(0,0,0);
	static const glm::vec3 red   (1,0,0);
	static const glm::vec3 green (0,1,0);
	static const glm::vec3 blue  (0,0,1);

	vector(x, origin, red);
	vector(y, origin, green);
	vector(z, origin, blue);

	glEnable(GL_TEXTURE_2D);
}
