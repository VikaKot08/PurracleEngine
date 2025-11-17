#include "FlyingCamera.h"
#include <glm/glm.hpp>                  
#include <glm/gtc/matrix_transform.hpp>  
#include <cmath>

void FlyingCamera::Update(glm::vec4 direction, float deltaTime)
{
    if (!camera) return;

    glm::vec3 pos = camera->GetPosition();

    glm::vec3 front;
    front.x = cos(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
    front.y = sin(glm::radians(camera->pitch));
    front.z = sin(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
    front = glm::normalize(front);

    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));

    // Apply movement
    pos += right * direction.x * speed * deltaTime;      // left/right
    pos += glm::vec3(0, 1, 0) * direction.y * speed * deltaTime; // up/down
    pos += front * direction.z * speed * deltaTime;      // forward/back

    camera->SetPosition(pos);
}
