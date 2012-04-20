/************************************************************************/
/* Camera
/* ------
/* A simple 3d camera
/************************************************************************/
#include "Camera.h"
#include "../Core/Common.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Input.hpp>

#include <iostream>

using namespace std;
using namespace sf;


const float Camera::MOUSE_SENSITIVITY = 15.0;

Camera::Camera()
	: debug(false)
	, _position(0.f, 2.f, 2.f)
	, _rotation(30.f, 0.f, 0.f)
	, _rotationSpeed(0.7f, 1.f, 1.f)
	, mouseView(true)
{ }

void Camera::apply() const
{
	static const glm::vec3 xAxis(1.f, 0.f, 0.f);
	static const glm::vec3 yAxis(0.f, 1.f, 0.f);
	static const glm::vec3 zAxis(0.f, 0.f, 1.f);

	glm::mat4 m;
	m = glm::rotate(m, 180.f, xAxis);
	m = glm::rotate(m, _rotation.x, xAxis);
	m = glm::rotate(m, _rotation.y, yAxis);
	m = glm::rotate(m, _rotation.z, zAxis);
	m = glm::translate(m, _position);

	glLoadMatrixf(glm::value_ptr(m));
}

void Camera::lookAt(const glm::vec3& eye
	, const glm::vec3& center
	, const glm::vec3& up)
{
	glm::mat4 m(glm::lookAt(eye,center,up));
	glLoadMatrixf(glm::value_ptr(m));
}

void Camera::processInput(const Input& input, const Clock& clock)
{

	if( input.IsKeyDown(Key::Left))		turn(left, 1.0, clock);
	if( input.IsKeyDown(Key::Right))	turn(right, 1.0, clock);
	if( input.IsKeyDown(Key::Up))		turn(up, 1.0, clock);
	if( input.IsKeyDown(Key::Down))		turn(down, 1.0, clock);
	if( input.IsKeyDown(Key::Z) )		turn(lroll, 1.0, clock);
	if( input.IsKeyDown(Key::X) )		turn(rroll, 1.0, clock);

	//mouse view, only do if mouseView is true
	if( mouseView)
	{
		if( input.GetMouseX() < 640 )	turn(left, (640 - input.GetMouseX())/MOUSE_SENSITIVITY, clock);
		if( input.GetMouseX() > 640 )	turn(right, (input.GetMouseX() - 640)/MOUSE_SENSITIVITY, clock);
		if( input.GetMouseY() < 480 )	turn(up, (480 - input.GetMouseY())/MOUSE_SENSITIVITY, clock);
		if( input.GetMouseY() > 480 )	turn(down, (input.GetMouseY() - 480)/MOUSE_SENSITIVITY, clock);
	}


	float moveSpeed   = 0.f;
	if( input.IsKeyDown(Key::W) )		moveSpeed = -0.1f;
	if( input.IsKeyDown(Key::S) )		moveSpeed =  0.1f;

	float strafeSpeed = 0.f;
	if( input.IsKeyDown(Key::A) )		strafeSpeed =  0.1f;
	if( input.IsKeyDown(Key::D) )		strafeSpeed = -0.1f;

	move(moveSpeed,strafeSpeed);

	float y = 0.1f;
	if( input.IsKeyDown(Key::Q) )		moveY(-y);
	if( input.IsKeyDown(Key::E) )		moveY(y);

	if( debug )
	{
		cout << "(x,y,z):("
			<< _position.x <<","<< _position.y <<","<< _position.z <<")\t"
			<< "pitch : " << _rotation.x << "  "
			<< "yaw : " << _rotation.y << "  "
			<< "roll : " << _rotation.z << endl;
	}
}

void Camera::turn(const Direction& direction, float speed, const sf::Clock& clock)
{
	switch(direction)
	{
	case left:	_rotation.y += _rotationSpeed.y * speed; break;
	case right: _rotation.y -= _rotationSpeed.y * speed; break;
	case up:	_rotation.x -= _rotationSpeed.x * speed; break;
	case down:	_rotation.x += _rotationSpeed.x * speed; break;
	case lroll: _rotation.z += _rotationSpeed.z * speed; break;
	case rroll: _rotation.z -= _rotationSpeed.z * speed; break;
	};
}

void Camera::move(const float forwardSpeed, const float strafeSpeed)
{
	float& x = _position.x;
	float& y = _position.y;
	float& z = _position.z;

	x -= static_cast<float>(glm::sin(glm::radians(_rotation.y))) * forwardSpeed;
	y += static_cast<float>(glm::sin(glm::radians(_rotation.x))) * forwardSpeed;
	z += static_cast<float>(glm::cos(glm::radians(_rotation.y))) * forwardSpeed;

	x += static_cast<float>(glm::cos(glm::radians(_rotation.y))) * strafeSpeed;
	z += static_cast<float>(glm::sin(glm::radians(_rotation.y))) * strafeSpeed;
}

inline void Camera::moveY(const float speed)
{
	_position.y += speed;
}
