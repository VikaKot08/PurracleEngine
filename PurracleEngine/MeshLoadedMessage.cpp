#include "MeshLoadedMessage.h"

MeshLoadedMessage::MeshLoadedMessage(const std::string& meshPath, bool success, int requestId)
	: Message(MessageType::MeshLoaded, meshPath), success(success), requestId(requestId)
{
}
