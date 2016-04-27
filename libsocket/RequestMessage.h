#ifndef LIBSOCKET_REQUESTMESSAGE_H
#define LIBSOCKET_REQUESTMESSAGE_H

#include "MessageActionType.h"

#include <vector>
#include <string>

NS_LIB_SOCKET_START

class RequestMessage
{
public:
	typedef unsigned int Sequence;

	RequestMessage() = default;
	RequestMessage(const Sequence seqnum, const MessageActionType type)
		:
		seqnum(seqnum),
		type(type)
	{

	}

	virtual ~RequestMessage() = default;

	Sequence getSeqNum() const
	{
		return this->seqnum;
	}

	MessageActionType getType() const
	{
		return this->type;
	}

	virtual std::vector<char> getDataBuffer() = 0;

private:
	Sequence seqnum;
	MessageActionType type;
};

NS_LIB_SOCKET_END

#endif

