#include "Light.h"

Light::Light(LightType aType)
{
    type = ModelType::LightModel;
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    typeLight = aType;
    direction = glm::vec3(0.0f, 0.0f, 1.0f);
    ambient = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    attenuation = glm::vec3(0.0f, 0.0f, 0.0f);
    cutoffAngle = 0.0f;
    outerCutoffAngle = 0.0f;
    enabled = true;
    dirty = true;
}

void Light::UpdateDirectionFromRotation()
{
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);

    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::vec4 rotatedDirection = rotationMatrix * glm::vec4(forward, 0.0f);
    direction = glm::vec3(rotatedDirection);

    dirty = true;
}
