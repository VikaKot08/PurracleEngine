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
    selectedSceneIndex(-1)
{
    memset(newSceneName, 0, sizeof(newSceneName));
    RefreshSceneList();
}

void SceneManagerPanel::Draw()
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
                    RefreshSceneList();
                    selectedSceneIndex = -1;
                }
            }
        }
    }

    ImGui::End();
}

void SceneManagerPanel::RefreshSceneList()
{
    availableScenes = SceneSerializer::GetAvailableScenes();
}

void SceneManagerPanel::SaveCurrentScene(const std::string& sceneName)
{
    if (sceneName.empty())
        return;

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
        return;

    std::string filepath = SceneSerializer::GetSceneDirectory() + "/" + sceneName;

    if (editorManager)
    {
        editorManager->RequestLoadScene(filepath);
        selectionManager->ClearSelection();
    }
}