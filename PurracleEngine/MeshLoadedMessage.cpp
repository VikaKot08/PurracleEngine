#include "MeshLoadedMessage.h"
#include "Model.h"

MeshLoadedMessage::MeshLoadedMessage(Model* aModel, const std::string& meshPath, bool success, int requestId) : Message(MessageType::MeshLoaded, meshPath), model(aModel),  success(success), requestId(requestId)
{
}
