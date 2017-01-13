#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>
#include <vector>
#include "Camera.h"
#include "Model.h"
#include "Scene.h"

class Texture;

class Application3D : public aie::Application {
public:

	Application3D();
	virtual ~Application3D();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	Camera camera;

	Model buddha;
	Model cube;

	Texture* m_texture;

	Scene m_scene;

	int m_currentObject;
};