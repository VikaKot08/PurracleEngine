#pragma once
#include <vector>
#include "Renderable.h"
#include "Camera.h"

class Scene
{
public:
	Scene();
	void AddRenderable(Renderable* aRenderable) { objects.push_back(aRenderable); }
	Camera* GetCamera() const { return mainCamera; }
	std::vector<Renderable*> objects;
private:
	Camera* mainCamera;
};

