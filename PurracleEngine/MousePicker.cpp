#include "MousePicker.h"
#include "Camera.h"
#include "Model.h"
#include <limits>
#include <algorithm>

glm::vec3 MousePicker::GetMouseRay(double mouseX, double mouseY, int screenWidth, int screenHeight, Camera* camera)
{
    float x = (2.0f * mouseX) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenHeight;

    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);

    glm::vec4 rayEye = glm::inverse(camera->GetProjection()) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    glm::vec3 rayWorld = glm::vec3(glm::inverse(camera->GetView()) * rayEye);
    rayWorld = glm::normalize(rayWorld);

    return rayWorld;
}

bool MousePicker::RayIntersectsAABB(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 aabbMin, glm::vec3 aabbMax, float& distance)
{
    float tmin = (aabbMin.x - rayOrigin.x) / rayDirection.x;
    float tmax = (aabbMax.x - rayOrigin.x) / rayDirection.x;

    if (tmin > tmax) std::swap(tmin, tmax);

    float tymin = (aabbMin.y - rayOrigin.y) / rayDirection.y;
    float tymax = (aabbMax.y - rayOrigin.y) / rayDirection.y;

    if (tymin > tymax) std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
        return false;

    if (tymin > tmin)
        tmin = tymin;

    if (tymax < tmax)
        tmax = tymax;

    float tzmin = (aabbMin.z - rayOrigin.z) / rayDirection.z;
    float tzmax = (aabbMax.z - rayOrigin.z) / rayDirection.z;

    if (tzmin > tzmax) std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    if (tzmin > tmin)
        tmin = tzmin;

    if (tzmax < tmax)
        tmax = tzmax;

    distance = tmin;
    return tmin >= 0;
}

Model* MousePicker::PickModel(glm::vec3 rayOrigin, glm::vec3 rayDirection, std::vector<Model*>* models, float maxDistance)
{
    if (!models) return nullptr;

    Model* closestModel = nullptr;
    float closestDistance = maxDistance;

    for (Model* model : *models)
    {
        // Create a simple AABB around the model
        // This assumes the model is roughly centered and has a scale
        glm::vec3 modelCenter = model->position;
        glm::vec3 modelExtents = model->scale * 1.5f; // Approximate bounding box

        glm::vec3 aabbMin = modelCenter - modelExtents;
        glm::vec3 aabbMax = modelCenter + modelExtents;

        float distance;
        if (RayIntersectsAABB(rayOrigin, rayDirection, aabbMin, aabbMax, distance))
        {
            if (distance < closestDistance)
            {
                closestDistance = distance;
                closestModel = model;
            }
        }
    }

    return closestModel;
}