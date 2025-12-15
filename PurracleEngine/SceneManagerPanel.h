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

private:
    EditorManager* editorManager;
    SelectionManager* selectionManager;

    std::vector<std::string> availableScenes;
    int selectedSceneIndex;
    char newSceneName[256];

    void SaveCurrentScene(const std::string& sceneName);
    void LoadSelectedScene(const std::string& sceneName);
};

