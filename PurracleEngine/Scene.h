#pragma once
#include "Camera.h"
#include "FlyingCamera.h"
#include "Renderable.h"
#include <embree4/rtcore.h>
#include <vector>

class Model;

class Scene
{
public:
    Scene();
    ~Scene();

    void AddRenderable(Renderable* renderable);
    std::vector<Renderable*> GetRenderables() const { return renderables; }
    Camera* GetCamera() const { return mainCamera; }
    void UpdateFlyingCamera(glm::vec4 direction, float deltaTime);

    // Embree ray tracing
    void BuildEmbreeScene();
    void UpdateEmbreeTransforms();
    Model* TraceRay(const glm::vec3& origin, const glm::vec3& direction);
    void MarkDirty() { sceneNeedsUpdate = true; }


    std::vector<Renderable*> renderables;

private:
    struct GeometryInfo {
        Model* model;
        uint32_t geomID;
        RTCGeometry instance;
        RTCScene baseScene;  // Store base scene for cleanup
    };

    Camera* mainCamera;
    FlyingCamera* flyingCamera;

    // Embree data
    RTCDevice embreeDevice;
    RTCScene embreeScene;
    std::vector<GeometryInfo> geometryMap;
    bool sceneNeedsUpdate;

    void AddModelToEmbreeScene(Model* model);
    void UpdateInstanceTransform(RTCGeometry instance, Model* model);
    void ClearEmbreeScene();
};