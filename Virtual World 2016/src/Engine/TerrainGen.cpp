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

	float *perline_Data = GeneratePerlinNoise(m_size, nullptr);

	for (unsigned int row = 0; row < m_size; ++row)
	{
		for (unsigned int column = 0; column < m_size; ++column)
		{	
			//vertex position
			glm::vec4 position((float)column - ((float)m_size / 2.f), perline_Data[column * m_size + row] * 5,
				(float)row - ((float)m_size / 2.f), 1);

			//vertex texture coord
			glm::vec2 texCoord((float)column, (float)row);
			glm::vec2 heightMapCoord = texCoord / m_size;

			//vertex normal
			glm::vec3 normal(0, 1, 0);

			m_vertexData[row * m_size + column].position = position;
			m_vertexData[row * m_size + column].texCoord = texCoord;
			m_vertexData[row * m_size + column].normal = normal;
		}
	}
	for (int i = 0; i < 10; ++i)
	{
		Vertex* currentVert = &m_vertexData[(rand() % m_size) * (rand() % m_size)];
		m_boulders[i] = new FBXLoader("./res/Art Assets/Models/Rock1.fbx",
			"./res/Art Assets/Models/Rock-Texture-Surface.jpg", glm::vec3(currentVert->position.x, currentVert->position.y,
				currentVert->position.z), m_pDirLight->GetDirection());
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
	unsigned int indiciesSize = (m_size - 1) *
		(m_size - 1) * 6;
	for (unsigned int i = 0; i < indiciesSize; i += 3)
	{
		Vertex* vertex1 = &m_vertexData[m_indicies[i + 2]];
		Vertex* vertex2 = &m_vertexData[m_indicies[i + 1]];
		Vertex* vertex3 = &m_vertexData[m_indicies[i]];

		GenerateNormal(vertex1, vertex2, vertex3);
	}

	GenerateBuffers();
	m_shaders.AddShader("./res/Shaders/Env.frag", ShaderType::FRAGMENT);
	m_shaders.AddShader("./res/Shaders/Env.Vert", ShaderType::VERTEX);

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
}
void TerrainGen::GenerateEnvironment()
{
}
void TerrainGen::GenerateNormal(Vertex* a_vert1, Vertex* a_vert2, Vertex* a_vert3)
{
	glm::vec3 d1(a_vert3->position - a_vert1->position);
	glm::vec3 d2(a_vert2->position - a_vert1->position);

	glm::vec3 crossProduct = glm::cross(d1, d2);

	glm::vec3 normal = glm::normalize(crossProduct);

	a_vert1->normal = normal;
	a_vert2->normal = normal;
	a_vert3->normal = normal;
}
float* TerrainGen::GeneratePerlinNoise(int a_dims, float* a_data)
{
	int dims = a_dims;
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
	return perlin_data;
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
	for (int i = 0; i < 10; ++i)
	{
		m_boulders[i]->Update(dt);
	}
	return true;
}
void TerrainGen::Draw(BaseCamera& a_camera)
{
	for (int i = 0; i < 10; ++i)
	{
		m_boulders[i]->Draw(&a_camera);
	}
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