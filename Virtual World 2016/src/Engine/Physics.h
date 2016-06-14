#pragma once
#include <vector>

#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd\PxVisualDebugger.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>

#include <Utilities\Gizmos.h>

#include "Engine\MyAllocator.h"
#include "Engine\BaseCamera.h"
#include "Engine\TerrainGen.h"
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
	void ShootProjectile(BaseCamera* a_camera);
	void CreateMeshCollider(int a_size, int* a_data, int a_rowsColumns);
	void CreateLiquid();
	PxScene* CreatePhysicsScene();
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
	void AddWidget(PxShape* shape, PxRigidActor* actor, glm::vec4 geo_color);
	void RenderGizmos(PxScene* a_scene);

	float m_projectileSpeed;

	std::vector<PxActor*> m_PhysXActors;
	
};

