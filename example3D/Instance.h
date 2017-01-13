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
		m_texture(NULL),
		m_texture2(NULL),
		m_texture3(NULL),
		m_shader(NULL),
		m_pos(0, 0, 0),
		m_euler(0, 0, 0),
		m_scale(1, 1, 1) { Init();}
	Instance(Model* model, glm::vec3 pos, glm::vec3 angles = glm::vec3(0, 0, 0), float scale = 1, Texture* tex = NULL) : m_model(model), m_shader(NULL), m_pos(pos), m_euler(angles), m_scale(scale, scale, scale), m_texture(tex) { Init(); }

	void Init();
	void UpdateTransform();
	std::string m_name;
	Model* m_model;
	Texture* m_texture;
	Texture* m_texture2;
	Texture* m_texture3;
	glm::mat4 m_transform;
	Shader* m_shader; 

	glm::vec3 m_pos;
	glm::vec3 m_euler;
	glm::vec3 m_scale;

	int currentAnim;
	float m_timer; // animation timer
};
