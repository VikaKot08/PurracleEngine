#include "ModelManager.h"

ModelManager* ModelManager::instance = nullptr;

ModelManager::ModelManager()
{

}

ModelManager::~ModelManager()
{
	for (Model* model : modelList)
	{
		delete model;
	}
	modelList.clear();
	Deallocate();
}

void ModelManager::Allocate()
{
	if (instance == nullptr)
	{
		instance = new ModelManager();
	}
}

void ModelManager::Deallocate()
{
	if (instance != nullptr)
	{
		delete instance;
		instance = nullptr;
	}
}

ModelManager* ModelManager::Get()
{
	return instance;
}

Model* ModelManager::GetModel(const std::string& aPath)
{
	for (Model* model : modelList)
	{
		if (model->path == aPath)
		{
			return model;
		}
	}
	return nullptr;
}

Model* ModelManager::LoadModel(const std::string& path, const char* pathTex)
{
	Model* model = GetModel(path);
	if(model != nullptr)
	{
		return model;
	}
	model = new Model(path, pathTex);
	modelList.push_back(model);
	return model;
}