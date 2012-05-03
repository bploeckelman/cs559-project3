/************************************************************************/
/* Camera
/* ------
/* A simple 3d camera
/************************************************************************/
#include "Camera.h"
#include "../Core/Common.h"
#include "../Core/MainWindow.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Input.hpp>

#include <iostream>

using namespace std;
using namespace sf;
using namespace glm;


const float Camera::MOUSE_SENSITIVITY = 15.0;

const glm::vec3 xAxis(1.f, 0.f, 0.f);
const glm::vec3 yAxis(0.f, 1.f, 0.f);
const glm::vec3 zAxis(0.f, 0.f, 1.f);


Camera::Camera(HeightMap& heightmap 
			 , const glm::vec3& pos
			 , const glm::vec3& rot
			 , const glm::vec3& rotSpeed
			 , const glm::mat4& v
			 , const glm::mat4& proj)
	 : debug(false)
	 , mouseLook(true)
	 , heightmap(heightmap)
	 , _position(pos)
	 , _rotation(rot)
	 , _rotationSpeed(rotSpeed)
	 , _view(v)
	 , _projection(proj)
{
	const sf::VideoMode& videoMode(MainWindow::videoMode);
	const float aspect = static_cast<float>(videoMode.Width)
					   / static_cast<float>(videoMode.Height);
	const float fov    = 70.f;
	const float _near  = 1.f;
	const float _far   = 1000.f;

	_projection = glm::perspective(fov, aspect, _near, _far);
}

void Camera::apply()
{
	glm::mat4 m(_projection * _view);
	glLoadMatrixf(glm::value_ptr(m));
}

void Camera::lookAt(const glm::vec3& eye
				  , const glm::vec3& center
				  , const glm::vec3& up)
{
	_view = glm::lookAt(eye, center, up);

	glm::mat4 m(_projection * _view);
	glLoadMatrixf(glm::value_ptr(m));
}

void Camera::update(const sf::Clock& clock, const sf::Input& input)
{
	// Keep the camera above the heightmap
	const float height = heightmap.heightAt(_position.x, _position.z);
	if( _position.y < height )
		moveY(height - _position.y);

	// Apply the current transformations to the camera view
	_view = glm::mat4(1.0);
	_view = glm::rotate(_view, _rotation.x, xAxis);
	_view = glm::rotate(_view, _rotation.y, yAxis);
	_view = glm::rotate(_view, _rotation.z, zAxis);
	_view = glm::translate(_view, -_position);
}

void Camera::processInput(const Input& input, const Clock& clock)
{
	if( input.IsKeyDown(Key::Left))		turn(left,  1.0, clock);
	if( input.IsKeyDown(Key::Right))	turn(right, 1.0, clock);
	if( input.IsKeyDown(Key::Up))		turn(up,    1.0, clock);
	if( input.IsKeyDown(Key::Down))		turn(down,  1.0, clock);
	if( input.IsKeyDown(Key::Z) )		turn(lroll, 1.0, clock);
	if( input.IsKeyDown(Key::X) )		turn(rroll, 1.0, clock);

	if( mouseLook )
	{
		const sf::VideoMode& videoMode(MainWindow::videoMode);
		const int halfWidth  = videoMode.Width  / 2;
		const int halfHeight = videoMode.Height / 2;

		if( input.GetMouseX() < halfWidth  ) turn(left,  (halfWidth - input.GetMouseX())  / MOUSE_SENSITIVITY, clock);
		if( input.GetMouseX() > halfWidth  ) turn(right, (input.GetMouseX() - halfWidth)  / MOUSE_SENSITIVITY, clock);
		if( input.GetMouseY() < halfHeight ) turn(up,    (halfHeight - input.GetMouseY()) / MOUSE_SENSITIVITY, clock);
		if( input.GetMouseY() > halfHeight ) turn(down,  (input.GetMouseY() - halfHeight) / MOUSE_SENSITIVITY, clock);
	}

	float moveSpeed   = 0.f;
	if( input.IsKeyDown(Key::W) )		moveSpeed = -0.5f;
	if( input.IsKeyDown(Key::S) )		moveSpeed =  0.5;

	float strafeSpeed = 0.f;
	if( input.IsKeyDown(Key::A) )		strafeSpeed =  0.4f;
	if( input.IsKeyDown(Key::D) )		strafeSpeed = -0.4f;

	move(moveSpeed,strafeSpeed);

	const float y = 0.5f;
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
	case left:	_rotation.y -= _rotationSpeed.y * speed; break;
	case right: _rotation.y += _rotationSpeed.y * speed; break;
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

	x -= static_cast<float>(glm::cos(glm::radians(_rotation.y))) * strafeSpeed;
	z -= static_cast<float>(glm::sin(glm::radians(_rotation.y))) * strafeSpeed;
}

inline void Camera::moveY(const float speed)
{
	_position.y += speed;
}
