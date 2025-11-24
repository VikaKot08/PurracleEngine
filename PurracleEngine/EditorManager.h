#pragma once
#include "MessageQueue.h"
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Model;
class MeshManager;

class EditorManager : public MessageQueue
{
public:
	EditorManager();
	~EditorManager();

	void ChangeMesh(Model* aModel, const std::string& meshPath);
	void ChangeTexture(Model* aModel, const std::string& texturePath);
	void UpdateTransform(Model* aModel, glm::vec3 positionVec, glm::vec3 rotationVec, glm::vec3 scaleVec);
	void UpdateTransformMatrix(Model* aModel, glm::mat4 aMatrix);

	void RequestLoadMesh(const std::string& meshPath);
	void SetMeshManager(MeshManager* meshManager);

	void Update();

protected:
	void ProcessMessage(Message* aMessage) override;

private:
	MeshManager* meshManager;
	int nextRequestId;
};

