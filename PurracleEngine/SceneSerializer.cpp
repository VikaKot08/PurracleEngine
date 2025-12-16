#include "SceneSerializer.h"
#include "Scene.h"
#include "Model.h"

#include "Light.h"
#include "Camera.h"
#include "MeshManager.h"
#include <iostream>
#include <filesystem>
#include <glm/glm.hpp>

void SceneSerializer::WriteString(std::ofstream& file, const std::string& str)
{
    uint32_t length = static_cast<uint32_t>(str.length());
    file.write((const char*)(&length), sizeof(uint32_t));
    file.write(str.c_str(), length);
}

std::string SceneSerializer::ReadString(std::ifstream& file)
{
    uint32_t length = 0;
    file.read((char*)(&length), sizeof(uint32_t));

    if (length > 10000) // Sanity check
    {
        std::cerr << "Invalid string length: " << length << std::endl;
        return "";
    }

    std::string str(length, '\0');
    file.read(&str[0], length);
    return str;
}

int SceneSerializer::FindModelIndex(Model* model, const std::vector<Model*>& models)
{
    for (size_t i = 0; i < models.size(); ++i)
    {
        if (models[i] == model)
            return static_cast<int>(i);
    }
    return -1;
}

bool SceneSerializer::SaveScene(Scene* scene, const std::string& filepath)
{
    if (!scene)
    {
        std::cerr << "SceneSerializer: Cannot save null scene" << std::endl;
        return false;
    }

    std::filesystem::path path(filepath);
    if (path.has_parent_path())
    {
        std::filesystem::create_directories(path.parent_path());
    }

    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "SceneSerializer: Failed to open file for writing: " << filepath << std::endl;
        return false;
    }

    // Write header
    file.write((const char*)(&MAGIC_NUMBER), sizeof(uint32_t));
    file.write((const char*)(&VERSION), sizeof(uint32_t));

    // Get all renderables
    std::vector<Renderable*> renderables = scene->GetRenderables();

    std::vector<Model*> models;
    std::vector<Camera*> cameras;
    std::vector<Light*> lights;

    for (Renderable* r : renderables)
    {
        Model* model = dynamic_cast<Model*>(r);
        if (model)
        {
            if (model->type == ModelType::CameraModel)
            {
                Camera* cam = dynamic_cast<Camera*>(model);
                if (cam) cameras.push_back(cam);
            }
            else if (model->type == ModelType::LightModel)
            {
                Light* light = dynamic_cast<Light*>(model);
                if (light) lights.push_back(light);
            }
            else
            {
                models.push_back(model);
            }
        }
    }

    //Write model count
    uint32_t modelCount = static_cast<uint32_t>(models.size());
    file.write((const char*)(&modelCount), sizeof(uint32_t));

    // Write models
    for (Model* model : models)
    {
        WriteString(file, model->name);
        WriteString(file, model->path);
        WriteString(file, model->myTexture->path);

        file.write((const char*)(&model->position), sizeof(glm::vec3));
        file.write((const char*)(&model->rotation), sizeof(glm::vec3));
        file.write((const char*)(&model->scale), sizeof(glm::vec3));

        int32_t modelType = static_cast<int32_t>(model->type);
        file.write((const char*)(&modelType), sizeof(int32_t));

        file.write((const char*)(&model->isActive), sizeof(bool));

        // Write parent index
        int32_t parentIndex = -1;
        if (model->parent)
        {
            Model* parentModel = dynamic_cast<Model*>(model->parent);
            if (parentModel)
            {
                parentIndex = FindModelIndex(parentModel, models);
            }
        }
        file.write((const char*)(&parentIndex), sizeof(int32_t));
    }

    // Write camera count
    uint32_t cameraCount = static_cast<uint32_t>(cameras.size());
    file.write((const char*)(&cameraCount), sizeof(uint32_t));

    // Write cameras
    for (Camera* cam : cameras)
    {
        WriteString(file, cam->name);

        glm::vec3 pos = cam->GetPosition();
        file.write((const char*)(&pos), sizeof(glm::vec3));
        file.write((const char*)(&cam->rotation), sizeof(glm::vec3));
        file.write((const char*)(&cam->yaw), sizeof(float));
        file.write((const char*)(&cam->pitch), sizeof(float));
        file.write((const char*)(&cam->fov), sizeof(float));
        file.write((const char*)(&cam->near), sizeof(float));
        file.write((const char*)(&cam->far), sizeof(float));

        bool isMainCamera = (cam == scene->mainCamera);
        file.write((const char*)(&isMainCamera), sizeof(bool));
        file.write((const char*)(&cam->isActive), sizeof(bool));

        // Parent index
        int32_t parentIndex = -1;
        if (cam->parent)
        {
            Model* parentModel = dynamic_cast<Model*>(cam->parent);
            if (parentModel)
            {
                parentIndex = FindModelIndex(parentModel, models);
            }
        }
        file.write((const char*)(&parentIndex), sizeof(int32_t));
    }


    // Write light count
    uint32_t lightCount = static_cast<uint32_t>(lights.size());
    file.write((const char*)(&lightCount), sizeof(uint32_t));

    // Write lights
    for (Light* light : lights)
    {
        WriteString(file, light->name);

        file.write((const char*)(&light->position), sizeof(glm::vec3));
        file.write((const char*)(&light->rotation), sizeof(glm::vec3));

        int32_t lightType = static_cast<int32_t>(light->typeLight);
        file.write((const char*)(&lightType), sizeof(int32_t));

        file.write((const char*)(&light->direction), sizeof(glm::vec3));
        file.write((const char*)(&light->ambient), sizeof(glm::vec4));
        file.write((const char*)(&light->diffuse), sizeof(glm::vec4));
        file.write((const char*)(&light->specular), sizeof(glm::vec4));
        file.write((const char*)(&light->attenuation), sizeof(glm::vec3));
        file.write((const char*)(&light->cutoffAngle), sizeof(float));
        file.write((const char*)(&light->outerCutoffAngle), sizeof(float));
        file.write((const char*)(&light->enabled), sizeof(bool));
        file.write((const char*)(&light->isActive), sizeof(bool));

        // Parent index
        int32_t parentIndex = -1;
        if (light->parent)
        {
            Model* parentModel = dynamic_cast<Model*>(light->parent);
            if (parentModel)
            {
                parentIndex = FindModelIndex(parentModel, models);
            }
        }
        file.write((const char*)(&parentIndex), sizeof(int32_t));
    }

    file.close();
    std::cout << "SceneSerializer: Scene saved successfully to " << filepath << std::endl;
    return true;
}

