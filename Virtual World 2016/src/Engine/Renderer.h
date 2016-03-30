#pragma once
#include <vector>

#include "Engine/MerffEngine.h"
class Renderer
{
public:
	Renderer();
	~Renderer();
	void Update(float dt);
	void Draw(BaseCamera* a_camera);
private:
	unsigned int m_fbo, m_vao, m_vbo, m_ibo, m_fboTexture, m_fboDepth;
	Shader* m_shader;
};
