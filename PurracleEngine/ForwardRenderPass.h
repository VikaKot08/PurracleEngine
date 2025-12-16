#pragma once
#include "RenderPass.h"
#include "Shader.h"
#include "Texture.h"

class ForwardRenderPass : public RenderPass
{
public:
	ForwardRenderPass();
	void Execute(Scene& aScene) override;
	void SwitchShader();

private:
	Texture* myTexture;
	Shader* myShader;
	Shader* myPhongShader;
	Shader* mySimpleShader;
};

