#pragma once
#include "Renderable.h"
#include "Shader.h"

class Cube : public Renderable
{
public:
	Cube();
	~Cube() override;
	void Render(Shader* myShader) override;

private:
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;
	int indicesCount;
	float rotationAngle = 0;
};

