#include "ForwardRenderPass.h"

ForwardRenderPass::ForwardRenderPass()
{
	myTexture = new Texture("Assets/Textures/Lava.jpg");
	myShader = new Shader("Assets/Shaders/VertexShader.glsl", "Assets/Shaders/FragmentShader.glsl");
}

void ForwardRenderPass::Execute(Scene& aScene) 
{
	myShader->Use();
	myTexture->Bind(0);
	myShader->SetInt(0, "myTexture");

	glm::mat4 view = aScene.GetCamera()->GetView();
	glm::mat4 proj = aScene.GetCamera()->GetProjection();

	myShader->SetMatrix4(view, "view");
	myShader->SetMatrix4(proj, "projection");

	for(auto& object : aScene.objects)
	{
		object->Render(myShader);
	}

	myTexture->Unbind();
	myShader->EndUse();
}
