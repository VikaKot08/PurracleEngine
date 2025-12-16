#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "Light.h"

Shader::Shader(const char* aVertexPath, const char* aFragmentPath) 
{
	myFragmentPath = aFragmentPath;
	myVertexPath = aVertexPath;
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

void Shader::Reload()
{
	if (myShaderProgram != 0)
	{
		glDeleteProgram(myShaderProgram);
		myShaderProgram = 0;
	}

	Initialize(myVertexPath.c_str(), myFragmentPath.c_str());
	reloaded = true;
	std::cout << "Shader reloaded: "
		<< myVertexPath << " / "
		<< myFragmentPath << std::endl;
}

void Shader::Use(std::vector<Light*> lights)
{
	glUseProgram(myShaderProgram);

	if(useLight)
	{
		int numLights = 0;
		for (Light* light : lights)
		{
			if (reloaded) 
			{
				SetLight(numLights, *light);
				light->dirty = false;
			}
			else if (light->dirty)
			{
				SetLight(numLights, *light);
				light->dirty = false;
			}
			numLights++;
		}
		reloaded = false;
		glUniform1i(glGetUniformLocation(myShaderProgram, "numLights"), numLights);
	}
}

void Shader::EndUse()
{
	glUseProgram(0);
}

Shader::~Shader() 
{
	glDeleteProgram(myShaderProgram);
}

void Shader::SetMatrix4(glm::mat4 aMatrix, const std::string& aName)
{
	glUniformMatrix4fv(glGetUniformLocation(myShaderProgram, aName.c_str()), 1, GL_FALSE, glm::value_ptr(aMatrix));
}

void Shader::SetInt(int aInt, const std::string& aName)
{
	glUniform1i(glGetUniformLocation(myShaderProgram, aName.c_str()), aInt);
}

void Shader::SetEyePosition(glm::vec3 aPosition)
{
	glUniform3fv(glGetUniformLocation(myShaderProgram, "eyePosition"), 1, &aPosition[0]);
}

void Shader::SetLight(int index, const Light& light) 
{
	std::string base = "lights[" + std::to_string(index) + "].";

	glUniform1i(glGetUniformLocation(myShaderProgram, (base + "type").c_str()), light.typeLight);
	glUniform3fv(glGetUniformLocation(myShaderProgram, (base + "position").c_str()), 1, &light.position[0]);
	glUniform3fv(glGetUniformLocation(myShaderProgram, (base + "direction").c_str()), 1, &light.direction[0]);
	glUniform4fv(glGetUniformLocation(myShaderProgram, (base + "ambient").c_str()), 1, &light.ambient[0]);
	glUniform4fv(glGetUniformLocation(myShaderProgram, (base + "diffuse").c_str()), 1, &light.diffuse[0]);
	glUniform4fv(glGetUniformLocation(myShaderProgram, (base + "specular").c_str()), 1, &light.specular[0]);
	glUniform3fv(glGetUniformLocation(myShaderProgram, (base + "attenuation").c_str()), 1, &light.attenuation[0]);
	glUniform1f(glGetUniformLocation(myShaderProgram, (base + "cutoffAngle").c_str()), light.cutoffAngle);
	glUniform1f(glGetUniformLocation(myShaderProgram, (base + "outerCutoffAngle").c_str()), light.outerCutoffAngle);
	glUniform1i(glGetUniformLocation(myShaderProgram, (base + "enabled").c_str()), light.enabled);
}