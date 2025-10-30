#pragma once
#include <string>

class Shader
{
private:
	std::string LoadShader(const char* aPath);
	unsigned int LoadVertexShader(const char* aPath);
	unsigned int LoadFragmentShader(const char* aPath);
public:
	Shader(const char* aVertexPath, const char* aFragmentPath);
	unsigned int myShaderProgram;
	void Initialize(const char* aVertexPath, const char* aFragmentPath);
	void Use();
	void EndUse();
	~Shader();
};

