#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include "Instance.h"
#include "Model.h"
#include "Scene.h"
#include "Shader.h"
#include "Texture.h"

using namespace glm;

void Instance::Init()
{
	if (m_shader == NULL)
		m_shader = m_model->isAnimated() ? Model::GetAnimated() : Model::GetStatic();

	currentAnim = -1;
	m_timer = 0;
}

void Instance::Update()
{
	m_model->Update(m_timer);
}

void Instance::Draw(Scene* scene)
{
	UpdateTransform();
	scene->UseShader(m_shader);
	m_shader->SetTexture("diffuse", 0, m_texture);
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

bool Instance::UsesTexture(uint texture)
{
	return m_texture && m_texture->GetID() == texture;
}