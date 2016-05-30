#pragma once
#include "glm\ext.hpp"
#include "glm\glm.hpp"
#include "Utilities\Gizmos.h"
enum ShapeType
{
	Plane,
	Sphere,
	Box
};
class PhysicsObjectBase
{
public:
	ShapeType m_shapeID;
	void virtual Update(glm::vec3 a_gravity, float a_timeStep) = 0;
	void virtual debug() = 0;
	void virtual MakeGizmo() = 0;
	void virtual ResetPosition() {};
protected:
	glm::vec3 m_position;
};