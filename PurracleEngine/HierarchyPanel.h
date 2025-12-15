#pragma once
#include <vector>
#include <glm/glm.hpp>

class Model;
class Renderable;
class SelectionManager;
class Scene;
class AssetBrowser;

class HierarchyPanel
{
public:
    HierarchyPanel(
        SelectionManager* selectionMgr,
        Scene* scn,
        std::vector<Model*>* models,
        AssetBrowser* assets
    );

    void Draw();

private:
    SelectionManager* selectionManager;
    Scene* scene;
    std::vector<Model*>* modelList;
    AssetBrowser* assetBrowser;

    void DrawModelNode(Renderable* model);
    void AddModel();
};

