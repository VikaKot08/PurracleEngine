#include "Light.h"

Light::Light(LightType aType)
{
    type = ModelType::LightModel;
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    typeLight = aType;
    direction = glm::vec3(0.0f, 0.0f, 0.0f);
    ambient = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    attenuation = glm::vec3(0.0f, 0.0f, 0.0f);
    cutoffAngle = 0.0f;
    outerCutoffAngle = 0.0f;
    enabled = true;
    dirty = true;
}
