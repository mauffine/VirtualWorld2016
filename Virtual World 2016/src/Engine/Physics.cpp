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
	PxBoxGeometry box = PxBoxGeometry(1, 1, 1);
	PxTransform transform(PxVec3(0, 0, 0));
	//PxRigidStatic* actor = PxCreateStatic(*m_physics, transform, box, *m_physicsMaterial);
	//actor->userData = this;
	//actor->
	/*PxVec3 *verts = new PxVec3[a_numVerts];
	int vertIDX = 0;

	for (int vertcount = 0; vertcount < a_numVerts; ++vertcount)
	{
		glm::vec3 tmp = glm::vec3(a_vertexArray[vertcount].position.x, a_vertexArray[vertcount].position.y, a_vertexArray[vertcount].position.z);
		verts[vertIDX++] = PxVec3(tmp.x, tmp.y, tmp.z);
	}
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = a_numVerts;
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = verts;
	int triCount = a_numIndicies / 3;
	meshDesc.triangles.count = triCount;
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = a_indicies;

	PxDefaultMemoryOutputStream* buf = new PxDefaultMemoryOutputStream();
	assert(m_physicsCooker->cookTriangleMesh(meshDesc, *buf));
	PxU8* contents = buf->getData();	
	PxU32 size = buf->getSize();
	PxDefaultMemoryInputData input(contents, size);
	PxTriangleMesh* triangleMesh = m_physics->createTriangleMesh(input);
	PxTransform pose = PxTransform(PxVec3(0.0f, 0, 0.0f));
	PxShape* convexShape = actor->createShape(PxTriangleMeshGeometry(triangleMesh), *m_physicsMaterial, pose);*/

	PxHeightFieldDesc hfDesc;
	hfDesc.format = PxHeightFieldFormat::eS16_TM;
	hfDesc.nbColumns = a_rowsColumns;
	hfDesc.nbRows = a_rowsColumns;
	hfDesc.samples.data = a_data;
	hfDesc.samples.stride = sizeof(PxHeightFieldSample);
	hfDesc.thickness = -100.0f;
	PxHeightField* aHeightField = m_physics->createHeightField(hfDesc);
	PxHeightFieldGeometry hfGeom(aHeightField, PxMeshGeometryFlags(), 1,
		a_size, a_size);
	PxRigidStatic* actor = PxCreateStatic(*m_physics, transform, hfGeom, *m_physicsMaterial);
	PxTransform pose = PxTransform(PxVec3(0.0f, 0, 0.0f));
	PxShape* heightmap = actor->createShape((PxHeightFieldGeometry)hfGeom,
		*m_physicsMaterial, pose);

	//PxDefaultMemoryOutputStream* buf = new PxDefaultMemoryOutputStream();
	assert(heightmap);
		/*m_physicsCooker->cookHeightField(hfDesc, *buf));
	PxU8* contents = buf->getData();
	PxU32 size = buf->getSize();
	PxDefaultMemoryInputData input(contents, size);*/
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
}