#ifndef LIBSOCKET_LOGINRESPONSEMESSAGE_H
#define LIBSOCKET_LOGINRESPONSEMESSAGE_H

#include "ResponseMessage.h"

#include "SocketUtility.h"
#include <string>

NS_LIB_SOCKET_START

class LoginResponseMessage : public ResponseMessage
{
public:
	LoginResponseMessage(const MessageActionType type, const size_t size)
		:ResponseMessage(type, size)
	{
	}
	virtual ~LoginResponseMessage() = default;

	const bool isSuccess() const
	{
		return this->success;
	}

	const std::string & getReason() const
	{
		return this->reason;
	}

	const bool isRetry() const
	{
		return this->retry;
	}

	const std::string & getUserId() const
	{
		return this->userId;
	}

	const int getSessionId() const
	{
		return this->sessionId;
	}

	void setDataWithBuffer(std::vector<char> & buffer);

private:
	bool success;
	std::string reason;
	bool retry;
	std::string userId;
	int sessionId;
};

NS_LIB_SOCKET_END

#endif

