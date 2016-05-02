#include "Game.h"
bool Game::InitApp()
{
	m_lightDir = glm::vec3(.5f, -.5f, 0);
	m_rockPos = glm::vec3(0, 24, 0);
	FlyCamera* camera = new FlyCamera();
	camera->SetInputWindow(m_window);

	camera->SetupPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f);
	camera->LookAt(glm::vec3(-50, 50, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	m_camera = camera;
	m_globalLight = new DirectionalLight(glm::vec3(1, 1, 1), .2,
		m_lightDir, .7, 0);
	m_terrain = new TerrainGen(100, m_globalLight, 10);

	//TwInit(TW_OPENGL_CORE, nullptr);
	//TwWindowSize(1280, 720);

	//m_bar = TwNewBar("my bar");

	//TwAddVarRW(m_bar, "LightDirection", TW_TYPE_DIR3F, &m_lightDir[0], "");

	return true;
}
void Game::DeInitApp()
{
}
bool Game::Update(double dt)
{
	m_camera->Update(dt);

	m_terrain->Update(dt);
	m_globalLight->SetDirection(m_lightDir);
	//m_camera->LookAt(m_rockPos, glm::vec3(0, 1, 0));

	return true;
}
void Game::Draw()
{
	DisplayGrid(100);

	m_terrain->Draw(*m_camera);
	
	//TwDraw();
}