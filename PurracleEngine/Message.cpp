#include "Message.h"

Message::Message(MessageType aType, std::string aMsg) : type(aType), msg(aMsg)
{

}
Message::~Message()
{
	//Do cleanup if any data allocated on heap
}
