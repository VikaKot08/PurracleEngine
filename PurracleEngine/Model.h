#pragma once
#include "Renderable.h"
#include "Mesh.h"
#include <iostream>
#include <memory>

enum ModelType
{
	CameraModel,
	Normal
};
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
	Model();
	~Model() override;
	void Render(Shader* myShader) override;

	ModelType type;
	bool isActive = false;

	std::vector<Mesh*>* meshes;
	std::unique_ptr<Texture> myTexture;
	
	int meshIndex;
	int textureIndex;
	std::string path;
	const char* pathTex;
};

