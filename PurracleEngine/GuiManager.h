#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Model;
class Camera;
class FrameBuffer;

class GuiManager
{
public:
    glm::vec3 positionVec = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotationVec = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotationVecInput = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scaleVec = glm::vec3(1.0f, 1.0f, 1.0f);

    Model* selectedModel = nullptr;
    std::vector<Model*>* modelList = nullptr;
    Camera* camera = nullptr;
    FrameBuffer* frameBuffer = nullptr;

    void Start(GLFWwindow* aWindow);
    void Update(GLFWwindow* aWindow);
    void Close();
    void SetIcon(GLFWwindow* aWindow);
    void SetModelList(std::vector<Model*>* models);
    void SetCamera(Camera* cam);
    void SetFrameBuffer(FrameBuffer* fb);
    void SelectModel(Model* model);
    void UpdateSelectedModelTransform();
    void HandleMouseClick(GLFWwindow* window);

    int GetViewportWidth() const { return (int)viewportSize.x; }
    int GetViewportHeight() const { return (int)viewportSize.y; }

private:
    float WrapAngle(float angle);
    void DrawSceneHierarchy();
    void DrawTransformControls();
    void DrawViewport();

    bool m_viewportHovered = false;
    ImVec2 viewportPos;
    ImVec2 viewportSize;
};