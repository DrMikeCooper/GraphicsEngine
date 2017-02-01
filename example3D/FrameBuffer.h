#pragma once
#include "Texture.h"

class Scene;
class Model;

class FrameBuffer
{
public:
	FrameBuffer(int w, int h) : width(w), height(h) {}

	void SetUp();
	void RenderScene(Scene& scene);
	void Draw(unsigned int shader);

	Texture* GetTexture() { return &frameTexture; }

private:
	Texture frameTexture;
	static Model* m_model;

	unsigned int m_fbo; // frame buffer object
	unsigned int m_fboTexture;
	unsigned int m_fboDepth;

	int width, height;

};
