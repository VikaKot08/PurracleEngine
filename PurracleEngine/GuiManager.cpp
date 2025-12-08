#include "GuiManager.h"
#include "Model.h"
#include "Camera.h"
#include "FrameBuffer.h"
#include "EditorManager.h"
#include "MemoryMessage.h"
#include "SceneSerializer.h"
#include "SceneOperationCompleteMessage.h"
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
    UpdateFileNames();
    RefreshAssets();
    RefreshSceneList();
    RefreshSceneList();
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

void GuiManager::SetEditorManager(EditorManager* em)
{
    editorManager = em;
}

void GuiManager::SetFrameBuffer(FrameBuffer* fb)
{
    frameBuffer = fb;
}

void GuiManager::UpdateSelectedModelTransform()
{
    if (selectedModel)
    {
        selectedModel->position = positionVec;
        selectedModel->SetRotationFromEuler(rotationVec);
        selectedModel->scale = scaleVec;

        if (scene) {
            scene->MarkDirty();
        }
    }
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

void GuiManager::AddModel()
{
    Model* newModel = scene->LoadModel("Assets/Models/Cube.obj", "Assets/Textures/Cube.png");
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

    editorManager->RequestLoadMesh(selectedModel, meshPath);
}

void GuiManager::ChangeTexture(const std::string& texturePath)
{
    if (!selectedModel)
        return;

    editorManager->ChangeTexture(selectedModel, texturePath);
    
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

    glm::mat4 projection = scene->GetCamera()->GetProjection();
    glm::mat4 view = scene->GetCamera()->GetView();

    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);
    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    glm::vec3 rayWorld = glm::vec3(glm::inverse(view) * rayEye);
    rayWorld = glm::normalize(rayWorld);

    glm::vec3 rayOrigin = scene->GetCamera()->GetPosition();

    // Use scene's ray tracing
    Model* pickedModel = scene->TraceRay(rayOrigin, rayWorld);
    if (pickedModel) {
        SelectModel(pickedModel);
    }
}

