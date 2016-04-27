#ifndef LIBSOCKET_SOCKETMESSAGEREADER_H
#define LIBSOCKET_SOCKETMESSAGEREADER_H

#include <vector>

#include "SocketUtility.h"

NS_LIB_SOCKET_START

class RequestMessage;

class SocketMessageReader
{
public:
	SocketMessageReader() = default;
	virtual ~SocketMessageReader() = default;

	virtual std::vector<char> readMessage(RequestMessage * message) = 0;
};

NS_LIB_SOCKET_END

#endif