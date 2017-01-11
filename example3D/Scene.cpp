#include "Camera.h"
#include "Scene.h"

glm::mat4 Scene::GetCameraMatrix()
{
	return m_camera->GetProjectionMatrix(1000,1000) * m_camera->GetViewMatrix();
}