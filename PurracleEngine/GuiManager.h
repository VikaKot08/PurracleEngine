#pragma once
#include <GLFW/glfw3.h>

class GuiManager
{
public:
	void SetIcon(GLFWwindow* aWindow);
	void Start(GLFWwindow* aWindow);
	void Update(GLFWwindow* aWindow);
	void Close();
};

