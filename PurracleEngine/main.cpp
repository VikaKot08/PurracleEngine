#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Triangle.h"
#include "Cube.h"
#include "Shader.h"

#include "EngineContext.h"
#include "ForwardRenderer.h"
#include "GuiManager.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height) 
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    GLFWwindow* window;

    if(!glfwInit()) 
    {
        std::cout << "Failed to init GLFW" << std::endl;
        return -1;
    }

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "PurracleEngine", NULL, NULL);

    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        return -1;
    }

    GuiManager* gui = new GuiManager();
    gui->SetIcon(window);

    glfwMakeContextCurrent(window);

    gui->Start(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Scene* scene = new Scene();
    ForwardRenderer* renderer = new ForwardRenderer();
    EngineContext* context = new EngineContext(scene, renderer);
    
    Triangle* triangle = new Triangle();
    Cube* cube = new Cube();
    scene->AddRenderable(cube);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glfwPollEvents();

        gui->Update(window);
        context->Draw();

        glfwSwapBuffers(window);
        
    }

    gui->Close();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}



