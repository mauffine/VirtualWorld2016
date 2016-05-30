#include "Physics Objects\Sphere.h"

SphereObject::SphereObject(glm::vec3 a_position,
	glm::vec3 a_velocity, float a_mass, float a_radius,
	glm::vec4 colour)
{
	m_position = a_position;
}