#pragma once
#include "Model.h"
#include <vector>

class ModelManager
{
public:
	ModelManager();
	static void Allocate();
	static void Deallocate();
	static ModelManager* Get();
	Model* GetModel(const std::string& path);
	Model* LoadModel(const std::string& path, const char* pathTex);
	~ModelManager();

private:
	static ModelManager* instance;
	std::vector<Model*> modelList;
};

