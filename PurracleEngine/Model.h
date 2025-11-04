#pragma once
#include "Renderable.h"
#include "Mesh.h"
#define UFBX_REAL_IS_FLOAT
#include "FBX/ufbx.h"
#include <iostream>

class Model : public Renderable
{
public:

	Model(const std::string& path);
	~Model() override;
	void Render(Shader* myShader) override;

private:
	std::vector<Mesh*> meshes;

	void LoadModel(const std::string& path);
};

