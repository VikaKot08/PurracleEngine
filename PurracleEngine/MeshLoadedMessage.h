#pragma once
#include "Message.h"

class MeshLoadedMessage : public Message
{
public:
	MeshLoadedMessage(const std::string& meshPath, bool success, int requestId = 0);
	bool success;
	int requestId;
	std::string errorMessage;
};

