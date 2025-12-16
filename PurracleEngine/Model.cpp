#include "Model.h"
#include "Texture.h"
#include "MeshManager.h"
#include <iostream>

Model::Model(const std::string& aPath, const char* aPathTex)
    : path(aPath),
    pathTex(aPathTex),
    meshes(nullptr),
    myTexture(std::make_unique<Texture>(aPathTex)),
    meshIndex(0),
    textureIndex(0)
{
    type = ModelType::Normal;
}

Model::Model()
    : path("Assets/Models/Camera.obj"),
    pathTex("Assets/Textures/Camera.jpg"),
    meshes(nullptr),
    myTexture(std::make_unique<Texture>("Assets/Textures/Camera.jpg")),
    meshIndex(0),
    textureIndex(0)
{
    scale = glm::vec3{ 1.0f };
    type = ModelType::CameraModel;
    name = "Flying Camera";
}

Model::~Model()
{
    if (!path.empty())
    {
        MeshManager::Get()->ReleaseMeshes(path);
    }
    meshes = nullptr;
}

void Model::Render(Shader* myShader)
{
    if (type == ModelType::CameraModel && isActive) return;
    myShader->SetMatrix4(GetMatrix(), "transform");
    myTexture->Bind(0);
    myShader->SetInt(0, "myTexture");

    if(myShader->useLight)
    {
        myTexture->Bind(1);
        myShader->SetInt(1, "specularMap");
    }

    if (meshes)
    {
        for (auto mesh : *meshes)
            mesh->Render(myShader);
    }

    myTexture->Unbind();
}