bool SceneSerializer::LoadScene(Scene* scene, const std::string& filepath, std::vector<Model*>* modelList)
{
    if (!scene || !modelList)
    {
        std::cerr << "SceneSerializer: Invalid scene or model list" << std::endl;
        return false;
    }

    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "SceneSerializer: Failed to open file for reading: " << filepath << std::endl;
        return false;
    }

    // Read and verify header
    uint32_t magic, version;
    file.read((char*)(&magic), sizeof(uint32_t));
    file.read((char*)(&version), sizeof(uint32_t));

    if (magic != MAGIC_NUMBER)
    {
        std::cerr << "SceneSerializer: Invalid file format" << std::endl;
        file.close();
        return false;
    }

    if (version != VERSION)
    {
        std::cerr << "SceneSerializer: Unsupported version: " << version << std::endl;
        file.close();
        return false;
    }

    std::vector<Renderable*> toDelete;
    for (Renderable* r : scene->renderables)
    {
        if (r)
        {
            toDelete.push_back(r);
        }
    }

    for (Renderable* r : toDelete)
    {
        scene->DeleteModel(r);
    }
    modelList->clear();

    // Read model count
    uint32_t modelCount = 0;
    file.read((char*)(&modelCount), sizeof(uint32_t));

    if (modelCount > 10000)
    {
        std::cerr << "SceneSerializer: Invalid model count: " << modelCount << std::endl;
        file.close();
        return false;
    }

    std::vector<Model*> loadedModels;
    std::vector<int32_t> parentIndices;

    // Read models
    for (uint32_t i = 0; i < modelCount; ++i)
    {
        std::string name = ReadString(file);
        std::string meshPath = ReadString(file);
        std::string texturePath = ReadString(file);

        glm::vec3 position, rotation, scale;
        file.read((char*) (&position), sizeof(glm::vec3));
        file.read((char*)(&rotation), sizeof(glm::vec3));
        file.read((char*)(&scale), sizeof(glm::vec3));

        int32_t modelType;
        file.read((char*)(&modelType), sizeof(int32_t));

        bool isActive;
        file.read((char*)(&isActive), sizeof(bool));

        int32_t parentIndex;
        file.read((char*)(&parentIndex), sizeof(int32_t));

        // Create model
        Model* model = scene->LoadModel(meshPath, texturePath.c_str());
        model->name = name;
        model->position = position;
        model->rotation = rotation;
        model->scale = scale;
        model->type = static_cast<ModelType>(modelType);
        model->isActive = isActive;

        loadedModels.push_back(model);
        parentIndices.push_back(parentIndex);
        modelList->push_back(model);
        scene->AddRenderable(model);
    }

    // Read camera count
    uint32_t cameraCount = 0;
    file.read((char*)(&cameraCount), sizeof(uint32_t));

    if (cameraCount > 100)
    {
        std::cerr << "SceneSerializer: Invalid camera count: " << cameraCount << std::endl;
        file.close();
        return false;
    }

    std::vector<Camera*> loadedCameras;
    std::vector<int32_t> cameraParentIndices;

    // Read cameras
    Camera* mainCameraToSet = nullptr;
    for (uint32_t i = 0; i < cameraCount; ++i)
    {
        std::string name = ReadString(file);

        glm::vec3 position, rotation;
        float yaw, pitch, fov, nearPlane, farPlane;
        bool isMainCamera, isActive;
        int32_t parentIndex;

        file.read((char*)(&position), sizeof(glm::vec3));
        file.read((char*)(&rotation), sizeof(glm::vec3));
        file.read((char*)(&yaw), sizeof(float));
        file.read((char*)(&pitch), sizeof(float));
        file.read((char*)(&fov), sizeof(float));
        file.read((char*)(&nearPlane), sizeof(float));
        file.read((char*)(&farPlane), sizeof(float));
        file.read((char*)(&isMainCamera), sizeof(bool));
        file.read((char*)(&isActive), sizeof(bool));
        file.read((char*)(&parentIndex), sizeof(int32_t));

        Camera* camera = new Camera();
        camera->name = name;
        camera->SetPosition(position);
        camera->rotation = rotation;
        camera->yaw = yaw;
        camera->pitch = pitch;
        camera->fov = fov;
        camera->near = nearPlane;
        camera->far = farPlane;
        camera->isActive = isActive;
        camera->meshes = MeshManager::Get()->LoadMeshes("Assets/Models/Camera.obj", 0);

        loadedCameras.push_back(camera);
        cameraParentIndices.push_back(parentIndex);
        modelList->push_back(camera);
        scene->AddRenderable(camera);

        if (isMainCamera)
        {
            mainCameraToSet = camera;
        }
    }

    // Read light count
    uint32_t lightCount = 0;
    file.read((char*)(&lightCount), sizeof(uint32_t));

    if (lightCount > 100)
    {
        std::cerr << "SceneSerializer: Invalid light count: " << lightCount << std::endl;
        file.close();
        return false;
    }

    std::vector<Light*> loadedLights;
    std::vector<int32_t> lightParentIndices;

    // Read lights
    for (uint32_t i = 0; i < lightCount; ++i)
    {
        std::string name = ReadString(file);

        glm::vec3 position, rotation, direction, attenuation;
        glm::vec4 ambient, diffuse, specular;
        int32_t lightType;
        float cutoffAngle, outerCutoffAngle;
        bool enabled, isActive;
        int32_t parentIndex;

        file.read((char*)(&position), sizeof(glm::vec3));
        file.read((char*)(&rotation), sizeof(glm::vec3));
        file.read((char*)(&lightType), sizeof(int32_t));
        file.read((char*)(&direction), sizeof(glm::vec3));
        file.read((char*)(&ambient), sizeof(glm::vec4));
        file.read((char*)(&diffuse), sizeof(glm::vec4));
        file.read((char*)(&specular), sizeof(glm::vec4));
        file.read((char*)(&attenuation), sizeof(glm::vec3));
        file.read((char*)(&cutoffAngle), sizeof(float));
        file.read((char*)(&outerCutoffAngle), sizeof(float));
        file.read((char*)(&enabled), sizeof(bool));
        file.read((char*)(&isActive), sizeof(bool));
        file.read((char*)(&parentIndex), sizeof(int32_t));

        Light* light = new Light(static_cast<LightType>(lightType));
        light->name = name;
        light->position = position;
        light->rotation = rotation;
        light->direction = direction;
        light->ambient = ambient;
        light->diffuse = diffuse;
        light->specular = specular;
        light->attenuation = attenuation;
        light->cutoffAngle = cutoffAngle;
        light->outerCutoffAngle = outerCutoffAngle;
        light->enabled = enabled;
        light->isActive = isActive;
        light->dirty = true;

        if(light->typeLight == LightType::DirectionalLight)
        {
            light->meshes = MeshManager::Get()->LoadMeshes("Assets/Models/Light.obj", 0);
            light->myTexture = std::make_unique<Texture>("Assets/Textures/Light.png");
        } else if (light->typeLight == LightType::PointLight)
        {
            light->meshes = MeshManager::Get()->LoadMeshes("Assets/Models/PointLight.obj", 0);
            light->myTexture = std::make_unique<Texture>("Assets/Textures/PointLight.png");
        } else 
        {
            light->meshes = MeshManager::Get()->LoadMeshes("Assets/Models/Light.obj", 0);
            light->myTexture = std::make_unique<Texture>("Assets/Textures/Light.png");
        }

        loadedLights.push_back(light);
        lightParentIndices.push_back(parentIndex);
        modelList->push_back(light);
        scene->AddRenderable(light);
    }

    file.close();

    // Restore parent relationships for models
    for (size_t i = 0; i < loadedModels.size(); ++i)
    {
        if (parentIndices[i] >= 0 && parentIndices[i] < static_cast<int32_t>(loadedModels.size()))
        {
            loadedModels[i]->SetParent(loadedModels[parentIndices[i]]);
        }
    }

    // Restore parent relationships for cameras
    for (size_t i = 0; i < loadedCameras.size(); ++i)
    {
        if (cameraParentIndices[i] >= 0 && cameraParentIndices[i] < static_cast<int32_t>(loadedModels.size()))
        {
            loadedCameras[i]->SetParent(loadedModels[cameraParentIndices[i]]);
        }
    }

    // Restore parent relationships for lights
    for (size_t i = 0; i < loadedLights.size(); ++i)
    {
        if (lightParentIndices[i] >= 0 && lightParentIndices[i] < static_cast<int32_t>(loadedModels.size()))
        {
            loadedLights[i]->SetParent(loadedModels[lightParentIndices[i]]);
        }
    }

    if (mainCameraToSet)
    {
        scene->SetMainCamera(mainCameraToSet);
        scene->SetCamera(mainCameraToSet);
    }

    scene->BuildEmbreeScene();

    std::cout << "SceneSerializer: Scene loaded successfully from " << filepath << std::endl;
    std::cout << "  Models: " << modelCount << ", Cameras: " << cameraCount << std::endl;
    return true;
}

std::vector<std::string> SceneSerializer::GetAvailableScenes(const std::string& directory)
{
    std::vector<std::string> scenes;

    if (!std::filesystem::exists(directory))
    {
        std::filesystem::create_directories(directory);
        return scenes;
    }

    for (const auto& entry : std::filesystem::directory_iterator(directory))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".scene")
        {
            scenes.push_back(entry.path().filename().string());
        }
    }

    std::sort(scenes.begin(), scenes.end());
    return scenes;
}