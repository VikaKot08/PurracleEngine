#include "AssetBrowser.h"
#include <filesystem>
#include <algorithm>

AssetBrowser::AssetBrowser()
{
    RefreshAssets();
}

void AssetBrowser::RefreshAssets()
{
    availableMeshes.clear();
    availableTextures.clear();

    const std::string meshDir = "Assets/Models";
    const std::string textureDir = "Assets/Textures";

    const std::vector<std::string> meshExtensions = {
        ".obj", ".fbx", ".gltf", ".glb", ".dae", ".blend", ".3ds", ".ply"
    };

    const std::vector<std::string> textureExtensions = {
        ".png", ".jpg", ".jpeg", ".bmp", ".tga", ".dds", ".hdr"
    };

    if (std::filesystem::exists(meshDir) && std::filesystem::is_directory(meshDir))
    {
        for (const auto& entry : std::filesystem::directory_iterator(meshDir))
        {
            if (entry.is_regular_file())
            {
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                if (std::find(meshExtensions.begin(), meshExtensions.end(), ext) != meshExtensions.end())
                {
                    availableMeshes.push_back(entry.path().string());
                }
            }
        }
    }

    if (std::filesystem::exists(textureDir) && std::filesystem::is_directory(textureDir))
    {
        for (const auto& entry : std::filesystem::directory_iterator(textureDir))
        {
            if (entry.is_regular_file())
            {
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                if (std::find(textureExtensions.begin(), textureExtensions.end(), ext) != textureExtensions.end())
                {
                    availableTextures.push_back(entry.path().string());
                }
            }
        }
    }

    std::sort(availableMeshes.begin(), availableMeshes.end());
    std::sort(availableTextures.begin(), availableTextures.end());

    UpdateFileNames();
}

void AssetBrowser::UpdateFileNames()
{
    meshFilenames.clear();
    meshNames.clear();
    textureFilenames.clear();
    textureNames.clear();

    meshFilenames.reserve(availableMeshes.size());
    meshNames.reserve(availableMeshes.size());
    textureFilenames.reserve(availableTextures.size());

    for (const auto& mesh : availableMeshes)
    {
        if (mesh.empty()) continue;

        std::filesystem::path p(mesh);
        std::string filename = p.stem().string();
        if (filename.empty()) continue;

        meshFilenames.push_back(filename);
        meshNames.push_back(meshFilenames.back().c_str());
    }

    for (const auto& tex : availableTextures)
    {
        if (tex.empty()) continue;

        std::filesystem::path p(tex);
        std::string filename = p.stem().string();
        if (filename.empty()) continue;

        textureFilenames.push_back(filename);
        textureNames.push_back(textureFilenames.back().c_str());
    }
}

int AssetBrowser::FindMeshIndex(const std::string& meshPath) const
{
    std::string normalizedSearch = meshPath;
    std::replace(normalizedSearch.begin(), normalizedSearch.end(), '\\', '/');

    for (size_t i = 0; i < availableMeshes.size(); i++)
    {
        std::string normalizedAvailable = availableMeshes[i];
        std::replace(normalizedAvailable.begin(), normalizedAvailable.end(), '\\', '/');

        if (normalizedAvailable == normalizedSearch)
            return static_cast<int>(i);
    }
    return 0;
}

int AssetBrowser::FindTextureIndex(const std::string& texturePath) const
{
    std::string normalizedSearch = texturePath;
    std::replace(normalizedSearch.begin(), normalizedSearch.end(), '\\', '/');

    for (size_t i = 0; i < availableTextures.size(); i++)
    {
        std::string normalizedAvailable = availableTextures[i];
        std::replace(normalizedAvailable.begin(), normalizedAvailable.end(), '\\', '/');

        if (normalizedAvailable == normalizedSearch)
            return static_cast<int>(i);
    }
    return 0;
}
