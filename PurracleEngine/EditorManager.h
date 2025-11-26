#pragma once
#include "MessageQueue.h"
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Model;
class MeshManager;
class GuiManager;

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

	void Update();

	void SaveOptimizedMesh(Model* aPointer);

protected:
	void ProcessMessage(Message* aMessage) override;

private:
	MeshManager* meshManager;
	GuiManager* guiManager;
	int nextRequestId;
};

