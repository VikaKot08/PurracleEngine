#pragma once
#include "Camera.h"
#include "MeshManager.h"
#include "FlyingCamera.h"
#include "Renderable.h"
#include <embree4/rtcore.h>
#include <vector>

class Model;
class Light;

class Scene
{
public:
    Scene();
    ~Scene();
    void SetMeshManager(MeshManager* aManager);

    Model* LoadModel(const std::string& aPath, const char* aPathTex);
    void AddRenderable(Model* aModel);
    void DeleteModel(Renderable* renderable);
    std::vector<Renderable*> GetRenderables() const { return renderables; }
    Camera* GetCamera() const { return activeCamera; }
    void UpdateFlyingCamera(glm::vec4 direction, float deltaTime);

    void BuildEmbreeScene();
    void UpdateEmbreeTransforms();
    Model* TraceRay(const glm::vec3& origin, const glm::vec3& direction);
    void MarkDirty() { sceneNeedsUpdate = true; }
    void InitializeDefaultModels();
    void SetCamera(Camera* aCamera);
    void SetMainCamera(Camera* aCamera);
    std::vector<Light*> GetLight();

    std::vector<Renderable*> renderables;

    Camera* mainCamera;

    FlyingCamera* flyingCamera;

private:
    struct GeometryInfo {
        Model* model;
        uint32_t geomID;
        RTCGeometry instance;
        RTCScene baseScene;
    };

    Camera* activeCamera;

    RTCDevice embreeDevice;
    RTCScene embreeScene;
    std::vector<GeometryInfo> geometryMap;
    bool sceneNeedsUpdate;
    MeshManager* meshManager;

    void AddModelToEmbreeScene(Model* model);
    void UpdateInstanceTransform(RTCGeometry instance, Model* model);
    void ClearEmbreeScene();
};