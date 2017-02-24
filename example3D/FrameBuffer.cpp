#include <gl_core_4_4.h>
#include <glm\glm\ext.hpp>
#include "FrameBuffer.h"
#include "Scene.h"
#include "Model.h"
#include "Shader.h"

Model* FrameBuffer::m_model = NULL;

// sets up our render target
void FrameBuffer::SetUp()
{
	// set up the frame buffer object
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glGenTextures(1, &m_fboTexture);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, width, height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fboTexture, 0);
	glGenRenderbuffers(1, &m_fboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_fboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fboDepth);
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer Error!\n");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// store this so our engine can use it!
	frameTexture.SetID(m_fboTexture);

	if (m_model == NULL)
	{
		m_model = new Model();
		m_model->MakePostProcessQuad();
	}
}

// draws the specified scene into our render target
void FrameBuffer::RenderScene(Scene& scene)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, width, height);
	glClearColor(0.75f, 0.75f, 0.75f, 1);  // could pass this in as argument, or ignore altogether
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw the actual scene here
	scene.Draw((float)width, (float)height);

	// restore normal frame buffer after
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);
	glClearColor(0.0f, 0.0f, 0.25f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FrameBuffer::Draw(unsigned int shader)
{
	// draw out full-screen quad
	glUseProgram(shader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);
	int loc = glGetUniformLocation(shader, "target");
	glUniform1i(loc, 0);

	glBindVertexArray(m_model->GetVAO());
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void FrameBuffer::Draw(Shader* shader, unsigned int buffers[])
{
	// draw out full-screen quad
	shader->Use();

	shader->SetTexture("pos", 0, buffers[0]);
	shader->SetTexture("normal", 1, buffers[1]);
	shader->SetTexture("albedo", 2, buffers[2]);

	glBindVertexArray(m_model->GetVAO());
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