void GuiManager::RefreshAssets()
{
    availableMeshes.clear();
    availableTextures.clear();

    const std::string meshDir = "Assets/Models";
    const std::string textureDir = "Assets/Textures";

    const std::vector<std::string> meshExtensions = {
        ".obj", ".fbx", ".gltf", ".glb", ".dae", ".blend", ".3ds", ".ply"
    };

    const std::vector<std::string> textureExtensions = {
        ".png", ".jpg", ".jpeg", ".bmp", ".tga", ".dds", ".hdr"
    };
    if (std::filesystem::exists(meshDir) && std::filesystem::is_directory(meshDir))
    {
        for (const auto& entry : std::filesystem::directory_iterator(meshDir))
        {
            if (entry.is_regular_file())
            {
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                if (std::find(meshExtensions.begin(), meshExtensions.end(), ext) != meshExtensions.end())
                {
                    availableMeshes.push_back(entry.path().string());
                }
            }
        }
    }
    if (std::filesystem::exists(textureDir) && std::filesystem::is_directory(textureDir))
    {
        for (const auto& entry : std::filesystem::directory_iterator(textureDir))
        {
            if (entry.is_regular_file())
            {
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                if (std::find(textureExtensions.begin(), textureExtensions.end(), ext) != textureExtensions.end())
                {
                    availableTextures.push_back(entry.path().string());
                }
            }
        }
    }

    std::sort(availableMeshes.begin(), availableMeshes.end());
    std::sort(availableTextures.begin(), availableTextures.end());

    UpdateFileNames();
    UpdateModelIndices();
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

int GuiManager::FindMeshIndex(const std::string& meshPath)
{
    std::string normalizedSearch = meshPath;
    std::replace(normalizedSearch.begin(), normalizedSearch.end(), '\\', '/');

    for (size_t i = 0; i < availableMeshes.size(); i++)
    {
        std::string normalizedAvailable = availableMeshes[i];
        std::replace(normalizedAvailable.begin(), normalizedAvailable.end(), '\\', '/');

        if (normalizedAvailable == normalizedSearch)
            return static_cast<int>(i);
    }
    return 0;
}

int GuiManager::FindTextureIndex(const std::string& texturePath)
{
    std::string normalizedSearch = texturePath;
    std::replace(normalizedSearch.begin(), normalizedSearch.end(), '\\', '/');

    for (size_t i = 0; i < availableTextures.size(); i++)
    {
        std::string normalizedAvailable = availableTextures[i];
        std::replace(normalizedAvailable.begin(), normalizedAvailable.end(), '\\', '/');

        if (normalizedAvailable == normalizedSearch)
            return static_cast<int>(i);
    }
    return 0;
}

void GuiManager::UpdateModelIndices()
{
    if (!modelList) return;

    for (Model* model : *modelList)
    {
        if (!model) continue;

        model->meshIndex = FindMeshIndex(model->path);

        model->textureIndex = FindTextureIndex(model->myTexture ->path);
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

    if (ImGui::Button("Refresh Assets", ImVec2(-1, 0)))
    {
        RefreshAssets();
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
    ImGui::Begin("Controls");

    if (selectedModel)
    {
        if(selectedModel -> type == ModelType::Normal)
        {
            ImGui::Text("Model Name");
            char nameBuf[256];
            strncpy_s(nameBuf, sizeof(nameBuf), selectedModel->name.c_str(), _TRUNCATE);

            if (ImGui::InputText("##ModelName", nameBuf, sizeof(nameBuf)))
            {
                if (!std::string(nameBuf).empty())
                {
                    selectedModel->name = std::string(nameBuf);
                }
            }


            if (ImGui::Button("Convert to camera", ImVec2(-1, 0)))
            {
                Camera* newCamera = new Camera();
                newCamera->SetPosition(selectedModel->position);
                newCamera->rotation = selectedModel->rotation;
                newCamera->scale = glm::vec3{ 1.0f };

                newCamera->SyncRotationToYawPitch();

                newCamera->name = selectedModel->name + " (Camera)";

                DeleteSelectedModel();
                SelectModel(newCamera);
                ChangeMesh("Assets/Models/Camera.obj");
                ChangeTexture("Assets/Textures/Camera.jpg");

                modelList->push_back(newCamera);
                scene->AddRenderable(newCamera);
                scene->BuildEmbreeScene();
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

            if (ImGui::Button("Save Optimized Mesh", ImVec2(-1, 0)))
            {
                SaveOptimizedMesh(selectedModel);
            }

            ImGui::Separator();

            if (ImGui::Button("Delete Model", ImVec2(-1, 0)))
            {
                DeleteSelectedModel();
            }
        } 
        else if (selectedModel->type == ModelType::CameraModel)
        {
            Camera* selectedCamera = dynamic_cast<Camera*>(selectedModel);
            ImGui::Text("Camera Name");
            char nameBuf[256];
            strncpy_s(nameBuf, sizeof(nameBuf), selectedModel->name.c_str(), _TRUNCATE);

            if (ImGui::InputText("##CameraName", nameBuf, sizeof(nameBuf)))
            {
                if (!std::string(nameBuf).empty())
                {
                    selectedModel->name = std::string(nameBuf);
                }
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            positionVec = selectedCamera->position;

            if (selectedCamera == scene->mainCamera)
            { 
                selectedCamera->SyncYawPitchToRotation();
            }
            rotationVecInput = selectedCamera->rotation;

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
                selectedCamera->SyncRotationToYawPitch();
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (selectedCamera)
            {
                ImGui::SeparatorText("Camera Settings");

                if (selectedCamera == scene->mainCamera)
                {
                    ImGui::Text("Flying speed");
                    float speed = scene->flyingCamera->GetSpeed();
                    if (ImGui::SliderFloat("##FS", &speed, 1.0f, 20.0f, "%.1f"))
                    {
                        scene->flyingCamera->SetSpeed(speed);
                    }
                }

                ImGui::Text("Field of View");
                float fov = selectedCamera->fov;
                if (ImGui::SliderFloat("##FOV", &fov, 1.0f, 120.0f, "%.1f degrees"))
                {
                    selectedCamera->fov = fov;
                }

                ImGui::Text("Near Plane");
                float nearPlane = selectedCamera->near;
                if (ImGui::InputFloat("##Near", &nearPlane, 0.01f, 0.1f, "%.3f"))
                {
                    if (nearPlane > 0.001f && nearPlane < selectedCamera->far)
                    {
                        selectedCamera->near = nearPlane;
                    }
                }

                ImGui::Text("Far Plane");
                float farPlane = selectedCamera->far;
                if (ImGui::InputFloat("##Far", &farPlane, 1.0f, 10.0f, "%.1f"))
                {
                    if (farPlane > selectedCamera->near)
                    {
                        selectedCamera->far = farPlane;
                    }
                }
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
            }

            if(!selectedModel->isActive)
            {
                if (ImGui::Button("Switch to camera", ImVec2(-1, 0)))
                {
                    Camera* selectedCamera = dynamic_cast<Camera*>(selectedModel);
                    scene->SetCamera(selectedCamera);
                }

                ImGui::Separator();

                if (selectedModel != scene -> mainCamera)
                {
                    if (ImGui::Button("Delete Model", ImVec2(-1, 0)))
                    {
                        DeleteSelectedModel();
                    }
                }
            }
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
    if (selectedModel && scene->GetCamera())
    {
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);

        glm::mat4 view = scene->GetCamera()->GetView();
        glm::mat4 proj = scene->GetCamera()->GetProjection();
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
            if(selectedModel -> type == ModelType::CameraModel) 
            {
                Camera* selectedCamera = dynamic_cast<Camera*>(selectedModel);
                selectedCamera->SyncRotationToYawPitch();
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
    editorManager->Update();
    ProcessMessages();

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
    DrawSceneManager();

    if (ImGui::IsMouseClicked(0) && m_viewportHovered && !ImGui::IsAnyItemActive())
    {
        HandleMouseClick(aWindow);
    }


    if (ImGui::IsKeyPressed(ImGuiKey_T)) sGizmoOperation = ImGuizmo::TRANSLATE; 
    if (ImGui::IsKeyPressed(ImGuiKey_R)) sGizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(ImGuiKey_Y)) sGizmoOperation = ImGuizmo::SCALE;


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

void GuiManager::ProcessMessage(Message* aMessage)
{
    if (aMessage->type == MessageType::SceneOperationComplete)
    {
        SceneOperationCompleteMessage* completeMsg = static_cast<SceneOperationCompleteMessage*>(aMessage);

        std::string title = completeMsg->operation == "save" ? "Save Scene" : "Load Scene";

        if (completeMsg->success)
        {
            ShowPopup(title, completeMsg->message);

            if (completeMsg->operation == "load")
            {
                // Refresh the GUI after loading
                if (modelList && !modelList->empty())
                {
                    SelectModel(modelList->at(0));
                }
                else
                {
                    selectedModel = nullptr;
                }
            }
        }
        else
        {
            ShowPopup(title + " Error", completeMsg->message);
        }
    } else if (aMessage->type == MessageType::Memory)
    {
        MemoryMessage* memMsg = static_cast<MemoryMessage*>(aMessage);

        std::cout << "GuiManager: Received Memory message: " << memMsg->msg
            << " (Request ID: " << memMsg->requestId << ")" << std::endl;

        ShowPopup("Memory Status", memMsg->msg);
    }
    else
    {
        MessageQueue::ProcessMessage(aMessage);
    }
}

void GuiManager::ShowPopup(const std::string& title, const std::string& message)
{
    popupTitle = title;
    popupMessage = message;
    showPopup = true;
}

void GuiManager::DrawPopup()
{
    if (showPopup)
    {
        ImGui::OpenPopup(popupTitle.c_str());
        showPopup = false; // Only open once
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal(popupTitle.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextWrapped("%s", popupMessage.c_str());
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void GuiManager::SaveOptimizedMesh(Model* aModel)
{
    editorManager->SaveOptimizedMesh(aModel);
}

void GuiManager::RefreshSceneList()
{
    availableScenes = SceneSerializer::GetAvailableScenes();
}

void GuiManager::SaveCurrentScene(const std::string& sceneName)
{
    if (sceneName.empty())
    {
        ShowPopup("Save Error", "Scene name cannot be empty!");
        return;
    }

    std::string filepath = SceneSerializer::GetSceneDirectory() + "/" + sceneName;
    if (filepath.find(".scene") == std::string::npos)
    {
        filepath += ".scene";
    }

    if (editorManager)
    {
        editorManager->RequestSaveScene(filepath);
    }
}

void GuiManager::LoadSelectedScene(const std::string& sceneName)
{
    if (sceneName.empty())
    {
        ShowPopup("Load Error", "No scene selected!");
        return;
    }

    std::string filepath = SceneSerializer::GetSceneDirectory() + "/" + sceneName;

    if (editorManager)
    {
        editorManager->RequestLoadScene(filepath);
    }
}

void GuiManager::DrawSceneManager()
{
    ImGui::Begin("Scene Manager");

    ImGui::SeparatorText("Save Scene");

    ImGui::Text("Scene Name:");
    ImGui::InputText("##SceneName", newSceneName, sizeof(newSceneName));

    if (ImGui::Button("Save Scene", ImVec2(-1, 0)))
    {
        SaveCurrentScene(std::string(newSceneName));
        RefreshSceneList();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::SeparatorText("Load Scene");

    if (ImGui::Button("Refresh Scene List", ImVec2(-1, 0)))
    {
        RefreshSceneList();
    }

    ImGui::Spacing();

    if (availableScenes.empty())
    {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No saved scenes found");
    }
    else
    {
        ImGui::Text("Available Scenes:");

        ImGui::BeginChild("SceneList", ImVec2(0, 200), true);

        for (size_t i = 0; i < availableScenes.size(); ++i)
        {
            bool isSelected = (selectedSceneIndex == static_cast<int>(i));

            if (ImGui::Selectable(availableScenes[i].c_str(), isSelected))
            {
                selectedSceneIndex = static_cast<int>(i);
            }

            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndChild();

        if (selectedSceneIndex >= 0 && selectedSceneIndex < static_cast<int>(availableScenes.size()))
        {
            if (ImGui::Button("Load Selected Scene", ImVec2(-1, 0)))
            {
                LoadSelectedScene(availableScenes[selectedSceneIndex]);
            }

            ImGui::SameLine();

            if (ImGui::Button("Delete Selected", ImVec2(-1, 0)))
            {
                std::string filepath = SceneSerializer::GetSceneDirectory() + "/" + availableScenes[selectedSceneIndex];
                if (std::filesystem::remove(filepath))
                {
                    ShowPopup("Delete Success", "Scene deleted successfully!");
                    RefreshSceneList();
                    selectedSceneIndex = -1;
                }
                else
                {
                    ShowPopup("Delete Error", "Failed to delete scene file!");
                }
            }
        }
    }

    ImGui::End();
}