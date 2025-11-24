#pragma once
#include <string>

enum MessageType
{
	LoadMesh,
	MeshLoaded,
	Memory
};
class Message
{
public:
	Message(MessageType aType, std::string aMsg);
	virtual ~Message();
	MessageType type;
	std::string msg;
};

