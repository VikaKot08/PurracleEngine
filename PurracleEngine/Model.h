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

	Model(const std::string& aPath, const char* pathTex);
	~Model() override;
	void Render(Shader* myShader) override;
	void ChangeMesh(const std::string& newPath);

	std::vector<Mesh*>* meshes;
	std::unique_ptr<Texture> myTexture;
	
	int meshIndex;
	int textureIndex;
	std::string path;
	const char* pathTex;
};

