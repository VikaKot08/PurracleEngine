#pragma once
#include <vector>
#include "RenderPass.h"
#include "Scene.h"

class Renderer
{
public: 
	virtual void Init() = 0;
	virtual void BeginFrame(Scene& aScene) = 0;
	virtual void RenderFrame(Scene& aScene) = 0;
	virtual void EndFrame(Scene& aScene) = 0;

protected:
	std::vector<RenderPass*> renderPasses;
};

