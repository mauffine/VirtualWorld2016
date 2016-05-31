#include "Physics Objects\Physics Scene.h"
void PhysicsScene::AddActor(PhysicsObjectBase* a_actor)
{
	m_actors.push_back(a_actor);
}
void PhysicsScene::RemoveActor(PhysicsObjectBase* a_actor)
{
}
void PhysicsScene::Update()
{
	for (int i = 0; i <= m_actors.size; ++i)
	{
		m_actors[i]->Update(glm::vec3(0, -10, 0), .001f);
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