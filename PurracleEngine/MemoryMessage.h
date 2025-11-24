#pragma once
#include "Message.h"
class MemoryMessage : public Message
{
public:
	MemoryMessage(const std::string& text, bool success, int requestId);
	bool success;
	int requestId;
};

