#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	Camera();
	void SetPosition(const glm::vec3& aPos);
	void LookAt(const glm::vec3& aTarget, const glm::vec3& aUp);

	glm::mat4 GetView() const;
	glm::mat4 GetProjection() const;

	const glm::vec3& GetPosition() { return position; }

	void SetFov(float aFov) { fov = aFov; };
	void SetAspectRatioInternal(float aAspect) { aspectRatio = aAspect; }
	void SetAspectRatio(float width, float height);
	void SetNearFar(float aNearPlane, float aFarPlane) { near = aNearPlane; far = aFarPlane; }

	glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);

private:
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	float fov = 90.0f;
	float aspectRatio = 1280.0f / 720.0f;
	float near = 0.1f;
	float far = 100.0f;
};

