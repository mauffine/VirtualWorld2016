#include "Physics Objects\RigidBody.h"
RigidBody::RigidBody(glm::vec3 a_position, glm::vec3 a_velocity, glm::quat a_rotation,
	float a_mass)
{
}

RigidBody::~RigidBody()
{
}
void RigidBody::ApplyForce(glm::vec3 a_force)
{
	m_velocity += a_force;
}
void RigidBody::ApplyForceToActor(RigidBody* a_actor2, glm::vec3 a_force)
{
	a_actor2->ApplyForce(a_force * m_mass);
}