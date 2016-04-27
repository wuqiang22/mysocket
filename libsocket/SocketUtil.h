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
		ǰ���Э����string��ת��
		�ַ�����String���ṹ˵�����ַ���ΪUTF - 8��ǰ2byteΪ������Ϣ
	*/
	static void pushStringToBuffer(const std::string & str, std::vector<char> &buffer);
	static std::string getStringFromBuffer(std::vector<char> &buffer);

	/*
		intת������С��ת����
	*/
	static void pushIntToBuffer(const int src, std::vector<char> &buffer);
	static int getIntFromBuffer(std::vector<char> &buffer);

	/*
		longת������С��ת����
	*/
	static void pushLongToBuffer(const long long src, std::vector<char> &buffer);
	static long long getLongFromBuffer(std::vector<char> &buffer);

	/*
		byteת��
	*/
	static void pushByteToBuffer(const char src, std::vector<char> &buffer);
	static char getByteFromBuffer(std::vector<char> &buffer);
	static bool getBoolByteFromBuffer(std::vector<char> &buffer);
};

NS_LIB_SOCKET_END

#endif
