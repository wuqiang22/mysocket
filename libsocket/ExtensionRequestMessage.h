#ifndef LIBSOCKET_EXTENSIONREQUESTMESSAGE_H
#define LIBSOCKET_EXTENSIONREQUESTMESSAGE_H

#include "RequestMessage.h"

#include <vector>

#include "SocketUtility.h"

NS_LIB_SOCKET_START

class ExtensionRequestMessage : public RequestMessage
{
public:
	ExtensionRequestMessage(const Sequence seqnum, const MessageActionType type)
		:RequestMessage(seqnum,type)
	{
	}
	virtual ~ExtensionRequestMessage() = default;

	void setData(const int extensionId, const std::string & cmd, const int tag, std::vector<char> && data)
	{
		this->extensionId = extensionId;
		this->cmd = cmd;
		this->tag = tag;
		this->data = std::forward<std::vector<char> &&>(data);
		//this->data = data;
	}

	const int getExtensionId() const
	{
		return this->extensionId;
	}

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

	virtual std::vector<char> getDataBuffer();

private:
	int extensionId;
	std::string cmd;
	char tag;
	std::vector<char> data;
};

NS_LIB_SOCKET_END

#endif
