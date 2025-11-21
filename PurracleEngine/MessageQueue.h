#pragma once
#include <vector>
class Message;

class MessageQueue
{
public:
	void QueueMessage(Message* message);

private:
	void ProcessMessages();
	void ProcessMessage(Message* aMessage);

	std::vector<Message*> messages;
};

