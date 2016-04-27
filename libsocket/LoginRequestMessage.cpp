#include "LoginRequestMessage.h"
#include "SocketUtil.h"
#include "SocketEncryptManager.h"
#include "odsocket/ODSocket.h"

NS_LIB_SOCKET_START

std::vector<char> LoginRequestMessage::getDataBuffer()
{
	std::vector<char> result;
	//username
	SocketUtil::pushStringToBuffer(this->getUsername(), result);
	//Password
	SocketUtil::pushStringToBuffer(this->getPassword(), result);
	//retry 重连 4位 0为否，1为是
	SocketUtil::pushIntToBuffer(this->isRetry(), result);

	return result;
}

NS_LIB_SOCKET_END