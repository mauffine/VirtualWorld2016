#include "Engine\Physics.h"
Physics::Physics()
{
	m_projectileSpeed = 50;
}

Physics::~Physics()
{
	m_physicsScene->release();
	m_physics->release();
	m_physicsFoundation->release();
}

void Physics::Update(float a_dt)
{
	if (a_dt > 0)
	{
		m_physicsScene->simulate(a_dt > 0.033f ? 0.033f : a_dt);
		while (m_physicsScene->fetchResults() == false);

	}
	if (m_particleEmitter)
	{
		m_particleEmitter->update(a_dt);
		m_particleEmitter->renderParticles();
	}
	RenderGizmos(m_physicsScene);
}
void Physics::SetupPhysx()
{
	m_defailtFilterShader = PxDefaultSimulationFilterShader;
	m_physicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION,
		m_defaultAllocatorCallback, m_defaultErrorCallback);
	m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_physicsFoundation,
		PxTolerancesScale());
	PxInitExtensions(*m_physics);
	m_physicsMaterial = m_physics->createMaterial(1, 1, 0);
	m_physicsCooker = PxCreateCooking(PX_PHYSICS_VERSION, *m_physicsFoundation,
		PxCookingParams(PxTolerancesScale()));

	//m_physicsScene = CreatePhysicsScene();
}
void Physics::SetupVisualDebugger()
{
	if (m_physics->getPvdConnectionManager() == NULL)
		return;
	const char* pvdHostIp = "Localhost";
	int port = 5425;
	unsigned int timeout = 100;
	PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags();
	auto theConnection = PxVisualDebuggerExt::createConnection(m_physics->getPvdConnectionManager(),
		pvdHostIp, port, timeout, connectionFlags);
}
PxScene* Physics::CreatePhysicsScene()
{
	PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0, -9.8007f, 0);
	sceneDesc.filterShader = &PxDefaultSimulationFilterShader;
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(8);
	PxScene* result = m_physics->createScene(sceneDesc);
	return result;
}
void Physics::ShootProjectile(BaseCamera* a_camera)
{
	PxTransform boxTransform(PxVec3(a_camera->GetPosition().x, a_camera->GetPosition().y, a_camera->GetPosition().z));
	PxSphereGeometry sphere(0.75f);
	float density = 10;
	PxRigidDynamic* newActor = PxCreateDynamic(*m_physics, boxTransform, sphere, *m_physicsMaterial, density);

	glm::vec3 direction(-a_camera->GetTransform()[2]);
	PxVec3 velocity = PxVec3(direction.x, direction.y, direction.z) * m_projectileSpeed;
	newActor->setLinearVelocity(velocity, true);
	m_physicsScene->addActor(*newActor);
}
void Physics::CreateMeshCollider(int a_size, int* a_data, int a_rowsColumns)
{
	PxHeightFieldDesc hfDesc;
	hfDesc.format = PxHeightFieldFormat::eS16_TM;
	hfDesc.nbColumns = a_rowsColumns;
	hfDesc.nbRows = a_rowsColumns;
	hfDesc.samples.data = a_data;
	hfDesc.samples.stride = sizeof(PxHeightFieldSample);
	hfDesc.thickness = -100;

	PxHeightField* heightField = m_physics->createHeightField(hfDesc);
	PxHeightFieldGeometry hfGeom(heightField, PxMeshGeometryFlags(), .1, 1, 1);
	PxTransform pose = PxTransform(PxVec3(-50, 0, -50));
	PxRigidStatic* actor = PxCreateStatic(*m_physics, pose, hfGeom, *m_physicsMaterial);
	//PxShape* heightMap = actor->createShape(hfGeom, *m_physicsMaterial, pose);
	//assert(heightMap);
	m_physicsScene->addActor(*actor);
}
void Physics::CreateLiquid()
{
	PxParticleFluid* pf;

	PxU32 maxParticles = 4000;
	bool perParticleRestOffset = false;
	pf = m_physics->createParticleFluid(maxParticles, perParticleRestOffset);

	pf->setRestParticleDistance(.3f);
	pf->setDynamicFriction(0.1);
	pf->setStaticFriction(0.1);
	pf->setDamping(0.1);
	pf->setParticleMass(.1);
	pf->setRestitution(0);
	//pf->setParticleReadDataFlag(PxParticleReadDataFlag::eDENSITY_BUFFER,
	// true);
	pf->setParticleBaseFlag(PxParticleBaseFlag::eCOLLISION_TWOWAY, true);
	pf->setStiffness(100);

	if (pf)
	{
		m_physicsScene->addActor(*pf);
		m_particleEmitter = new ParticleFluidEmitter(maxParticles,
			PxVec3(0, 10, 0), pf, .01);
		m_particleEmitter->setStartVelocityRange(-2.0f, 0, -2.0f,
			2.0f, 0.0f, 2.0f);
	}
}
void Physics::RenderGizmos(PxScene* a_scene)
{
	PxActorTypeFlags desiredTypes = PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC;
	PxU32 actor_count = a_scene->getNbActors(desiredTypes);
	PxActor** actor_list = new PxActor*[actor_count];
	a_scene->getActors(desiredTypes, actor_list, actor_count);

	glm::vec4 geo_color(1, 0, 0, 1);
	for (int actor_index = 0;
	actor_index < (int)actor_count;
		++actor_index)
	{
		PxActor* curr_actor = actor_list[actor_index];
		if (curr_actor->isRigidActor())
		{
			PxRigidActor* rigid_actor = (PxRigidActor*)curr_actor;
			PxU32 shape_count = rigid_actor->getNbShapes();
			PxShape** shapes = new PxShape*[shape_count];
			rigid_actor->getShapes(shapes, shape_count);

			for (int shape_index = 0;
			shape_index < (int)shape_count;
				++shape_index)
			{
				PxShape* curr_shape = shapes[shape_index];
				AddWidget(curr_shape, rigid_actor, geo_color);
			}

			delete[]shapes;
		}
	}

	delete[] actor_list;

	int articulation_count = a_scene->getNbArticulations();

	for (int a = 0; a < articulation_count; ++a)
	{
		PxArticulation* articulation;
		a_scene->getArticulations(&articulation, 1, a);

		int link_count = articulation->getNbLinks();

		PxArticulationLink** links = new PxArticulationLink*[link_count];
		articulation->getLinks(links, link_count);

		for (int l = 0; l < link_count; ++l)
		{
			PxArticulationLink* link = links[l];
			int shape_count = link->getNbShapes();

			for (int s = 0; s < shape_count; ++s)
			{
				PxShape* shape;
				link->getShapes(&shape, 1, s);
				AddWidget(shape, link, geo_color);
			}
		}
		delete[] links;
	}

}
void Physics::AddWidget(PxShape* shape, PxRigidActor* actor, glm::vec4 geo_color)
{
	PxTransform full_transform = PxShapeExt::getGlobalPose(*shape, *actor);
	glm::vec3 actor_position(full_transform.p.x, full_transform.p.y, full_transform.p.z);
	glm::quat actor_rotation(full_transform.q.w,
		full_transform.q.x,
		full_transform.q.y,
		full_transform.q.z);
	glm::mat4 rot(actor_rotation);

	glm::mat4 rotate_matrix = glm::rotate(10.f, glm::vec3(7, 7, 7));

	PxGeometryType::Enum geo_type = shape->getGeometryType();

	switch (geo_type)
	{
	case (PxGeometryType::eBOX) :
	{
		PxBoxGeometry geo;
		shape->getBoxGeometry(geo);
		glm::vec3 extents(geo.halfExtents.x, geo.halfExtents.y, geo.halfExtents.z);
		Gizmos::addAABBFilled(actor_position, extents, geo_color, &rot);
	} break;
	case (PxGeometryType::eCAPSULE) :
	{
		PxCapsuleGeometry geo;
		shape->getCapsuleGeometry(geo);
		Gizmos::addCapsule(actor_position, geo.halfHeight * 2, geo.radius, 16, 16, geo_color, &rot);
	} break;
	case (PxGeometryType::eSPHERE) :
	{
		PxSphereGeometry geo;
		shape->getSphereGeometry(geo);
		Gizmos::addSphereFilled(actor_position, geo.radius, 16, 16, geo_color, &rot);
	} break;
	case (PxGeometryType::ePLANE) :
	{

	} break;
	}
}
void Physics::SetUpTutorial1()
{
	m_physicsScene = CreatePhysicsScene();

	//add plane
	PxTransform pose = PxTransform(PxVec3(0.0f, 0, 0.0f), PxQuat(PxHalfPi*1.0f,
		PxVec3(0.0f, 0.0f, 1.0f)));

	PxRigidStatic* plane = PxCreateStatic(*m_physics, pose, PxPlaneGeometry(),
		*m_physicsMaterial);

	//add to scene
	m_physicsScene->addActor(*plane);

	//add box
	float density = 10;
	PxBoxGeometry box(2, 2, 2);
	PxTransform transform(PxVec3(0, 30, 0));
	PxRigidDynamic* dynamicActor = PxCreateDynamic(*m_physics, transform, box, *m_physicsMaterial, density);

	//add to scene
	m_physicsScene->addActor(*dynamicActor);
	/*PxTransform pose = PxTransform(PxVec3(0.0f, 0, 0.0f), PxQuat(PxHalfPi,
		PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = PxCreateStatic(*m_physics, pose, PxPlaneGeometry(),
		*m_physicsMaterial);
	const PxU32 numShapes = plane->getNbShapes();
	m_physicsScene->addActor(*plane);
	PxBoxGeometry side1(4.5, 1, .5);
	PxBoxGeometry side2(.5, 1, 4.5);
	pose = PxTransform(PxVec3(0.0f, 0.5, 4.0f));
	PxRigidStatic* box = PxCreateStatic(*m_physics, pose, side1,
		*m_physicsMaterial);
	m_physicsScene->addActor(*box);
	m_PhysXActors.push_back(box);
	pose = PxTransform(PxVec3(0.0f, 0.5, -4.0f));
	box = PxCreateStatic(*m_physics, pose, side1, *m_physicsMaterial);
	m_physicsScene->addActor(*box);
	m_PhysXActors.push_back(box);
	pose = PxTransform(PxVec3(4.0f, 0.5, 0));
	box = PxCreateStatic(*m_physics, pose, side2, *m_physicsMaterial);
	m_physicsScene->addActor(*box);
	m_PhysXActors.push_back(box);
	pose = PxTransform(PxVec3(-4.0f, 0.5, 0));
	box = PxCreateStatic(*m_physics, pose, side2, *m_physicsMaterial);
	m_physicsScene->addActor(*box);
	m_PhysXActors.push_back(box);*/
}