#include "Engine\TerrainGen.h"

TerrainGen::TerrainGen(const unsigned int& a_size, DirectionalLight* a_pDirLight)
{
	m_size = a_size;
	m_pDirLight = a_pDirLight;
	GeneratePlane();
}

TerrainGen::~TerrainGen()
{
}

void TerrainGen::GeneratePlane()
{
	//set up the Vertex Data and the Index Array for the plane that's used as a height map Height map
	m_vertexData = new Vertex[m_size * m_size];
	m_indicies = new unsigned int[(m_size - 1) * (m_size - 1) * 6];

	for (unsigned int row = 0; row < m_size; ++row)
	{
		for (unsigned int column = 0; column < m_size; ++column)
		{	
			//vertex position
			glm::vec4 position((float)column - ((float)m_size / 2.f), 0,
				(float)row - ((float)m_size / 2.f), 1);

			//vertex texture coord
			glm::vec2 texCoord((float)column, (float)row);
			glm::vec2 heightMapCoord = texCoord / m_size;

			//vertex normal
			glm::vec3 normal(0, 1, 0);

			m_vertexData[row * m_size + column].position = position;
			m_vertexData[row * m_size + column].texCoord = texCoord;
			m_vertexData[row * m_size + column].normal = normal;
			m_vertexData[row * m_size + column].heightMapCoord = heightMapCoord;
		}
	}
	//builds the plane a quad at a time
	unsigned int index = 0;

	for (unsigned int row = 0; row < (m_size - 1); ++row)
	{
		for (unsigned int column = 0; column < (m_size - 1); ++column)
		{
			//first triangle
			int currentVert = row * m_size + column;
			m_indicies[index++] = currentVert;
			m_indicies[index++] = currentVert + m_size;
			m_indicies[index++] = currentVert + m_size + 1;

			//second triangle
			m_indicies[index++] = currentVert;
			m_indicies[index++] = currentVert + m_size + 1;
			m_indicies[index++] = currentVert + 1;
		}
	}
	GenerateBuffers();
	m_shaders.AddShader("./res/Shaders/EnvfShader.txt", ShaderType::FRAGMENT);
	m_shaders.AddShader("./res/Shaders/EnvvShader.txt", ShaderType::VERTEX);

	m_shaders.LinkProgram();

	//Load Texture and bind to an ID
	unsigned char* m_diffuseTex = stbi_load("./res/Art Assets/Textures/grass.jpg",
		&m_diffuseWidth, &m_diffuseHeight, &m_imageType, STBI_default);

	glGenTextures(1, &m_diffuseID);
	glBindTexture(GL_TEXTURE_2D, m_diffuseID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_diffuseWidth, m_diffuseHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, m_diffuseTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(m_diffuseTex);
	GeneratePerlinNoise(100, nullptr);
}
void TerrainGen::GenerateEnvironment()
{
}
void TerrainGen::GeneratePerlinNoise(int a_dims, float* a_data)
{
	int dims = 64;
	float *perlin_data = new float[a_dims* a_dims];
	float scale = (1.f / a_dims) * 3;
	int octaves = 6;

	for (int x = 0; x < a_dims; ++x)
	{
		for (int y = 0; y < a_dims; ++y)
		{
			float amplitude = 2.f;
			float persistence = 0.3f;
			perlin_data[y * a_dims + x] = 0;

			for (int o = 0; o < octaves; ++o)
			{
				float freq = powf(2, (float)o);
				float perlin_sample = 
					glm::perlin(glm::vec2((float)x, (float)y) * scale * freq) * 0.5f + 0.5f;
				perlin_data[y * a_dims + x] += perlin_sample * amplitude;
				amplitude *= persistence;
			}
		}
	}
	glGenTextures(1, &m_perlinTexture);
	glBindTexture(GL_TEXTURE_2D, m_perlinTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, a_dims, a_dims, 0, GL_RED, GL_FLOAT, perlin_data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

}

void TerrainGen::GenerateBuffers()
{
	//bind the vertex array
	glBindVertexArray(m_vao);

	//populate the VBO
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, (m_size * m_size) * sizeof(Vertex),
		m_vertexData, GL_DYNAMIC_DRAW);

	// Set Attribute Location 0 (position)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	// Set Attrib Location 1 (normals)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)(sizeof(glm::vec4)));

	// Set Attrib Location 2 (texCood)
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)(sizeof(glm::vec4) + sizeof(glm::vec3)));

	// Set Attrib Location 3 (heightCoord)
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)(sizeof(glm::vec4) + sizeof(glm::vec3) + sizeof(glm::vec2)));

	// Populate the Index Buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		(m_size - 1) * (m_size - 1) * 6 * sizeof(unsigned int), m_indicies, GL_STATIC_DRAW);

	//Clean
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}
bool TerrainGen::Update(double dt)
{
	return true;
}
void TerrainGen::Draw(const BaseCamera& a_camera)
{
	m_shaders.Bind();

	// Pass through projection view matrix to shader
	glUniformMatrix4fv(m_shaders.GetUniform("projectionView"),
		1, GL_FALSE, &a_camera.GetProjectionView()[0][0]);

	// Update normal matrix
	glm::mat3 normalMatrix = glm::inverseTranspose(
		glm::mat3(a_camera.GetView()));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_diffuseID);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_perlinTexture);

	glUniformMatrix3fv(m_shaders.GetUniform("normalMat"), 1, GL_FALSE, &normalMatrix[0][0]);

	// Set material
	glUniform4fv(m_shaders.GetUniform("material.ambient"), 1, &glm::vec4(1.f, 1.f, 1.f, 1.f)[0]);

	glUniform4fv(m_shaders.GetUniform("material.diffuse"), 1, &glm::vec4(1.f, 1.f, 1.f, 1.f)[0]);

	glUniform4fv(m_shaders.GetUniform("material.specular"), 1, &glm::vec4(1.f, 1.f, 1.f, 1.f)[0]);

	glUniform1i(m_shaders.GetUniform("material.diffuseTex"), 0);

	glUniform1i(m_shaders.GetUniform("perlin_texture"), 1);
	// Pass through Directional Light properties
	glm::vec3 lightDir = -m_pDirLight->GetDirection();
	glUniform3fv(m_shaders.GetUniform("dirLight.direction"), 1, &lightDir[0]);

	glUniform3fv(m_shaders.GetUniform("dirLight.ambient"), 1, &m_pDirLight->GetColour()[0]);

	glUniform3fv(m_shaders.GetUniform("dirLight.diffuse"), 1, &m_pDirLight->GetColour()[0]);

	glUniform1f(m_shaders.GetUniform("dirLight.ambientIntensity"), m_pDirLight->GetAmbientIntensity());

	glUniform1f(m_shaders.GetUniform("dirLight.diffuseIntensity"), m_pDirLight->GetDiffuseIntensity());

	glUniform1f(m_shaders.GetUniform("dirLight.specularIntensity"), m_pDirLight->GetSpecularIntensity());

	// Pass through camera position to shader for specular highlighting
	glUniform3fv(m_shaders.GetUniform("cameraPos"), 1, &a_camera.GetPosition()[0]);

	// Draw terrain
	glBindVertexArray(m_vao);
	unsigned int indexCount = (m_size - 1) * (m_size - 1) * 6;
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, m_indicies);
}