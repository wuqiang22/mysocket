#include "SocketUtil.h"
#include "odsocket/ODSocket.h"
#include <vector>
#include <string>

NS_LIB_SOCKET_START

uint64_t
ntoh64(const uint64_t *input)
{
	uint64_t rval;
	uint8_t *data = (uint8_t *)&rval;

	data[0] = *input >> 56;
	data[1] = *input >> 48;
	data[2] = *input >> 40;
	data[3] = *input >> 32;
	data[4] = *input >> 24;
	data[5] = *input >> 16;
	data[6] = *input >> 8;
	data[7] = *input >> 0;

	return rval;
}

uint64_t
hton64(const uint64_t *input)
{
	return (ntoh64(input));
}

uint16_t
ntoh16(const uint16_t *input)
{
	uint16_t rval;
	uint8_t *data = (uint8_t *)&rval;

	data[0] = *input >> 8;
	data[1] = *input >> 0;

	return rval;
}

uint16_t
hton16(const uint16_t *input)
{
	return (ntoh16(input));
}

void SocketUtil::pushStringToBuffer(const std::string & str, std::vector<char> & buffer)
{
	auto sizeLen = htonl(str.length());
	char* sizeChar = (char*)&sizeLen;
	buffer.insert(buffer.end(), sizeChar+2, sizeChar + 4);
	buffer.insert(buffer.end(), str.c_str(), str.c_str() + str.length());
}

std::string SocketUtil::getStringFromBuffer(std::vector<char> &buffer)
{
	uint16_t size = *(uint16_t*)(&buffer[0]);
	int16_t hostSize = (int16_t)ntoh16(&size);
	std::string result;
	result = result.assign(buffer.begin() + 2, buffer.begin() + 2 + hostSize);
	buffer.erase(buffer.begin(), buffer.begin() + 2 + hostSize);
	return result;
}

void SocketUtil::pushIntToBuffer(const int src, std::vector<char> &buffer)
{
	auto netSrc = htonl(src);
	char* netSrcChar = (char *)&netSrc;
	buffer.insert(buffer.end(), netSrcChar, netSrcChar + 4);
}

int SocketUtil::getIntFromBuffer(std::vector<char> &buffer)
{
	int des = *(int32_t*)&(*buffer.begin());
	int localDes = (int)ntohl(des);
	buffer.erase(buffer.begin(), buffer.begin() + 4);
	return localDes;
}

void SocketUtil::pushLongToBuffer(const long long src, std::vector<char> &buffer)
{
	int64_t netSrc = (int64_t)hton64((uint64_t*)src);
	char* netSrcChar = (char *)&netSrc;
	buffer.insert(buffer.end(), netSrcChar, netSrcChar + 8);
}

long long SocketUtil::getLongFromBuffer(std::vector<char> &buffer)
{
	uint64_t des = *(uint64_t*)&(*buffer.begin());
	int64_t localDes = ntoh64(&des);
	buffer.erase(buffer.begin(), buffer.begin() + 8);
	return localDes;
}

void SocketUtil::pushByteToBuffer(const char src, std::vector<char> &buffer)
{
	buffer.insert(buffer.end(), &src, &src + 1);
}

char SocketUtil::getByteFromBuffer(std::vector<char> &buffer)
{
	char des = *(unsigned char*)&(*buffer.begin());
	buffer.erase(buffer.begin(), buffer.begin() + 1);
	return des;
}

bool SocketUtil::getBoolByteFromBuffer(std::vector<char> &buffer)
{
	char des = *(unsigned char*)&(*buffer.begin());
	buffer.erase(buffer.begin(), buffer.begin() + 1);
	return des >= '1';
}

NS_LIB_SOCKET_END
