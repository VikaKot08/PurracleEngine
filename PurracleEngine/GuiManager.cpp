#include "GuiManager.h"
#include "Model.h"
#include "Camera.h"
#include "FrameBuffer.h"
#include "Scene.h"
#include <stb_image.h>
#include "ImGuizmo.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <filesystem>
#include <memory>
#include <cstring>

namespace fs = std::filesystem;

static ImGuizmo::OPERATION sGizmoOperation = ImGuizmo::TRANSLATE;

void SetPurracleStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.GrabRounding = 6.0f;
    style.ScrollbarRounding = 8.0f;
    style.TabRounding = 6.0f;
    style.ChildRounding = 8.0f;
    style.TreeLinesRounding = 6.0f;
    style.FramePadding = ImVec2(6, 4);
    style.ItemSpacing = ImVec2(8, 6);
    style.WindowPadding = ImVec2(10, 10);
    style.IndentSpacing = 20.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
}

void SetPurracleDarkStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    auto hex = [](int r, int g, int b) { return ImVec4(r / 255.f, g / 255.f, b / 255.f, 1.f); };

    ImVec4 softBeige = hex(240, 209, 190);
    ImVec4 softLilac = hex(80, 74, 112);
    ImVec4 paleCream = hex(246, 234, 227);
    ImVec4 darkLilac = hex(45, 41, 65);
    ImVec4 highlight = hex(255, 230, 210);

    colors[ImGuiCol_WindowBg] = darkLilac;
    colors[ImGuiCol_ChildBg] = hex(55, 50, 78);
    colors[ImGuiCol_PopupBg] = hex(60, 55, 85);
    colors[ImGuiCol_Header] = softLilac;
    colors[ImGuiCol_HeaderHovered] = hex(100, 92, 135);
    colors[ImGuiCol_HeaderActive] = hex(120, 110, 150);
    colors[ImGuiCol_Button] = softLilac;
    colors[ImGuiCol_ButtonHovered] = hex(100, 92, 135);
    colors[ImGuiCol_ButtonActive] = hex(120, 110, 150);
    colors[ImGuiCol_FrameBg] = hex(65, 60, 90);
    colors[ImGuiCol_FrameBgHovered] = hex(80, 75, 110);
    colors[ImGuiCol_FrameBgActive] = hex(100, 90, 130);
    colors[ImGuiCol_Text] = paleCream;
    colors[ImGuiCol_TextDisabled] = hex(160, 150, 180);
    colors[ImGuiCol_TitleBg] = softLilac;
    colors[ImGuiCol_TitleBgActive] = hex(90, 82, 125);
    colors[ImGuiCol_TitleBgCollapsed] = hex(70, 65, 100);
    colors[ImGuiCol_Tab] = softLilac;
    colors[ImGuiCol_TabHovered] = hex(100, 92, 135);
    colors[ImGuiCol_TabActive] = hex(120, 110, 150);
    colors[ImGuiCol_TabUnfocused] = hex(70, 65, 100);
    colors[ImGuiCol_TabUnfocusedActive] = hex(90, 82, 125);
    colors[ImGuiCol_Border] = hex(110, 100, 130);
    colors[ImGuiCol_ScrollbarBg] = darkLilac;
    colors[ImGuiCol_ScrollbarGrab] = softLilac;
    colors[ImGuiCol_ScrollbarGrabHovered] = hex(100, 92, 135);
    colors[ImGuiCol_ScrollbarGrabActive] = hex(120, 110, 150);
    colors[ImGuiCol_CheckMark] = softBeige;
    colors[ImGuiCol_SliderGrab] = softBeige;
    colors[ImGuiCol_SliderGrabActive] = highlight;

    style.FrameRounding = 8.0f;
    style.WindowRounding = 12.0f;
    style.ChildRounding = 10.0f;
    style.PopupRounding = 10.0f;
    style.ScrollbarRounding = 8.0f;
    style.GrabRounding = 6.0f;
    style.WindowPadding = ImVec2(10, 10);
    style.FramePadding = ImVec2(8, 4);
    style.ItemSpacing = ImVec2(8, 6);
    style.ItemInnerSpacing = ImVec2(6, 4);
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
}

