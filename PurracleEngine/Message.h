#pragma once
#include <string>

enum MessageType
{
	String,
	Mouse,
	FloatMessage,
	Raycast
};
class Message
{
public:
	Message(MessageType aType, std::string aMsg);
	virtual ~Message();
	MessageType type;
	std::string msg;
};

