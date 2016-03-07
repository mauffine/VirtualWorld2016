#pragma once
#include "FBXFile.h"
#include "gl_core_4_4.h"

#include "BaseCamera.h"
#include "Engine/Shader.h"
class FBXLoader
{
public:
	FBXLoader(char* a_fileLoc);
	~FBXLoader();
	void Update(float dt);
	void Draw(BaseCamera* a_camera);
private:

	void CreateOpenGLBuffers(FBXFile* fbx);
	void CleanupOpenGLBuffers(FBXFile* fbx);

	FBXFile* m_fbx;
	unsigned int m_program;

	Shader m_shader;
};
