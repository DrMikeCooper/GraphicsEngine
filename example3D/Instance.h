#pragma once
#include <glm/glm.hpp>
#include <string>

class Scene;
class Texture;
class Model;
class Shader;

class Instance
{
public:
	void Draw(Scene* scene);
	Instance() : m_model(NULL),
		texture(NULL),
		texture2(NULL),
		texture3(NULL),
		m_shader(NULL),
		m_slider(0),
		m_slider2(0),
		m_pos(0, 0, 0),
		m_euler(0, 0, 0),
		m_scale(1, 1, 1),
		usesTarget(false) {}

	void UpdateTransform();
	float m_slider;
	float m_slider2;
	std::string m_name;
	Model* m_model;
	Texture* texture;
	Texture* texture2;
	Texture* texture3;
	glm::mat4 m_transform;
	Shader* m_shader; 

	glm::vec3  m_pos;
	glm::vec3 m_euler;
	glm::vec3 m_scale;
	int currentAnim = 0;
	float m_timer = 0; // animation timer

	bool usesTarget;
};
