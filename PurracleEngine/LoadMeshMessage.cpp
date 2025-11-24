#include "LoadMeshMessage.h"

LoadMeshMessage::LoadMeshMessage(const std::string& meshPath, int requestId)
	: Message(MessageType::LoadMesh, meshPath), requestId(requestId)
{
}
