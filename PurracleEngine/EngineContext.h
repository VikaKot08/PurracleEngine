#pragma once
#include "Renderer.h"
#include "Scene.h"

class EngineContext
{
public:
	EngineContext(Scene* aScene, Renderer* aRenderer);
	void Draw();

private:
	Scene* scene;
	Renderer* renderer;
};

