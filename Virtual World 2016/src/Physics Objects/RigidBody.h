#pragma once
#include "Physics Objects\PhysicsObjectBase.h"
class RigidBody : public PhysicsObjectBase
{
public:
	RigidBody(glm::vec3 a_position, glm::vec3 a_velocity, glm::quat a_rotation,
		float a_mass);
	glm::vec2 m_position;
	glm::vec2 m_velocity;

	float m_mass;
	float m_rotation2D;

	virtual void Update(glm::vec2 a_gravity, float a_timeStep);
	virtual void Debug();
	void ApplyForce(glm::vec2 a_force);
	void applyForceToActor(RigidBody* a_actor2, glm::vec3 a_force);
	~RigidBody();

private:

};

RigidBody::RigidBody(glm::vec3 a_position, glm::vec3 a_velocity, glm::quat a_rotation,
	float a_mass)
{
}

RigidBody::~RigidBody()
{
}