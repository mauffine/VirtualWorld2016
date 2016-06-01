#pragma once
#include <vector>

#include "glm\glm.hpp"
#include "glm\ext.hpp"

#include "Physics Objects\PhysicsObjectBase.h"
class PhysicsScene
{
public:
	glm::vec3 m_gravity;
	float m_timestep;
	float m_nextUpdate;
	std::vector<PhysicsObjectBase*> m_actors;

	void AddActor(PhysicsObjectBase* a_actor);
	void RemoveActor(PhysicsObjectBase* a_actor);
	void Update(double a_dt);
	void DebugScene();
	void AddGizmos();
	void UpdateGizmos();
private:

};