#include "ForwardRenderPass.h"

ForwardRenderPass::ForwardRenderPass()
{
	myShader = new Shader("Assets/Shaders/PhongVertex.glsl", "Assets/Shaders/PhongFragment.glsl");
}

void ForwardRenderPass::Execute(Scene& aScene) 
{
	myShader->Use();

	glm::mat4 view = aScene.GetCamera()->GetView();
	glm::mat4 proj = aScene.GetCamera()->GetProjection();

	myShader->SetMatrix4(view, "view");
	myShader->SetMatrix4(proj, "projection");

	for(auto& object : aScene.renderables)
	{
		object->Render(myShader);
	}

	myShader->EndUse();
}
