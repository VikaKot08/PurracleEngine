#pragma once
#include <string>
#include <glm/glm.hpp>

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
	void SetMatrix4(glm::mat4 aMatrix, const std::string& aName);
	void SetInt(int aInt, const std::string& aName);
};

