#include "Engine\FBXLoader.h"

FBXLoader::FBXLoader(char* a_fileLoc, char* a_textureLoc, glm::vec3 a_position,
	glm::vec3 a_lightDir)
{
	m_fbx = new FBXFile();
	m_fbx->load(a_fileLoc);
	m_position = a_position;
	m_worldTransform = glm::mat4(1);
	for (int i = 0; i < 3; ++i)
	{
		m_worldTransform[i][3] = m_position[i];
	}
	m_worldTransform[3].w = 1;

	m_lightDir = a_lightDir;

	CreateOpenGLBuffers(m_fbx);

	m_shader.AddShader("./res/Shaders/FBX.frag", ShaderType::FRAGMENT);
	m_shader.AddShader("./res/Shaders/FBX.vert", ShaderType::VERTEX);
	
	unsigned char* m_diffuseTex = stbi_load(a_textureLoc, &m_diffuseWidth,
		&m_diffuseHeight, &m_imageType, STBI_default);

	glGenTextures(1, &m_diffuseID);
	glBindTexture(GL_TEXTURE_2D, m_diffuseID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_diffuseWidth, m_diffuseHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, m_diffuseTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	glGenerateMipmap(GL_TEXTURE_2D);
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

		glEnableVertexAttribArray(2); // TexCoord
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
			sizeof(FBXVertex),
			((char*)0) + FBXVertex::TexCoord1Offset);
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
	m_shader.Bind();

	glUniformMatrix4fv(m_shader.GetUniform("ProjectionView"), 
		1, GL_FALSE, &(a_camera->GetProjectionView()[0][0]));

	glUniformMatrix4fv(m_shader.GetUniform("WorldTransform"), 
		1, GL_FALSE, &(m_worldTransform[0][0]));

	glUniform4f(m_shader.GetUniform("LightDir"), m_lightDir.x, 
		m_lightDir.y, m_lightDir.z, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_diffuseID);

	glUniform1i(m_shader.GetUniform("material.diffuseTex"), 0);

	//bind VAO and draw mesh
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), 
			GL_UNSIGNED_INT, 0);
	}
}