#include "ViewportPanel.h"
#include "ImGuizmo.h""
#include "FrameBuffer.h"
#include "SelectionManager.h"
#include "Scene.h"
#include "Model.h"
#include "Camera.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

static ImGuizmo::OPERATION sGizmoOperation = ImGuizmo::TRANSLATE;

ViewportPanel::ViewportPanel(
    FrameBuffer* fb,
    SelectionManager* selectionMgr,
    Scene* scn
)
    : frameBuffer(fb),
    selectionManager(selectionMgr),
    scene(scn),
    viewportHovered(false),
    viewportPos(0, 0),
    viewportSize(0, 0)
{
}

void ViewportPanel::Draw()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");

    viewportHovered = ImGui::IsWindowHovered();
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

    // Handle gizmo operation keys
    if (ImGui::IsKeyPressed(ImGuiKey_T)) sGizmoOperation = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(ImGuiKey_R)) sGizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(ImGuiKey_Y)) sGizmoOperation = ImGuizmo::SCALE;
}

void ViewportPanel::HandleMouseClick(GLFWwindow* window)
{
    if (!viewportHovered || !scene)
        return;

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    ImGuiViewport* mainViewport = ImGui::GetMainViewport();
    mouseX -= (viewportPos.x - mainViewport->Pos.x);
    mouseY -= (viewportPos.y - mainViewport->Pos.y);

    if (mouseX < 0 || mouseY < 0 || mouseX > viewportSize.x || mouseY > viewportSize.y)
        return;

    double x = (2.0f * mouseX) / viewportSize.x - 1.0f;
    double y = 1.0f - (2.0f * mouseY) / viewportSize.y;

    glm::mat4 projection = scene->GetCamera()->GetProjection();
    glm::mat4 view = scene->GetCamera()->GetView();

    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);
    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    glm::vec3 rayWorld = glm::vec3(glm::inverse(view) * rayEye);
    rayWorld = glm::normalize(rayWorld);

    glm::vec3 rayOrigin = scene->GetCamera()->GetPosition();

    Model* pickedModel = scene->TraceRay(rayOrigin, rayWorld);
    if (pickedModel) {
        selectionManager->Select(pickedModel);
    }
}

void ViewportPanel::ApplyGizmosAndTransform()
{
    Model* selectedModel = selectionManager->GetSelected();

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

            if (scene) {
                scene->MarkDirty();
            }

            if (selectedModel->type == ModelType::CameraModel)
            {
                Camera* selectedCamera = dynamic_cast<Camera*>(selectedModel);
                selectedCamera->SyncRotationToYawPitch();
            }
        }
    }
}
