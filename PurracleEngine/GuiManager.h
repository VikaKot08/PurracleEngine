#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Model.h"

class GuiManager
{
public:
	void SetIcon(GLFWwindow* aWindow);
	void Start(GLFWwindow* aWindow);
	void Update(GLFWwindow* aWindow);
	void Close();

	float WrapAngle(float angle);

	Model* selectedModel = nullptr;

	float scale = 1.0f;

	glm::vec3 positionVec{ 0.0f };
	glm::vec3 scaleVec{ 1.0f };
	glm::vec3 rotationVec{ 0.0f };
	glm::vec3 rotationVecInput{ 0.0f };
};

