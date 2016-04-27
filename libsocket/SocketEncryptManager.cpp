#include "SocketEncryptManager.h"

#include <climits>

NS_LIB_SOCKET_START

SocketEncryptManager* SocketEncryptManager::_pInstance = nullptr;

SocketEncryptManager* SocketEncryptManager::getInstance()
{
	if (_pInstance == nullptr)
	{
		_pInstance = new SocketEncryptManager();
	}
	return _pInstance;
}

std::vector<char> SocketEncryptManager::encrypt(const std::vector<char> &buffer,const int sequence)
{
	std::vector<char> result;
	char decryptKey[4];
	int keyLen = 4;
	decryptKey[0] = (sequence >> 24);
	decryptKey[1] = (sequence >> 16);
	decryptKey[2] = (sequence >> 8);
	decryptKey[3] = (sequence >> 0);

	for (unsigned i = 0; i < buffer.size(); i++)
	{
		result.push_back(buffer[i] ^ decryptKey[i % keyLen]);
	}
	return result;
}

SocketEncryptManager::SocketEncryptManager()
{
};

SocketEncryptManager::~SocketEncryptManager()
{
};

NS_LIB_SOCKET_END