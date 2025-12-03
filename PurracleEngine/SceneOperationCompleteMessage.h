#pragma once
#include "Message.h"
#include <string>
class SceneOperationCompleteMessage : public Message
{
public:
    SceneOperationCompleteMessage(const std::string& operation, bool success, const std::string& message, int requestId)
        : Message(MessageType::SceneOperationComplete, message), operation(operation), success(success), message(message)
    {
        type = MessageType::SceneOperationComplete;
        this->requestId = requestId;
    }

    std::string operation; // "save" or "load"
    bool success;
    std::string message;
    int requestId;
};

