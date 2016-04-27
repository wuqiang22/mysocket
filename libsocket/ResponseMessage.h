#ifndef LIBSOCKET_RESPONSEMESSAGE_H
#define LIBSOCKET_RESPONSEMESSAGE_H

#include "MessageActionType.h"
#include <vector>
#include <string>
NS_LIB_SOCKET_START

class ResponseMessage
{
public:
	ResponseMessage(const MessageActionType type, const size_t size)
		:
		type(type),
		size(size)
	{

	}
	virtual ~ResponseMessage() = default;

	size_t getSize() const
	{
		return this->size;
	}

	MessageActionType getType() const
	{
		return this->type;
	}

	virtual void setDataWithBuffer(std::vector<char> & buffer)
	{};

private:
	MessageActionType type;
	size_t size;
};

NS_LIB_SOCKET_END

#endif

