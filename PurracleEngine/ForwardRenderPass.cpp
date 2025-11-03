#include "ForwardRenderPass.h"

ForwardRenderPass::ForwardRenderPass()
{
	myShader = new Shader("Assets/Shaders/CubeVertex.glsl", "Assets/Shaders/CubeFragment.glsl");
}

void ForwardRenderPass::Execute(Scene& aScene) 
{
	myShader->Use();

	for(auto& object : aScene.objects)
	{
		object->Render(myShader);
	}

	myShader->EndUse();
}
