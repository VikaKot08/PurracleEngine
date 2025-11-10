#include "FlyingCamera.h"
#include <glm/glm.hpp>                  
#include <glm/gtc/matrix_transform.hpp>  
#include <cmath>

void FlyingCamera::Update(glm::vec4 direction, float deltaTime)
{
    if (!camera) return; // make sure the pointer is valid

    glm::vec3 pos = camera->GetPosition();
    glm::vec3 front = glm::normalize(camera->target - camera->GetPosition()); // direction camera is looking
    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0))); // world right

    // Apply movement along the axes
    pos += right * direction.x * speed * deltaTime;   // left/right
    pos += glm::vec3(0, 1, 0) * direction.y * speed * deltaTime; // up/down
    pos += front * direction.z * speed * deltaTime;   // forward/back

    camera->SetPosition(pos);
    camera->LookAt(pos + front, glm::vec3(0, 1, 0)); // maintain forward direction
}
