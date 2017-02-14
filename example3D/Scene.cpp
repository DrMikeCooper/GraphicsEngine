#include <gl_core_4_4.h>
#include <glm\glm\glm.hpp>
#include <glm\glm\ext.hpp>
#include "Camera.h"
#include "Scene.h"
#include "Shader.h"
#include "Gizmos.h"

using namespace glm;

Scene::Scene()
{
	//set out some nice lights
	m_pointLightPowers[0] = m_pointLightPowers[1] = m_pointLightPowers[2] = m_pointLightPowers[3] = 50;
	m_pointLights[0] = vec3(-10, 10, -10);
	m_pointLights[1] = vec3(-10, 10, 10);
	m_pointLights[2] = vec3(10, 10, -10);
	m_pointLights[3] = vec3(10, 10, 10);

	m_pointLightColours[0] = vec3(1, 0, 0);
	m_pointLightColours[1] = vec3(1, 1, 0);
	m_pointLightColours[2] = vec3(0, 1, 0);
	m_pointLightColours[3] = vec3(0, 0, 1);
}

glm::mat4 Scene::GetCameraMatrix()
{
	return m_cameraMatrix;
}

void Scene::AddInstance(const char* nm, Model* m, glm::vec3 pos, glm::vec3 angles /*= glm::vec3(0, 0, 0)*/, float scale /*= 1*/, Texture* tex /*=NULL*/)
{
	m_instances.push_back(Instance(nm, m, pos, angles, scale, tex));
}

void Scene::Draw(float w, float h)
{
	mat4 projectionMatrix = m_camera->GetProjectionMatrix(w, h);
	mat4 viewMatrix = m_camera->GetViewMatrix();
	m_cameraMatrix = projectionMatrix * viewMatrix;

	aie::Gizmos::draw(m_cameraMatrix);

	for (int i = 0; i < m_instances.size(); i++)
	{
		m_instances[i].Update();
		m_instances[i].Draw(this);
	}
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

Instance* Scene::FindByName(const char* name)
{
	for (int i = 0; i < m_instances.size(); i++)
	{
		if (m_instances[i].m_name == name)
			return &m_instances[i];
	}
	return NULL;
}