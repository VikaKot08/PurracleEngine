#pragma once
#include "Renderer.h"
#include "Scene.h"

class EngineContext
{
public:
	EngineContext(Scene* aScene, Renderer* aRenderer);
	void Draw();
	Scene* GetScene() const { return scene; }

private:
	Scene* scene;
	Renderer* renderer;
};

