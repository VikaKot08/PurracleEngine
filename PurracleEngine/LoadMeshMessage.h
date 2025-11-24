#pragma once
#include "Message.h"

class LoadMeshMessage : public Message
{
public:
	LoadMeshMessage(const std::string& meshPath, int aRequestId = 0);
	int requestId;
};

