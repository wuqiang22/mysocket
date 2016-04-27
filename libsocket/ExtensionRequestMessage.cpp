#include "ExtensionRequestMessage.h"
#include "SocketUtil.h"
#include "SocketEncryptManager.h"
#include "odsocket/ODSocket.h"

NS_LIB_SOCKET_START

std::vector<char> ExtensionRequestMessage::getDataBuffer()
{
	std::vector<char> result;
	//extensionId
	SocketUtil::pushIntToBuffer(this->getExtensionId(), result);
	//cmd
	SocketUtil::pushStringToBuffer(this->getCmd(), result);
	//tag
	SocketUtil::pushByteToBuffer(this->getTag(), result);
	//data
	result.insert(result.end(), this->data.begin(), this->data.end());

	return result;
}

NS_LIB_SOCKET_END