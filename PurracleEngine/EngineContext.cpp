#include "EngineContext.h"

EngineContext::EngineContext(Scene* aScene, Renderer* aRenderer)
{
	scene = aScene;
	renderer = aRenderer;
	renderer->Init();
}

void EngineContext::Draw() 
{
	renderer->BeginFrame(*scene);

	renderer->RenderFrame(*scene);

	renderer->EndFrame(*scene);
}


