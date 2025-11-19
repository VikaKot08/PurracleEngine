#pragma once
#include "Camera.h"
#include "FlyingCamera.h"
#include "Renderable.h"
#include <embree4/rtcore.h>
#include <vector>

class Model;
class ModelManager;

class Scene
{
public:
    Scene();
    ~Scene();

    Model* LoadModel(const std::string& aPath, const char* aPathTex);
    void AddRenderable(Model* aModel);
    void DeleteModel(Renderable* renderable);
    std::vector<Renderable*> GetRenderables() const { return renderables; }
    Camera* GetCamera() const { return mainCamera; }
    void UpdateFlyingCamera(glm::vec4 direction, float deltaTime);

    void BuildEmbreeScene();
    void UpdateEmbreeTransforms();
    Model* TraceRay(const glm::vec3& origin, const glm::vec3& direction);
    void MarkDirty() { sceneNeedsUpdate = true; }
    ModelManager* GetModelManager() { return modelManager; }


    std::vector<Renderable*> renderables;

private:
    struct GeometryInfo {
        Model* model;
        uint32_t geomID;
        RTCGeometry instance;
        RTCScene baseScene;
    };

    Camera* mainCamera;
    FlyingCamera* flyingCamera;
    ModelManager* modelManager;

    RTCDevice embreeDevice;
    RTCScene embreeScene;
    std::vector<GeometryInfo> geometryMap;
    bool sceneNeedsUpdate;

    void AddModelToEmbreeScene(Model* model);
    void UpdateInstanceTransform(RTCGeometry instance, Model* model);
    void ClearEmbreeScene();
};