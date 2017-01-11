#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include "Instance.h"
#include "Model.h"
#include "Scene.h"
#include "Shader.h"

using namespace glm;

void Instance::Draw(Scene* scene)
{
	m_model->Draw(m_transform, scene->GetCameraMatrix(), m_shader);
}

void Instance::UpdateTransform()
{

	float rad = 6.28f / 360.0f;
	m_transform = glm::translate(m_pos)
		* glm::rotate(m_euler.z * rad, vec3(0, 0, 1))
		* glm::rotate(m_euler.y * rad, vec3(0, 1, 0))
		* glm::rotate(m_euler.x * rad, vec3(1, 0, 0))
		* glm::scale(m_scale);
}