float GuiManager::WrapAngle(float angle)
{
    while (angle >= 360.0f) angle -= 360.0f;
    while (angle < 0.0f) angle += 360.0f;
    return angle;
}

void GuiManager::Start(GLFWwindow* aWindow)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsPurracle();
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 1.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    SetPurracleDarkStyle();

    UpdateFileNames();

    ImGui_ImplGlfw_InitForOpenGL(aWindow, true);
    ImGui_ImplOpenGL3_Init();
}

void GuiManager::SetIcon(GLFWwindow* aWindow)
{
    GLFWimage images[1];
    images[0].pixels = stbi_load("Assets/Purracle.png", &images[0].width, &images[0].height, nullptr, 4);
    if (images[0].pixels)
    {
        glfwSetWindowIcon(aWindow, 1, images);
        stbi_image_free(images[0].pixels);
    }
}

void GuiManager::SetModelList(std::vector<Model*>* models)
{
    modelList = models;
}

void GuiManager::SetScene(Scene* scn)
{
    scene = scn;
    camera = scn->GetCamera();
}

void GuiManager::SetFrameBuffer(FrameBuffer* fb)
{
    frameBuffer = fb;
}

void GuiManager::SelectModel(Model* model)
{
    selectedModel = model;
    if (selectedModel)
    {
        positionVec = selectedModel->position;
        rotationVec = selectedModel->rotation;
        rotationVecInput = selectedModel->rotation;
        scaleVec = selectedModel->scale;
    }
}

void GuiManager::UpdateSelectedModelTransform()
{
    if (selectedModel)
    {
        selectedModel->position = positionVec;
        selectedModel->rotation = rotationVec;
        selectedModel->scale = scaleVec;

        if (scene) {
            scene->MarkDirty();
        }
    }
}

void GuiManager::AddModel()
{
    Model* newModel = new Model("Assets/Models/Cube.obj", "Assets/Textures/Cube.png");
    newModel->position = glm::vec3(0.0f, 0.0f, 0.0f);
    newModel->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    newModel->scale = glm::vec3(1.0f, 1.0f, 1.0f);

    modelList->push_back(newModel);

    if (scene)
    {
        scene->AddRenderable(newModel);
        scene->BuildEmbreeScene();
    } else 
    {
        std::cout << "Fail, no scene for gui " << std::endl;
    }

    SelectModel(newModel);
}

void GuiManager::DeleteSelectedModel()
{
    if (!selectedModel || !modelList)
        return;

    auto it = std::find(modelList->begin(), modelList->end(), selectedModel);
    if (it != modelList->end())
    {
        scene->DeleteModel(selectedModel);
        modelList->erase(it);
        selectedModel = nullptr;
    }
}

void GuiManager::ChangeMesh(const std::string& meshPath)
{
    if (!selectedModel)
        return;

    glm::vec3 pos = selectedModel->position;
    glm::vec3 rot = selectedModel->rotation;
    glm::vec3 scl = selectedModel->scale;

    for (auto mesh : selectedModel->meshes)
        delete mesh;
    selectedModel->meshes.clear();

    selectedModel->LoadModel(meshPath);

    selectedModel->position = pos;
    selectedModel->rotation = rot;
    selectedModel->scale = scl;

    if (scene)
    {
        scene->BuildEmbreeScene();
    }
}

void GuiManager::ChangeTexture(const std::string& texturePath)
{
    if (!selectedModel)
        return;

    selectedModel->myTexture = std::make_unique<Texture>(texturePath.c_str());
    
}

void GuiManager::HandleMouseClick(GLFWwindow* window)
{
    if (!m_viewportHovered || !camera || !modelList || !scene)
        return;

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    ImGuiViewport* mainViewport = ImGui::GetMainViewport();
    mouseX -= (viewportPos.x - mainViewport->Pos.x);
    mouseY -= (viewportPos.y - mainViewport->Pos.y);

    if (mouseX < 0 || mouseY < 0 || mouseX > viewportSize.x || mouseY > viewportSize.y)
        return;

    // Calculate ray direction
    float x = (2.0f * mouseX) / viewportSize.x - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / viewportSize.y;

    glm::mat4 projection = camera->GetProjection();
    glm::mat4 view = camera->GetView();

    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);
    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    glm::vec3 rayWorld = glm::vec3(glm::inverse(view) * rayEye);
    rayWorld = glm::normalize(rayWorld);

    glm::vec3 rayOrigin = camera->GetPosition();

    // Use scene's ray tracing
    Model* pickedModel = scene->TraceRay(rayOrigin, rayWorld);
    if (pickedModel) {
        SelectModel(pickedModel);
    }
}

