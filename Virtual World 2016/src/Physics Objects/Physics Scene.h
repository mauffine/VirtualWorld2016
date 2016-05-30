#pragma once
#include <vector>

#include "glm\glm.hpp"
#include "glm\ext.hpp"

#include "Physics Objects\PhysicsObjectBase.h"
class PhysicsScene
{
public:
	PhysicsScene();
	~PhysicsScene();

	glm::vec3 m_gravity;
	float m_timestep;
	std::vector<PhysicsScene*> m_actors;
	void AddActor(PhysicsObjectBase*);
	void RemoveActor(PhysicsObjectBase);
	void Update();
	void DebugScene();
	void AddGizmos();
	void UpdateGizmos();
private:

};