#pragma once
#include <vector>

#include "FBXFile.h"
#include "stb_image.h"
#include "gl_core_4_4.h"

#include "BaseCamera.h"
#include "Engine/Shader.h"
#include "Engine/DirectionalLight.h"
class FBXLoader
{
public:
	FBXLoader(char* a_fileLoc, char* a_textureLoc, glm::vec3 a_position,
		DirectionalLight* a_dirLight);
	~FBXLoader();
	void Update(float dt);
	void Draw(const BaseCamera& a_camera);
private:

	void CreateOpenGLBuffers(FBXFile* fbx);
	void CleanupOpenGLBuffers(FBXFile* fbx);

	FBXFile* m_fbx;
	unsigned int m_program;

	Shader m_shader;
	unsigned int m_diffuseID;
	int m_diffuseWidth, m_diffuseHeight,
		m_imageType;

	glm::vec3 m_position;
	glm::mat4 m_worldTransform;
	DirectionalLight* m_pDirLight;
};
