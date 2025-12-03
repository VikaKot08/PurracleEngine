#pragma once
#include <string>

enum MessageType
{
	LoadMesh,
	MeshLoaded,
	Memory,
	SaveScene,
	LoadScene,
	SceneOperationComplete
};
class Message
{
public:
	Message(MessageType aType, std::string aMsg);
	virtual ~Message();
	MessageType type;
	std::string msg;
};

