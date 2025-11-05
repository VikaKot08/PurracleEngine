#include "GuiManager.h"
#include <stb_image.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


void SetPurracleStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    // --- Layout tweaks ---
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

void SetPurracleDarkStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    auto hex = [](int r, int g, int b) { return ImVec4(r / 255.f, g / 255.f, b / 255.f, 1.f); };

    ImVec4 softBeige = hex(240, 209, 190);  // #f0d1be
    ImVec4 softLilac = hex(80, 74, 112);    // #504a70
    ImVec4 paleCream = hex(246, 234, 227);  // #f6eae3
    ImVec4 darkLilac = hex(45, 41, 65);     // deepened base
    ImVec4 highlight = hex(255, 230, 210);  // soft glow highlight

    // Window and panels
    colors[ImGuiCol_WindowBg] = darkLilac;
    colors[ImGuiCol_ChildBg] = hex(55, 50, 78);
    colors[ImGuiCol_PopupBg] = hex(60, 55, 85);

    // Headers / active sections
    colors[ImGuiCol_Header] = softLilac;
    colors[ImGuiCol_HeaderHovered] = hex(100, 92, 135);
    colors[ImGuiCol_HeaderActive] = hex(120, 110, 150);

    // Buttons
    colors[ImGuiCol_Button] = softLilac;
    colors[ImGuiCol_ButtonHovered] = hex(100, 92, 135);
    colors[ImGuiCol_ButtonActive] = hex(120, 110, 150);

    // Frames
    colors[ImGuiCol_FrameBg] = hex(65, 60, 90);
    colors[ImGuiCol_FrameBgHovered] = hex(80, 75, 110);
    colors[ImGuiCol_FrameBgActive] = hex(100, 90, 130);

    // Text
    colors[ImGuiCol_Text] = paleCream;
    colors[ImGuiCol_TextDisabled] = hex(160, 150, 180);

    // Title bar
    colors[ImGuiCol_TitleBg] = softLilac;
    colors[ImGuiCol_TitleBgActive] = hex(90, 82, 125);
    colors[ImGuiCol_TitleBgCollapsed] = hex(70, 65, 100);

    // Tabs
    colors[ImGuiCol_Tab] = softLilac;
    colors[ImGuiCol_TabHovered] = hex(100, 92, 135);
    colors[ImGuiCol_TabActive] = hex(120, 110, 150);
    colors[ImGuiCol_TabUnfocused] = hex(70, 65, 100);
    colors[ImGuiCol_TabUnfocusedActive] = hex(90, 82, 125);

    // Borders & scrollbars
    colors[ImGuiCol_Border] = hex(110, 100, 130);
    colors[ImGuiCol_ScrollbarBg] = darkLilac;
    colors[ImGuiCol_ScrollbarGrab] = softLilac;
    colors[ImGuiCol_ScrollbarGrabHovered] = hex(100, 92, 135);
    colors[ImGuiCol_ScrollbarGrabActive] = hex(120, 110, 150);

    // Highlights (checkbox, sliders, etc.)
    colors[ImGuiCol_CheckMark] = softBeige;
    colors[ImGuiCol_SliderGrab] = softBeige;
    colors[ImGuiCol_SliderGrabActive] = highlight;

    // Rounding and padding
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

void GuiManager::Update(GLFWwindow* aWindow)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::DockSpace(viewport->ID, viewport->Size);


    ImGui::SetNextWindowPos(ImVec2(1000, 300), 4);
    ImGui::Begin("Transform Controls");
    ImGui::SliderFloat("Scale", &scale, 0.1f, 5.0f);

    ImGui::Text("Position");
    ImGui::InputFloat3("##Position", glm::value_ptr(positionVec)); 

    ImGui::Text("Rotation (degrees)");
    ImGui::InputFloat3("##Rotation", glm::value_ptr(rotationVecInput));

    float maxSafe = 1e6f;
    for (int i = 0; i < 3; i++)
    {
        if (rotationVecInput[i] > maxSafe) rotationVecInput[i] = maxSafe;
        if (rotationVecInput[i] < -maxSafe) rotationVecInput[i] = -maxSafe;
    }

    rotationVec.x = fmod(rotationVecInput.x, 360.0f);
    if (rotationVec.x < 0.0f) rotationVec.x += 360.0f;

    rotationVec.y = fmod(rotationVecInput.y, 360.0f);
    if (rotationVec.y < 0.0f) rotationVec.y += 360.0f;

    rotationVec.z = fmod(rotationVecInput.z, 360.0f);
    if (rotationVec.z < 0.0f) rotationVec.z += 360.0f;

    rotationVec.x = WrapAngle(rotationVecInput.x);
    rotationVec.y = WrapAngle(rotationVecInput.y);
    rotationVec.z = WrapAngle(rotationVecInput.z);

    rotationVecInput.x = WrapAngle(rotationVecInput.x);
    rotationVecInput.y = WrapAngle(rotationVecInput.y);
    rotationVecInput.z = WrapAngle(rotationVecInput.z);

    ImGui::Text("Scale");
    ImGui::InputFloat3("##Scale", glm::value_ptr(scaleVec));

    ImGui::End();




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
