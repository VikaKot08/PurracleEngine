#include "ForwardRenderPass.h"

ForwardRenderPass::ForwardRenderPass()
{
	myPhongShader = new Shader("Assets/Shaders/PhongVertex.glsl", "Assets/Shaders/PhongFragmentMultiple.glsl");
	myPhongShader->useLight = true;
	mySimpleShader = new Shader("Assets/Shaders/VertexShader.glsl", "Assets/Shaders/FragmentShader.glsl");
	mySimpleShader->useLight = false;
	myShader = myPhongShader;
}

void ForwardRenderPass::Execute(Scene& aScene) 
{
	if(aScene.switchShader)
	{
		SwitchShader();
		aScene.switchShader = false;
	}

	if (aScene.reloadShader)
	{
		ReloadShader();
		aScene.reloadShader = false;
	}

	myShader->Use(aScene.GetLight());

	glm::mat4 view = aScene.GetCamera()->GetView();
	glm::mat4 proj = aScene.GetCamera()->GetProjection();

	myShader->SetMatrix4(view, "view");
	myShader->SetMatrix4(proj, "projection");
	myShader->SetEyePosition(aScene.GetCamera()->position);

	for(auto& object : aScene.renderables)
	{
		object->Render(myShader);
	}

	myShader->EndUse();
}

void ForwardRenderPass::SwitchShader()
{
	if(myShader == myPhongShader)
	{
		myShader = mySimpleShader;
	}
	else 
	{
		myShader = myPhongShader;
	}
}

void ForwardRenderPass::ReloadShader()
{
	myPhongShader->Reload();
	mySimpleShader->Reload();
}


