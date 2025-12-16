#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Model.h"
#include "Shader.h"
#include "SceneSerializer.h"

#include "EngineContext.h"
#include "ForwardRenderer.h"
#include "GuiManager.h"
#include "FrameBuffer.h"

#include "MeshManager.h"
#include "EditorManager.h"

#include <chrono>
#include <filesystem>
#include <imgui.h>

auto lastTime = std::chrono::high_resolution_clock::now();

bool firstMouse = true;
bool rightMousePressed = false;
double lastX = 0.0;
double lastY = 0.0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            rightMousePressed = true;
            firstMouse = true; 
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else if (action == GLFW_RELEASE)
        {
            rightMousePressed = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (!rightMousePressed)
        return;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }

    float xoffset = static_cast<float>(xpos - lastX);
    float yoffset = static_cast<float>(lastY - ypos); // Reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    EngineContext* ctx = static_cast<EngineContext*>(glfwGetWindowUserPointer(window));
    if (ctx)
    {
        Scene* scene = ctx->GetScene();
        if (scene)
        {
            Camera* camera = scene->GetCamera();
            if (camera == scene ->mainCamera)
            {
                camera->ProcessMouseMovement(xoffset, yoffset);
            }
        }
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    EngineContext* ctx = static_cast<EngineContext*>(glfwGetWindowUserPointer(window));
    if (ctx)
    {
        Scene* scene = ctx->GetScene();
        if (scene)
        {
            Camera* camera = scene->GetCamera();
            if (camera)
            {
                camera->ProcessMouseScroll(static_cast<float>(yoffset));
            }
        }
    }
}

void processInput(GLFWwindow* window, Scene* scene, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    glm::vec4 direction(0.0f);

    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard && io.WantTextInput)
    {
        return;
    }

    //Forward/back
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        direction.z += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        direction.z -= 1.0f;

    // Left/right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        direction.x += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        direction.x -= 1.0f;

    // Up/down
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        direction.y += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        direction.y -= 1.0f;

    if (direction != glm::vec4(0.0f))
        scene->UpdateFlyingCamera(direction, deltaTime);



    int rightState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    if (rightState == GLFW_PRESS && !rightMousePressed)
    {
        rightMousePressed = true;
        firstMouse = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else if (rightState == GLFW_RELEASE && rightMousePressed)
    {
        rightMousePressed = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    if (rightMousePressed)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
            return;
        }

        float xoffset = static_cast<float>(xpos - lastX);
        float yoffset = static_cast<float>(lastY - ypos); // inverted y

        lastX = xpos;
        lastY = ypos;

        if (scene)
        {
            Camera* camera = scene->GetCamera();
            if (camera == scene -> mainCamera)
            {
                camera->ProcessMouseMovement(xoffset, yoffset);
            }
        }
    }

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

    MeshManager::Allocate();
    MeshManager* meshManager = MeshManager::Get();

    EditorManager* editorManager = new EditorManager();
    editorManager->SetMeshManager(meshManager);
    editorManager->SetGuiManager(gui);
    meshManager->SetGuiQueue(gui);

    Scene* scene = new Scene();
    ForwardRenderer* renderer = new ForwardRenderer();
    EngineContext* context = new EngineContext(scene, renderer);

    glfwSetWindowUserPointer(window, context);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    FrameBuffer* frameBuffer = new FrameBuffer(800, 600);
    std::vector<Model*> models;
    scene->SetMeshManager(meshManager);

    editorManager->SetScene(scene);
    editorManager->SetModelList(&models);

    gui->SetModelList(&models);
    gui->SetFrameBuffer(frameBuffer);
    gui->SetEditorManager(editorManager);
    gui->SetScene(scene);

    std::string autoLoadScene = "Assets/Scenes/Default.scene";
    if (std::filesystem::exists(autoLoadScene))
    {
        std::cout << "Auto-loading scene: " << autoLoadScene << std::endl;
        SceneSerializer::LoadScene(scene, autoLoadScene, &models);
    }
    else
    {
        // Initialize with default models if no scene exists
        scene->InitializeDefaultModels();

        // Add the default models to the model list
        for (Renderable* r : scene->GetRenderables())
        {
            Model* model = dynamic_cast<Model*>(r);
            if (model)
            {
                models.push_back(model);
            }
        }
    }

    gui->VerifyAtStart();

    while (!glfwWindowShouldClose(window))
    {
        if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> delta = currentTime - lastTime;
        float deltaTime = delta.count();
        lastTime = currentTime;

        processInput(window, scene, deltaTime);
        glfwPollEvents();

        int viewportWidth = gui->GetViewportWidth();
        int viewportHeight = gui->GetViewportHeight();
        if (viewportWidth > 0 && viewportHeight > 0)
        {
            scene->GetCamera()->SetAspectRatio((float)viewportWidth, (float)viewportHeight);
        }

        // Render to framebuffer
        frameBuffer->Bind();
        glViewport(0, 0, frameBuffer->GetWidth(), frameBuffer->GetHeight());
        glClearColor(0.314f, 0.290f, 0.439f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        context->Draw();

        frameBuffer->Unbind();

        // Render GUI
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

    delete editorManager;
    MeshManager::Deallocate();

    delete gui;
    glfwDestroyWindow(window);
    glfwTerminate();
}