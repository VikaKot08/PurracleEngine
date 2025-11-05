#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <cfloat>  // for FLT_MAX
#include <algorithm>

Model::Model(const std::string& path)
{
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
	for (auto mesh : meshes)
		mesh->Render(myShader);

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

    // Check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    std::cout << "Model loaded successfully!" << std::endl;
    std::cout << "Number of meshes: " << scene->mNumMeshes << std::endl;

    // Process all meshes in the scene
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

        std::cout << "  Mesh " << i << ": " << vertices.size() << " vertices, "
            << indices.size() << " indices" << std::endl;

        // Create mesh
        meshes.push_back(new Mesh(vertices, indices));
    }

    std::cout << "Total meshes loaded: " << meshes.size() << std::endl;

    std::cout << "Total meshes loaded: " << meshes.size() << std::endl;

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

    std::cout << "Model bounds - Min: (" << minBounds.x << ", " << minBounds.y << ", " << minBounds.z << ")" << std::endl;
    std::cout << "Model bounds - Max: (" << maxBounds.x << ", " << maxBounds.y << ", " << maxBounds.z << ")" << std::endl;
    std::cout << "Model size: (" << size.x << ", " << size.y << ", " << size.z << ")" << std::endl;
    std::cout << "Model center: (" << center.x << ", " << center.y << ", " << center.z << ")" << std::endl;
}