#include "Engine\FBXLoader.h"

FBXLoader::FBXLoader(char* a_fileLoc)
{
	m_fbx = new FBXFile();
	m_fbx->load(a_fileLoc);
	CreateOpenGLBuffers(m_fbx);

	m_shader.AddShader("./res/Shaders/FBX.frag", ShaderType::FRAGMENT);
	m_shader.AddShader("./res/Shaders/FBX.vert", ShaderType::VERTEX);

	m_shader.LinkProgram();

}

FBXLoader::~FBXLoader()
{
	CleanupOpenGLBuffers(m_fbx);
}
void FBXLoader::CreateOpenGLBuffers(FBXFile* fbx)
{
	//create GL VAO/VBO/IBO data
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		//storage for opengl data
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

		glEnableVertexAttribArray(0); //position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
			sizeof(FBXVertex), 0);

		glEnableVertexAttribArray(1); // normal
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE,
			sizeof(FBXVertex),
			((char*)0) + FBXVertex::NormalOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;
	}
}

void FBXLoader::CleanupOpenGLBuffers(FBXFile* fbx)
{
	//clean vertex data attached to each mesh
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;
		glDeleteVertexArrays(1, &glData[0]);
		glDeleteBuffers(1, &glData[1]);
		glDeleteBuffers(1, &glData[2]);

		delete[] glData;
	}
}
void FBXLoader::Update(float dt)
{

}
void FBXLoader::Draw(BaseCamera* a_camera)
{
	glUniformMatrix4fv(m_shader.GetUniform("ProjectionView"), 1, GL_FALSE, 
		&(a_camera->GetProjectionView()[0][0]));

	//bind VAO and draw mesh
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
}