#include "Camera.h"

Camera::Camera()
{
	position = glm::vec3(0.0f, 0.0f, 3.0f);
	target = glm::vec3(0.0f, 0.0f, 0.0f);
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

void Camera::LookAt(const glm::vec3& aTarget, const glm::vec3& aUp)
{
	target = aTarget;
	up = aUp;
}

glm::mat4 Camera::GetView() const
{
	return glm::lookAt(position, target, up);
}

glm::mat4 Camera::GetProjection() const
{
	return glm::perspective(glm::radians(fov), aspectRatio, near, far);
}

void Camera::SetAspectRatio(float width, float height) 
{
	SetAspectRatioInternal(width / height);
}
