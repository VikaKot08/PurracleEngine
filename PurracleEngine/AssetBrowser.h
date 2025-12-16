#pragma once
#include <vector>
#include <string>

class Model;

class AssetBrowser
{
public:
    AssetBrowser();

    void RefreshAssets();

    const std::vector<std::string>& GetMeshes() const { return availableMeshes; }
    const std::vector<std::string>& GetTextures() const { return availableTextures; }
    const std::vector<const char*>& GetMeshNames() const { return meshNames; }
    const std::vector<const char*>& GetTextureNames() const { return textureNames; }
    void VerifyNames(std::vector<Model*> models);

    int FindMeshIndex(const std::string& meshPath) const;
    int FindTextureIndex(const std::string& texturePath) const;

private:
    std::vector<std::string> availableMeshes;
    std::vector<std::string> availableTextures;

    std::vector<std::string> meshFilenames;
    std::vector<std::string> textureFilenames;

    std::vector<const char*> meshNames;
    std::vector<const char*> textureNames;

    void UpdateFileNames();
};

