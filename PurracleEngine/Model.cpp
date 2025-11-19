#include "Model.h"
#include "Texture.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <cfloat> 
#include <algorithm>

#include <fstream>
#include <istream>
#include <sstream>
#include <map>

Model::Model(const std::string& aPath, const char* pathTex) : path(aPath)
{
    meshIndex = 0;
    textureIndex = 0;
    myTexture = std::make_unique<Texture>(pathTex);
    OwnLoadOBJ(aPath);
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

        vertices.reserve(mesh->mNumVertices);
        indices.reserve(mesh->mNumFaces * 3);

        for (unsigned int v = 0; v < mesh->mNumVertices; v++)
        {
            Vertex vertex;

            vertex.Position = glm::vec3(
                mesh->mVertices[v].x,
                mesh->mVertices[v].y,
                mesh->mVertices[v].z
            );

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

    glm::vec3 center = (minBounds + maxBounds) * 0.5f;

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

    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        vertices.reserve(mesh->mNumVertices);
        indices.reserve(mesh->mNumFaces * 3);

        for (unsigned int v = 0; v < mesh->mNumVertices; v++)
        {
            Vertex vertex;

            vertex.Position = glm::vec3(
                mesh->mVertices[v].x,
                mesh->mVertices[v].y,
                mesh->mVertices[v].z
            );

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

void Model::OwnLoadOBJ(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << path << std::endl;
        return;
    }

    std::vector<Position> temp_vertices;
    std::vector<Position> temp_uvs;
    std::vector<Position> temp_normals;

    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "v")
        {
            Position v;
            iss >> v.x >> v.y >> v.z;
            temp_vertices.push_back(v);
        }
        else if (token == "vt")
        {
            Position uv;
            iss >> uv.x >> uv.y;
            temp_uvs.push_back(uv);
        }
        else if (token == "vn")
        {
            Position n;
            iss >> n.x >> n.y >> n.z;
            temp_normals.push_back(n);
        }
        else if (token == "f")
        {
            std::string fd;
            std::vector<unsigned int> vI, uvI, nI;

            while (iss >> fd)
            {
                unsigned int v = 0, t = 0, n = 0;

                // v/vt/vn
                if (sscanf_s(fd.c_str(), "%u/%u/%u", &v, &t, &n) == 3)
                {
                    vI.push_back(v);
                    uvI.push_back(t);
                    nI.push_back(n);
                }
                // v//vn
                else if (sscanf_s(fd.c_str(), "%u//%u", &v, &n) == 2)
                {
                    vI.push_back(v);
                    uvI.push_back(0);
                    nI.push_back(n);
                }
                // v/vt
                else if (sscanf_s(fd.c_str(), "%u/%u", &v, &t) == 2)
                {
                    vI.push_back(v);
                    uvI.push_back(t);
                    nI.push_back(0);
                }
                // v only
                else if (sscanf_s(fd.c_str(), "%u", &v) == 1)
                {
                    vI.push_back(v);
                    uvI.push_back(0);
                    nI.push_back(0);
                }
                else
                {
                    continue;
                }
            }

            //Triangulation loop
            for (size_t k = 1; k + 1 < vI.size(); k++)
            {
                unsigned int tri[3] = { 0, k, k + 1};

                for (int s = 0; s < 3; s++)
                {
                    vertexIndices.push_back(vI[tri[s]]);
                    uvIndices.push_back(uvI[tri[s]]);
                    normalIndices.push_back(nI[tri[s]]);
                }
            }
        }
    }

    file.close();

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < vertexIndices.size(); i++)
    {
        Vertex vertex;

        vertex.Position = glm::vec3{ temp_vertices[vertexIndices[i] - 1].x, temp_vertices[vertexIndices[i] - 1].y, temp_vertices[vertexIndices[i] - 1].z };

        if (uvIndices[i] > 0) 
        {
            vertex.TexCoords = glm::vec2{
                temp_uvs[uvIndices[i] - 1].x,
                temp_uvs[uvIndices[i] - 1].y
            };
            vertex.TexCoords.y = 1.0f - vertex.TexCoords.y;
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        if (normalIndices[i] > 0) 
        {
            vertex.Normal = glm::vec3{
                temp_normals[normalIndices[i] - 1].x,
                temp_normals[normalIndices[i] - 1].y,
                temp_normals[normalIndices[i] - 1].z
            };
        }
        else
        {
            vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f); // default normal
        }

        vertices.push_back(vertex);
        indices.push_back(i);
    }

    glm::vec3 minB(FLT_MAX), maxB(-FLT_MAX);
    for (auto& v : vertices)
    {
        minB = glm::min(minB, v.Position);
        maxB = glm::max(maxB, v.Position);
    }

    glm::vec3 center = (minB + maxB) * 0.5f;
    for (auto& v : vertices)
        v.Position -= center;

    meshes.push_back(new Mesh(vertices, indices));
}