#pragma once
#include "Message.h"
#include <string>

class SaveSceneMessage : public Message
{
public:
    SaveSceneMessage(const std::string& filepath, int requestId)
        : Message(MessageType::SaveScene, filepath), filepath(filepath)
    {
        type = MessageType::SaveScene;
        this->requestId = requestId;
    }

    std::string filepath;
    int requestId;
};

