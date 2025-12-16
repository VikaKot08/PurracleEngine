#include "InspectorPanel.h"
#include "Model.h"
#include "Camera.h"
#include "SelectionManager.h"
#include "EditorManager.h"
#include "AssetBrowser.h"
#include "Scene.h"
#include "Light.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <cstring>

InspectorPanel::InspectorPanel(
    SelectionManager* selectionMgr,
    EditorManager* editorMgr,
    AssetBrowser* assets,
    Scene* scn,
    std::vector<Model*>* models
)
    : selectionManager(selectionMgr),
    editorManager(editorMgr),
    assetBrowser(assets),
    scene(scn),
    modelList(models),
    positionVec(0.0f),
    rotationVec(0.0f),
    rotationVecInput(0.0f),
    scaleVec(1.0f)
{
    selectionManager->AddListener([this](Model* model) {
        OnSelectionChanged(model);
        });
}

void InspectorPanel::Draw()
{
    ImGui::Begin("Controls");

    Model* selected = selectionManager->GetSelected();

    if (selected)
    {
        if (selected->type == ModelType::Normal)
        {
            DrawNormalModelControls(selected);
        }
        else if (selected->type == ModelType::CameraModel)
        {
            DrawCameraControls(selected);
        }
        else if (selected->type == ModelType::LightModel)
        {
            DrawLightControls(selected);
        }
    }
    else
    {
        ImGui::Text("No model selected");
        ImGui::TextWrapped("Click on a model in the viewport or scene hierarchy to select it.");
    }

    ImGui::End();
}

void InspectorPanel::OnSelectionChanged(Model* newSelection)
{
    if (newSelection)
    {
        positionVec = newSelection->position;
        rotationVec = newSelection->rotation;
        rotationVecInput = newSelection->rotation;
        scaleVec = newSelection->scale;
    }
}

void InspectorPanel::UpdateSelectedModelTransform()
{
    Model* selected = selectionManager->GetSelected();
    if (selected)
    {
        //selected->position = positionVec;
        selected->SetRotationFromEuler(rotationVec);
        //selected->scale = scaleVec;

        if (scene) {
            scene->MarkDirty();
        }
    }
}

