#include "ForwardRenderPass.h"

ForwardRenderPass::ForwardRenderPass()
{
	myShader = new Shader("Assets/Shaders/VertexShader.glsl", "Assets/Shaders/FragmentShader.glsl");
}

void ForwardRenderPass::Execute(Scene& aScene) 
{
	myShader->Use();

	for(auto& object : aScene.objects)
	{
		object->Render();
	}

	myShader->EndUse();
}
