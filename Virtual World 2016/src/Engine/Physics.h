#pragma once

#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd\PxVisualDebugger.h>

#include <Utilities\Gizmos.h>

#include "Engine\MyAllocator.h"
using namespace physx;
class Physics
{
public:
	Physics();
	~Physics();

	void Update(float a_dt);

	void SetupPhysx();
	void SetupVisualDebugger();
	void SetUpTutorial1();
	//Physics
	PxFoundation* m_physicsFoundation;
	PxPhysics* m_physics;
	PxScene* m_physicsScene;

	PxDefaultErrorCallback m_defaultErrorCallback;
	PxDefaultAllocator m_defaultAllocatorCallback;
	PxSimulationFilterShader m_defailtFilterShader = PxDefaultSimulationFilterShader;
	
	PxMaterial* m_physicsMaterial;
	PxMaterial* m_boxMaterial;
	PxCooking* m_physicsCooker;
	PxControllerManager* m_controllerManager;
private:
};

