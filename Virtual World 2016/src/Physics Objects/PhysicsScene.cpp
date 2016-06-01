#include "Physics Objects\Physics Scene.h"
void PhysicsScene::AddActor(PhysicsObjectBase* a_actor)
{
	m_actors.push_back(a_actor);
}
void PhysicsScene::RemoveActor(PhysicsObjectBase* a_actor)
{
}
void PhysicsScene::Update(double a_dt)
{
	m_nextUpdate -= a_dt;
	if (m_nextUpdate <= 0 && m_actors.size() > 0)
	{
		for (int i = 0; i <= m_actors.size(); ++i)
		{
			m_actors[i]->Update(glm::vec3(0, -10, 0), .001f);
		}
		m_nextUpdate = m_timestep;
	}
}
void PhysicsScene::DebugScene()
{

}
void PhysicsScene::AddGizmos()
{

}
void PhysicsScene::UpdateGizmos()
{

}