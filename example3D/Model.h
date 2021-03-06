#pragma once
#include <gl_core_4_4.h>
#include <glm/glm.hpp>
#include <../dependencies/tinyobjloader/tiny_obj_loader.h>
#include <../dependencies/FBXLoader/FBXFile.h>

class Shader;

class Model
{
public:
	Model() : fbxFile(NULL) {}
	bool Load(const char* filename);
	void Draw(glm::mat4 transform, glm::mat4 cameraMatrix, Shader* shader);
	void Update(float timer);
	static void SetDefaultShaders(const char* basicVertex, const char* animVertex, const char* fragment);
	static void SetDefaultShaders(Shader* basic, Shader* anim) { defaultStatic = basic; defaultAnimated = anim;	}
	unsigned int GetVAO() { return m_gl_info[0].m_VAO; }
	static Shader* GetStatic() { return defaultStatic; }
	static Shader* GetAnimated() { return defaultAnimated; }

	void MakePostProcessQuad();

	bool isAnimated() { return fbxFile && fbxFile->getSkeletonCount() > 0; }

private:
	void CreateBuffersOBJ();
	void CreateBuffersFBX();

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	struct GLInfo
	{
		unsigned int m_VAO;
		unsigned int m_VBO;
		unsigned int m_IBO;
		unsigned int m_index_count;
	};

	std::vector<GLInfo> m_gl_info;

	FBXFile* fbxFile;

	static Shader* defaultStatic;
	static Shader* defaultAnimated;
};