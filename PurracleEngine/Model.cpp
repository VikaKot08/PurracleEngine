#include "Model.h"
#include "Texture.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <cfloat>  // for FLT_MAX
#include <algorithm>

Model::Model(const std::string& path, const char* pathTex)
{
    meshIndex = 0;
    textureIndex = 0;
    myTexture = std::make_unique<Texture>(pathTex);
	LoadModel(path);
}

Model::~Model()
{
	for (auto mesh : meshes)
		delete mesh;
	meshes.clear();
}
void Model::Render(Shader* myShader) 
{
	myShader->SetMatrix4(GetMatrix(), "transform");
    myTexture->Bind(0);
    myShader->SetInt(0, "myTexture");
	for (auto mesh : meshes)
		mesh->Render(myShader);
    myTexture->Unbind();
}

void Model::LoadModelSimple(const std::string& path)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |           
        aiProcess_GenNormals |            
        aiProcess_FlipUVs |               
        aiProcess_CalcTangentSpace |      
        aiProcess_JoinIdenticalVertices   
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // Reserve space
        vertices.reserve(mesh->mNumVertices);
        indices.reserve(mesh->mNumFaces * 3);

        // Process vertices
        for (unsigned int v = 0; v < mesh->mNumVertices; v++)
        {
            Vertex vertex;

            // Position
            vertex.Position = glm::vec3(
                mesh->mVertices[v].x,
                mesh->mVertices[v].y,
                mesh->mVertices[v].z
            );

            // Normal
            if (mesh->HasNormals())
            {
                vertex.Normal = glm::vec3(
                    mesh->mNormals[v].x,
                    mesh->mNormals[v].y,
                    mesh->mNormals[v].z
                );
            }
            else
            {
                vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
            }

            // Texture coordinates
            if (mesh->mTextureCoords[0]) // Check if mesh has texture coordinates
            {
                vertex.TexCoords = glm::vec2(
                    mesh->mTextureCoords[0][v].x,
                    mesh->mTextureCoords[0][v].y
                );
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        // Process indices
        for (unsigned int f = 0; f < mesh->mNumFaces; f++)
        {
            aiFace face = mesh->mFaces[f];
            for (unsigned int idx = 0; idx < face.mNumIndices; idx++)
            {
                indices.push_back(face.mIndices[idx]);
            }
        }

        meshes.push_back(new Mesh(vertices, indices));
    }

    // Calculate bounding box
    glm::vec3 minBounds(FLT_MAX);
    glm::vec3 maxBounds(-FLT_MAX);

    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];
        for (unsigned int v = 0; v < mesh->mNumVertices; v++)
        {
            minBounds.x = std::min(minBounds.x, mesh->mVertices[v].x);
            minBounds.y = std::min(minBounds.y, mesh->mVertices[v].y);
            minBounds.z = std::min(minBounds.z, mesh->mVertices[v].z);

            maxBounds.x = std::max(maxBounds.x, mesh->mVertices[v].x);
            maxBounds.y = std::max(maxBounds.y, mesh->mVertices[v].y);
            maxBounds.z = std::max(maxBounds.z, mesh->mVertices[v].z);
        }
    }

    glm::vec3 size = maxBounds - minBounds;
    glm::vec3 center = (minBounds + maxBounds) * 0.5f;
}

void Model::LoadModel(const std::string& path)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    // Calculate bounding box
    glm::vec3 minBounds(FLT_MAX);
    glm::vec3 maxBounds(-FLT_MAX);

    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];
        for (unsigned int v = 0; v < mesh->mNumVertices; v++)
        {
            minBounds.x = std::min(minBounds.x, mesh->mVertices[v].x);
            minBounds.y = std::min(minBounds.y, mesh->mVertices[v].y);
            minBounds.z = std::min(minBounds.z, mesh->mVertices[v].z);

            maxBounds.x = std::max(maxBounds.x, mesh->mVertices[v].x);
            maxBounds.y = std::max(maxBounds.y, mesh->mVertices[v].y);
            maxBounds.z = std::max(maxBounds.z, mesh->mVertices[v].z);
        }
    }

    // Compute center
    glm::vec3 center = (minBounds + maxBounds) * 0.5f;

    // Shift all meshes so pivot is at center
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];

        for (unsigned int v = 0; v < mesh->mNumVertices; v++)
        {
            mesh->mVertices[v].x -= center.x;
            mesh->mVertices[v].y -= center.y;
            mesh->mVertices[v].z -= center.z;
        }
    }

    // Load meshes
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        vertices.reserve(mesh->mNumVertices);
        indices.reserve(mesh->mNumFaces * 3);

        // Process vertices
        for (unsigned int v = 0; v < mesh->mNumVertices; v++)
        {
            Vertex vertex;

            // Position (already centered)
            vertex.Position = glm::vec3(
                mesh->mVertices[v].x,
                mesh->mVertices[v].y,
                mesh->mVertices[v].z
            );

            // Normal
            if (mesh->HasNormals())
            {
                vertex.Normal = glm::vec3(
                    mesh->mNormals[v].x,
                    mesh->mNormals[v].y,
                    mesh->mNormals[v].z
                );
            }
            else
            {
                vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
            }

            // Texture coordinates
            if (mesh->mTextureCoords[0])
            {
                vertex.TexCoords = glm::vec2(
                    mesh->mTextureCoords[0][v].x,
                    mesh->mTextureCoords[0][v].y
                );
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        // Process indices
        for (unsigned int f = 0; f < mesh->mNumFaces; f++)
        {
            aiFace face = mesh->mFaces[f];
            for (unsigned int idx = 0; idx < face.mNumIndices; idx++)
            {
                indices.push_back(face.mIndices[idx]);
            }
        }

        meshes.push_back(new Mesh(vertices, indices));
    }
}

glm::mat4 Model::GetModelMatrix() const
{
    glm::mat4 transform = glm::mat4(1.0f);

    // Apply transformations in order: Scale -> Rotate -> Translate
    transform = glm::translate(transform, position);

    transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0, 0, 1));

    transform = glm::scale(transform, scale);

    return transform;
}