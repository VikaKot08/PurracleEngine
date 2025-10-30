#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>

Shader::Shader(const char* aVertexPath, const char* aFragmentPath) 
{
	Initialize(aVertexPath, aFragmentPath);
}

std::string Shader::LoadShader(const char* aPath)
{
	std::string shaderCode;
	std::ifstream shaderFile;

	shaderFile.open(aPath);

	if (!shaderFile.is_open())
	{
		std::cerr << "Failed to open shader file: " << aPath << std::endl;
		return "";
	}

	std::stringstream shaderStream;
	shaderStream << shaderFile.rdbuf();

	shaderFile.close();
	shaderCode = shaderStream.str();

	return shaderCode;
}

unsigned int Shader::LoadVertexShader(const char* aPath)
{
	int result;
	char Log[512];

	std::string shaderCodeString = LoadShader(aPath);
	const char* shaderCode = shaderCodeString.c_str();

	unsigned int shaderObject;

	shaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shaderObject, 1, &shaderCode, NULL);
	glCompileShader(shaderObject);

	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);

	if (!result) 
	{
		glGetShaderInfoLog(shaderObject, 512, NULL, Log);
		std::cout << "Failed to compile vertex shader \n" << Log << std::endl;
	}

	return shaderObject;

}

unsigned int Shader::LoadFragmentShader(const char* aPath)
{
	int result;
	char Log[512];

	std::string shaderCodeString = LoadShader(aPath);
	const char* shaderCode = shaderCodeString.c_str();

	unsigned int shaderObject;

	shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shaderObject, 1, &shaderCode, NULL);
	glCompileShader(shaderObject);

	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);

	if (!result)
	{
		glGetShaderInfoLog(shaderObject, 512, NULL, Log);
		std::cout << "Failed to compile fragment shader \n" << Log << std::endl;
	}

	return shaderObject;
}

void Shader::Initialize(const char* aVertexPath, const char* aFragmentPath)
{
	int result;
	char Log[512];

	unsigned int VertexShader = LoadVertexShader(aVertexPath);
	unsigned int FragmentShader = LoadFragmentShader(aFragmentPath);

	myShaderProgram = glCreateProgram();
	glAttachShader(myShaderProgram, VertexShader);
	glAttachShader(myShaderProgram, FragmentShader);
	glLinkProgram(myShaderProgram);

	glGetProgramiv(myShaderProgram, GL_LINK_STATUS, &result);
	if (!result) 
	{
		glGetProgramInfoLog(myShaderProgram, 512, NULL, Log);
		std::cout << "Shader program could not compile \n" << Log << std::endl;
	}

	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);
}

void Shader::Use()
{
	glUseProgram(myShaderProgram);
}

void Shader::EndUse()
{
	glUseProgram(0);
}

Shader::~Shader() 
{
	glDeleteProgram(myShaderProgram);
}