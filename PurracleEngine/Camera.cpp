#include "Camera.h"

Camera::Camera()
{
    position = glm::vec3(0.0f, 0.0f, 3.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);

    fov = 90.0f;
    aspectRatio = 1280.0f / 720.0f;
    near = 0.1f;
    far = 100.0f;
}

void Camera::SetPosition(const glm::vec3& aPos)
{
    position = aPos;
}

glm::mat4 Camera::GetView() const
{
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::GetProjection() const
{
    return glm::perspective(glm::radians(fov), aspectRatio, near, far);
}

void Camera::SetAspectRatio(float width, float height)
{
    SetAspectRatioInternal(width / height);
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Constrain pitch to prevent screen flip
    if (constrainPitch)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }
}
