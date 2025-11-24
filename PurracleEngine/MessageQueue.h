#pragma once
#include <queue>
#include <mutex>

class Message;

class MessageQueue
{
public:
	MessageQueue();
	virtual ~MessageQueue();

	void QueueMessage(Message* message);
	void ProcessMessages();
	bool HasMessages() const;
	void Clear();

protected:
	virtual void ProcessMessage(Message* aMessage);

private:
	std::queue<Message*> messages;
	mutable std::mutex queueMutex;
};

