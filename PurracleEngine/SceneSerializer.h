#pragma once
#include <string>
#include <vector>
#include <fstream>

class Scene;
class Model;
class Camera;

class SceneSerializer
{
public:
    static bool SaveScene(Scene* scene, const std::string& filepath);
    static bool LoadScene(Scene* scene, const std::string& filepath, std::vector<Model*>* modelList);

    static std::vector<std::string> GetAvailableScenes(const std::string& directory = "Assets/Scenes");
    static std::string GetSceneDirectory() { return "Assets/Scenes"; }

private:
    static const uint32_t MAGIC_NUMBER = 0x53434E45; // "SCNE"
    static const uint32_t VERSION = 1;

    static void WriteString(std::ofstream& file, const std::string& str);
    static std::string ReadString(std::ifstream& file);

    static int FindModelIndex(Model* model, const std::vector<Model*>& models);
};

