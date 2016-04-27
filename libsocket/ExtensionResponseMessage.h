#ifndef LIBSOCKET_EXTENSIONRESPONSEMESSAGE_H
#define LIBSOCKET_EXTENSIONRESPONSEMESSAGE_H

#include "ResponseMessage.h"

#include "SocketUtility.h"

NS_LIB_SOCKET_START

class ExtensionResponseMessage : public ResponseMessage
{
public:
	ExtensionResponseMessage(const MessageActionType type, const size_t size)
		:ResponseMessage(type,size)
	{
	}
	virtual ~ExtensionResponseMessage() = default;

	const std::string & getCmd() const
	{
		return this->cmd;
	}

	const int getTag() const
	{
		return this->tag;
	}

	const std::vector<char> & getData() const
	{
		return this->data;
	}

	void setDataWithBuffer(std::vector<char> & buffer);

private:
	std::string cmd;
	char tag;
	std::vector<char> data;
};

NS_LIB_SOCKET_END

#endif
