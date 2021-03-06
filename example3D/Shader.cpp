#include <gl_core_4_4.h>
#include <iostream>
#include <fstream>
#include <string>
#include "Shader.h"
#include "Texture.h"

void Shader::Use()
{
	glUseProgram(m_id);
}

void Shader::SetInt(const char* name, int value)
{
	int loc = glGetUniformLocation(m_id, name);
	glUniform1i(loc, value);
}

void Shader::SetFloat(const char* name, float value)
{
	int loc = glGetUniformLocation(m_id, name);
	glUniform1f(loc, value);
}

void Shader::SetVector(const char* name, glm::vec3 value)
{
	int loc = glGetUniformLocation(m_id, name);
	glUniform3f(loc, value.x, value.y, value.z);
}

void Shader::SetMatrix(const char* name, glm::mat4 value)
{
	int loc = glGetUniformLocation(m_id, name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)&value);
}

void Shader::SetTexture(const char* name, int slot, Texture* texture)
{
	SetTexture(name, slot, texture->GetID());
}

void Shader::SetTexture(const char* name, int slot, unsigned int id)
{
	// set texture slot 0 to use the texture we created earlier
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, id);

	// tell the shader where it is - use slot provided
	unsigned int loc = glGetUniformLocation(m_id, name);
	glUniform1i(loc, slot);
}

bool Shader::LoadShaderFromFile(const char* filePath, std::string& code)
{
	code = "";

	// Read the Vertex Shader code from the file
	std::ifstream VertexShaderStream(filePath, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::string line;
		while (getline(VertexShaderStream, line))
			code += line + "\n";
		VertexShaderStream.close();
		return true;
	}
	else {
		printf("Cannot open shader file %s.\n", filePath);
		return false;
	}
}

GLuint Shader::CompileShaders(const char* vsFile, const char* fsFile)
{
	printf("%s\n%s\n", vsFile, fsFile);
	std::string vsCode, fsCode;
	if (!(LoadShaderFromFile(vsFile, vsCode) && LoadShaderFromFile(fsFile, fsCode)))
		printf("failed to load shaders");
	const char* vsSource = vsCode.c_str();
	const char* fsSource = fsCode.c_str();

	GLuint programID;
	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);
	programID = glCreateProgram();
	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);
	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		int infoLogLength = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(programID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	return programID;
}