#include "LoginResponseMessage.h"
#include "SocketUtil.h"

NS_LIB_SOCKET_START

void LoginResponseMessage::setDataWithBuffer(std::vector<char> & buffer)
{
	bool isSuccess = SocketUtil::getBoolByteFromBuffer(buffer);
	auto failedReason = SocketUtil::getStringFromBuffer(buffer);
	bool isReconnect = SocketUtil::getBoolByteFromBuffer(buffer);
	long long userId = SocketUtil::getLongFromBuffer(buffer);
	int sessinoId = SocketUtil::getIntFromBuffer(buffer);
	this->success = isSuccess;
	this->reason = failedReason;
	this->retry = isReconnect;
	//char pUserId[32] = { 0 };
	//ltoa(userId, pUserId, 10);
	//this->userId = std::string(pUserId);
	char pUserId[32] = { 0 };
	sprintf(pUserId, "%I64d", userId);
	this->userId = std::string(pUserId);
	this->sessionId = sessinoId;
}

NS_LIB_SOCKET_END