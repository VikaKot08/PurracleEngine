#include "MessageQueue.h"

void MessageQueue::QueueMessage(Message* message)
{
	messages.push_back(message);
}

void MessageQueue::ProcessMessages()
{
	while(messages.size())
	{
		Message* message = messages.back();
		ProcessMessage(message);
		delete message;
	}
}
void MessageQueue::ProcessMessage(Message* aMessage)
{
}
