#pragma once
/************************************************************************/
/* Camera
/* ------
/* A simple 3d camera
/************************************************************************/
#include <glm/glm.hpp>


namespace sf
{
	class Clock;
	class Input;
}


class Camera
{
public:
	enum Direction { left, right, up, down, lroll, rroll };

private:
	bool debug;

	bool mouseView;

	static const float MOUSE_SENSITIVITY;

	glm::vec3 _position;
	glm::vec3 _rotation;
	glm::vec3 _rotationSpeed;

	void turn(const Direction& direction, float speed, const sf::Clock& clock);
	void move(const float forwardSpeed, const float strafeSpeed=0.f);
	void moveY(const float speed=1.f);

public:
	Camera();

	void setMouseView(bool val);

	void apply() const;
	void lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);
	void processInput(const sf::Input& input, const sf::Clock& clock);

	void toggleDebug();

	void position(const glm::vec3& p);
	void position(const float x, const float y, const float z);
	const glm::vec3& position() const;

	void rotation(const glm::vec3& r);
	void rotation(const float x, const float y, const float z);
	const glm::vec3& rotation() const;

	void rotationSpeed(const glm::vec3& r);
	void rotationSpeed(const float x, const float y, const float z);
	const glm::vec3& rotationSpeed() const;
};

inline void Camera::setMouseView(bool val) { mouseView = val; }

inline void Camera::toggleDebug() { debug = !debug; }

inline void Camera::position(const glm::vec3& p) { _position = p; }
inline void Camera::position(const float x, const float y, const float z) { _position = glm::vec3(x,y,z); }
inline const glm::vec3& Camera::position() const { return _position; }

inline void Camera::rotation(const glm::vec3& r) { _rotation = r; }
inline void Camera::rotation(const float x, const float y, const float z) { _rotation = glm::vec3(x,y,z); }
inline const glm::vec3& Camera::rotation() const { return _rotation; }

inline void Camera::rotationSpeed(const glm::vec3& r) { _rotationSpeed = r; }
inline void Camera::rotationSpeed(const float x, const float y, const float z) { _rotationSpeed = glm::vec3(x,y,z); }
inline const glm::vec3& Camera::rotationSpeed() const { return _rotationSpeed; }
