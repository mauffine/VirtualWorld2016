#pragma once
#include <vector>

#include "Engine/MerffEngine.h"
class Renderer
{
public:
	Renderer();
	~Renderer();

private:
	unsigned int m_fbo, m_fboTexture;
};
