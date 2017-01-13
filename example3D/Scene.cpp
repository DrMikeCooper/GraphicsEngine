#include <gl_core_4_4.h>
#include <glm\glm\glm.hpp>
#include <glm\glm\ext.hpp>
#include "Camera.h"
#include "Scene.h"
#include "Shader.h"
#include "Gizmos.h"

using namespace glm;

glm::mat4 Scene::GetCameraMatrix()
{
	return m_cameraMatrix;
}

void Scene::AddInstance(Model* m, glm::vec3 pos, glm::vec3 angles /*= glm::vec3(0, 0, 0)*/, float scale /*= 1*/, Texture* tex /*=NULL*/)
{
	m_instances.push_back(Instance(m, pos, angles, scale, tex));
}

void Scene::Draw(float w, float h)
{
	mat4 projectionMatrix = m_camera->GetProjectionMatrix(w, h);
	mat4 viewMatrix = m_camera->GetViewMatrix();
	m_cameraMatrix = projectionMatrix * viewMatrix;

	aie::Gizmos::draw(m_cameraMatrix);

	for (int i = 0; i < m_instances.size(); i++)
		m_instances[i].Draw(this);
}

void Scene::UseShader(Shader* shader)
{
	shader->Use();
	shader->SetVector("cameraPosition", m_camera->GetPos());
	shader->SetVector("lightDir", glm::normalize(m_lightDir));
	// point lights
	int loc = glGetUniformLocation(shader->GetID(), "lightPos");
	glUniform3fv(loc, 4, &m_pointLights[0].x);
	loc = glGetUniformLocation(shader->GetID(), "lightCol");
	glUniform3fv(loc, 4, &m_pointLightColours[0].x);
	loc = glGetUniformLocation(shader->GetID(), "lightPower");
	glUniform1f(loc, m_pointLightPowers[0]);

}