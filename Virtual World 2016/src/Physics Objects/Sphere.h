#pragma once
#include "Physics Objects\RigidBody.h"

class Sphere : public RigidBody
{
public:
	float m_radius;
	Sphere( glm::vec3 a_position,
			glm::vec3 a_velocity, float a_mass, float a_radius,
			glm::vec4 colour);
	~Sphere();

private:

};

Sphere::Sphere(
	glm::vec3 a_position,
	glm::vec3 a_velocity, float a_mass, float a_radius,
	glm::vec4 colour
	)
{
}

Sphere::~Sphere()
{
}