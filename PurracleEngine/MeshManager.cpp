#include <windows.h>
#include "MeshManager.h"
#include "Mesh.h"
#include "Message.h"
#include "MessageQueue.h"
#include "MeshLoadedMessage.h"
#include "LoadMeshMessage.h"
#include "MemoryMessage.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <cfloat>
#include <algorithm>

#include <fstream>
#include <istream>
#include <sstream>
#include <filesystem>

MeshManager* MeshManager::instance = nullptr;

MeshManager::MeshManager()
{
}

MeshManager::~MeshManager()
{
    ClearCache();
}

void MeshManager::Allocate()
{
    if (instance == nullptr)
    {
        instance = new MeshManager();
    }
}

void MeshManager::Deallocate()
{
    if (instance != nullptr)
    {
        delete instance;
        instance = nullptr;
    }
}

MeshManager* MeshManager::Get()
{
    return instance;
}

void MeshManager::ReleaseMeshes(const std::string& path)
{
    auto it = meshCache.find(path);
    if (it != meshCache.end())
    {
        it->second.refCount--;
        if (it->second.refCount <= 0)
        {
            for (Mesh* mesh : it->second.meshes)
            {
                delete mesh;
            }
            meshCache.erase(it);
        }
    }
}

void MeshManager::ClearCache()
{
    for (auto& pair : meshCache)
    {
        for (Mesh* mesh : pair.second.meshes)
        {
            delete mesh;
        }
    }
    meshCache.clear();
}

bool MeshManager::CheckMemory(int msgId)
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    DWORDLONG physMemory = statex.ullAvailPhys / (1024 * 1024);
    DWORDLONG virMemory = statex.ullAvailVirtual / (1024 * 1024);

    std::cout << physMemory << " MB of physical memory" << std::endl;
    std::cout << virMemory << " MB of virtual memory" << std::endl;

    std::stringstream ss;
    ss << "Available Memory:\n"
        << physMemory << " MB Physical Memory\n"
        << virMemory << " MB Virtual Memory";

    if (guiQueue != nullptr)
    {
        MemoryMessage* guiMsg = new MemoryMessage(ss.str(), true, msgId);
        guiQueue->QueueMessage(guiMsg);
    }
    if (replyQueue != nullptr)
    {
        MemoryMessage* response = new MemoryMessage("Completed checking memory", false, msgId);
        replyQueue->QueueMessage(response);
    }

    if (physMemory < 500)
    {
        if (guiQueue != nullptr)
        {
            MemoryMessage* warningMsg = new MemoryMessage(
                "WARNING: Low memory! Less than 500MB available.\nMesh loading may fail.",
                false,
                msgId
            );
            guiQueue->QueueMessage(warningMsg);
        }
        return false;
    }

    return true;
}

std::vector<Mesh*>* MeshManager::GetFromChache(const std::string& path)
{
    auto it = meshCache.find(path);
    if (it != meshCache.end())
    {
        it->second.refCount++;
        return &it->second.meshes;
    } else 
    {
        std::cout << "Mesh Chache for: " + path + " is nullptr." << std::endl;
        return nullptr;
    }
}

std::vector<Mesh*>* MeshManager::LoadMeshes(const std::string& path, int msgId)
{
    auto it = meshCache.find(path);
    if (it != meshCache.end())
    {
        it->second.refCount++;
        return &it->second.meshes;
    }

    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << path << std::endl;
        return nullptr;
    }

    if (!CheckMemory(msgId)) return nullptr;

    std::string optimizedPath = GetOptimizedPath(path);

    // Try to load from optimized file if it exists and is newer
    if (HasOptimizedFile(path) && IsOptimizedFileNewer(path, optimizedPath))
    {
        std::cout << "Loading from optimized file: " << optimizedPath << std::endl;
        std::vector<Mesh*>* result = Deserialize(path);
        if (result != nullptr)
        {
            return result;
        }
        std::cout << "Failed to load optimized file, falling back to original" << std::endl;
    }
    return LoadMeshesObj(path);
}

