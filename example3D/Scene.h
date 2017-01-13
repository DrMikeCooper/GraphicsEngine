#pragma once
#include <vector>
#include "Instance.h"

class Camera;
class Shader;
class Texture;

class Scene
{
public:
	void SetCamera(Camera* cam) { m_camera = cam; }
	glm::mat4 GetCameraMatrix();

	void AddInstance(Model* m, glm::vec3 pos, glm::vec3 angles = glm::vec3(0, 0, 0), float scale = 1, Texture* tex1 = NULL);
	void Draw(float w, float h);
	void UseShader(Shader* shader);
	void SetDirectionalLight(glm::vec3 dir) { m_lightDir = dir; }

	float* GetLightDirPtr() { return &m_lightDir.x; }

	std::vector<Instance>& GetInstances() { return m_instances; }

private:
	Camera* m_camera;

	// sunlight
	glm::vec3 m_lightDir;
	std::vector<glm::vec3> m_pointLights;
	std::vector<Instance> m_instances;

	glm::mat4 m_cameraMatrix;
};