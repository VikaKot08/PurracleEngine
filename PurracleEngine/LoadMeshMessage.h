#pragma once
#include "Message.h"
class Model;

class LoadMeshMessage : public Message
{
public:
	LoadMeshMessage(Model* aModel, const std::string& meshPath, int aRequestId = 0);
	int requestId;
	Model* model;
};

