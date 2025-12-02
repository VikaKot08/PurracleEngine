#include "Camera.h"

Camera::Camera()
{
    aPosition = glm::vec3(0.0f, 0.0f, 3.0f);
    position = aPosition;
    up = glm::vec3(0.0f, 1.0f, 0.0f);

    fov = 90.0f;
    aspectRatio = 1280.0f / 720.0f;
    near = 0.1f;
    far = 100.0f;
}

void Camera::SyncRotationToYawPitch()
{
    yaw = rotation.y - 90.0f;
    pitch = -rotation.x;
}

void Camera::SyncYawPitchToRotation()
{
    rotation.y = yaw + 90.0f;
    rotation.x = -pitch;
    rotation.z = 0.0f;
}

void Camera::SetPosition(const glm::vec3& aPos)
{
    aPosition = aPos;
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

void Camera::ProcessMouseScroll(float yoffset)
{
    fov -= yoffset;

    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 120.0f)
        fov = 120.0f;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (constrainPitch)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }
}