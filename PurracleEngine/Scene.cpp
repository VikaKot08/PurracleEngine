#include "Scene.h"
#include "Model.h"

#include "ModelManager.h"
#include <cstring>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

Scene::Scene()
    : mainCamera(nullptr)
    , embreeDevice(nullptr)
    , embreeScene(nullptr)
    , sceneNeedsUpdate(false)
{
    mainCamera = new Camera();

    flyingCamera = new FlyingCamera(mainCamera);

    modelManager = new ModelManager();
    modelManager->Allocate();

    embreeDevice = rtcNewDevice(nullptr);
    if (!embreeDevice) {
        std::cerr << "Failed to create Embree device" << std::endl;
    }
}

void Scene::UpdateFlyingCamera(glm::vec4 direction, float deltaTime)
{
    flyingCamera->Update(direction, deltaTime);
}

void Scene::DeleteModel(Renderable* model)
{
    auto it = std::find(renderables.begin(), renderables.end(), model);
    if (it != renderables.end())
    {
        delete model;
        renderables.erase(it);
    }
    BuildEmbreeScene();
}

Scene::~Scene()
{
    ClearEmbreeScene();

    if (embreeDevice) {
        rtcReleaseDevice(embreeDevice);
    }

    delete mainCamera;
}

void Scene::AddRenderable(Renderable* renderable)
{
    renderables.push_back(renderable);
    sceneNeedsUpdate = true;
}

void Scene::ClearEmbreeScene()
{
    for (auto& info : geometryMap) {
        if (info.instance) {
            rtcReleaseGeometry(info.instance);
        }
        if (info.baseScene) {
            rtcReleaseScene(info.baseScene);
        }
    }
    geometryMap.clear();

    if (embreeScene) {
        rtcReleaseScene(embreeScene);
        embreeScene = nullptr;
    }
}

void Scene::BuildEmbreeScene()
{
    ClearEmbreeScene();

    if (!embreeDevice || renderables.empty()) {
        return;
    }

    embreeScene = rtcNewScene(embreeDevice);
    rtcSetSceneFlags(embreeScene, RTC_SCENE_FLAG_DYNAMIC);
    rtcSetSceneBuildQuality(embreeScene, RTC_BUILD_QUALITY_LOW);

    for (Renderable* renderable : renderables) {
        Model* model = dynamic_cast<Model*>(renderable);
        if (model) {
            AddModelToEmbreeScene(model);
        }
    }

    rtcCommitScene(embreeScene);
    sceneNeedsUpdate = false;
}

void Scene::AddModelToEmbreeScene(Model* model)
{
    if (!model || !embreeDevice) return;

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
    RTCScene baseScene = rtcNewScene(embreeDevice);
    RTCGeometry geom = rtcNewGeometry(embreeDevice, RTC_GEOMETRY_TYPE_TRIANGLE);

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

    RTCGeometry instance = rtcNewGeometry(embreeDevice, RTC_GEOMETRY_TYPE_INSTANCE);
    rtcSetGeometryInstancedScene(instance, baseScene);

    UpdateInstanceTransform(instance, model);

    rtcCommitGeometry(instance);
    uint32_t geomID = rtcAttachGeometry(embreeScene, instance);
    geometryMap.push_back({ model, geomID, instance, baseScene });
}

void Scene::UpdateInstanceTransform(RTCGeometry instance, Model* model)
{
    glm::mat4 transform = model->GetMatrix();
    rtcSetGeometryTransform(instance, 0, RTC_FORMAT_FLOAT4X4_COLUMN_MAJOR, glm::value_ptr(transform));

}

void Scene::UpdateEmbreeTransforms()
{
    if (!embreeScene) return;

    std::cout << "Updating Embree transforms..." << std::endl;

    for (auto& info : geometryMap) {
        if (info.model && info.instance) {
            UpdateInstanceTransform(info.instance, info.model);
            rtcCommitGeometry(info.instance);
        }
    }

    rtcCommitScene(embreeScene);
    sceneNeedsUpdate = false;
}

Model* Scene::TraceRay(const glm::vec3& origin, const glm::vec3& direction)
{
    if (!embreeScene) {
        std::cerr << "No Embree scene!" << std::endl;
        return nullptr;
    }

    if (sceneNeedsUpdate) {
        UpdateEmbreeTransforms();
    }

    RTCRayHit rayhit;
    rayhit.ray.org_x = origin.x; rayhit.ray.org_y = origin.y; rayhit.ray.org_z = origin.z;
    rayhit.ray.dir_x = direction.x; rayhit.ray.dir_y = direction.y; rayhit.ray.dir_z = direction.z;
    rayhit.ray.tnear = 0.001f;
    rayhit.ray.tfar = std::numeric_limits<float>::infinity();
    rayhit.ray.mask = 0xFFFFFFFF;
    rayhit.ray.flags = 0;
    rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

    RTCIntersectArguments args;
    rtcInitIntersectArguments(&args);
    rtcIntersect1(embreeScene, &rayhit, &args);

    if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID || rayhit.hit.instID[0] != RTC_INVALID_GEOMETRY_ID)
    {
        uint32_t topLevelGeomID = (rayhit.hit.instID[0] != RTC_INVALID_GEOMETRY_ID)
            ? rayhit.hit.instID[0]
            : rayhit.hit.geomID;

        for (const auto& info : geometryMap) {
            if (info.geomID == topLevelGeomID) {
                return info.model;
            }
        }
    }
    else {
        std::cout << "No hit detected" << std::endl;
    }

    return nullptr;
}