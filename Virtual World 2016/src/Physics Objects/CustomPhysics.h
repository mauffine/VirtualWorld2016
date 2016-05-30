#pragma once
#include "Physics Objects\Physics Scene.h"
#include "Physics Objects\RigidBody.h"
#include "Physics Objects\Sphere.h"
class CustomPhysics
{
public:
	CustomPhysics();
	~CustomPhysics();
	void SetupPhysics();
	void OnUpdate(double a_dt);
private:
	PhysicsScene* m_physicsScene;
};
