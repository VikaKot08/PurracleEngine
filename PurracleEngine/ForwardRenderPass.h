#pragma once
#include "RenderPass.h"
#include "Shader.h"
#include "Texture.h"

class ForwardRenderPass : public RenderPass
{
public:
	ForwardRenderPass();
	void Execute(Scene& aScene) override;

private:
	Texture* myTexture;
	Shader* myShader;
};

