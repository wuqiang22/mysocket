#ifndef LIBSOCKET_SOCKETMESSAGEWRITER_H
#define LIBSOCKET_SOCKETMESSAGEWRITER_H

#include <vector>

#include "SocketUtility.h"

NS_LIB_SOCKET_START

class ResponseMessage;

class SocketMessageWriter
{
public:
	SocketMessageWriter() = default;
	virtual ~SocketMessageWriter() = default;

	virtual ResponseMessage * writeMessage(std::vector<char> & buffer) = 0;
};

NS_LIB_SOCKET_END

#endif

