#pragma once
#include <glm/glm.hpp>

class Camera
{
public:
	Camera() : theta(0), phi(-20), position(-10,4,0), lastMouseX(0), lastMouseY(0) {}

	glm::mat4 GetProjectionMatrix(float w, float h);
	glm::mat4 GetViewMatrix();

	void Update();

	glm::vec3 GetPos() { return position; }

private:
	float theta;
	float phi;
	glm::vec3 position;

	int lastMouseX;
	int lastMouseY;
};