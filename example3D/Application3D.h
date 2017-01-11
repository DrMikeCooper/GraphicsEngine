#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>
#include <vector>
#include "Camera.h"
#include "Model.h"

class Application3D : public aie::Application {
public:

	Application3D();
	virtual ~Application3D();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void GenerateGrid(unsigned int rows, unsigned int cols);

protected:

	Camera camera;

	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

	// our vertex and index buffers
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	unsigned int m_programID;
	unsigned int m_animProgramID;

	unsigned int m_texture;

	int rows;
	int columns;

	Model buddha;
	Model cube;
};