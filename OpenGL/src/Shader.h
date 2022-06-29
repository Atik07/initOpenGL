#pragma once
#include <string>
#include <unordered_map>

struct ShaderProgramSource {
	std::string vertexSource;
	std::string fragmentSource;
};

/* this class shall consider that ONLY ONE shader file will be provided */
class Shader
{
private:
	std::string m_FilePath;
	unsigned int m_RendererID;
	//caching for Uniforms
	std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	// Set uniforms
	void SetUniform1i(const std::string& name, int value);
	void SetUnfiorm4f(const std::string& name, float v0, float v1, float v2, float v3);
private:
	ShaderProgramSource ParseShader(const std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	int GetUniformLocation(const std::string& name);
};