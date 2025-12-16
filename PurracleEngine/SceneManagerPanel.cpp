#include "SceneManagerPanel.h"
#include "EditorManager.h"
#include "SelectionManager.h"
#include "SceneSerializer.h"
#include <imgui.h>
#include <filesystem>
#include <cstring>

SceneManagerPanel::SceneManagerPanel(EditorManager* editorMgr, SelectionManager* selectionMgr)
    : editorManager(editorMgr),
    selectionManager(selectionMgr),
    selectedSceneIndex(-1),
    showPopup(false)
{
    memset(newSceneName, 0, sizeof(newSceneName));
    RefreshSceneList();
}

void SceneManagerPanel::Draw()
{
    ImGui::Begin("Scene Manager");

    ImGui::SeparatorText("Global Settings");

    if (ImGui::Checkbox("Use Phong Shader", &usePhong))
    {
        editorManager->ChangeShader();
    }

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

    DrawPopup();
}

void SceneManagerPanel::RefreshSceneList()
{
    availableScenes = SceneSerializer::GetAvailableScenes();
}

void SceneManagerPanel::SaveCurrentScene(const std::string& sceneName)
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

void SceneManagerPanel::LoadSelectedScene(const std::string& sceneName)
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
        selectionManager->ClearSelection();
    }
}

void SceneManagerPanel::ShowPopup(const std::string& title, const std::string& message)
{
    popupTitle = title;
    popupMessage = message;
    showPopup = true;
}

void SceneManagerPanel::DrawPopup()
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