#pragma once
#include "Renderable.h"
#include "Mesh.h"
#include <iostream>
#include <memory>

struct Position
{
	float x;
	float y;
	float z = 0;
};

class Model : public Renderable
{
public:

	Model(const std::string& path, const char* pathTex);
	~Model() override;
	void Render(Shader* myShader) override;
	std::vector<Mesh*> meshes;
	std::unique_ptr<Texture> myTexture;
	
	int meshIndex;
	int textureIndex;

	void LoadModel(const std::string& path);
	void OwnLoadOBJ(const std::string& path);

private:
	void LoadModelSimple(const std::string& path);
};

