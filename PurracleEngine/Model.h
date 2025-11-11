#pragma once
#include "Renderable.h"
#include "Mesh.h"
#include <iostream>

class Model : public Renderable
{
public:

	Model(const std::string& path, const char* pathTex);
	~Model() override;
	void Render(Shader* myShader) override;
    glm::mat4 GetModelMatrix() const;
	std::vector<Mesh*> meshes;
	Texture* myTexture;

	void LoadModel(const std::string& path);

private:
	void LoadModelSimple(const std::string& path);
};

