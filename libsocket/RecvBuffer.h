#ifndef LIBSOCKET_RECVBUFFER_H
#define LIBSOCKET_RECVBUFFER_H

#include "SocketUtility.h"

#include <cstdint>
#include <vector>

NS_LIB_SOCKET_START

class RecvBuffer
{
public:
	RecvBuffer();
	~RecvBuffer();
	void pushData(std::vector<char> &data);
	std::vector<char> popData();
	void clear();
private:
	std::vector<char> _data;
private:
	
};

NS_LIB_SOCKET_END

#endif