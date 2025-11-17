#include "RayTracer.h"
#include "Model.h"
#include <cstring>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

RayTracer::RayTracer()
    : device(nullptr)
    , rtcScene(nullptr)
    , modelList(nullptr)
{
    device = rtcNewDevice(nullptr);
    if (!device) {
        std::cerr << "Failed to create Embree device" << std::endl;
    }
}

RayTracer::~RayTracer()
{
    Clear();
    if (device) {
        rtcReleaseDevice(device);
    }
}

void RayTracer::Clear()
{
    if (rtcScene) {
        rtcReleaseScene(rtcScene);
        rtcScene = nullptr;
    }
    geometryMap.clear();
}

void RayTracer::BuildScene(const std::vector<Model*>* models)
{
    Clear();
    if (!models || models->empty()) {
        return;
    }

    modelList = models;
    rtcScene = rtcNewScene(device);

    // Set scene flags for dynamic scenes (allows transform updates)
    rtcSetSceneFlags(rtcScene, RTC_SCENE_FLAG_DYNAMIC);
    rtcSetSceneBuildQuality(rtcScene, RTC_BUILD_QUALITY_LOW); // Faster builds for dynamic scenes

    for (Model* model : *models) {
        AddModelToScene(model);
    }

    rtcCommitScene(rtcScene);
}

void RayTracer::AddModelToScene(Model* model)
{
    if (!model) return;

    std::vector<glm::vec3> localVertices;
    std::vector<unsigned int> localIndices;

    for (Mesh* mesh : model->meshes) {
        size_t baseVertex = localVertices.size();

        for (const Vertex& v : mesh->vertices) {
            localVertices.push_back(v.Position);
        }

        for (unsigned int idx : mesh->indicies) {
            localIndices.push_back(static_cast<unsigned int>(baseVertex + idx));
        }
    }

    if (localVertices.empty() || localIndices.empty()) {
        return;
    }

    RTCGeometry instance = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_INSTANCE);

    RTCScene baseScene = rtcNewScene(device);
    RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

    float* vertices = (float*)rtcSetNewGeometryBuffer(
        geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3,
        3 * sizeof(float), localVertices.size()
    );

    for (size_t i = 0; i < localVertices.size(); i++) {
        vertices[i * 3 + 0] = localVertices[i].x;
        vertices[i * 3 + 1] = localVertices[i].y;
        vertices[i * 3 + 2] = localVertices[i].z;
    }

    size_t triangleCount = localIndices.size() / 3;
    uint32_t* indices = (uint32_t*)rtcSetNewGeometryBuffer(
        geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3,
        3 * sizeof(uint32_t), triangleCount
    );
    memcpy(indices, localIndices.data(), localIndices.size() * sizeof(uint32_t));

    rtcCommitGeometry(geom);
    rtcAttachGeometry(baseScene, geom);
    rtcReleaseGeometry(geom);
    rtcCommitScene(baseScene);

    rtcSetGeometryInstancedScene(instance, baseScene);

    UpdateInstanceTransform(instance, model);

    rtcCommitGeometry(instance);
    uint32_t geomID = rtcAttachGeometry(rtcScene, instance);

    geometryMap.push_back({ model, geomID, instance });

}

void RayTracer::UpdateInstanceTransform(RTCGeometry instance, Model* model)
{
    glm::mat4 transform = model->GetMatrix();

    float transformMatrix[16];
    memcpy(transformMatrix, glm::value_ptr(glm::transpose(transform)), 16 * sizeof(float));

    rtcSetGeometryTransform(instance, 0, RTC_FORMAT_FLOAT4X4_COLUMN_MAJOR, transformMatrix);
}

void RayTracer::UpdateTransforms()
{
    if (!rtcScene || !modelList) return;

    for (auto& info : geometryMap) {
        if (info.model && info.instance) {
            UpdateInstanceTransform(info.instance, info.model);
            rtcCommitGeometry(info.instance);
        }
    }

    rtcCommitScene(rtcScene);
}

void RayTracer::Rebuild()
{
    if (modelList) {
        BuildScene(modelList);
    }
}

Model* RayTracer::TraceRay(const glm::vec3& origin, const glm::vec3& direction)
{
    if (!rtcScene) {
        return nullptr;
    }

    RTCRayHit rayhit;
    rayhit.ray.org_x = origin.x;
    rayhit.ray.org_y = origin.y;
    rayhit.ray.org_z = origin.z;
    rayhit.ray.dir_x = direction.x;
    rayhit.ray.dir_y = direction.y;
    rayhit.ray.dir_z = direction.z;
    rayhit.ray.tnear = 0.001f; 
    rayhit.ray.tfar = std::numeric_limits<float>::infinity();
    rayhit.ray.mask = 0xFFFFFFFF;
    rayhit.ray.flags = 0;
    rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

    RTCIntersectArguments args;
    rtcInitIntersectArguments(&args);
    rtcIntersect1(rtcScene, &rayhit, &args);

    if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
        for (const auto& info : geometryMap) {
            if (info.geomID == rayhit.hit.geomID) {
                std::cout << info.model << std::endl;
                return info.model;
            }
        }
    }

    return nullptr;
}