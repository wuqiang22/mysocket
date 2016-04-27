#include "SocketMessageWriterImpl.h"
#include "ResponseMessage.h"
#include "ExtensionResponseMessage.h"
#include "LoginResponseMessage.h"
#include "SocketUtil.h"

NS_LIB_SOCKET_START

ResponseMessage* SocketMessageWriterImpl::writeMessage(std::vector<char> & buffer)
{
	ResponseMessage* result = nullptr;
	int size = SocketUtil::getIntFromBuffer(buffer);
	MessageActionType actionType = (MessageActionType)SocketUtil::getByteFromBuffer(buffer);
	switch (actionType)
	{
	case MessageActionType::Login:
		result = new LoginResponseMessage(actionType, size);
		break;
	case MessageActionType::Extension:
		result = new ExtensionResponseMessage(actionType, size);
		break;
	case MessageActionType::Crowd:
		result = new ResponseMessage(actionType, size);
		break;
	default:
		printf("actionType:%d illegal\n", actionType);
		break;
	}
    if (result != nullptr) {
        result->setDataWithBuffer(buffer);
    }else
    {
        printf("result == nullprt\n");
    }
	return result;
}
NS_LIB_SOCKET_END