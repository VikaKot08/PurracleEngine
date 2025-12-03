#include "EditorManager.h"
#include "GuiManager.h"
#include "Model.h"
#include "Message.h"
#include "MeshManager.h"
#include "MeshLoadedMessage.h"
#include "LoadMeshMessage.h"
#include <iostream>
#include "SaveSceneMessage.h"
#include "LoadSceneMessage.h"
#include "SceneSerializer.h"
#include "SceneOperationCompleteMessage.h"

EditorManager::EditorManager() : meshManager(nullptr), nextRequestId(0)
{
}

EditorManager::~EditorManager()
{
}

void EditorManager::RequestSaveScene(const std::string& filepath)
{
	std::cout << "EditorManager: Saving scene to: " << filepath << std::endl;

	bool success = SceneSerializer::SaveScene(scene, filepath);

	std::string message = success
		? "Scene saved successfully to " + filepath
		: "Failed to save scene to " + filepath;

	SceneOperationCompleteMessage* completeMsg = new SceneOperationCompleteMessage(
		"save", success, message, nextRequestId++
	);

	if (guiManager)
	{
		guiManager->QueueMessage(completeMsg);
	}
}

void EditorManager::RequestLoadScene(const std::string& filepath)
{
	std::cout << "EditorManager: Loading scene from: " << filepath << std::endl;

	bool success = SceneSerializer::LoadScene(scene, filepath, modelList);

	std::string message = success
		? "Scene loaded successfully from " + filepath
		: "Failed to load scene from " + filepath;

	SceneOperationCompleteMessage* completeMsg = new SceneOperationCompleteMessage(
		"load", success, message, nextRequestId++
	);

	if (guiManager)
	{
		guiManager->QueueMessage(completeMsg);
	}
}

void EditorManager::SaveOptimizedMesh(Model* aModel)
{
	meshManager->Get()->Serialize(aModel->path, *aModel->meshes);
}

void EditorManager::SetMeshManager(MeshManager* meshManager)
{
	this->meshManager = meshManager;

	if (meshManager != nullptr)
	{
		meshManager->SetReplyQueue(this);
	}
}

void EditorManager::SetGuiManager(GuiManager* aGuiManager)
{
	guiManager = aGuiManager;
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

void EditorManager::RequestLoadMesh(Model* aModel, const std::string& meshPath)
{
	if (meshManager != nullptr)
	{
		std::cout << "EditorManager: Sending LoadMesh request for: " << meshPath << std::endl;

		LoadMeshMessage* loadMsg = new LoadMeshMessage(aModel, meshPath, nextRequestId++);
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
	if (aMessage->type == MessageType::SaveScene)
	{
		SaveSceneMessage* saveMsg = static_cast<SaveSceneMessage*>(aMessage);
		RequestSaveScene(saveMsg->filepath);
	}
	else if (aMessage->type == MessageType::LoadScene)
	{
		LoadSceneMessage* loadMsg = static_cast<LoadSceneMessage*>(aMessage);
		RequestLoadScene(loadMsg->filepath);
	} else if (aMessage->type == MessageType::MeshLoaded)
	{
		MeshLoadedMessage* loadedMsg = static_cast<MeshLoadedMessage*>(aMessage);

		if (loadedMsg->success)
		{
			std::cout << "EditorManager: Received MeshLoaded confirmation for: "
				<< loadedMsg->msg << " (Request ID: " << loadedMsg->requestId << ")" << std::endl;
			loadedMsg->model->path = loadedMsg->msg;
			loadedMsg->model->meshes = meshManager->Get()->GetFromChache(loadedMsg->msg);
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
		case MessageType::Memory:
			std::cout << "EditorManager: Received string message: " << msg << std::endl;
			break;

		default:
			MessageQueue::ProcessMessage(aMessage);
			break;
		}
	}
}