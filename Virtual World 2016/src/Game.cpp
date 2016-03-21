#include "Game.h"
bool Game::InitApp()
{
	glm::vec3 lightDir = glm::vec3(.5f, .5f, 0);
	m_rockPos = glm::vec3(0, 24, 0);
	FlyCamera* camera = new FlyCamera();
	camera->SetInputWindow(m_window);

	camera->SetupPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f);
	camera->LookAt(glm::vec3(50, 50, 50), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	m_camera = camera;

	m_terrain = new TerrainGen(100, 
		new DirectionalLight(glm::vec3(1,1,1), .5, 
			lightDir, .5, 0));

	m_rock = new FBXLoader("./res/Art Assets/Models/Rock1.fbx",
		"./res/Art Assets/Models/Rock-Texture-Surface.jpg", m_rockPos, lightDir);

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

	//m_camera->LookAt(m_rockPos, glm::vec3(0, 1, 0));

	return true;
}
void Game::Draw()
{
	DisplayGrid(100);

	m_terrain->Draw(*m_camera);
	m_rock->Draw(m_camera);
}