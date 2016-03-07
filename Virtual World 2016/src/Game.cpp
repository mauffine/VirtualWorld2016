#include "Game.h"
bool Game::InitApp()
{
	FlyCamera* camera = new FlyCamera();
	camera->SetInputWindow(m_window);

	camera->SetupPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f);
	camera->LookAt(glm::vec3(50, 50, 50), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	m_camera = camera;

	m_terrain = new TerrainGen(500, 
		new DirectionalLight(glm::vec3(0,1,0), 1, glm::vec3(0,-1,0), 1, 1));

	m_rock = new FBXLoader("./res/Art Assets/Models/Rock1.fbx");

	return true;
}
void Game::DeInitApp()
{
}
bool Game::Update(double dt)
{
	m_camera->Update(dt);

	m_terrain->Update(dt);

	m_rock->Update((float)dt);

	return true;
}
void Game::Draw()
{
	DisplayGrid(100);

	m_terrain->Draw(*m_camera);
	m_rock->Draw(m_camera);
}
//void Game::RunMSAA(unsigned int a_SampleNum)
//{
//	unsigned int tex, m;
//	glGenTextures(1, &tex);
//	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);
//	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 
//		a_SampleNum, GL_RGBA8, GetScreenWidth(), GetScreenHeight(), false);
//
//	glGenFramebuffers(1, &m_fbo);
//	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE,
//		tex, 0);
//
//	GLenum status = glCheckFramebufferStatus();
//}