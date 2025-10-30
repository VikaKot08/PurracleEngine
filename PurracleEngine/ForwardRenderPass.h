#pragma once
#include "RenderPass.h"
#include "Shader.h"

class ForwardRenderPass : public RenderPass
{
public:
	ForwardRenderPass();
	void Execute(Scene& aScene) override;

private:
	Shader* myShader;
};

