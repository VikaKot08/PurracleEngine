#pragma once
#include <embree4/rtcore.h>
#include <glm/glm.hpp>
#include <vector>

class Model;

class RayTracer
{
public:
    RayTracer();
    ~RayTracer();

    void BuildScene(const std::vector<Model*>* models);
    void UpdateTransforms();
    void Rebuild();
    Model* TraceRay(const glm::vec3& origin, const glm::vec3& direction);
    void Clear();

private:
    struct GeometryInfo {
        Model* model;
        uint32_t geomID;
        RTCGeometry instance;
    };

    RTCDevice device;
    RTCScene rtcScene;
    const std::vector<Model*>* modelList;
    std::vector<GeometryInfo> geometryMap;

    void AddModelToScene(Model* model);
    void UpdateInstanceTransform(RTCGeometry instance, Model* model);
};