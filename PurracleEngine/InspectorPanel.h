#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

class Model;
class SelectionManager;
class EditorManager;
class AssetBrowser;
class Scene;

class InspectorPanel
{
public:
    InspectorPanel(
        SelectionManager* selectionMgr,
        EditorManager* editorMgr,
        AssetBrowser* assets,
        Scene* scn,
        std::vector<Model*>* models
    );

    void Draw();

private:
    SelectionManager* selectionManager;
    EditorManager* editorManager;
    AssetBrowser* assetBrowser;
    Scene* scene;
    std::vector<Model*>* modelList;

    glm::vec3 positionVec;
    glm::vec3 rotationVec;
    glm::vec3 rotationVecInput;
    glm::vec3 scaleVec;

    void OnSelectionChanged(Model* newSelection);
    void UpdateSelectedModelTransform();
    void DrawNormalModelControls(Model* model);
    void DrawCameraControls(Model* model);
    void DeleteSelectedModel();
    void ConvertToCamera(Model* model);

    static float WrapAngle(float angle);
};
