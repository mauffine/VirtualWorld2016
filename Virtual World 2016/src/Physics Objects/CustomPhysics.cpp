#include "Physics Objects\CustomPhysics.h"

CustomPhysics::CustomPhysics()
{
}

CustomPhysics::~CustomPhysics()
{
}
void CustomPhysics::SetupPhysics()
{
	m_physicsScene = new PhysicsScene();
	m_physicsScene->m_gravity = glm::vec3(0, -10, 0);
	m_physicsScene->m_timestep = .001f;
	SphereObject* newball;
	newball = new SphereObject(glm::vec3(-40, 0, 0),
		glm::vec3(0, 0, 0),
		3.0f, 1,
		glm::vec4(1, 0, 0, 1));
	m_physicsScene->AddActor(newball);
}
void CustomPhysics::OnUpdate(double a_dt)
{
	m_physicsScene->Update();
	m_physicsScene->UpdateGizmos();
}