void InspectorPanel::DrawNormalModelControls(Model* model)
{
    ImGui::Text("Model Name");
    char nameBuf[256];
    strncpy_s(nameBuf, sizeof(nameBuf), model->name.c_str(), _TRUNCATE);

    if (ImGui::InputText("##ModelName", nameBuf, sizeof(nameBuf)))
    {
        if (!std::string(nameBuf).empty())
        {
            model->name = std::string(nameBuf);
        }
    }

    if (ImGui::Button("Convert to camera", ImVec2(-1, 0)))
    {
        ConvertToCamera(model);
        return;
    }

    if (ImGui::Button("Convert to light", ImVec2(-1, 0)))
    {
        ConvertToLight(model);
        return;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Position");
    if (ImGui::InputFloat3("##Position", glm::value_ptr(model->position)))
    {
        UpdateSelectedModelTransform();
    }

    rotationVecInput = model->rotation;
    ImGui::Text("Rotation (degrees)");
    if (ImGui::InputFloat3("##Rotation", glm::value_ptr(rotationVecInput)))
    {
        float maxSafe = 1e6f;
        for (int i = 0; i < 3; i++)
        {
            if (rotationVecInput[i] > maxSafe) rotationVecInput[i] = maxSafe;
            if (rotationVecInput[i] < -maxSafe) rotationVecInput[i] = -maxSafe;
        }

        rotationVec.x = WrapAngle(rotationVecInput.x);
        rotationVec.y = WrapAngle(rotationVecInput.y);
        rotationVec.z = WrapAngle(rotationVecInput.z);

        rotationVecInput.x = WrapAngle(rotationVecInput.x);
        rotationVecInput.y = WrapAngle(rotationVecInput.y);
        rotationVecInput.z = WrapAngle(rotationVecInput.z);

        UpdateSelectedModelTransform();
    }

    ImGui::Text("Scale");
    if (ImGui::InputFloat3("##Scale", glm::value_ptr(model->scale)))
    {
        UpdateSelectedModelTransform();
    }


    ImGui::Spacing();
    ImGui::SeparatorText("Mesh");

    if (ImGui::Combo("##MeshType", &model->meshIndex,
        assetBrowser->GetMeshNames().data(),
        assetBrowser->GetMeshNames().size()))
    {
        editorManager->RequestLoadMesh(model, assetBrowser->GetMeshes()[model->meshIndex]);
    }


    ImGui::Spacing();
    ImGui::SeparatorText("Texture");

    if (ImGui::Combo("##Texture", &model->textureIndex,
        assetBrowser->GetTextureNames().data(),
        assetBrowser->GetTextureNames().size()))
    {
        editorManager->ChangeTexture(model, assetBrowser->GetTextures()[model->textureIndex]);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Save Optimized Mesh", ImVec2(-1, 0)))
    {
        editorManager->SaveOptimizedMesh(model);
    }

    ImGui::Separator();

    if (ImGui::Button("Delete Model", ImVec2(-1, 0)))
    {
        DeleteSelectedModel();
    }
}

void InspectorPanel::DrawCameraControls(Model* model)
{
    Camera* selectedCamera = dynamic_cast<Camera*>(model);

    ImGui::Text("Camera Name");
    char nameBuf[256];
    strncpy_s(nameBuf, sizeof(nameBuf), model->name.c_str(), _TRUNCATE);

    if (ImGui::InputText("##CameraName", nameBuf, sizeof(nameBuf)))
    {
        if (!std::string(nameBuf).empty())
        {
            model->name = std::string(nameBuf);
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (selectedCamera == scene->mainCamera)
    {
        selectedCamera->SyncYawPitchToRotation();
    }

    ImGui::Text("Position");
    if (ImGui::InputFloat3("##Position", glm::value_ptr(model->position)))
    {
        UpdateSelectedModelTransform();
    }

    rotationVecInput = model->rotation;
    ImGui::Text("Rotation (degrees)");
    if (ImGui::InputFloat3("##Rotation", glm::value_ptr(rotationVecInput)))
    {
        float maxSafe = 1e6f;
        for (int i = 0; i < 3; i++)
        {
            if (rotationVecInput[i] > maxSafe) rotationVecInput[i] = maxSafe;
            if (rotationVecInput[i] < -maxSafe) rotationVecInput[i] = -maxSafe;
        }

        rotationVec.x = WrapAngle(rotationVecInput.x);
        rotationVec.y = WrapAngle(rotationVecInput.y);
        rotationVec.z = WrapAngle(rotationVecInput.z);

        rotationVecInput.x = WrapAngle(rotationVecInput.x);
        rotationVecInput.y = WrapAngle(rotationVecInput.y);
        rotationVecInput.z = WrapAngle(rotationVecInput.z);

        UpdateSelectedModelTransform();
        selectedCamera->SyncRotationToYawPitch();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (selectedCamera)
    {
        ImGui::SeparatorText("Camera Settings");

        if (selectedCamera == scene->mainCamera)
        {
            ImGui::Text("Flying speed");
            float speed = scene->flyingCamera->GetSpeed();
            if (ImGui::SliderFloat("##FS", &speed, 1.0f, 20.0f, "%.1f"))
            {
                scene->flyingCamera->SetSpeed(speed);
            }
        }

        ImGui::Text("Field of View");
        float fov = selectedCamera->fov;
        if (ImGui::SliderFloat("##FOV", &fov, 1.0f, 120.0f, "%.1f degrees"))
        {
            selectedCamera->fov = fov;
        }

        ImGui::Text("Near Plane");
        float nearPlane = selectedCamera->near;
        if (ImGui::InputFloat("##Near", &nearPlane, 0.01f, 0.1f, "%.3f"))
        {
            if (nearPlane > 0.001f && nearPlane < selectedCamera->far)
            {
                selectedCamera->near = nearPlane;
            }
        }

        ImGui::Text("Far Plane");
        float farPlane = selectedCamera->far;
        if (ImGui::InputFloat("##Far", &farPlane, 1.0f, 10.0f, "%.1f"))
        {
            if (farPlane > selectedCamera->near)
            {
                selectedCamera->far = farPlane;
            }
        }
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
    }

    if (!model->isActive)
    {
        if (ImGui::Button("Switch to camera", ImVec2(-1, 0)))
        {
            Camera* selectedCamera = dynamic_cast<Camera*>(model);
            scene->SetCamera(selectedCamera);
        }

        ImGui::Separator();

        if (model != scene->mainCamera)
        {
            if (ImGui::Button("Delete Model", ImVec2(-1, 0)))
            {
                DeleteSelectedModel();
            }
        }
    }
}

void InspectorPanel::DrawLightControls(Model* model)
{
    Light* light = dynamic_cast<Light*>(model);
    if (!light) return;

    ImGui::Text("Light Name");
    char nameBuf[256];
    strncpy_s(nameBuf, sizeof(nameBuf), model->name.c_str(), _TRUNCATE);

    if (ImGui::InputText("##LightName", nameBuf, sizeof(nameBuf)))
    {
        if (!std::string(nameBuf).empty())
        {
            model->name = std::string(nameBuf);
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Light Type
    ImGui::Text("Light Type");
    const char* lightTypes[] = { "Directional", "Point", "Spot" };
    int currentType = static_cast<int>(light->typeLight);
    if (ImGui::Combo("##LightType", &currentType, lightTypes, 3))
    {
        light->typeLight = static_cast<LightType>(currentType);
        if (currentType == LightType::DirectionalLight)
        {
            editorManager->RequestLoadMesh(light, "Assets/Models/Light.obj");
            editorManager->ChangeTexture(light, "Assets/Textures/Light.png");
        } else if (currentType == LightType::SpotLight)
        {
            editorManager->RequestLoadMesh(light, "Assets/Models/Light.obj");
            editorManager->ChangeTexture(light, "Assets/Textures/Light.png");
        } else 
        {
            editorManager->RequestLoadMesh(light, "Assets/Models/PointLight.obj");
            editorManager->ChangeTexture(light, "Assets/Textures/PointLight.png");
        }
        light->dirty = true;
    }

    ImGui::Spacing();

    // Position (for point and spot lights)
    if (light->typeLight == PointLight || light->typeLight == SpotLight)
    {
        ImGui::Text("Position");
        if (ImGui::InputFloat3("##Position", glm::value_ptr(model->position)))
        {
            UpdateSelectedModelTransform();
            light->dirty = true;
        }
    }

    rotationVecInput = light->rotation;

    if (light->typeLight == DirectionalLight || light->typeLight == SpotLight)
    {
        ImGui::Text("Rotation (degrees)");
        if (ImGui::InputFloat3("##Rotation", glm::value_ptr(rotationVecInput)))
        {
            float maxSafe = 1e6f;
            for (int i = 0; i < 3; i++)
            {
                if (rotationVecInput[i] > maxSafe) rotationVecInput[i] = maxSafe;
                if (rotationVecInput[i] < -maxSafe) rotationVecInput[i] = -maxSafe;
            }

            rotationVec.x = WrapAngle(rotationVecInput.x);
            rotationVec.y = WrapAngle(rotationVecInput.y);
            rotationVec.z = WrapAngle(rotationVecInput.z);

            rotationVecInput.x = WrapAngle(rotationVecInput.x);
            rotationVecInput.y = WrapAngle(rotationVecInput.y);
            rotationVecInput.z = WrapAngle(rotationVecInput.z);

            UpdateSelectedModelTransform();
            light->UpdateDirectionFromRotation();
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Ambient Color");
    if (ImGui::ColorEdit4("##Ambient", glm::value_ptr(light->ambient)))
    {
        light->dirty = true;
    }

    ImGui::Text("Diffuse Color");
    if (ImGui::ColorEdit4("##Diffuse", glm::value_ptr(light->diffuse)))
    {
        light->dirty = true;
    }

    ImGui::Text("Specular Color");
    if (ImGui::ColorEdit4("##Specular", glm::value_ptr(light->specular)))
    {
        light->dirty = true;
    }

    // Attenuation (for point and spot lights)
    if (light->typeLight == PointLight || light->typeLight == SpotLight)
    {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Attenuation");
        ImGui::Text("Constant");
        if (ImGui::SliderFloat("##AttConst", &light->attenuation.x, 0.0f, 2.0f))
        {
            light->dirty = true;
        }

        ImGui::Text("Linear");
        if (ImGui::SliderFloat("##AttLinear", &light->attenuation.y, 0.0f, 1.0f))
        {
            light->dirty = true;
        }

        ImGui::Text("Quadratic");
        if (ImGui::SliderFloat("##AttQuad", &light->attenuation.z, 0.0f, 0.5f))
        {
            light->dirty = true;
        }

        if (ImGui::Button("Short Range (7)"))
        {
            light->attenuation = glm::vec3(1.0f, 0.7f, 1.8f);
            light->dirty = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Medium Range (32)"))
        {
            light->attenuation = glm::vec3(1.0f, 0.09f, 0.032f);
            light->dirty = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Long Range (100)"))
        {
            light->attenuation = glm::vec3(1.0f, 0.014f, 0.0007f);
            light->dirty = true;
        }
    }

    // Spotlight cone angles
    if (light->typeLight == SpotLight)
    {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Inner Cutoff Angle");
        if (ImGui::SliderFloat("##InnerCutoff", &light->cutoffAngle, 0.0f, 90.0f))
        {
            if (light->cutoffAngle > light->outerCutoffAngle)
            {
                light->outerCutoffAngle = light->cutoffAngle;
            }
            light->dirty = true;
        }

        ImGui::Text("Outer Cutoff Angle");
        if (ImGui::SliderFloat("##OuterCutoff", &light->outerCutoffAngle, 0.0f, 90.0f))
        {
            if (light->outerCutoffAngle < light->cutoffAngle)
            {
                light->cutoffAngle = light->outerCutoffAngle;
            }
            light->dirty = true;
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Enable/Disable
    bool enabled = light->enabled;
    if (ImGui::Checkbox("Enabled", &enabled))
    {
        light->enabled = enabled;
        light->dirty = true;
    }

    ImGui::Separator();

    if (ImGui::Button("Delete Light", ImVec2(-1, 0)))
    {
        DeleteSelectedModel();
    }
}

void InspectorPanel::DeleteSelectedModel()
{
    Model* selected = selectionManager->GetSelected();
    if (!selected || !modelList)
        return;

    auto it = std::find(modelList->begin(), modelList->end(), selected);
    if (it != modelList->end())
    {
        scene->DeleteModel(selected);
        modelList->erase(it);
        selectionManager->ClearSelection();
    }
}

void InspectorPanel::ConvertToCamera(Model* model)
{
    Camera* newCamera = new Camera();
    newCamera->SetPosition(model->position);
    newCamera->rotation = model->rotation;
    newCamera->scale = glm::vec3{ 1.0f };
    newCamera->SyncRotationToYawPitch();
    newCamera->name = model->name + " (Camera)";
    newCamera->path = "Assets/Models/Camera.obj";
    newCamera->pathTex = "Assets/Textures/Camera.jpg";
    newCamera->isActive = false;


    editorManager->RequestLoadMesh(newCamera, "Assets/Models/Camera.obj");
    editorManager->ChangeTexture(newCamera, "Assets/Textures/Camera.jpg");

    modelList->push_back(newCamera);
    scene->AddRenderable(newCamera);
    selectionManager->Select(newCamera);

    // Delete the old model
    auto it = std::find(modelList->begin(), modelList->end(), model);
    if (it != modelList->end())
    {
        scene->DeleteModel(model);
        modelList->erase(it);
    }

    scene->BuildEmbreeScene();
}

void InspectorPanel::ConvertToLight(Model* model)
{
    Light* newLight = new Light(DirectionalLight);
    newLight->direction = glm::vec3(0.0f, 0.0f, 1.0f);
    newLight->ambient = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    newLight->diffuse = glm::vec4(0.8f, 0.8f, 0.7f, 1.0f);
    newLight->specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    newLight->attenuation = glm::vec3(1.0f, 0.09f, 0.032f);
    newLight->cutoffAngle = 12.5f;
    newLight->outerCutoffAngle = 17.5f;

    newLight->position = model->position;
    newLight->rotation = model->rotation;
    newLight->scale = glm::vec3{ 1.0f };
    newLight->name = model->name + " (Light)";


    selectionManager->Select(newLight);

    // Delete the old model
    auto it = std::find(modelList->begin(), modelList->end(), model);
    if (it != modelList->end())
    {
        scene->DeleteModel(model);
        modelList->erase(it);
    }

    newLight->UpdateDirectionFromRotation();

    editorManager->RequestLoadMesh(newLight, "Assets/Models/Light.obj");
    editorManager->ChangeTexture(newLight, "Assets/Textures/Light.png");

    modelList->push_back(newLight);
    scene->AddRenderable(newLight);
    scene->BuildEmbreeScene();
}

float InspectorPanel::WrapAngle(float angle)
{
    while (angle >= 360.0f) angle -= 360.0f;
    while (angle < 0.0f) angle += 360.0f;
    return angle;
}
