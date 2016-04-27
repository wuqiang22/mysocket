#include "SocketMessageReaderImpl.h"
#include "RequestMessage.h"
#include "SocketEncryptManager.h"
#include "odsocket/ODSocket.h"
#include "SocketUtil.h"

NS_LIB_SOCKET_START

std::vector<char> SocketMessageReaderImpl::readMessage(RequestMessage * message)
{
	if (message == NULL)
	{
		return std::vector<char>();
	}
	std::vector<char> result;
	//msgSeqNo
	SocketUtil::pushIntToBuffer(message->getSeqNum(), result);

	//type-data
	std::vector<char> typeData;
	//actionType
	SocketUtil::pushByteToBuffer(message->getType(), typeData);
	//data
	std::vector<char> dataBuffer = message->getDataBuffer();
	typeData.insert(typeData.end(), dataBuffer.begin(), dataBuffer.end());
	//encrypted
	auto encryptedTypeData = SocketEncryptManager::getInstance()->encrypt(typeData, message->getSeqNum());
	result.insert(result.end(), encryptedTypeData.begin(), encryptedTypeData.end());

	//Size ¿ªÍ·Îªsize
	auto netSize = htonl(result.size());
	char* netSizeChar = (char *)&netSize;
	result.insert(result.begin(), netSizeChar, netSizeChar + 4);

	return result;
}

NS_LIB_SOCKET_END