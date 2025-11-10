#pragma once
#include "Camera.h"

class FlyingCamera
{
public:
	FlyingCamera(Camera* cam) : camera(cam) {}

	void Update(glm::vec4 direction,  float deltaTime);

private:
	Camera* camera;
	float speed = 5.0f;
	float sensitivity = 0.1f;
};

