#pragma once
#include <glm/glm.hpp>
#include <vector>

class Camera;
class Model;

class MousePicker
{
public:
    static glm::vec3 GetMouseRay(double mouseX, double mouseY, int screenWidth, int screenHeight, Camera* camera);
    static Model* PickModel(glm::vec3 rayOrigin, glm::vec3 rayDirection, std::vector<Model*>* models, float maxDistance = 100.0f);
    static bool RayIntersectsAABB(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 aabbMin, glm::vec3 aabbMax, float& distance);
};

