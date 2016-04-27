#ifndef LIBSOCKET_SOCKETMESSAGEWRITERIMPL_H
#define LIBSOCKET_SOCKETMESSAGEWRITERIMPL_H

#include <vector>

#include "SocketUtility.h"
#include "SocketMessageWriter.h"

NS_LIB_SOCKET_START

class ResponseMessage;

class SocketMessageWriterImpl :public SocketMessageWriter
{
public:
	SocketMessageWriterImpl() = default;
	virtual ~SocketMessageWriterImpl() = default;

	virtual ResponseMessage * writeMessage(std::vector<char> & buffer);
};

NS_LIB_SOCKET_END

#endif

