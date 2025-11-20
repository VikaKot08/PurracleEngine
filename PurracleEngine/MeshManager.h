#pragma once
#include <vector>
#include <string>
#include <map>

class Mesh;

struct MeshData
{
    std::string path;
    std::vector<Mesh*> meshes;
    int refCount = 0;
};

class MeshManager
{
private:
    static MeshManager* instance;
    std::map<std::string, MeshData> meshCache;

    MeshManager();
    ~MeshManager();

public:
    static void Allocate();
    static void Deallocate();
    static MeshManager* Get();

    std::vector<Mesh*>* LoadMeshesAssimp(const std::string& path);

    std::vector<Mesh*>* LoadMeshes(const std::string& path);

    void ReleaseMeshes(const std::string& path);

    void ClearCache();
};
