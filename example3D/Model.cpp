#include <string>
#include "Model.h"
#include "Shader.h"

Shader* Model::defaultStatic = NULL;
Shader* Model::defaultAnimated = NULL;

bool Model::Load(const char* filename)
{
	bool result = false;

	if (strstr(filename, ".obj") != NULL)
	{
		std::string err;
		tinyobj::LoadObj(shapes, materials, err, filename);
		CreateBuffersOBJ();
		result = true;
	}

	if (strstr(filename, ".fbx") != NULL)
	{
		fbxFile = new FBXFile();
		fbxFile->load(filename);
		CreateBuffersFBX();
		result = true;
	}

	return result;
}

void Model::Update(float timer)
{
	if (isAnimated())
	{
		// grab the skeleton and animation we want to use
		FBXSkeleton* skeleton = fbxFile->getSkeletonByIndex(0);
		FBXAnimation* animation = fbxFile->getAnimationByIndex(0);

		// evaluate the animation to update bones
		skeleton->evaluate(animation, timer);
		for (unsigned int bone_index = 0; bone_index < skeleton->m_boneCount; ++bone_index)
		{
			skeleton->m_nodes[bone_index]->updateGlobalTransform();
		}
	}
}

void Model::Draw(glm::mat4 transform, glm::mat4 cameraMatrix, Shader* shader)
{
	glm::mat4 mvp = cameraMatrix * transform;

	shader->SetMatrix("MVP", mvp);
	shader->SetMatrix("M", transform);

	if (isAnimated())
	{
		// grab the skeleton and animation we want to use
		FBXSkeleton* skeleton = fbxFile->getSkeletonByIndex(0);
		skeleton->updateBones();
		int bones_location = glGetUniformLocation(shader->GetID(), "bones");
		glUniformMatrix4fv(bones_location, skeleton->m_boneCount, GL_FALSE,
			(float*)skeleton->m_bones);
	}

	for (unsigned int i = 0; i < m_gl_info.size(); ++i) {
		glBindVertexArray(m_gl_info[i].m_VAO);
		glDrawElements(GL_TRIANGLES, m_gl_info[i].m_index_count,
			GL_UNSIGNED_INT, 0);
	}
}

void Model::CreateBuffersFBX()
{
	// resize the glinfo array
	m_gl_info.resize(fbxFile->getMeshCount());

	// create the GL VAO/VBO/IBO data for each mesh
	for (unsigned int i = 0; i < fbxFile->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbxFile->getMeshByIndex(i);
		// storage for the opengl data in 3 unsigned int
		unsigned int* glData = new unsigned int[3];
		glGenVertexArrays(1, &glData[0]);
		glBindVertexArray(glData[0]);
		glGenBuffers(1, &glData[1]);
		glGenBuffers(1, &glData[2]);
		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);
		glBufferData(GL_ARRAY_BUFFER,
			mesh->m_vertices.size() * sizeof(FBXVertex),
			mesh->m_vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			mesh->m_indices.size() * sizeof(unsigned int),
			mesh->m_indices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0); // position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);
		glEnableVertexAttribArray(1); // normal
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex),((char*)0) + FBXVertex::NormalOffset);
		glEnableVertexAttribArray(2); // UVs
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,	sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset);
		if (isAnimated())
		{
			glEnableVertexAttribArray(3); // weights
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::WeightsOffset);
			glEnableVertexAttribArray(4); // indices
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::IndicesOffset);
		}
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		mesh->m_userData = glData;

		m_gl_info[i].m_VAO = glData[0];
		m_gl_info[i].m_VBO = glData[1];
		m_gl_info[i].m_IBO = glData[2];
		m_gl_info[i].m_index_count = mesh->m_indices.size();
	}
}

void Model::CreateBuffersOBJ()
{
	m_gl_info.resize(shapes.size());
	for (unsigned int mesh_index = 0; mesh_index < shapes.size(); ++mesh_index)
	{
		glGenVertexArrays(1, &m_gl_info[mesh_index].m_VAO);
		glGenBuffers(1, &m_gl_info[mesh_index].m_VBO);
		glGenBuffers(1, &m_gl_info[mesh_index].m_IBO);
		glBindVertexArray(m_gl_info[mesh_index].m_VAO);
		unsigned int float_count = shapes[mesh_index].mesh.positions.size();
		float_count += shapes[mesh_index].mesh.normals.size();
		float_count += shapes[mesh_index].mesh.texcoords.size();
		std::vector<float> vertex_data;
		vertex_data.reserve(float_count);
		vertex_data.insert(vertex_data.end(),
			shapes[mesh_index].mesh.positions.begin(),
			shapes[mesh_index].mesh.positions.end());
		vertex_data.insert(vertex_data.end(),
			shapes[mesh_index].mesh.normals.begin(),
			shapes[mesh_index].mesh.normals.end());
		vertex_data.insert(vertex_data.end(),
			shapes[mesh_index].mesh.texcoords.begin(),
			shapes[mesh_index].mesh.texcoords.end());
		m_gl_info[mesh_index].m_index_count =
			shapes[mesh_index].mesh.indices.size();
		glBindBuffer(GL_ARRAY_BUFFER, m_gl_info[mesh_index].m_VBO);
		glBufferData(GL_ARRAY_BUFFER,
			vertex_data.size() * sizeof(float),
			vertex_data.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_info[mesh_index].m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			shapes[mesh_index].mesh.indices.size() * sizeof(unsigned int),
			shapes[mesh_index].mesh.indices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0); //position
		glEnableVertexAttribArray(1); //normal data
		glEnableVertexAttribArray(2); //uvs
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0,
			(void*)(sizeof(float)*shapes[mesh_index].mesh.positions.size()));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0,
			(void*)(sizeof(float)*(shapes[mesh_index].mesh.positions.size() + shapes[mesh_index].mesh.normals.size())));
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void Model::SetDefaultShaders(const char* basicVertex, const char* animVertex, const char* fragment)
{
	defaultStatic = new Shader(basicVertex, fragment);
	defaultAnimated = new Shader(animVertex, fragment);
}