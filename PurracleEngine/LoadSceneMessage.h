#pragma once
#include "Message.h"
#include <string>
class LoadSceneMessage : public Message
{
public:
    LoadSceneMessage(const std::string& filepath, int requestId)
        : Message(MessageType::LoadScene, filepath), filepath(filepath)
    {
        type = MessageType::LoadScene;
        this->requestId = requestId;
    }

    std::string filepath;
    int requestId;
};

