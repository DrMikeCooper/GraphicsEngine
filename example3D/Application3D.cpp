#include <gl_core_4_4.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <GLFW/glfw3.h>
#include <../dependencies/imgui/imgui.h>
#include "Application3D.h"
#include "Gizmos.h"
#include "Input.h"
#include "Shader.h"
#include "Texture.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

struct Vertex {
	vec4 position;
	vec4 colour;
};

Application3D::Application3D() {
}

Application3D::~Application3D() {

}

bool Application3D::startup() {
	
	setBackgroundColour(0.25f, 0.25f, 0.25f);
	m_scene.SetCamera(&camera);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	Model::SetDefaultShaders("..\\shaders\\BasicVertexShader.txt", "..\\shaders\\AnimatedVertexShader.txt", "..\\shaders\\BasicFragmentShader.txt");
	ppShader = Shader::CompileShaders("..\\shaders\\PostProcessVertex.txt", "..\\shaders\\PostProcessFragment.txt");
	m_texture = new Texture("textures\\numbered_grid.tga");
	buddha.Load("characters\\pyro\\pyro.fbx");
	cube.Load("data\\cube.fbx");

	m_currentObject = 0;

	frameBuffer = new FrameBuffer(getWindowWidth(), getWindowHeight());
	frameBuffer->SetUp();

	ppFrameBuffer = new FrameBuffer(getWindowWidth(), getWindowHeight());
	ppFrameBuffer->SetUp();

	// set up our scene
	m_scene.AddInstance("char1", &buddha, vec3(0, 0, 0), vec3(0,0,0), 0.002f, m_texture);
	m_scene.AddInstance("char2", &buddha, vec3(-5, 0, 0), vec3(0, 90, 0), 0.004f, m_texture);
	m_scene.AddInstance("vidscreen", &cube, vec3(5, 0, 0), vec3(0,0,0),1.0f, frameBuffer->GetTexture());

	char1 = m_scene.FindByName("char1");
	char2 = m_scene.FindByName("char2");

	return true;
}

void Application3D::SetUpDeferredRendering()
{
	basicPos.CompileShaders("..\\shaders\\BasicVertexShader.txt", "..\\shaders\\DeferredPos.txt");
	animPos.CompileShaders("..\\shaders\\AnimVertexShader.txt", "..\\shaders\\DeferredPos.txt");
	basicNormal.CompileShaders("..\\shaders\\BasicVertexShader.txt", "..\\shaders\\DeferredNormal.txt");
	animNormal.CompileShaders("..\\shaders\\AnimVertexShader.txt", "..\\shaders\\DeferredNormal.txt");
	basicAlbedo.CompileShaders("..\\shaders\\BasicVertexShader.txt", "..\\shaders\\DeferredAlbedo.txt");
	animAlbedo.CompileShaders("..\\shaders\\AnimVertexShader.txt", "..\\shaders\\DeferredAlbedo.txt");

	posBuffer = new FrameBuffer(getWindowWidth(), getWindowHeight());
	posBuffer->SetUp();
	normalBuffer = new FrameBuffer(getWindowWidth(), getWindowHeight());
	normalBuffer->SetUp();
	albedoBuffer = new FrameBuffer(getWindowWidth(), getWindowHeight());
	albedoBuffer->SetUp();
}

void Application3D::DeferredRenderToBuffers()
{
	Model::SetDefaultShaders(&basicPos, &animPos);
	posBuffer->RenderScene(m_scene);
	Model::SetDefaultShaders(&basicNormal, &animNormal);
	normalBuffer->RenderScene(m_scene);
	Model::SetDefaultShaders(&basicAlbedo, &animAlbedo);
	albedoBuffer->RenderScene(m_scene);
}


void Application3D::shutdown() 
{
	Gizmos::destroy();
}

