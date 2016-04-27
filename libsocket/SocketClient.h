#ifndef LIBSOCKET_SOCKETCLIENT_H
#define LIBSOCKET_SOCKETCLIENT_H

#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <condition_variable>
#include <sstream>

#include "odsocket/ODSocket.h"

#include "SocketUtility.h"
#include "RecvBuffer.h"

#if defined(_MSC_VER)
#define SOCKETPRINT( x )           printf(x);
#elif defined (ANDROID_NDK)
#include <android/log.h>
#define SOCKETPRINT( x )           __android_log_print(ANDROID_LOG_INFO, "SOCKETPRINT", "print:'%s' in '%s' at %d.", x, __FILE__, __LINE__);
#else
#include <assert.h>
#define TIXMLASSERT                assert
#define SOCKETPRINT( x )           printf(x);
#endif



class ODSocket;

NS_LIB_SOCKET_START

class RequestMessage;
class ResponseMessage;
class SocketMessageReader;
class SocketMessageWriter;
class RecvBuffer;
class SocketClient
{
public:
	enum SocketStatus
	{
		Offline,
		Connecting,
		Online
	};
private:
	struct SocketThread
	{
		volatile bool running;
		std::mutex mutex;
		std::condition_variable condition;
        SocketThread()
        :running(true)
        {
        }
		virtual ~SocketThread();
	};
	struct ConnectThread : public SocketThread
	{
		std::string ip;
		unsigned int port;
		SocketStatus status;
		virtual ~ConnectThread() = default;
	};
	struct MessageThread : public SocketThread
	{
		virtual ~MessageThread() = default;
	};
	struct SendMessageThread : public MessageThread
	{
		RequestMessage* loginMessageToSend;
		std::vector<RequestMessage *> queue;
		SocketMessageReader * reader;
		SendMessageThread()
			:loginMessageToSend(nullptr)
		{}
		virtual ~SendMessageThread();
	};
	struct ReceiveMessageThread : public MessageThread
	{
		std::vector<ResponseMessage *> queue;
		SocketMessageWriter * writer;
		RecvBuffer* recvBuffer;
		ReceiveMessageThread()
		{
			recvBuffer = new RecvBuffer();
		}
		virtual ~ReceiveMessageThread();
	};
public:
	~SocketClient();

	static SocketClient * create(SocketMessageReader * reader, SocketMessageWriter * writer);

	bool connectServer(const std::string & ip, const unsigned int port);
	bool closeClient();

	bool sendMessage(RequestMessage * message);
	std::vector<ResponseMessage *> receiveMessages();

	SocketStatus getStatus();

private:
	void setStatus(SocketStatus status);
	SocketClient();
	bool init(SocketMessageReader * reader, SocketMessageWriter * writer);

	bool newSocket();

	void handleConnectServer(ConnectThread * connectThread);
	void handleSendMessages(SendMessageThread * sendThread);
	void handleReceiveMessages(ReceiveMessageThread * receiveThread);
private:
	ODSocket socket;
	ConnectThread * connectThread;
	SendMessageThread * sendThread;
	ReceiveMessageThread * receiveThread;
};

NS_LIB_SOCKET_END

#endif
