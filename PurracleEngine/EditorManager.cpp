#include "EditorManager.h"
#include "Model.h"
#include "Message.h"
#include "MeshManager.h"
#include "MeshLoadedMessage.h"
#include "LoadMeshMessage.h"
#include <iostream>

EditorManager::EditorManager() : meshManager(nullptr), nextRequestId(0)
{
}

EditorManager::~EditorManager()
{
}

void EditorManager::SetMeshManager(MeshManager* meshManager)
{
	this->meshManager = meshManager;

	if (meshManager != nullptr)
	{
		meshManager->SetReplyQueue(this);
	}
}

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

void EditorManager::RequestLoadMesh(const std::string& meshPath)
{
	if (meshManager != nullptr)
	{
		std::cout << "EditorManager: Sending LoadMesh request for: " << meshPath << std::endl;

		LoadMeshMessage* loadMsg = new LoadMeshMessage(meshPath, nextRequestId++);
		meshManager->QueueMessage(loadMsg);
	}
	else
	{
		std::cerr << "EditorManager: MeshManager not set!" << std::endl;
	}
}

void EditorManager::Update()
{
	ProcessMessages();
	if (meshManager != nullptr)
	{
		meshManager->ProcessMessages();
	}
}

void EditorManager::ProcessMessage(Message* aMessage)
{
	if (aMessage->type == MessageType::MeshLoaded)
	{
		MeshLoadedMessage* loadedMsg = static_cast<MeshLoadedMessage*>(aMessage);

		if (loadedMsg->success)
		{
			std::cout << "EditorManager: Received MeshLoaded confirmation for: "
				<< loadedMsg->msg << " (Request ID: " << loadedMsg->requestId << ")" << std::endl;
		}
		else
		{
			std::cerr << "EditorManager: Received MeshLoadFailed for: "
				<< loadedMsg->msg << " - " << loadedMsg->errorMessage << std::endl;

			// Handle failure
		}
	}
	else
	{
		std::string& msg = aMessage->msg;

		switch (aMessage->type)
		{
		case MessageType::String:
			std::cout << "EditorManager: Received string message: " << msg << std::endl;
			break;

		default:
			MessageQueue::ProcessMessage(aMessage);
			break;
		}
	}
}