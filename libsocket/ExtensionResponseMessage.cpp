#include "ExtensionResponseMessage.h"
#include "SocketUtil.h"
#include "ZLibString.h"

NS_LIB_SOCKET_START

void ExtensionResponseMessage::setDataWithBuffer(std::vector<char> & buffer)
{
	auto cmd = SocketUtil::getStringFromBuffer(buffer);
	auto tag = SocketUtil::getByteFromBuffer(buffer);
	this->cmd = cmd;
	this->tag = tag;

	auto isCompressed = SocketUtil::getByteFromBuffer(buffer) != 0;
	if (isCompressed && buffer.size() > 0)
	{
		unsigned long ulUncompLen = 0;
		char *pcUnCompBuf = CZlibMgr::getInstance()->newUnCompressCharP(&buffer[0], buffer.size(), ulUncompLen);
		auto data = std::vector<char>(&pcUnCompBuf[0], &pcUnCompBuf[ulUncompLen]);
		this->data = data;
		delete[]pcUnCompBuf;
	}
	else
	{
		auto data = std::vector<char>(buffer.begin(), buffer.end());
		this->data = data;
	}
}

NS_LIB_SOCKET_END