#pragma once
#include <vector>
#include "Instance.h"

class Camera;
class Shader;
class Texture;

class Scene
{
public:
	Scene();

	void SetCamera(Camera* cam) { m_camera = cam; }
	glm::mat4 GetCameraMatrix();

	void AddInstance(const char* nm, Model* m, glm::vec3 pos, glm::vec3 angles = glm::vec3(0, 0, 0), float scale = 1, Texture* tex1 = NULL);
	void Draw(float w, float h, unsigned int forbiddenTexture = 0);
	void UseShader(Shader* shader);
	void SetDirectionalLight(glm::vec3 dir) { m_lightDir = dir; }

	float* GetLightDirPtr() { return &m_lightDir.x; }
	float* GetPointLights(int i) { return &m_pointLights[i].x; }
	float* GetPointLightColours(int i) { return &m_pointLightColours[i].x; }
	float* GetPointLightPowers(int i) { return &m_pointLightPowers[i]; }

	std::vector<Instance>& GetInstances() { return m_instances; }

	Instance* FindByName(const char* name);

private:
	Camera* m_camera;

	// sunlight
	glm::vec3 m_lightDir;
	glm::vec3 m_pointLights[4];
	glm::vec3 m_pointLightColours[4];
	float m_pointLightPowers[4];
	std::vector<Instance> m_instances;

	glm::mat4 m_cameraMatrix;
};