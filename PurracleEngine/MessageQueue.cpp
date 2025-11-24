#include "MessageQueue.h"
#include "Message.h"
#include <iostream>

MessageQueue::MessageQueue()
{
}

MessageQueue::~MessageQueue()
{
	while (!messages.empty())
	{
		Message* msg = messages.front();
		messages.pop();
		delete msg;
	}
}

void MessageQueue::QueueMessage(Message* message)
{
	std::lock_guard<std::mutex> lock(queueMutex);
	messages.push(message);
}

void MessageQueue::ProcessMessages()
{
	std::lock_guard<std::mutex> lock(queueMutex);

	while (!messages.empty())
	{
		Message* message = messages.front();
		messages.pop();

		ProcessMessage(message);

		delete message;
	}
}

bool MessageQueue::HasMessages() const
{
	std::lock_guard<std::mutex> lock(queueMutex);
	return !messages.empty();
}

void MessageQueue::ProcessMessage(Message* aMessage)
{
	std::cout << "Processing message type: " << aMessage->type << std::endl;
}