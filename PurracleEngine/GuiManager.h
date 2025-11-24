#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class Model;
class Camera;
class FrameBuffer;
class Scene;
class Renderable;
class EditorManager;

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
    void SetEditorManager(EditorManager* em);

    void SelectModel(Model* model);


    int GetViewportWidth() const { return (int)viewportSize.x; }
    int GetViewportHeight() const { return (int)viewportSize.y; }

    void AddModel();
    void DeleteSelectedModel();
    void ChangeMesh(const std::string& meshPath);
    void ChangeTexture(const std::string& texturePath);

    std::vector<Model*>* GetModelList() { return modelList; }
    int FindMeshIndex(const std::string& meshPath);
    int FindTextureIndex(const std::string& meshPath);
    void UpdateModelIndices();

    void RefreshAssets();

private:
    std::vector<Model*>* modelList = nullptr;
    EditorManager* editorManager;
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
    void ApplyGizmosAndTransform();
    void DrawViewport();
    void HandleMouseClick(GLFWwindow* window);
    void UpdateSelectedModelTransform();
    void DrawModelNode(Renderable* model);

    float WrapAngle(float angle);

    std::vector<std::string> availableTextures = {"Assets/Textures/Cube.png","Assets/Textures/PurpleLava.jpg","Assets/Textures/RedLava.png", "Assets/Textures/Purple.png","Assets/Textures/Blue.png","Assets/Textures/Beige.png"};

    std::vector<std::string> availableMeshes = {"Assets/Models/Cube.obj","Assets/Models/CubePrimitive.obj","Assets/Models/CylinderPrimitive.obj"};

    std::vector<const char*> meshNames;
    std::vector<const char*> textureNames;

    void UpdateFileNames();

    std::vector<std::string> meshFilenames;
    std::vector<std::string> textureFilenames;

    int selectedMeshIndex = 0;
    int selectedTextureIndex = 0;
};