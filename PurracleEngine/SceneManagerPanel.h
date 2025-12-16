#pragma once
#include <vector>
#include <string>

class EditorManager;
class SelectionManager;

class SceneManagerPanel
{
public:
    SceneManagerPanel(EditorManager* editorMgr, SelectionManager* selectionMgr);

    void Draw();
    void RefreshSceneList();
    void ShowPopup(const std::string& title, const std::string& message);

private:
    EditorManager* editorManager;
    SelectionManager* selectionManager;

    std::vector<std::string> availableScenes;
    int selectedSceneIndex;
    char newSceneName[256];

    // Popup system
    bool showPopup;
    std::string popupTitle;
    std::string popupMessage;
    bool usePhong = true;

    void SaveCurrentScene(const std::string& sceneName);
    void LoadSelectedScene(const std::string& sceneName);
    void DrawPopup();
};
