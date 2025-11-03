#include "ForwardRenderer.h"
#include "ForwardRenderPass.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void ForwardRenderer::Init()
{
	renderPasses.push_back(new ForwardRenderPass());
	glEnable(GL_DEPTH_TEST);
}
void ForwardRenderer::BeginFrame(Scene& aScene) 
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.58f, 0.53f, 0.73f, 1.0f);
}
void ForwardRenderer::RenderFrame(Scene& aScene) 
{
	for (size_t i = 0; i < renderPasses.size(); i++) 
	{
		renderPasses[i]->Execute(aScene);
	}
}
void ForwardRenderer::EndFrame(Scene& aScene) 
{
}