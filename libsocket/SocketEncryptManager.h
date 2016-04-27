#ifndef LIBSOCKET_SOCKETENCRYPTMANAGER_H
#define LIBSOCKET_SOCKETENCRYPTMANAGER_H

#include "SocketUtility.h"
#include <vector>

NS_LIB_SOCKET_START

class SocketEncryptManager
{
public:
	static SocketEncryptManager* getInstance();

	std::vector<char> encrypt(const std::vector<char> &buffer,const int sequence);

private:
	static SocketEncryptManager* _pInstance;

private:
	SocketEncryptManager();
	~SocketEncryptManager();
};

NS_LIB_SOCKET_END

#endif