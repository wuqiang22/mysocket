/*******************************************************************
**
** author : xzben 2014/12/09
** 客户端网络数据通信功能封装
*******************************************************************/

#ifndef __2014_12_09_NETWORK_H__
#define __2014_12_09_NETWORK_H__

#include <thread>
#include <queue>
//SOCKET 句柄类型
#if _WIN32
#include<winsock2.h>
#include <MSWSock.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment ( lib, "mswsock.lib")
typedef SOCKET					SOCKET_HANDLE;
typedef int						socklen_t;
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
typedef int					int32;
typedef int					SOCKET_HANDLE;
#endif//平台相关

#include "packet.h"
#include "Mutex.h"

enum NETSTATE
{
	DISCONNECT = 0,
	CONNECTING = 1,
	SUCCESS = 2,
	FAILED = 3,
};

class TCPSocket
{
public:
	TCPSocket();
	~TCPSocket();

	int		init();
	bool	set_unblock();
	/*
	** host 链接的地址
	** port 链接的端口
	** timeval 链接等待超时时间单位毫秒， 默认1000毫秒
	*/
	bool	connect(const char* host, short port, int timeval = 1000);
	int		recv_msg(void* buffer, int len);
	int		send_msg(void* buffer, int len);
	bool	close();
	void	get_status(bool *pReadReady = nullptr, bool* pWriteReady = nullptr, bool* pExceptReady = nullptr, int timeval = 1000);
private:
	SOCKET_HANDLE m_hSocket;
};

class CCNetwork
{
public:
	typedef std::queue<PacketBuffer*>			PacketQueue;
	typedef std::function<void(int)>			CallbackStatus;
	typedef std::function<void(const char*, int)>CallbackRecvData;
	CCNetwork();
	~CCNetwork();
	int		connect(const char* host, short port, int timeval = 1000);
	int		send_msg(PacketBuffer* buf);
	void	update(CallbackStatus statusCallback, CallbackRecvData  dataCallback);
protected:
	static void	socketThreadFunc(void* param);
	bool	init();

	void	reset();
	void	push_status(int state);
private:
	TCPSocket		 m_socket;

	PacketQueue		 m_sendPackets;
	Mutex			 m_send_lock;
	PacketQueue		 m_recvPackets;
	Mutex			 m_recv_lock;
	std::queue<int>  m_status;
	Mutex			 m_status_lock;
	std::thread		 *m_recvThread;
	Condition		 m_recvThreadCond;
	bool			 m_close;
	bool			 m_bConnected;
	char			 m_szHost[20];
	short			 m_port;
};
#endif//__2014_12_09_PACKET_H__