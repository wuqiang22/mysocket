#include "RecvBuffer.h"
#include "odsocket/ODSocket.h"

NS_LIB_SOCKET_START

RecvBuffer::RecvBuffer()
{
	_data = std::vector<char>();
}

RecvBuffer::~RecvBuffer()
{
}

void RecvBuffer::pushData(std::vector<char> &data)
{
	_data.insert(_data.end(), data.begin(), data.end());
}

std::vector<char> RecvBuffer::popData()
{
	 std::vector<char> result;
	 if (4 > _data.size())
	 {
		 return result;
	 }
	 int size = *(int32_t*)&(*_data.begin());
	 int localsize = (int)ntohl(size);
	 if (localsize + 4 > _data.size())
	 {
		 return result;
	 }
	 result.insert(result.begin(), _data.begin(), _data.begin() + localsize + 4);
	 _data.erase(_data.begin(), _data.begin() + localsize + 4);
	 return result;
}

void RecvBuffer::clear()
{
	_data.clear();
}

NS_LIB_SOCKET_END