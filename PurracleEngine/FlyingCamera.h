#pragma once
#include "Camera.h"

class FlyingCamera
{
public:
	FlyingCamera(Camera* cam) : camera(cam) {}

	void Update(glm::vec4 direction,  float deltaTime);
	float GetSpeed() { return speed; }
	void SetSpeed(float aSpeed) { speed = aSpeed; }
	void SetNewCamera(Camera* aCamera) { camera = aCamera; }

private:
	Camera* camera;
	float speed = 5.0f;
	float sensitivity = 0.1f;
};

