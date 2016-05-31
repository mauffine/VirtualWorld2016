#include "Physics Objects\Sphere.h"

SphereObject::SphereObject(glm::vec3 a_position,
	glm::vec3 a_velocity, float a_mass, float a_radius,
	glm::vec4 colour)
{
	m_position = a_position;
	m_velocity = a_velocity;
	m_mass = a_mass;
	m_radius = a_radius;
	MakeGizmo();
}
void SphereObject::MakeGizmo()
{
	Gizmos::addSphere(m_position, m_radius, 4, 4, glm::vec4(1, 1, 1, 1));
}
void SphereObject::Update(glm::vec3 a_gravity, float a_timeStep)
{
	m_velocity += a_gravity;
	m_position += m_velocity * a_timeStep;
}
void SphereObject::Debug()
{
}