void GuiManager::UpdateFileNames()
{
    meshFilenames.clear();
    meshNames.clear();
    textureFilenames.clear();
    textureNames.clear();

    meshFilenames.reserve(availableMeshes.size());
    meshNames.reserve(availableMeshes.size());
    textureFilenames.reserve(availableTextures.size());

    for (const auto& mesh : availableMeshes)
    {
        if (mesh.empty()) continue;

        std::filesystem::path p(mesh);
        std::string filename = p.stem().string();
        if (filename.empty()) continue;

        meshFilenames.push_back(filename);
        meshNames.push_back(meshFilenames.back().c_str());
    }

    // Textures
    for (const auto& tex : availableTextures)
    {
        if (tex.empty()) continue;

        std::filesystem::path p(tex);
        std::string filename = p.stem().string();
        if (filename.empty()) continue;

        textureFilenames.push_back(filename);
        textureNames.push_back(textureFilenames.back().c_str());
    }
}

void GuiManager::DrawModelNode(Renderable* model)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

    if (selectedModel == model)
        flags |= ImGuiTreeNodeFlags_Selected;

    bool isLeaf = model->children.empty();
    if (isLeaf)
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

    bool opened = ImGui::TreeNodeEx((void*)model, flags, "%s", model->name.c_str());

    if (ImGui::BeginDragDropSource())
    {
        Renderable* payload = model;
        ImGui::SetDragDropPayload("MODEL", &payload, sizeof(Renderable*));
        ImGui::Text("%s", model->name.c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MODEL"))
        {
            Renderable* dropped = *(Renderable**)payload->Data;
            if (dropped != model)  // Don't parent to self
            {
                dropped->SetParent(model);
                if (scene) {
                    scene->MarkDirty();
                }
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (ImGui::IsItemClicked())
    {
        SelectModel(dynamic_cast<Model*>(model));
    }

    // Only push/pop if the node was opened AND it's not a leaf
    if (opened && !isLeaf)
    {
        for (Renderable* child : model->children)
            DrawModelNode(child);

        ImGui::TreePop();
    }
}

void GuiManager::DrawSceneHierarchy()
{
    ImGui::Begin("Scene Hierarchy");

    if (ImGui::Button("Add Model", ImVec2(-1, 0)))
    {
        AddModel();
    }

    ImGui::Separator();

    if (modelList)
    {
        for (Model* model : *modelList)
        {
            if (model->parent == nullptr)
                DrawModelNode(model);
        }
    }

    ImGui::End();
}

void GuiManager::DrawTransformControls()
{
    ImGui::Begin("Transform Controls");

    if (selectedModel)
    {
        ImGui::Text("Model Name");
        char nameBuf[256];
        strncpy_s(nameBuf, sizeof(nameBuf), selectedModel->name.c_str(), _TRUNCATE);

        if (ImGui::InputText("##ModelName", nameBuf, sizeof(nameBuf)))
        {
            selectedModel->name = std::string(nameBuf);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();


        ImGui::Text("Position");
        if (ImGui::InputFloat3("##Position", glm::value_ptr(positionVec)))
        {
            UpdateSelectedModelTransform();
        }

        ImGui::Text("Rotation (degrees)");
        if (ImGui::InputFloat3("##Rotation", glm::value_ptr(rotationVecInput)))
        {
            float maxSafe = 1e6f;
            for (int i = 0; i < 3; i++)
            {
                if (rotationVecInput[i] > maxSafe) rotationVecInput[i] = maxSafe;
                if (rotationVecInput[i] < -maxSafe) rotationVecInput[i] = -maxSafe;
            }

            rotationVec.x = WrapAngle(rotationVecInput.x);
            rotationVec.y = WrapAngle(rotationVecInput.y);
            rotationVec.z = WrapAngle(rotationVecInput.z);

            rotationVecInput.x = WrapAngle(rotationVecInput.x);
            rotationVecInput.y = WrapAngle(rotationVecInput.y);
            rotationVecInput.z = WrapAngle(rotationVecInput.z);

            UpdateSelectedModelTransform();
        }

        ImGui::Text("Scale");
        if (ImGui::InputFloat3("##Scale", glm::value_ptr(scaleVec)))
        {
            UpdateSelectedModelTransform();
        }

        ImGui::Spacing();

        ImGui::SeparatorText("Mesh");

        if (ImGui::Combo("##MeshType", &selectedModel->meshIndex, meshNames.data(), meshNames.size()))
        {
            ChangeMesh(availableMeshes[selectedModel->meshIndex]);
        }


        ImGui::Spacing();

        ImGui::SeparatorText("Texture");
   
        if (ImGui::Combo("##Texture", &selectedModel->textureIndex, textureNames.data(), textureNames.size()))
        {
            ChangeTexture(availableTextures[selectedModel->textureIndex]);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Delete Model", ImVec2(-1, 0)))
        {
            DeleteSelectedModel();
        }

    }
    else
    {
        ImGui::Text("No model selected");
        ImGui::TextWrapped("Click on a model in the viewport or scene hierarchy to select it.");
    }

    ImGui::End();
}

void GuiManager::ApplyGizmosAndTransform()
{
    if (selectedModel && camera)
    {
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);

        glm::mat4 view = camera->GetView();
        glm::mat4 proj = camera->GetProjection();
        glm::mat4 modelMatrix = selectedModel->GetMatrix(); 

        ImGuizmo::MODE gizmoMode = selectedModel->parent ? ImGuizmo::LOCAL : ImGuizmo::WORLD;

        bool wasManipulated = ImGuizmo::Manipulate(
            glm::value_ptr(view),
            glm::value_ptr(proj),
            sGizmoOperation,
            gizmoMode,
            glm::value_ptr(modelMatrix),
            nullptr,
            nullptr
        );

        if (wasManipulated && ImGuizmo::IsUsing())
        {
            selectedModel->SetMatrix(modelMatrix);

            positionVec = selectedModel->position;
            rotationVec = selectedModel->rotation;

            auto normalizeAngle = [](float angle) -> float {
                while (angle >= 360.0f) angle -= 360.0f;
                while (angle < 0.0f) angle += 360.0f;
                return angle;
                };

            rotationVecInput.x = normalizeAngle(selectedModel->rotation.x);
            rotationVecInput.y = normalizeAngle(selectedModel->rotation.y);
            rotationVecInput.z = normalizeAngle(selectedModel->rotation.z);

            scaleVec = selectedModel->scale;

            if (scene) {
                scene->MarkDirty();
            }
        }
    }
}

void GuiManager::DrawViewport()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");

    m_viewportHovered = ImGui::IsWindowHovered();
    viewportPos = ImGui::GetCursorScreenPos();
    viewportSize = ImGui::GetContentRegionAvail();

    if (frameBuffer && viewportSize.x > 0 && viewportSize.y > 0)
    {
        frameBuffer->ValidSize((int)viewportSize.x, (int)viewportSize.y);

        ImGui::Image(
            (void*)(intptr_t)frameBuffer->GetTexture(),
            viewportSize,
            ImVec2(0, 1),
            ImVec2(1, 0)
        );

        ApplyGizmosAndTransform();
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

void GuiManager::Update(GLFWwindow* aWindow)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuizmo::BeginFrame();

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();

    DrawSceneHierarchy();
    DrawTransformControls();
    DrawViewport();

    if (ImGui::IsMouseClicked(0) && m_viewportHovered && !ImGui::IsAnyItemActive())
    {
        HandleMouseClick(aWindow);
    }


    if (ImGui::IsKeyPressed(ImGuiKey_T)) sGizmoOperation = ImGuizmo::TRANSLATE; 


    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void GuiManager::Close()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}