#pragma once

#include "Engine\MerffEngine.h"
#include "Engine\Shader.h"
#include "Engine\DirectionalLight.h"
class TerrainGen
{
public:
	TerrainGen(const unsigned int& a_size, DirectionalLight* a_pDirLight);
	~TerrainGen();

	bool Update(double dt);
	void Draw(const BaseCamera& a_camera);

	void GenerateEnvironment();
private:
	struct Vertex
	{
		glm::vec4 position;
		glm::vec3 normal;
		glm::vec2 texCoord;
		glm::vec2 heightMapCoord;
	};
	Vertex* m_vertexData;
	unsigned int* m_indicies;

	unsigned int m_size;
	unsigned int m_vao, m_vbo, m_ibo;
	unsigned int m_perlinTexture;

	float m_seed;

	void GeneratePlane();
	void GenerateBuffers();
	void GeneratePerlinNoise(int a_dims, float* a_data);

	unsigned int m_diffuseID;
	int m_diffuseHeight, m_diffuseWidth, m_imageType;

	Shader m_shaders;

	DirectionalLight* m_pDirLight;
};
