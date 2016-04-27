#ifndef LIBSOCKET_SOCKETUTIL_H
#define LIBSOCKET_SOCKETUTIL_H

#include "SocketUtility.h"

#include <cstdint>
#include <string>
#include <vector>

NS_LIB_SOCKET_START

class SocketUtil
{
public:

	/*
		前后端协议中string的转换
		字符串（String）结构说明：字符集为UTF - 8，前2byte为长度信息
	*/
	static void pushStringToBuffer(const std::string & str, std::vector<char> &buffer);
	static std::string getStringFromBuffer(std::vector<char> &buffer);

	/*
		int转换（大小端转换）
	*/
	static void pushIntToBuffer(const int src, std::vector<char> &buffer);
	static int getIntFromBuffer(std::vector<char> &buffer);

	/*
		long转换（大小端转换）
	*/
	static void pushLongToBuffer(const long long src, std::vector<char> &buffer);
	static long long getLongFromBuffer(std::vector<char> &buffer);

	/*
		byte转换
	*/
	static void pushByteToBuffer(const char src, std::vector<char> &buffer);
	static char getByteFromBuffer(std::vector<char> &buffer);
	static bool getBoolByteFromBuffer(std::vector<char> &buffer);
};

NS_LIB_SOCKET_END

#endif
