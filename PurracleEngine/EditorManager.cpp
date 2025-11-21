#include "EditorManager.h"
#include "Model.h"
#include "Message.h"

void EditorManager::ChangeMesh(Model* aModel, const std::string& meshPath)
{
	aModel->ChangeMesh(meshPath);
}

void EditorManager::ChangeTexture(Model* aModel, const std::string& texturePath)
{
	aModel->myTexture = std::make_unique<Texture>(texturePath.c_str());
}

void EditorManager::UpdateTransform(Model* aModel, glm::vec3 positionVec, glm::vec3 rotationVec, glm::vec3 scaleVec)
{
	aModel->position = positionVec;
	aModel->SetRotationFromEuler(rotationVec);
	aModel->scale = scaleVec;
}

void EditorManager::UpdateTransformMatrix(Model* aModel, glm::mat4 aMatrix)
{
	aModel->SetMatrix(aMatrix);
}

void EditorManager::ProcessMessage(Message* aMsg)
{
	std::string& msg = aMsg->msg;
	
}