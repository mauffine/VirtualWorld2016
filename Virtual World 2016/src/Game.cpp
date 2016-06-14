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
	m_phyisicsPtr = new Physics();
	m_phyisicsPtr->SetupPhysx();
	//m_phyisicsPtr->CreatePhysicsScene();
	m_phyisicsPtr->SetupVisualDebugger();
	m_phyisicsPtr->SetUpTutorial1();
	m_phyisicsPtr->CreateMeshCollider(m_terrain->GetPhysicalSize(), 
		m_terrain->GetHeightData(), m_terrain->GetPhysicalSize());

	return true;
}
void Game::DeInitApp()
{
}
bool Game::Update(double dt)
{
	if (glfwGetKey(this->m_window, GLFW_KEY_R) == GLFW_PRESS && shooting == false)
	{
		m_phyisicsPtr->ShootProjectile(m_camera);
		shooting = true;
	}
	else if (!(glfwGetKey(this->m_window, GLFW_KEY_R) == GLFW_PRESS))
		shooting = false;

	m_phyisicsPtr->Update(dt);
	m_camera->Update(dt);

	m_terrain->Update(dt);
	m_globalLight->SetDirection(m_lightDir);
	return true;
}
void Game::Draw()
{
	DisplayGrid(100);

	m_terrain->Draw(*m_camera);
}