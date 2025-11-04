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

    EngineContext* ctx = static_cast<EngineContext*>(glfwGetWindowUserPointer(window));
    if (!ctx) {
        std::cerr << "framebuffer_size_callback: EngineContext is null!" << std::endl;
        return;
    }

    Scene* scene = ctx->GetScene();
    if (!scene) {
        std::cerr << "framebuffer_size_callback: Scene is null!" << std::endl;
        return;
    }

    Camera* camera = scene->GetCamera();
    if (!camera) {
        std::cerr << "framebuffer_size_callback: Camera is null!" << std::endl;
        return;
    }

    camera->SetAspectRatio((float)width, (float)height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

int main()
{
    if(!glfwInit()) 
    {
        std::cout << "Failed to init GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "PurracleEngine", NULL, NULL);

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

    Scene* scene = new Scene();
    ForwardRenderer* renderer = new ForwardRenderer();
    EngineContext* context = new EngineContext(scene, renderer);

    glfwSetWindowUserPointer(window, context);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
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
    delete context;
    delete renderer;
    delete scene;
    delete gui;
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}



