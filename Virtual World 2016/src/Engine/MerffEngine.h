#pragma once
#include "gl_core_4_4.h"

#include <string>
#include <iostream>
#include <vector>
#include <fstream>

#include "AntTweakBar.h"
#include "GLFW\glfw3.h"
#include "Utilities\Gizmos.h"
#include "glm\ext.hpp"
#include "glm\glm.hpp"
#include "stb_image.h"
#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd\PxVisualDebugger.h>

#include "Engine\BaseCamera.h"
#include "Engine\Shader.h"
#include "Engine\MyAllocator.h"

using namespace physx;
class MerffEngine
{
public:
	MerffEngine(std::string a_appName, unsigned int a_screenWidth, unsigned int a_screenHeight);

	~MerffEngine();

	bool InitGL();
	void Run();

	virtual bool InitApp() = 0;
	virtual void DeInitApp() = 0;

	virtual bool Update(double a_deltaTime) = 0;
	virtual void Draw() = 0;

	double m_deltaTime;
	double m_totalRunTime;

	void DisplayGrid(int a_size);


protected:
	GLFWwindow* m_window;
	std::string m_appName;

	BaseCamera* m_camera;

	unsigned int m_program;
	
	unsigned int GetScreenWidth() const { return m_screenWidth; }
	unsigned int GetScreenHeight() const { return m_screenHeight; }
private:
	unsigned int m_screenWidth, m_screenHeight;

	void OnMouseButton(GLFWwindow*, int b, int a, int m) {
		TwEventMouseButtonGLFW(b, a);
	}
	void OnMousePosition(GLFWwindow*, double x, double y) {
		TwEventMousePosGLFW((int)x, (int)y);
	}
	void OnMouseScroll(GLFWwindow*, double x, double y) {
		TwEventMouseWheelGLFW((int)y);
	}
	void OnKey(GLFWwindow*, int k, int s, int a, int m) {
		TwEventKeyGLFW(k, a);
	}
	void OnChar(GLFWwindow*, unsigned int c) {
		TwEventCharGLFW(c, GLFW_PRESS);
	}
};
