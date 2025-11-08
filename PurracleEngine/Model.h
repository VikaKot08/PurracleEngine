#pragma once
#include "Renderable.h"
#include "Mesh.h"
#include <iostream>

class Model : public Renderable
{
public:

	Model(const std::string& path);
	~Model() override;
	void Render(Shader* myShader) override;
    glm::mat4 GetModelMatrix() const;
	std::vector<Mesh*> meshes;

private:
	void LoadModelSimple(const std::string& path);
	void LoadModel(const std::string& path);
};

