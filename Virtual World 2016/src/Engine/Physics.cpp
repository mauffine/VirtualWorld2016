#include "Engine\Physics.h"
Physics::Physics()
{
}

Physics::~Physics()
{
	m_physicsScene->release();
	m_physics->release();
	m_physicsFoundation->release();
}
void Physics::SetupPhysx()
{
	PxAllocatorCallback *myCallback = new MyAllocator();
	m_physicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *myCallback,
		m_defaultErrorCallback);
	m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_physicsFoundation,
		PxTolerancesScale());
	PxInitExtensions(*m_physics);
	//create the Physics Material
	m_physicsMaterial = m_physics->createMaterial(0.5f, 0.5f, 0.5f);
	PxSceneDesc sceneDesc(m_physics->getTolerancesScale);
	sceneDesc.gravity = PxVec3(0, -10.0f, 0);
	sceneDesc.filterShader = &physx::PxDefaultSimulationFilterShader;
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	m_physicsScene = m_physics->createScene(sceneDesc);
}

void Physics::Update(float a_dt)
{
	if (a_dt <= 0)
		return;
	m_physicsScene->simulate(a_dt);
	{
		while (m_physicsScene->fetchResults() == false)
		{

		}
	}
}
void Physics::SetupVisualDebugger()
{
	if (m_physics->getPvdConnectionManager() == NULL)
		return;

	const char* pvd_host_ip = "127.0.0.1";
	int port = 5425;
	unsigned int timeout = 100;

	PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags();
	auto theConnection = PxVisualDebuggerExt::createConnection(
		m_physics->getPvdConnectionManager(), pvd_host_ip, port, timeout, connectionFlags);
}
void Physics::SetUpTutorial1()
{
	PxTransform pose = PxTransform(PxVec3(0.0f, 0, 0.0f), PxQuat(PxHalfPi*1.0f, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = PxCreateStatic(*m_physics, pose, PxPlaneGeometry(), *m_physicsMaterial);
	m_physicsScene->addActor(*plane);

	float density = 10;
	PxBoxGeometry box(2, 2, 2);
	PxTransform transform(PxVec3(0, 5, 0));

	PxRigidDynamic* dynamicActor = PxCreateDynamic(*m_physics, transform, box, *m_physicsMaterial, density);
	m_physicsScene->addActor(*dynamicActor);
}