#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Model.h"

class Camera : public Model
{
public:
    glm::vec3 aPosition;
    glm::vec3 up;

    float yaw = -90.0f; 
    float pitch = 0.0f;

    float fov;
    float aspectRatio;
    float near;
    float far;

    float mouseSensitivity = 0.1f;

    Camera();
    void SyncRotationToYawPitch();
    void SyncYawPitchToRotation();

    void SetPosition(const glm::vec3& aPos);
    glm::vec3 GetPosition() const { return aPosition; }

    glm::mat4 GetView() const;
    glm::mat4 GetProjection() const;

    void SetAspectRatio(float width, float height);
    void SetAspectRatioInternal(float ratio) { aspectRatio = ratio; }

    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void ProcessMouseScroll(float yoffset);
};