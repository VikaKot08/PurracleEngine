#pragma once
#include <string>

enum MessageType
{
	String,
	Mouse,
	FloatMessage,
	Raycast,
	LoadMesh,
	MeshLoaded
};
class Message
{
public:
	Message(MessageType aType, std::string aMsg);
	virtual ~Message();
	MessageType type;
	std::string msg;
};

