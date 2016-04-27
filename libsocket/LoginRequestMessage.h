#ifndef LIBSOCKET_LOGINREQUESTMESSAGE_H
#define LIBSOCKET_LOGINREQUESTMESSAGE_H

#include "RequestMessage.h"

#include <string>

NS_LIB_SOCKET_START

class LoginRequestMessage : public RequestMessage
{
public:
	LoginRequestMessage(const Sequence seqnum, const MessageActionType type)
		:RequestMessage(seqnum, type)
	{
	};
	virtual ~LoginRequestMessage() = default;

	void setData(const std::string & username, const std::string & password, const bool retry)
	{
		this->username = username;
		this->password = password;
		this->retry = retry;
	}

	const std::string & getUsername() const
	{
		return this->username;
	}

	const std::string & getPassword() const
	{
		return this->password;
	}

	const bool isRetry() const
	{
		return this->retry;
	}

	virtual std::vector<char> getDataBuffer();

private:
	std::string username;
	std::string password;
	bool retry;
};

NS_LIB_SOCKET_END

#endif
