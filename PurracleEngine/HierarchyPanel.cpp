#include "HierarchyPanel.h"
#include "Model.h"
#include "Scene.h"
#include "SelectionManager.h"
#include "AssetBrowser.h"
#include <imgui.h>

HierarchyPanel::HierarchyPanel(
    SelectionManager* selectionMgr,
    Scene* scn,
    std::vector<Model*>* models,
    AssetBrowser* assets
)
    : selectionManager(selectionMgr),
    scene(scn),
    modelList(models),
    assetBrowser(assets)
{
}

void HierarchyPanel::Draw()
{
    ImGui::Begin("Scene Hierarchy");

    if (ImGui::Button("Add Model", ImVec2(-1, 0)))
    {
        AddModel();
    }

    if (ImGui::Button("Refresh Assets", ImVec2(-1, 0)))
    {
        assetBrowser->RefreshAssets();
    }

    ImGui::Separator();

    if (modelList)
    {
        for (Model* model : *modelList)
        {
            if (model->parent == nullptr)
                DrawModelNode(model);
        }
    }

    ImGui::End();
}

void HierarchyPanel::DrawModelNode(Renderable* model)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

    if (selectionManager->GetSelected() == model)
        flags |= ImGuiTreeNodeFlags_Selected;

    bool isLeaf = model->children.empty();
    if (isLeaf)
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

    bool opened = ImGui::TreeNodeEx((void*)model, flags, "%s", model->name.c_str());

    if (ImGui::BeginDragDropSource())
    {
        Renderable* payload = model;
        ImGui::SetDragDropPayload("MODEL", &payload, sizeof(Renderable*));
        ImGui::Text("%s", model->name.c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MODEL"))
        {
            Renderable* dropped = *(Renderable**)payload->Data;
            if (dropped != model)
            {
                dropped->SetParent(model);
                if (scene) {
                    scene->MarkDirty();
                }
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (ImGui::IsItemClicked())
    {
        selectionManager->Select(dynamic_cast<Model*>(model));
    }

    if (opened && !isLeaf)
    {
        for (Renderable* child : model->children)
            DrawModelNode(child);

        ImGui::TreePop();
    }
}

void HierarchyPanel::AddModel()
{
    Model* newModel = scene->LoadModel("Assets/Models/Cube.obj", "Assets/Textures/Cube.png");
    newModel->position = glm::vec3(0.0f, 0.0f, 0.0f);
    newModel->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    newModel->scale = glm::vec3(1.0f, 1.0f, 1.0f);

    modelList->push_back(newModel);

    if (scene)
    {
        scene->AddRenderable(newModel);
        scene->BuildEmbreeScene();
    }

    selectionManager->Select(newModel);
}
