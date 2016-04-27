#include "network.h"
#include <assert.h>
#include <cstring>
#include <cstdint>

TCPSocket::TCPSocket()
{
	m_hSocket = -1;
}

TCPSocket::~TCPSocket()
{
	this->close();
}

int		TCPSocket::init()
{
	this->close();

	m_hSocket = ::socket(AF_INET, SOCK_STREAM, 0);

	assert(m_hSocket != -1);
	if (m_hSocket == -1)
		return 1;

	return 0;
}

bool	TCPSocket::set_unblock()
{
	if (-1 == m_hSocket)
		return false;

	bool bBlock = false;
#if	_WIN32

	u_long iMode = bBlock ? 0 : 1;
	return (SOCKET_ERROR != ioctlsocket(m_hSocket, FIONBIO, &iMode));

#else

	int32 flag;
	if (flag = fcntl(m_hSocket, F_GETFL, 0) < 0)
		return false;

	if (!bBlock)
		flag |= O_NONBLOCK;
	else
		flag &= ~(O_NONBLOCK);

	if (fcntl(m_hSocket, F_SETFL, flag) < 0)
		return false;

	return true;
#endif
}
bool	TCPSocket::connect(const char* host, short port, int timeval /* = 1000 */)
{
	this->set_unblock();
	sockaddr_in	addrCon;
	memset(&addrCon, 0, sizeof(addrCon));
	addrCon.sin_family = AF_INET;
	addrCon.sin_port = htons(port);
	if (nullptr == host)
	{
#if _WIN32
		addrCon.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
#else
		addrCon.sin_addr.s_addr = htonl(INADDR_ANY);
#endif
	}
	else
	{
#if	_WIN32
		addrCon.sin_addr.S_un.S_addr = inet_addr(host);
#else
		addrCon.sin_addr.s_addr = inet_addr(host);
#endif
	}
	int nRet = ::connect(m_hSocket, (sockaddr*)&addrCon, sizeof(addrCon));

	if (nRet < 0)
	{

#if	_WIN32
		int error = ::GetLastError();
		if (WSAEWOULDBLOCK == error)
#else
		int error = errno;
		if (EINPROGRESS == error)
#endif
		{
			fd_set wset;
			FD_ZERO(&wset);
			FD_SET(m_hSocket, &wset);
#if _WIN32
			int nWidth = 0;
#else
			int32 nWidth = m_hSocket + 1;
#endif
			struct timeval	tmval;
			tmval.tv_sec = long(timeval / 1000);
			tmval.tv_usec = timeval % 1000 * 1000;

			if (::select(nWidth, NULL, &wset, NULL, &tmval) > 0 && FD_ISSET(m_hSocket, &wset))
			{
				int valopt;
				socklen_t nLen = sizeof(valopt);

				getsockopt(m_hSocket, SOL_SOCKET, SO_ERROR, (char*)(&valopt), &nLen);
				if (valopt)
				{
					//fprintf(stderr, "Error in connection() %d - %s/n", valopt, strerror(valopt));
					//exit(0);
					return false;
				}
			}
			else
			{
				//fprintf(stderr, "Timeout or error() %d - %s/n", valopt, strerror(valopt));
				//exit(0);
				return false;
			}
		}
		else
		{
			//fprintf(stderr, "Error connecting %d - %s/n", errno, strerror(errno));
			//exit(0);
			return false;
		}
	}

	return true;
}

int		TCPSocket::recv_msg(void* buffer, int len)
{
	int nRecvSize = ::recv(m_hSocket, (char*)buffer, len, 0);

	return nRecvSize;

}
int		TCPSocket::send_msg(void* buffer, int len)
{
	int nSendSize = ::send(m_hSocket, (char*)buffer, len, 0);

	return nSendSize;
}

bool	TCPSocket::close()
{
	if (-1 == m_hSocket)
		return true;

	bool bRet = false;
#if	_WIN32
	bRet = (0 == ::closesocket(m_hSocket));
#else
	bRet = (0 == ::close(m_hSocket));
#endif

	m_hSocket = -1;

	return bRet;
}

