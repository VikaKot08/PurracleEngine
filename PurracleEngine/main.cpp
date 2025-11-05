#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Triangle.h"
#include "Cube.h"
#include "Model.h"
#include "Shader.h"

#include "EngineContext.h"
#include "ForwardRenderer.h"
#include "GuiManager.h"
#include "FrameBuffer.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Don't set viewport here anymore - it will be set by the framebuffer size
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
    if (!glfwInit())
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

    FrameBuffer* frameBuffer = new FrameBuffer(800, 600);
    std::vector<Model*> models;

    // Create multiple models
    Model* model1 = new Model("Assets/Models/Cube.obj");
    model1->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    model1->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    model1->position = glm::vec3(-3.0f, 0.0f, -6.0f);
    scene->AddRenderable(model1);
    models.push_back(model1);

    Model* model2 = new Model("Assets/Models/Cube.obj");
    model2->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    model2->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    model2->position = glm::vec3(0.0f, 0.0f, -6.0f);
    scene->AddRenderable(model2);
    models.push_back(model2);

    Model* model3 = new Model("Assets/Models/Cube.obj");
    model3->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    model3->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    model3->position = glm::vec3(3.0f, 0.0f, -6.0f);
    scene->AddRenderable(model3);
    models.push_back(model3);

    // Pass model list, camera, and framebuffer to GUI manager
    gui->SetModelList(&models);
    gui->SetCamera(scene->GetCamera());
    gui->SetFrameBuffer(frameBuffer);

    // Select the first model by default
    if (!models.empty())
    {
        gui->SelectModel(models[0]);
    }

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glfwPollEvents();

        int viewportWidth = gui->GetViewportWidth();
        int viewportHeight = gui->GetViewportHeight();
        if (viewportWidth > 0 && viewportHeight > 0)
        {
            scene->GetCamera()->SetAspectRatio((float)viewportWidth, (float)viewportHeight);
        }

        frameBuffer->Bind();
        glViewport(0, 0, frameBuffer->GetWidth(), frameBuffer->GetHeight());
        glClearColor(0.314f, 0.290f, 0.439f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        context->Draw();

        frameBuffer->Unbind();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        gui->Update(window);

        glfwSwapBuffers(window);
    }

    // Cleanup
    for (Model* model : models)
    {
        delete model;
    }

    delete frameBuffer;
    gui->Close();
    delete context;
    delete renderer;
    delete scene;
    delete gui;
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}