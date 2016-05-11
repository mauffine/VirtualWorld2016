#include "Engine\MerffEngine.h"

MerffEngine::MerffEngine(std::string a_appName, unsigned int a_screenWidth, unsigned int a_screenHeight)
{
	m_appName = a_appName;
	m_camera = nullptr;

	m_screenWidth = a_screenWidth;
	m_screenHeight = a_screenHeight;

	m_totalRunTime = glfwGetTime();
	m_deltaTime = 0.0f;
}
MerffEngine::~MerffEngine()
{

}
bool MerffEngine::InitGL()
{
	//Initiate GL
	if (glfwInit() == false)
		return false;

	//Set and Check if the window exists
	m_window = glfwCreateWindow(m_screenWidth, m_screenHeight, m_appName.c_str(), nullptr, nullptr);

	if (m_window == nullptr)
	{
		glfwTerminate();
		return false;
	}
	//make the window the current one that gl is using
	glfwMakeContextCurrent(m_window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
		return false;
	}
	//start gizmos
	Gizmos::create();
	//set GL functions up
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glfwSetMouseButtonCallback(m_window, OnMouseButton);
	//glfwSetCursorPosCallback(m_window, OnMousePosition);
	//glfwSetScrollCallback(m_window, OnMouseScroll);
	//glfwSetKeyCallback(m_window, OnKey);
	//glfwSetCharCallback(m_window, OnChar);
	return true;
}
void MerffEngine::Run()
{
	InitGL(); //start opengl
	InitApp(); //start the application
	
	assert(m_camera != NULL && "Camera not initialized!");
	assert(m_camera->GetPerspectiveSet() == true && "Camera Perspective is not set");

	bool running = true;
	while (!glfwWindowShouldClose(m_window) && running)
	{
		double currentTime = glfwGetTime();
		m_deltaTime = currentTime - m_totalRunTime;
		m_totalRunTime = currentTime;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Gizmos::clear();

		running = Update(m_deltaTime);
		Draw();

		Gizmos::draw(m_camera->GetProjectionView());

		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}

	DeInitApp();

	Gizmos::destroy();

	glfwDestroyWindow(m_window);
	glfwTerminate();
}
void MerffEngine::DisplayGrid(int a_size)
{
	assert(a_size > 0 && "Grid size must be a positive number");

	Gizmos::addTransform(glm::mat4(1));

	glm::vec4 white(1);
	glm::vec4 black(1, 0, 0, 1);

	int halfSize = a_size / 2;
	for (int i = 0; i < a_size + 1; ++i)
	{
		Gizmos::addLine(glm::vec3(-halfSize + i, 0, halfSize),
			glm::vec3(-halfSize + i, 0, -halfSize),
			i == halfSize ? white : black);
		Gizmos::addLine(glm::vec3(halfSize, 0, -halfSize + i),
			glm::vec3(-halfSize, 0, -halfSize + i),
			i == halfSize ? white : black);
	}
}
void MerffEngine::SetupPhysx()
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