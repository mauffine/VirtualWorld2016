#pragma once

#include "Engine\MerffEngine.h"
#include "Engine\Shader.h"
#include "Engine\DirectionalLight.h"

struct Vertex
{
	glm::vec4 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
	glm::vec2 heightMapCoord;
};
#include "Engine\FBXLoader.h"
class TerrainGen
{
public:
	TerrainGen(const unsigned int& a_size, DirectionalLight* a_pDirLight, int a_numOfBoulders);
	~TerrainGen();

	bool Update(double dt);
	void Draw(const BaseCamera& a_camera);

	void GenerateEnvironment();
	int GetPhysicalSize()
	{
		return m_size;
	}
	int* GetHeightData()
	{
		int* tmp = new int[m_size * m_size];
		for (int i = m_size * m_size - 1; i > -1; i--)
		{
			tmp[i] = m_vertexData[i].position.y * 10;
		}
		return tmp;
	}
private:
	struct ModelPoint
	{
		glm::vec4 position;
		glm::vec4 rotation;
	};
	Vertex* m_vertexData;
	unsigned int* m_indicies;

	unsigned int m_size;
	unsigned int m_vao, m_vbo, m_ibo;
	unsigned int m_perlinTexture;

	FBXLoader* m_alphaBoulder;

	int m_numOfBoulders;
	ModelPoint* m_boulderPositions;

	float m_seed;

	void GeneratePlane();
	void GenerateBuffers();
	float* GeneratePerlinNoise(int a_dims, float* a_data);
	void GenerateNormal(Vertex* a_vert1, Vertex* a_vert2, Vertex* a_vert3);

	unsigned int m_diffuseID;
	int m_diffuseHeight, m_diffuseWidth, m_imageType;

	Shader m_shaders;

	DirectionalLight* m_pDirLight;

};
