#include "MemoryMessage.h"

MemoryMessage::MemoryMessage(const std::string& text, bool success, int requestId) : Message(MessageType::Memory, text), success(success), requestId(requestId)
{
}
