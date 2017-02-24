#pragma once
#include <glm/glm.hpp>
#include <string>

class Texture;

class Shader
{
public:
	Shader() {}
	Shader(const char* vsFile, const char* fsFile)
	{
		Compile(vsFile, fsFile);
	}

	void Compile(const char* vsFile, const char* fsFile)
	{
		m_id = CompileShaders(vsFile, fsFile);
	}

	static unsigned int CompileShaders(const char* vsFile, const char* fsFile);
	void Use();
	void SetInt(const char* name, int value);
	void SetFloat(const char* name, float value);
	void SetVector(const char* name, glm::vec3 value);
	void SetMatrix(const char* name, glm::mat4 value);
	void SetTexture(const char* name, int slot, Texture* texture);
	void SetTexture(const char* name, int slot, unsigned int id);

	unsigned int GetID() { return m_id; }

private:
	static bool LoadShaderFromFile(const char* filePath, std::string& code);

	unsigned int m_id;
};