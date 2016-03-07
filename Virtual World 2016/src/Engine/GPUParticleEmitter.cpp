#include "Engine\GPUParticleEmitter.h"


GPUParticleEmitter::GPUParticleEmitter() : m_GPUParticles(nullptr), m_maxParticles(0),
m_position(-50, 20, -50),
m_drawShader(0),
m_updateShader(0),
m_lastDrawTime(0) {
	m_vao[0] = 0;
	m_vao[1] = 0;
	m_vbo[0] = 0;
	m_vbo[1] = 0;
}

GPUParticleEmitter::~GPUParticleEmitter()
{
	delete[] m_GPUParticles;

	glDeleteVertexArrays(2, m_vao);
	glDeleteBuffers(2, m_vbo);

	delete(m_drawShader);
	delete(m_updateShader);
}
void GPUParticleEmitter::Initialize(unsigned int a_maxParticles, float a_lifespanMin, float a_lifespanMax,
	float a_velocityMin, float a_velocityMax, float a_startSize, float a_endSize,
	const glm::vec4 &a_startColour, const glm::vec4 &a_endColour, char* a_vertShaderFile)
{
	//store all the variables that have just been passed in
	m_startColour = a_startColour;
	m_endColour = a_endColour;
	m_startSize = a_startSize;
	m_endSize = a_endSize;
	m_velocityMin = a_velocityMin;
	m_velocityMax = a_velocityMax;
	m_lifespanMin = a_lifespanMin;
	m_lifespanMax = a_lifespanMax;
	m_maxParticles = a_maxParticles;
	m_vertShaderType = a_vertShaderFile;
	//create the particle array
	m_GPUParticles = new GPUParticle[a_maxParticles];

	//set our starting ping-pong buffer
	m_activeBuffer = 0;
	m_drawShader = new Shader();
	m_drawShader->AddShader("./res/Shaders/GPUParticle.frag", FRAGMENT);
	m_drawShader->AddShader(a_vertShaderFile, VERTEX);
	m_drawShader->AddShader("./res/Shaders/GPUParticle.geom", GEOMETRY);
	m_drawShader->LinkProgram();

	m_drawShader->Bind();

	// bind size information for interpolation that won’t change
	glUniform1f(m_drawShader->GetUniform("sizeStart"), m_startSize);
	glUniform1f(m_drawShader->GetUniform("sizeEnd"), m_endSize);

	// bind colour information for interpolation that wont change
	glUniform4fv(m_drawShader->GetUniform("colourStart"), 1, &m_startColour[0]);
	glUniform4fv(m_drawShader->GetUniform("colourEnd"), 1, &m_endColour[0]);

	m_updateShader = new Shader();
	m_updateShader->AddShader(a_vertShaderFile, VERTEX);
	const char* varyings[] = { "position", "velocity", "lifetime", "lifespan" };
	glTransformFeedbackVaryings(m_updateShader->GetProgram(), 4, varyings, GL_INTERLEAVED_ATTRIBS);
	m_updateShader->LinkProgram();

	//binf the shader so that we can set some uniforms that don't change per frame
	m_updateShader->Bind();

	//bind lifetime minimum and maximum
	glUniform1f(m_updateShader->GetUniform("lifeMin"), m_lifespanMin);

	glUniform1f(m_updateShader->GetUniform("lifeMax"), m_lifespanMax);

	CreateBuffers();
}
void GPUParticleEmitter::Draw(float a_time, const glm::mat4 & a_cameraTransform, 
	const glm::mat4 & a_projectionView)
{
	//update particles using transform feedback
	m_updateShader->Bind();
	//bind time information
	glUniform1f(m_updateShader->GetUniform("time"), a_time);
	
	float deltaTime = a_time - m_lastDrawTime;
	m_lastDrawTime = a_time;

	glUniform1f(m_updateShader->GetUniform("deltaTime"), deltaTime);

	//bind the emitter's position
	glUniform3fv(m_updateShader->GetUniform("emitterPosition"), 1, &m_position[0]);

	//disable rasterization
	glEnable(GL_RASTERIZER_DISCARD);

	//bind the buffer to update
	glBindVertexArray(m_vao[m_activeBuffer]);

	//work out the "other" buffer
	unsigned int otherBuffer = (m_activeBuffer + 1) % 2;

	// bind the buffer we will update into as points
	// and begin transform feedback
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0,
		m_vbo[otherBuffer]);
	glBeginTransformFeedback(GL_POINTS);

	glDrawArrays(GL_POINTS, 0, m_maxParticles);

	// disable transform feedback and enable rasterization again
	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);

	// draw the particles using the Geometry Shader to billboard them
	m_drawShader->Bind();

	glUniformMatrix4fv(m_drawShader->GetUniform("projectionView"), 1, false, &a_projectionView[0][0]);

	glUniformMatrix4fv(m_drawShader->GetUniform("cameraTransform"), 1, false, &a_cameraTransform[0][0]);

	// draw particles in the "other" buffer
	glBindVertexArray(m_vao[otherBuffer]);
	glDrawArrays(GL_POINTS, 0, m_maxParticles);

	// swap for next frame
	m_activeBuffer = otherBuffer;
}
void GPUParticleEmitter::CreateBuffers()
{
	//Create OpenGL buffers
	glGenVertexArrays(2, m_vao);
	glGenBuffers(2, m_vbo);

	//setup the first buffer
	glBindVertexArray(m_vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles *
		sizeof(GPUParticle), m_GPUParticles, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0);//position
	glEnableVertexAttribArray(1); // velocity
	glEnableVertexAttribArray(2); // lifetime
	glEnableVertexAttribArray(3); // lifespan
								  //glEnableVertexAttribArray(4); //texture
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(GPUParticle), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		sizeof(GPUParticle), ((char*)0) + 12);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE,
		sizeof(GPUParticle), ((char*)0) + 24);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE,
		sizeof(GPUParticle), ((char*)0) + 28);

	// setup the second buffer
	glBindVertexArray(m_vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles *
		sizeof(GPUParticle), 0, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // velocity
	glEnableVertexAttribArray(2); // lifetime
	glEnableVertexAttribArray(3); // lifespan
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(GPUParticle), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		sizeof(GPUParticle), ((char*)0) + 12);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE,
		sizeof(GPUParticle), ((char*)0) + 24);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE,
		sizeof(GPUParticle), ((char*)0) + 28);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
