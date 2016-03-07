#pragma once

#include <fstream>
#include <iostream>
#include <vector>

#include "gl_core_4_4.h"
#include "glm\glm.hpp"
#include "glm\ext.hpp"
enum ShaderType
{
	VERTEX,
	FRAGMENT,
	GEOMETRY
};
class Shader
{
public:
	Shader();
	~Shader();
	bool AddShader(const std::string &a_file, const ShaderType &a_type);
	void LinkProgram();

	void Bind() const;
	unsigned int GetUniform(const std::string &a_name) const;
	unsigned int GetProgram();
private:
	unsigned int m_program;
	std::string LoadFile(const std::string &a_filepath, bool &a_status);
	bool CheckErrors(const unsigned int &a_shader);
};
