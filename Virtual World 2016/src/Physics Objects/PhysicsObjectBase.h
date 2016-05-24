#pragma once
#include "glm\ext.hpp"
#include "glm\glm.hpp"
enum ShapeType
{
	Plane,
	Sphere,
	Box
};
class PhysicsObjectBase
{
public:
	PhysicsObjectBase();
	~PhysicsObjectBase();
	ShapeType m_shapeID;
	void virtual Update(glm::vec3 a_gravity, float a_timeStep) = 0;
	void virtual debug() = 0;
	void virtual MakeGizmo() = 0;
	void virtual ResetPosition() {};
private:

};