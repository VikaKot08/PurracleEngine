#pragma once
#include <imgui.h>
#include <glm/glm.hpp>

class FrameBuffer;
class SelectionManager;
class Scene;
class Model;
struct GLFWwindow;

class ViewportPanel
{
public:
    ViewportPanel(
        FrameBuffer* fb,
        SelectionManager* selectionMgr,
        Scene* scn
    );

    void Draw();
    void HandleMouseClick(GLFWwindow* window);
    bool IsHovered() const { return viewportHovered; }

    ImVec2 GetViewportPos() const { return viewportPos; }
    ImVec2 GetViewportSize() const { return viewportSize; }

private:
    FrameBuffer* frameBuffer;
    SelectionManager* selectionManager;
    Scene* scene;

    bool viewportHovered;
    ImVec2 viewportPos;
    ImVec2 viewportSize;

    void ApplyGizmosAndTransform();
};