std::vector<Mesh*>* MeshManager::LoadMeshesObj(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << path << std::endl;
        return nullptr;
    }

    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "v")
        {
            glm::vec3 v;
            iss >> v.x >> v.y >> v.z;
            temp_vertices.push_back(v);
        }
        else if (token == "vt")
        {
            glm::vec2 uv;
            iss >> uv.x >> uv.y;
            temp_uvs.push_back(uv);
        }
        else if (token == "vn")
        {
            glm::vec3 n;
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
            }

            // Triangulation loop
            for (size_t k = 1; k + 1 < vI.size(); k++)
            {
                unsigned int tri[3] = { 0, k, k + 1 };
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

        vertex.Position = temp_vertices[vertexIndices[i] - 1];

        if (uvIndices[i] > 0)
        {
            vertex.TexCoords = temp_uvs[uvIndices[i] - 1];
            vertex.TexCoords.y = 1.0f - vertex.TexCoords.y;
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        if (normalIndices[i] > 0)
        {
            vertex.Normal = temp_normals[normalIndices[i] - 1];
        }
        else
        {
            vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
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

    MeshData meshData;
    meshData.path = path;
    meshData.refCount = 1;
    meshData.meshes.push_back(new Mesh(vertices, indices));

    meshCache[path] = meshData;
    return &meshCache[path].meshes;
}

void MeshManager::SetReplyQueue(MessageQueue* replyQueue)
{
    this->replyQueue = replyQueue;
}

void MeshManager::ProcessMessage(Message* aMessage)
{
    if (aMessage->type == MessageType::LoadMesh)
    {
        LoadMeshMessage* loadMsg = static_cast<LoadMeshMessage*>(aMessage);
        std::cout << "MeshManager: Received LoadMesh request for: " << loadMsg->msg << std::endl;

        std::vector<Mesh*>* meshes = LoadMeshes(loadMsg->msg, loadMsg->requestId);

        if (replyQueue != nullptr)
        {
            if (meshes != nullptr)
            {
                std::cout << "MeshManager: Successfully loaded mesh, sending MeshLoaded message" << std::endl;
                MeshLoadedMessage* response = new MeshLoadedMessage(loadMsg ->model, loadMsg->msg, true, loadMsg->requestId);
                replyQueue->QueueMessage(response);
            }
            else
            {
                std::cout << "MeshManager: Failed to load mesh, sending failure message" << std::endl;
                MeshLoadedMessage* response = new MeshLoadedMessage(loadMsg -> model, loadMsg->msg, false, loadMsg->requestId);
                response->errorMessage = "Failed to load mesh from path: " + loadMsg->msg;
                replyQueue->QueueMessage(response);
            }
        }
    }
    else
    {
        MessageQueue::ProcessMessage(aMessage);
    }
}

void MeshManager::Serialize(const std::string& path, const std::vector<Mesh*>& meshes)
{
    std::string optimizedPath = GetOptimizedPath(path);

    std::fstream file;
    file.open(optimizedPath, std::ios_base::out | std::ios_base::binary);
    //Write meta-data
    // Write number of meshes
    int numMeshes = static_cast<int>(meshes.size());
    file.write(reinterpret_cast<const char*>(&numMeshes), sizeof(int));

    for (Mesh* mesh : meshes)
    {
        const std::vector<Vertex>& vertices = mesh->vertices;
        const std::vector<unsigned int>& indices = mesh->indicies;

        int numVertices = static_cast<int>(vertices.size());
        file.write((char*)&numVertices, sizeof(int));

        int numIndices = static_cast<int>(indices.size());
        file.write((char*)&numIndices, sizeof(int));

        // Write vertex data
        for (const Vertex& v : vertices)
        {
            file.write((char*)&v.Position, sizeof(glm::vec3));
            file.write((char*)&v.Normal, sizeof(glm::vec3));
            file.write((char*)&v.TexCoords, sizeof(glm::vec2));
        }

        // Write index data
        //for (const unsigned int& i : indices)
        //{
            //file.write((char*)&i, sizeof(unsigned int));
        //}
        file.write((char*)indices.data(), numIndices * sizeof(unsigned int));
    }
    file.close();
}

std::vector<Mesh*>* MeshManager::Deserialize(const std::string& path)
{
    std::string optimizedPath = GetOptimizedPath(path);
    
    std::fstream file;
    file.open(optimizedPath, std::ios_base::in | std::ios_base::binary);
    //Read meta data
    int numMeshes = 0;
    file.read((char*)&numMeshes, sizeof(int));

    if (numMeshes <= 0 || numMeshes > 50000)
    {
        std::cerr << "Invalid file: " << numMeshes << std::endl;
        file.close();
        return nullptr;
    }

    MeshData meshData;
    meshData.path = path;
    meshData.refCount = 1;

    for(int i = 0; i < numMeshes; ++i)
    {
        int numVertices = 0;
        int numIndices = 0;

        file.read((char*)&numVertices, sizeof(int));
        file.read((char*)&numIndices, sizeof(int));

        if (numVertices <= 0 || numVertices > 50000000 || numIndices <= 0 || numIndices > 50000000)
        {
            std::cerr << "Invalid file: Vertices: " << numVertices << " Indices: " << numIndices << std::endl;
            file.close();
            return nullptr;
        }

        std::vector<Vertex> vertices(numVertices);
        std::vector<unsigned int> indices(numIndices);

        //Read vertices
        for (int i = 0; i < numVertices; ++i)
        {
            file.read((char*)&vertices[i].Position, sizeof(glm::vec3));
            file.read((char*)&vertices[i].Normal, sizeof(glm::vec3));
            file.read((char*)&vertices[i].TexCoords, sizeof(glm::vec2));
        }

        //Read indices
        //for (int i = 0; i < numIndices; ++i)
        //{
            //file.read((char*)&indices[i], sizeof(unsigned int));
        //}
        file.read((char*)indices.data(), numIndices * sizeof(unsigned int));

        meshData.meshes.push_back(new Mesh(vertices, indices));
    }

    file.close();

    meshCache[path] = meshData;

    return &meshCache[path].meshes;
}

std::string MeshManager::GetOptimizedPath(const std::string& originalPath)
{
    std::filesystem::path p(originalPath);
    std::filesystem::path optimized = p;
    optimized.replace_extension(".opt");
    return optimized.string();
}

bool MeshManager::HasOptimizedFile(const std::string& originalPath)
{
    std::string optPath = GetOptimizedPath(originalPath);
    return std::filesystem::exists(optPath);
}

bool MeshManager::IsOptimizedFileNewer(const std::string& originalPath, const std::string& optimizedPath)
{
    if (!std::filesystem::exists(originalPath) || !std::filesystem::exists(optimizedPath))
        return false;

    auto originalTime = std::filesystem::last_write_time(originalPath);
    auto optimizedTime = std::filesystem::last_write_time(optimizedPath);

    return optimizedTime >= originalTime;
}