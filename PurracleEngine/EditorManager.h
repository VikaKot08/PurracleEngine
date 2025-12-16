#pragma once
#include "MessageQueue.h"
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Model;
class MeshManager;
class GuiManager;
class Scene;

class EditorManager : public MessageQueue
{
public:
	EditorManager();
	~EditorManager();

	void SetGuiManager(GuiManager* guiManager);
	void ChangeTexture(Model* aModel, const std::string& texturePath);
	void UpdateTransform(Model* aModel, glm::vec3 positionVec, glm::vec3 rotationVec, glm::vec3 scaleVec);
	void UpdateTransformMatrix(Model* aModel, glm::mat4 aMatrix);

	void RequestLoadMesh(Model* aModel, const std::string& meshPath);
	void SetMeshManager(MeshManager* meshManager);
	void ChangeShader();
	void ReloadShader();

	void Update();

	void SaveOptimizedMesh(Model* aPointer);

	void RequestSaveScene(const std::string& filepath);
	void RequestLoadScene(const std::string& filepath);
	void SetScene(Scene* scene) { this->scene = scene; }
	void SetModelList(std::vector<Model*>* models) { this->modelList = models; }

protected:
	void ProcessMessage(Message* aMessage) override;

private:
	MeshManager* meshManager;
	GuiManager* guiManager;
	int nextRequestId;
	Scene* scene = nullptr;
	std::vector<Model*>* modelList = nullptr;
};

