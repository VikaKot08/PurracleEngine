#include "LoadMeshMessage.h"
#include "Model.h"

LoadMeshMessage::LoadMeshMessage(Model* aModel, const std::string& meshPath, int requestId): Message(MessageType::LoadMesh, meshPath), model(aModel), requestId(requestId)
{
}
