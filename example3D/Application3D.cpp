#include <gl_core_4_4.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <GLFW/glfw3.h>

#include "Application3D.h"
#include "Gizmos.h"
#include "Input.h"
#include "Shader.h"

//#define STB_IMAGE_IMPLEMENTATION
#include <../dependencies/stb/stb_image.h>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

struct Vertex {
	vec4 position;
	vec4 colour;
};



unsigned int LoadTexture(const char* name)
{
	unsigned int texture;

	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data = stbi_load(name, &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	return texture;
}

// function to create a grid
void Application3D::GenerateGrid(unsigned int rows, unsigned int cols)
{
	Vertex* aoVertices = new Vertex[rows * cols];
	for (unsigned int r = 0; r < rows; ++r) {
		for (unsigned int c = 0; c < cols; ++c) {
			aoVertices[r * cols + c].position = vec4(
				(float)c, 0, (float)r, 1);
			// create some arbitrary colour based off something
			// that might not be related to tiling a texture
			vec3 colour = vec3(sinf((c / (float)(cols - 1)) *
				(r / (float)(rows - 1))));
			aoVertices[r * cols + c].colour = vec4(colour, 1);
		}
	}

	// defining index count based off quad count (2 triangles per quad)
	unsigned int* auiIndices = new unsigned int[(rows - 1) * (cols - 1) *
		6];
	unsigned int index = 0;
	for (unsigned int r = 0; r < (rows - 1); ++r) {
		for (unsigned int c = 0; c < (cols - 1); ++c) {
			// triangle 1
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			// triangle 2
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			auiIndices[index++] = r * cols + (c + 1);
		}
	}

	// create a Vertex Array Object and store a ticket for it.
	glGenVertexArrays(1, &m_VAO);
	// create vertex and index buffers, and store.
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	// set the current Vertex Array Object to the one we created a few lines ago.
	glBindVertexArray(m_VAO);

	// set the current Vertex Buffer Object to the one we created earlier. This also sets this VBO as the one associated with the current VAO.	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	// feed the columns array to it
	glBufferData(GL_ARRAY_BUFFER, (rows * cols) * sizeof(Vertex), aoVertices, GL_STATIC_DRAW);
	// turn on slots 0 and 1 in this Vertex Buffer Object
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	// make slot 0 for the current VBO start at the beginning of the data, with each vertex separated in memory by the size of a Vertex structure.
	// this separataion between vertices, or stride, is the fifth argument passed in.
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	// make slot 1 for the current VBO start one vec4 into the data, with each vertex separated in memory by the size of a Vertex structure. 
	// This gives us a "striped" data block which goes vertex, colour, vertex, colour, since that's the way our data is laid out.
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));

	// set the current index buffer to the one we created earlier, and also make this the index buffer for the current VAO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	// feed in our array of indices
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (rows - 1) * (cols - 1) * 6 *	sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);

	// set the current VAO to zero now, we're done setting up our data
	glBindVertexArray(0);
	// set the current VBO and IBO to zero. CAREFUL! If you do this before setting the current VAO to zero, it
	// messes with the VAO and sets its VBO and IBO to zero again. 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// we've now got our vertex data copied into graphics card memory and have a ticket for it.
	// we can delete the CPU side memory if we no longer need it.
	delete[] aoVertices;
}

Application3D::Application3D() {
	rows = 8;
	columns = 8;
}

Application3D::~Application3D() {

}

bool Application3D::startup() {
	
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	// create simple camera transforms
	m_viewMatrix = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  getWindowWidth() / (float)getWindowHeight(),
										  0.1f, 1000.f);


	
	m_programID = Shader::CompileShaders("shaders\\BasicVertexShader.txt", "shaders\\BasicFragmentShader.txt");
	m_animProgramID = Shader::CompileShaders("shaders\\AnimatedVertexShader.txt", "shaders\\BasicFragmentShader.txt");

	GenerateGrid(rows, columns);
	m_texture = LoadTexture("textures\\numbered_grid.tga");
	buddha.Load("characters\\pyro\\pyro.fbx");
	cube.Load("data\\sphere.fbx");

	return true;
}

void Application3D::shutdown() {

	Gizmos::destroy();
}

void Application3D::update(float deltaTime) {

	// query exact time from GLFW
	float time = (float)glfwGetTime();

	// rotate camera
	m_viewMatrix = camera.GetViewMatrix();

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

	camera.Update();

	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void Application3D::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// update perspective in case window resized
	m_projectionMatrix = camera.GetProjectionMatrix(getWindowWidth(), getWindowHeight());
	m_viewMatrix = camera.GetViewMatrix();
	glm::mat4 cameraMatrix = m_projectionMatrix * m_viewMatrix;
	Gizmos::draw(cameraMatrix);

	glm::mat4 modelMatrix;
	// rotate around 45 degrees around the y axis
	static float angle = 0;
	if (aie::Input::getInstance()->isKeyDown(aie::INPUT_KEY_Q))
		angle += 0.01f;

	modelMatrix = glm::rotate(angle, vec3(0, 1, 0));
	glm::mat4 mvp = cameraMatrix * modelMatrix;

	glUseProgram(m_programID);

	unsigned int camUniform = glGetUniformLocation(m_programID, "cameraPosition");
	glUniform4f(camUniform, camera.GetPos().x, camera.GetPos().y, camera.GetPos().z, 1);

	// set texture slot 0 to use the texture we created earlier
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	// tell the shader where it is - use slot 0
	unsigned int loc = glGetUniformLocation(m_programID, "diffuse");
	glUniform1i(loc, 0);

	glBindVertexArray(m_VAO);
	unsigned int indexCount = (rows - 1) * (columns - 1) * 6;
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	

	static float x = -5;
	x += 0.1f;
	if (x > 5)
		x -= 10;
	modelMatrix = glm::translate(vec3(x, 0, 0)) * modelMatrix;
	cube.Draw(modelMatrix, cameraMatrix, m_programID);

	glUseProgram(m_animProgramID);
	camUniform = glGetUniformLocation(m_animProgramID, "cameraPosition");
	glUniform4f(camUniform, camera.GetPos().x, camera.GetPos().y, camera.GetPos().z, 1);

	// set texture slot 0 to use the texture we created earlier
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	// tell the shader where it is - use slot 0
	loc = glGetUniformLocation(m_animProgramID, "diffuse");
	glUniform1i(loc, 0);


	static float animTimer = 0;
	animTimer += 0.1f;
	buddha.Update(animTimer);

	// draw a scaled down model
	buddha.Draw(glm::scale(vec3(0.001f, 0.001f, 0.001f))*modelMatrix, cameraMatrix, m_animProgramID);
	
}