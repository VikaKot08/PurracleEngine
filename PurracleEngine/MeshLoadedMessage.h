#pragma once
#include "Message.h"
class Model;

class MeshLoadedMessage : public Message
{
public:
	MeshLoadedMessage(Model* aModel, const std::string& meshPath, bool success, int requestId = 0);
	bool success;
	int requestId;
	std::string errorMessage;
	Model* model;
};

