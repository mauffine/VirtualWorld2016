#pragma once
#include "Physics Objects\RigidBody.h"
class SphereObject : public RigidBody
{
public:
	SphereObject( glm::vec3 a_position,
			glm::vec3 a_velocity, float a_mass, float a_radius,
			glm::vec4 colour);
	void MakeGizmo();
	virtual void Update(glm::vec3 a_gravity, float a_timeStep);
	virtual void Debug();
private:
	float m_radius;
	ShapeType m_shape = Sphere;
};
