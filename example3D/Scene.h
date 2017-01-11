#pragma once
#include <vector>
#include "Instance.h"

class Camera;

class Scene
{
public:
	glm::mat4 GetCameraMatrix();

private:
	Camera* m_camera;
	std::vector<Instance> m_instances;


};