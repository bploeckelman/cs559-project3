#pragma once
/************************************************************************/
/* Plane
/* -----
/* A simple 3d plane
/************************************************************************/
#include <glm/glm.hpp>


class Plane
{
private:
	glm::vec3 _point;
	glm::vec3 _normal;

public:
	// Create a plane from a point and normal
	Plane(const glm::vec3& point
		, const glm::vec3& normal);
	// Create a plane from 3 points
	Plane(const glm::vec3& p0
		, const glm::vec3& p1
		, const glm::vec3& p2);
	// Create a copy of another plane
	Plane(const Plane& other);

	// Get the distance of this plane from origin
	const float distance() const;
	// Get a point on this plane
	const glm::vec3& point() const;
	// Get the normal of this plane
	const glm::vec3& normal() const;

	// Is a point on this plane?
	bool isPointOn   (const glm::vec3& point);
	// Is a point above this plane (based on normal direction)?
	bool isPointAbove(const glm::vec3& point);
	// Is a point below this plane (based on normal direction)?
	bool isPointBelow(const glm::vec3& point);
};


Plane::Plane(const glm::vec3& point
				  , const glm::vec3& normal)
	: _point(point)
	, _normal(glm::normalize(normal))
{ }

Plane::Plane(const glm::vec3& p0
				  , const glm::vec3& p1
				  , const glm::vec3& p2)
	: _point(p0)
	, _normal(glm::normalize(glm::cross(p2 - p0, p1 - p0)))
{ }

const float Plane::distance()    const { return glm::dot(_normal, _point); }
const glm::vec3& Plane::point()  const { return _point; }
const glm::vec3& Plane::normal() const { return _normal; }

bool Plane::isPointOn(const glm::vec3& point)
{
	return (glm::dot(_normal, point - _point) == 0.f);
}

bool Plane::isPointAbove(const glm::vec3& point)
{
	return (glm::dot(_normal, point - _point) > 0.f);
}

bool Plane::isPointBelow(const glm::vec3& point)
{
	return (glm::dot(_normal, point - _point) < 0.f);
}
