#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>
#include <vector>
#include "Camera.h"
#include "Model.h"
#include "Scene.h"
#include "FrameBuffer.h"

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
	unsigned int ppShader;

	Texture* m_texture;
	
	Scene m_scene;

	FrameBuffer* frameBuffer;
	FrameBuffer* ppFrameBuffer;

	Instance* char1;
	Instance* char2;

	int m_currentObject;
};