void	TCPSocket::get_status(bool *pReadReady /* = nullptr */, bool* pWriteReady /* = nullptr */, bool* pExceptReady /* = nullptr */, int timeval /* = 1000 */)
{
	if (pReadReady)
		*pReadReady = false;

	if (pWriteReady)
		*pWriteReady = false;

	if (pExceptReady)
		*pExceptReady = false;

	if (-1 == m_hSocket)
		return;

	fd_set setRead, setWrite, setExcept;
	FD_ZERO(&setRead); FD_ZERO(&setWrite); FD_ZERO(&setExcept);
	FD_SET(m_hSocket, &setRead); FD_SET(m_hSocket, &setWrite); FD_SET(m_hSocket, &setExcept);

#if	_WIN32
	int selectWith = 0;
#else
	int selectWith = m_hSocket + 1;
#endif
	struct timeval	tmval;
	tmval.tv_sec = long(timeval / 1000);
	tmval.tv_usec = timeval % 1000 * 1000;

	int nRet = ::select(selectWith,
		pReadReady ? &setRead : nullptr,
		pWriteReady ? &setWrite : nullptr,
		pExceptReady ? &setExcept : nullptr, &tmval);

	if (nRet > 0)
	{
		if (FD_ISSET(m_hSocket, &setRead) && pReadReady != nullptr)
			*pReadReady = true;
		if (FD_ISSET(m_hSocket, &setWrite) && pWriteReady != nullptr)
			*pWriteReady = true;
		if (FD_ISSET(m_hSocket, &setExcept) && pExceptReady != nullptr)
			*pExceptReady = true;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///

void	CCNetwork::socketThreadFunc(void* param)
{
	CCNetwork *networkObj = (CCNetwork*)param;
	networkObj->m_recvThreadCond.wait();

	bool readable = false;
	bool writeable = false;
	int readSize = 0;

	while (!networkObj->m_close)
	{
		if (!networkObj->m_bConnected)
			networkObj->m_recvThreadCond.wait();

		readable = false;
		writeable = false;

		networkObj->m_socket.get_status(&readable, &writeable, nullptr, 1000);

		if (readable)
		{
			PacketBuffer* buf = new PacketBuffer;
			readSize = networkObj->m_socket.recv_msg(buf->getFreeBuffer(), buf->getFreeSize());
			if (readSize > 0)
			{
				buf->FillData(readSize);
				networkObj->m_recv_lock.lock();
				networkObj->m_recvPackets.push(buf);
				networkObj->m_recv_lock.unlock();
			}
			else
			{
				networkObj->push_status(NETSTATE::DISCONNECT);
				networkObj->reset();
			}
		}

		if (writeable)
		{
			networkObj->m_send_lock.lock();
			PacketBuffer* packet = nullptr;
			if (!networkObj->m_sendPackets.empty())
			{
				packet = networkObj->m_sendPackets.front();
			}
			networkObj->m_send_lock.unlock();

			if (packet != nullptr)
			{
				int data_size = packet->getDataSize();
				void* buffer = packet->getBuffer();

				int send_size = networkObj->m_socket.send_msg(buffer, data_size);
				if (send_size > 0)
				{
					packet->ReadData(send_size);
					if (send_size >= data_size)
					{
						networkObj->m_send_lock.lock();
						networkObj->m_sendPackets.pop();
						networkObj->m_send_lock.unlock();
						delete packet;
					}
				}
			}
		}
	}
}

CCNetwork::CCNetwork()
	:m_port(0),
	m_bConnected(false),
	m_close(false)
{
	strcpy(m_szHost, "");
	init();
}

CCNetwork::~CCNetwork()
{
	m_close = true;
	m_recvThread->join();
}

void CCNetwork::reset()
{
	m_socket.init();
	m_bConnected = false;
	m_close = false;
}

bool CCNetwork::init()
{
	m_socket.init();
	m_recvThread = new std::thread(socketThreadFunc, this);
	return true;
}

int	CCNetwork::connect(const char* host, short port, int timeval /*= 1000*/)
{
	strcpy(m_szHost, host);
	port = port;

	reset();
	m_bConnected = false;
	if (!m_socket.connect(host, port, timeval))
	{
		push_status(NETSTATE::FAILED);
		return -1;
	}

	push_status(NETSTATE::SUCCESS);
	m_recvThreadCond.notify_all();
	m_bConnected = true;
	return 0;
}

int	CCNetwork::send_msg(PacketBuffer* buf)
{
	Guard guard(&m_send_lock);
	m_sendPackets.push(buf);
	return 0;
}

void CCNetwork::push_status(int state)
{
	Guard guard(&m_status_lock);
	m_status.push(state);
}

void CCNetwork::update(CallbackStatus statusCallback, CallbackRecvData  dataCallback)
{
	//////////////////////////////////////////////////////////////////////////
	// ���� �Զ��� update ����

	m_status_lock.lock();
	while (!m_status.empty())
	{
		int state = m_status.front();
		m_status.pop();
		statusCallback(state);
	}
	m_status_lock.unlock();

	m_recv_lock.lock();
	while (!m_recvPackets.empty())
	{
		PacketBuffer* packet = m_recvPackets.front();
		const char* buffer = (const char*)packet->getBuffer();
		int len = packet->getDataSize();
		dataCallback(buffer, len);
	}
	m_recv_lock.unlock();
}

//////////////////////////////////////////////////////////////////////////
//�����Զ�װ�� SocketLib ��
#ifdef _WIN32

class SocketLibLoadHelper
{
public:
	SocketLibLoadHelper()
	{
		s_loadSockLib();
	}
	~SocketLibLoadHelper()
	{
		s_destroySockLib();
	}
private:
	static bool			s_loadSockLib(int32_t nHigh = 2, int32_t nLow = 2);
	static bool			s_destroySockLib();
private:
	static	bool		s_bLoadedSockLib;
};

bool SocketLibLoadHelper::s_bLoadedSockLib = false;
bool SocketLibLoadHelper::s_loadSockLib(int32_t nHigh /* = 2 */, int32_t nLow /* = 2 */)
{
	if (s_bLoadedSockLib) //�Ѿ����ع���ֱ�ӷ���
		return true;
	s_bLoadedSockLib = true;

	WORD wVersionRequested;
	WSADATA wsaData;
	int32_t err;

	wVersionRequested = MAKEWORD(nHigh, nLow);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
		return false;

	if (LOBYTE(wsaData.wVersion) != nHigh
		|| HIBYTE(wsaData.wVersion) != nLow)
	{
		WSACleanup();
		return false;
	}
	return true;
}
bool SocketLibLoadHelper::s_destroySockLib()
{
	if (!s_bLoadedSockLib) //��δ���ع������Ѿ�ж����
		return true;

	s_bLoadedSockLib = false;
	if (0 != WSACleanup())
		return false;

	return true;
}

SocketLibLoadHelper g_socketLibLoadHelper; //����һ��ȫ�ֱ�����ʹ���л����Զ�װ�غ�ж�� SocketLib
#endif //_WIN32