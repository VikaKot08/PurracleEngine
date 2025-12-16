#pragma once
#include "Model.h"
#include <glm/glm.hpp>

enum LightType {
    DirectionalLight = 0,
    PointLight = 1,
    SpotLight = 2
};

class Light : public Model
{
public:
    Light(LightType aType);
    LightType typeLight;
    glm::vec3 direction;
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    glm::vec3 attenuation;
    float cutoffAngle;
    float outerCutoffAngle;
    bool enabled;
    bool dirty;

    void UpdateDirectionFromRotation();
};

