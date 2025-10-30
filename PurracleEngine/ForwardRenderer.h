#pragma once
#include "Renderer.h"

class ForwardRenderer : public Renderer
{
	void Init() override;
	void BeginFrame(Scene& aScene) override;
	void RenderFrame(Scene& aScene) override;
	void EndFrame(Scene& aScene) override;
};

