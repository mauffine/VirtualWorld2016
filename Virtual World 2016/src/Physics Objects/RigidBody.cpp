#include "Physics Objects\RigidBody.h"
RigidBody::RigidBody(glm::vec3 a_position, glm::vec3 a_velocity, glm::quat a_rotation,
	float a_mass)
{
	m_position = a_position;
	m_velocity = a_velocity;
	m_mass = a_mass;
}
RigidBody::RigidBody()
{
	m_position = glm::vec3(0, 0, 0);
	m_velocity = glm::vec3(0, 0, 0);
	m_mass = 0;
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
void RigidBody::Update(glm::vec2 a_gravity, float a_timeStep)
{

}
void RigidBody::Debug()
{

}