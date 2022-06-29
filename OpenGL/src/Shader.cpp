#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "ErrorHandling.h"


Shader::Shader(const std::string& filepath)
	: m_FilePath(filepath), m_RendererID(0)
{
	ShaderProgramSource shaderSource = ParseShader(filepath);
	m_RendererID = CreateShader(shaderSource.vertexSource, shaderSource.fragmentSource);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));
}


ShaderProgramSource Shader::ParseShader(const std::string& filepath) {
	std::ifstream stream(filepath);
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos) {
				//set mode to vertex
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos) {
				//set mode to fragment
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << "\n";
		}
	}
	return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	GLCall(unsigned int shader_id = glCreateShader(type));
	const char* src = source.c_str(); //TODO: handle if source is not provided 

	GLCall(glShaderSource(shader_id, 1, &src, NULL));
	GLCall(glCompileShader(shader_id));

	//error handling in shader compilation
	int result;
	GLCall(glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result));

	if (result == GL_FALSE) {
		int length;
		GLCall(glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length));
		char* err_message = (char*)_malloca(length * sizeof(char));
		GLCall(glGetShaderInfoLog(shader_id, length, &length, err_message));
		std::cout << "Failed to compile " << type << " shader!" << std::endl;
		std::cout << err_message << std::endl;
		GLCall(glDeleteShader(shader_id));
		return 0;
	}

	return shader_id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	GLCall(unsigned int program = glCreateProgram());
	GLCall(unsigned int vshader = CompileShader(GL_VERTEX_SHADER, vertexShader));
	GLCall(unsigned int fshader = CompileShader(GL_FRAGMENT_SHADER, fragmentShader));

	GLCall(glAttachShader(program, vshader));
	GLCall(glAttachShader(program, fshader));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	// once the program is linked the intermediates can be deleted ... this deletes the .obj files
	GLCall(glDeleteShader(vshader));
	GLCall(glDeleteShader(fshader));

	return program;
}

void Shader::SetUniform1i(const std::string& name, int value)
{
	GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUnfiorm4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
	{
		return m_UniformLocationCache[name];
	}
	
	GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
	if (location == -1) 
	{
		std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
	}
	m_UniformLocationCache[name] = location;

	return location;
}


void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}