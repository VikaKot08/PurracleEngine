#include "GuiManager.h"
#include "Model.h"
#include "Scene.h"
#include "FrameBuffer.h"
#include "EditorManager.h"
#include "SelectionManager.h"
#include "AssetBrowser.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "ViewportPanel.h"
#include "SceneManagerPanel.h"
#include "MemoryMessage.h"
#include "SceneOperationCompleteMessage.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "ImGuizmo.h"
#include <stb_image.h>
#include <iostream>
#include <GLFW/glfw3.h>

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

GuiManager::GuiManager()
    : scene(nullptr),
    editorManager(nullptr),
    frameBuffer(nullptr),
    modelList(nullptr),
    showPopup(false)
{
}

GuiManager::~GuiManager()
{
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

    // Initialize shared managers
    selectionManager = std::make_unique<SelectionManager>();
    assetBrowser = std::make_unique<AssetBrowser>();
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
    InitializePanels();
}

void GuiManager::SetEditorManager(EditorManager* em)
{
    editorManager = em;
}

void GuiManager::SetFrameBuffer(FrameBuffer* fb)
{
    frameBuffer = fb;
}

void GuiManager::InitializePanels()
{
    if (!scene || !editorManager || !modelList)
        return;

    hierarchyPanel = std::make_unique<HierarchyPanel>(
        selectionManager.get(),
        scene,
        modelList,
        assetBrowser.get()
    );

    inspectorPanel = std::make_unique<InspectorPanel>(
        selectionManager.get(),
        editorManager,
        assetBrowser.get(),
        scene,
        modelList
    );

    viewportPanel = std::make_unique<ViewportPanel>(
        frameBuffer,
        selectionManager.get(),
        scene
    );

    sceneManagerPanel = std::make_unique<SceneManagerPanel>(
        editorManager,
        selectionManager.get()
    );
}

void GuiManager::Update(GLFWwindow* aWindow)
{
    editorManager->Update();
    ProcessMessages();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuizmo::BeginFrame();

    DrawDockSpace();

    if (hierarchyPanel) hierarchyPanel->Draw();
    if (inspectorPanel) inspectorPanel->Draw();
    if (viewportPanel) viewportPanel->Draw();
    if (sceneManagerPanel) sceneManagerPanel->Draw();

    DrawPopup();

    // Handle viewport mouse click
    if (viewportPanel && ImGui::IsMouseClicked(0) &&
        viewportPanel->IsHovered() && !ImGui::IsAnyItemActive())
    {
        viewportPanel->HandleMouseClick(aWindow);
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

void GuiManager::VerifyAtStart()
{
    assetBrowser->VerifyNames(*modelList);
}

void GuiManager::DrawDockSpace()
{
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
            if (sceneManagerPanel)
            {
                sceneManagerPanel->ShowPopup(title, completeMsg->message);
            }

            if (completeMsg->operation == "load")
            {
                // Refresh scene list after loading
                if (sceneManagerPanel)
                {
                    sceneManagerPanel->RefreshSceneList();
                }

                if (modelList && !modelList->empty())
                {
                    selectionManager->Select(modelList->at(0));
                    assetBrowser->VerifyNames(*modelList);
                }
                else
                {
                    selectionManager->ClearSelection();
                }
            }
        }
        else
        {
            if (sceneManagerPanel)
            {
                sceneManagerPanel->ShowPopup(title + " Error", completeMsg->message);
            }
        }
    }
    else if (aMessage->type == MessageType::Memory)
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
        showPopup = false;
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

float GuiManager::GetViewportHeight()
{
    return viewportPanel->GetViewportSize().y;
}

float GuiManager::GetViewportWidth()
{
    return viewportPanel->GetViewportSize().x;
}

Scene* GuiManager::GetScene()
{
    return scene;
}