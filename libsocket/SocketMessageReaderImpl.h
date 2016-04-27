#ifndef LIBSOCKET_SOCKETMESSAGEREADERIMPL_H
#define LIBSOCKET_SOCKETMESSAGEREADERIMPL_H

#include <vector>

#include "SocketUtility.h"
#include "SocketMessageReader.h"

NS_LIB_SOCKET_START

class RequestMessage;

class SocketMessageReaderImpl : public SocketMessageReader
{
public:
	SocketMessageReaderImpl() = default;
	virtual ~SocketMessageReaderImpl() = default;

	virtual std::vector<char> readMessage(RequestMessage * message);
};

NS_LIB_SOCKET_END

#endif