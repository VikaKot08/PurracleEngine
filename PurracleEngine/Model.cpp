#include "Model.h"
#include "Texture.h"
#include "MeshManager.h"
#include <iostream>

Model::Model(const std::string& aPath, const char* aPathTex)
    : path(aPath),
    pathTex(aPathTex),
    meshes(MeshManager::Get()->LoadMeshes(aPath)),
    myTexture(std::make_unique<Texture>(aPathTex)),
    meshIndex(0),
    textureIndex(0)
{}

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
    myShader->SetMatrix4(GetMatrix(), "transform");
    myTexture->Bind(0);
    myShader->SetInt(0, "myTexture");

    if (meshes)
    {
        for (auto mesh : *meshes)
            mesh->Render(myShader);
    }

    myTexture->Unbind();
}

void Model::ChangeMesh(const std::string& newPath)
{
    if (newPath == path)
        return;

    if (!path.empty())
    {
        MeshManager::Get()->ReleaseMeshes(path);
    }

    path = newPath;
    meshes = MeshManager::Get()->LoadMeshes(path);
}