void Application3D::update(float deltaTime) {

	// query exact time from GLFW
	float time = (float)glfwGetTime();

	// wipe the gizmos clean for this frame
	Gizmos::clear();

	// draw a simple grid with gizmos
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i < 21; ++i) {
		Gizmos::addLine(vec3(-10 + i, 0, 10),
						vec3(-10 + i, 0, -10),
						i == 10 ? white : black);
		Gizmos::addLine(vec3(10, 0, -10 + i),
						vec3(-10, 0, -10 + i),
						i == 10 ? white : black);
	}

	// add a transform so that we can see the axis
	Gizmos::addTransform(mat4(1));

	// demonstrate a few shapes
	Gizmos::addAABBFilled(vec3(0), vec3(1), vec4(0, 0.5f, 1, 0.25f));
	Gizmos::addSphere(vec3(5, 0, 5), 1, 8, 8, vec4(1, 0, 0, 0.5f));
	Gizmos::addRing(vec3(5, 0, -5), 1, 1.5f, 8, vec4(0, 1, 0, 1));
	Gizmos::addDisk(vec3(-5, 0, 5), 1, 16, vec4(1, 1, 0, 1));
	Gizmos::addArc(vec3(-5, 0, -5), 0, 2, 1, 8, vec4(1, 0, 1, 1));

	vec3* points = (vec3*)m_scene.GetPointLights(0);
	vec3* cols = (vec3*)m_scene.GetPointLights(0);
	float* powers = m_scene.GetPointLightPowers(0);
	for (int i = 0; i < 4; i++)
	{
		Gizmos::addSphere(points[i], 0.01f * powers[i], 8, 8, vec4(cols[i].x, cols[i].y, cols[i].z, 0.5));
	}

	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void Application3D::draw() {

	// wipe the screen to the background colour
	clearScreen();
	
	camera.Update();

	frameBuffer->RenderScene(m_scene);
	ppFrameBuffer->RenderScene(m_scene);

	// this is the basic draw with no Frame Buffers
	m_scene.Draw(getWindowWidth(), getWindowHeight());

	// and this is the post-processing draw call
	ppFrameBuffer->Draw(ppShader);
	
	ImGui::Begin("Animation");
	ImGui::SliderFloat("Character1 Anim", &char1->m_timer, 0, 10);
	ImGui::SliderFloat("Character2 Anim", &char2->m_timer, 0, 10);
	ImGui::End();

	ImGui::Begin("Lights");
	ImGui::SliderFloat3("Light Pos", m_scene.GetLightDirPtr(), -20, 20);
	ImGui::SliderFloat3("Light Pos1", m_scene.GetPointLights(0), -20, 20);
	ImGui::SliderFloat3("Light Color1", m_scene.GetPointLightColours(0), 0, 1);
	ImGui::SliderFloat("Light Power1", m_scene.GetPointLightPowers(0), 0, 100);
	ImGui::SliderFloat3("Light Pos2", m_scene.GetPointLights(1), -20, 20);
	ImGui::SliderFloat3("Light Color2", m_scene.GetPointLightColours(1), 0, 1);
	ImGui::SliderFloat("Light Power2", m_scene.GetPointLightPowers(1), 0, 100);
	ImGui::SliderFloat3("Light Pos3", m_scene.GetPointLights(2), -20, 20);
	ImGui::SliderFloat3("Light Color3", m_scene.GetPointLightColours(2), 0, 1);
	ImGui::SliderFloat("Light Power3", m_scene.GetPointLightPowers(2), 0, 100);
	ImGui::SliderFloat3("Light Pos4", m_scene.GetPointLights(3), -20, 20);
	ImGui::SliderFloat3("Light Color4", m_scene.GetPointLightColours(3), 0, 1);
	ImGui::SliderFloat("Light Power4", m_scene.GetPointLightPowers(3), 0, 100);
	ImGui::End();

	ImGui::Begin("Objects");
	ImGui::SliderInt("Object", &m_currentObject, 0, m_scene.GetInstances().size()-1);
	Instance& inst = m_scene.GetInstances()[m_currentObject];
	ImGui::SliderFloat3("Position", &inst.m_pos.x, -20, 20);
	ImGui::SliderFloat3("Angles", &inst.m_euler.x, 0, 360);
	ImGui::End();
}

