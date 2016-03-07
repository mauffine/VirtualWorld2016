#include <iostream>

#include <math.h>
#include <vector>

#include "Utilities\Gizmos.h"

#include "Engine\MerffEngine.h"
#include "Engine\FlyCamera.h"
#include "Engine\GPUParticleEmitter.h"
#include "Engine\TerrainGen.h"
#include "Engine\FBXLoader.h"
struct Vertex
{
	float x, y, z, w;
	float nx, ny, nz, nw;
	float tx, ty, tz, tw;
	float s, t;
};
struct Planet
{
	glm::vec3 m_position;
	glm::vec4 m_colour;

	float m_size;
	float m_rotationAngle = 0;

	unsigned int lines = 25;
	Planet(glm::vec3 a_position, glm::vec4 a_colour, float a_size)
	{
		m_position = a_position;
		m_colour = a_colour;
		m_size = a_size;
	}
};
class Game : public MerffEngine
{
private:
	float testAngle = 0;
	Shader *m_shader;
	unsigned int m_diffuse, m_normal;
	unsigned int m_vao, m_vbo, m_ibo, m_fbo;

	GPUParticleEmitter* m_iDontEven;
	GPUParticleEmitter* m_rain;
	GPUParticleEmitter* m_upParticles;

	TerrainGen* m_terrain;
	FBXLoader* m_rock;
	unsigned int m_MSAAtex;

	void RunMSAA(unsigned int a_SampleNum);
protected:
public:
	Game() : MerffEngine("Virtual World", 1280, 720) {};
	bool InitApp();
	void DeInitApp();
	bool Update(double dt);
	void Draw();

};