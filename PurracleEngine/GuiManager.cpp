#include "GuiManager.h"
#include "Model.h"
#include "Camera.h"
#include "MousePicker.h"
#include "FrameBuffer.h"
#include <stb_image.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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

void GuiManager::SetCamera(Camera* cam)
{
    camera = cam;
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
    }
}

void GuiManager::HandleMouseClick(GLFWwindow* window)
{
    // Only pick if viewport is hovered and we have valid data
    if (!m_viewportHovered || !camera || !modelList)
        return;

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    ImGuiViewport* mainViewport = ImGui::GetMainViewport();
    mouseX -= (viewportPos.x - mainViewport->Pos.x);
    mouseY -= (viewportPos.y - mainViewport->Pos.y);

    // Check if click is within viewport bounds
    if (mouseX < 0 || mouseY < 0 || mouseX > viewportSize.x || mouseY > viewportSize.y)
        return;

    glm::vec3 rayDirection = MousePicker::GetMouseRay(mouseX, mouseY,
        (int)viewportSize.x,
        (int)viewportSize.y,
        camera);
    glm::vec3 rayOrigin = camera->GetPosition();

    // Pick model
    Model* pickedModel = MousePicker::PickModel(rayOrigin, rayDirection, modelList);
    if (pickedModel)
    {
        SelectModel(pickedModel);
    }
}

void GuiManager::DrawSceneHierarchy()
{
    ImGui::Begin("Scene Hierarchy");

    if (modelList)
    {
        for (size_t i = 0; i < modelList->size(); i++)
        {
            Model* model = (*modelList)[i];
            std::string label = "Model " + std::to_string(i);

            bool isSelected = (selectedModel == model);
            if (ImGui::Selectable(label.c_str(), isSelected))
            {
                SelectModel(model);
            }
        }
    }

    ImGui::End();
}

void GuiManager::DrawTransformControls()
{
    ImGui::Begin("Transform Controls");

    if (selectedModel)
    {
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
    }
    else
    {
        ImGui::Text("No model selected");
        ImGui::TextWrapped("Click on a model in the viewport or scene hierarchy to select it.");
    }

    ImGui::End();
}

void GuiManager::DrawViewport()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");

    m_viewportHovered = ImGui::IsWindowHovered();
    viewportPos = ImGui::GetCursorScreenPos();
    viewportSize = ImGui::GetContentRegionAvail();

    // Resize framebuffer if viewport size changed
    if (frameBuffer && viewportSize.x > 0 && viewportSize.y > 0)
    {
        frameBuffer->ValidSize((int)viewportSize.x, (int)viewportSize.y);

        // Display the framebuffer texture
        ImGui::Image(
            (void*)(intptr_t)frameBuffer->GetTexture(),
            viewportSize,
            ImVec2(0, 1),  // UV coordinates flipped for OpenGL
            ImVec2(1, 0)
        );
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

void GuiManager::Update(GLFWwindow* aWindow)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Create main dockspace
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

    // Draw UI panels
    DrawSceneHierarchy();
    DrawTransformControls();
    DrawViewport();

    // Handle mouse picking
    if (ImGui::IsMouseClicked(0) && m_viewportHovered && !ImGui::IsAnyItemActive())
    {
        HandleMouseClick(aWindow);
    }

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