#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    glm::vec3 position;
    glm::vec3 up;

    float yaw = -90.0f;    // Initialize to -90 to look forward
    float pitch = 0.0f;

    float fov;
    float aspectRatio;
    float near;
    float far;

    // Mouse sensitivity
    float mouseSensitivity = 0.1f;

    Camera();

    void SetPosition(const glm::vec3& aPos);
    glm::vec3 GetPosition() const { return position; }

    glm::mat4 GetView() const;
    glm::mat4 GetProjection() const;

    void SetAspectRatio(float width, float height);
    void SetAspectRatioInternal(float ratio) { aspectRatio = ratio; }

    // Mouse input methods
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void ProcessMouseScroll(float yoffset);
};