#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class Model;
class Camera;
class FrameBuffer;
class Scene;

class GuiManager
{
public:
    void Start(GLFWwindow* aWindow);
    void Update(GLFWwindow* aWindow);
    void Close();
    void SetIcon(GLFWwindow* aWindow);

    void SetModelList(std::vector<Model*>* models);
    void SetScene(Scene* scn);
    void SetFrameBuffer(FrameBuffer* fb);

    void SelectModel(Model* model);


    int GetViewportWidth() const { return (int)viewportSize.x; }
    int GetViewportHeight() const { return (int)viewportSize.y; }

private:
    std::vector<Model*>* modelList = nullptr;
    Scene* scene = nullptr;
    Camera* camera = nullptr;
    FrameBuffer* frameBuffer = nullptr;
    Model* selectedModel = nullptr;

    glm::vec3 positionVec = glm::vec3(0.0f);
    glm::vec3 rotationVec = glm::vec3(0.0f);
    glm::vec3 rotationVecInput = glm::vec3(0.0f);
    glm::vec3 scaleVec = glm::vec3(1.0f);

    bool m_viewportHovered = false;
    ImVec2 viewportPos;
    ImVec2 viewportSize;

    void DrawSceneHierarchy();
    void DrawTransformControls();
    void DrawViewport();
    void HandleMouseClick(GLFWwindow* window);
    void UpdateSelectedModelTransform();

    float WrapAngle(float angle);
};