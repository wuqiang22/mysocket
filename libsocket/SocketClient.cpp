#include "SocketClient.h"

#include "SocketMessage.h"
#include "SocketMessageReader.h"
#include "SocketMessageWriter.h"
#include "MessageActionType.h"
#include "RequestMessage.h"

NS_LIB_SOCKET_START

#define TIME_OUT_TIME 4 //connect超时时间4秒

SocketClient::SocketThread::~SocketThread()
{
	std::unique_lock<std::mutex> lock(this->mutex);
	this->condition.notify_all();
}

SocketClient::SendMessageThread::~SendMessageThread()
{
	delete this->reader;
	this->reader = nullptr;

	for (auto msg : queue)
	{
		delete msg;
	}
	queue.clear();
}

SocketClient::ReceiveMessageThread::~ReceiveMessageThread()
{
	delete this->writer;
	this->writer = nullptr;

	delete this->recvBuffer;
	this->recvBuffer = nullptr;

	for (auto msg : queue)
	{
		delete msg;
	}
	queue.clear();
}

SocketClient::SocketClient()
{
}

SocketClient::~SocketClient()
{
	this->closeClient();
}

SocketClient * SocketClient::create(SocketMessageReader * reader, SocketMessageWriter * writer)
{
	auto client = new SocketClient();
	if (client && client->init(reader, writer))
	{
		return client;
	}
	else
	{
		if (nullptr != client)
		{
			delete client;
		}
		return nullptr;
	}
}

SocketClient::SocketStatus SocketClient::getStatus()
{
	std::unique_lock<std::mutex> lock(this->connectThread->mutex);
	auto status = this->connectThread->status;

	//std::ostringstream ostr;
	//ostr << "get socket status:" << status <<"\n";
	//SOCKETPRINT(ostr.str().c_str());

	return status;
}

void SocketClient::setStatus(SocketClient::SocketStatus status)
{
	std::unique_lock<std::mutex> lock(this->connectThread->mutex);
	this->connectThread->status = status;

	//std::ostringstream ostr;
	//ostr << "set socket status:" << status << "\n";
	//SOCKETPRINT(ostr.str().c_str());
}

bool SocketClient::connectServer(const std::string & ip, const unsigned int port)
{
	std::unique_lock<std::mutex> lock(this->connectThread->mutex);
	if (SocketStatus::Offline == this->connectThread->status  || SocketStatus::Connecting == this->connectThread->status)
	{
		this->newSocket();
		this->connectThread->status = SocketStatus::Connecting;
		this->connectThread->ip = ip;
		this->connectThread->port = port;
		this->connectThread->condition.notify_all();
		return true;
	}
	return false;
}

bool SocketClient::closeClient()
{
	do
	{
		if (SocketStatus::Offline == this->getStatus())
		{
			return false;
		}
		this->socket.Close();
		this->socket.Clean();
		this->setStatus(SocketStatus::Offline);
	} while (false);
	return true;
}

bool SocketClient::sendMessage(RequestMessage * message)
{
	std::unique_lock<std::mutex> lock(this->sendThread->mutex);
	if (message->getType() == MessageActionType::Login)
	{
		this->sendThread->loginMessageToSend = message;
		std::unique_lock<std::mutex> lock(this->receiveThread->mutex);
		this->receiveThread->recvBuffer->clear();
	}
	else
	{
		this->sendThread->queue.push_back(message);
	}
	this->sendThread->condition.notify_all();
	return true;
}

std::vector<ResponseMessage *> SocketClient::receiveMessages()
{
	std::unique_lock<std::mutex> lock(this->receiveThread->mutex);
	if (true == this->receiveThread->queue.empty())
	{
		return std::vector<ResponseMessage *>();
	}
	else
	{
		std::vector<ResponseMessage *> queue(std::move(this->receiveThread->queue));
		this->receiveThread->queue = std::vector<ResponseMessage *>();
		return queue;
	}
}

bool SocketClient::init(SocketMessageReader * reader, SocketMessageWriter * writer)
{
	this->connectThread = new ConnectThread();
	this->sendThread = new SendMessageThread();
	this->sendThread->reader = reader;
	this->receiveThread = new ReceiveMessageThread();
	this->receiveThread->writer = writer;

	this->setStatus(SocketStatus::Offline);

	std::thread(std::bind(&SocketClient::handleConnectServer, this, std::placeholders::_1), this->connectThread).detach();
	std::thread(std::bind(&SocketClient::handleSendMessages, this, std::placeholders::_1), this->sendThread).detach();
	std::thread(std::bind(&SocketClient::handleReceiveMessages, this, std::placeholders::_1), this->receiveThread).detach();

	return true;
}

bool SocketClient::newSocket()
{
	this->socket = ODSocket();
	if (false == this->socket.Create(AF_INET, SOCK_STREAM, 0))
	{
		return false;
	}
	ODSocket::Init();
	return true;
}

void SocketClient::handleConnectServer(ConnectThread * thread)
{
	while (true)
	{
		if (!thread->running)
		{
			break;
		}
		std::unique_lock<std::mutex> lock(thread->mutex);
		if (SocketStatus::Connecting == thread->status)
		{
			auto socket = this->socket;
			lock.unlock();

			int error = -1, len;
			len = sizeof(int);
			timeval tm = socket.buildTimeval(TIME_OUT_TIME, 0);;
			fd_set set;
			unsigned long ul = 1;
			socket.Ioctl(FIONBIO, &ul);  //设置为非阻塞模式
			bool ret = false;
			if (false == socket.Connect(thread->ip.c_str(), thread->port))
			{
				FD_ZERO(&set);
				FD_SET(socket.getSocket(), &set);
				int result = select(socket.getSocket() + 1, NULL, &set, NULL, &tm);
				if (result > 0)
				{
					#ifdef WIN32
						char optvar;
					#else
						int optvar;
					#endif
					error = getsockopt(socket.getSocket(), SOL_SOCKET, SO_ERROR, &optvar, (socklen_t *)&len);
					if (error == 0)
					{
						ret = true;
					}
					else
					{
						ret = false;
					}
				}
				else
				{
					ret = false;
				}
			}
			else
			{
				ret = true;
			}
			ul = 0;
			socket.Ioctl(FIONBIO, &ul);  //设置为阻塞模式

			if (true == ret)
			//if (true == socket.Connect(thread->ip.c_str(), thread->port))
			{
				std::unique_lock<std::mutex> connectLock(thread->mutex);
				thread->status = SocketStatus::Online;
				std::unique_lock<std::mutex> sendLock(this->sendThread->mutex);
				this->sendThread->condition.notify_all();
				std::unique_lock<std::mutex> receiveLock(this->receiveThread->mutex);
				this->receiveThread->condition.notify_all();
			}
			else
			{
				if (thread->status == SocketStatus::Online)
				{
					continue;
				}
				std::unique_lock<std::mutex> sendLock(this->sendThread->mutex);
				this->sendThread->loginMessageToSend = nullptr;
				thread->status = SocketStatus::Offline;
			}
		}
		else
		{
			thread->condition.wait(lock);
		}
	}
	delete thread;
}


void SocketClient::handleSendMessages(SendMessageThread * thread)
{
	while (true)
	{
		if (!thread->running)
		{
			break;
		}
		std::unique_lock<std::mutex> lock(thread->mutex);
		if (this->getStatus() == SocketStatus::Online && 
			(false == thread->queue.empty() || thread->loginMessageToSend != nullptr))
		{
			std::vector<RequestMessage *> queue(std::move(thread->queue));
			if (thread->loginMessageToSend != nullptr)
			{
				queue.insert(queue.begin(), thread->loginMessageToSend);
				thread->loginMessageToSend = nullptr;
			}
			thread->queue = std::vector <RequestMessage *>();
			lock.unlock();

			ODSocket & socket = this->socket;
			for (auto iterator = queue.begin(); iterator != queue.end(); iterator++)
			{
				auto message = *iterator;
				const std::vector<char> & chars = thread->reader->readMessage(message);
				delete message;

				int result = socket.Send(&chars.front(), chars.size(), 0);
				if (result <= 0)
				{
					//printf("socket error:%i\n", socket.GetError());
					std::ostringstream ostr;
					ostr << "socket error:" << socket.GetError() << "\n";
					SOCKETPRINT(ostr.str().c_str());

					socket.Clean();
					socket.Close();

					SOCKETPRINT("handleSend:socket closed~\n");
					
					this->setStatus(SocketStatus::Offline);
					continue;
				}
			}
		}
		else
		{
			thread->condition.wait(lock);
		}
	}
	delete thread;
}

void SocketClient::handleReceiveMessages(ReceiveMessageThread * thread)
{
	static const int BUFFER_SIZE = 1024;
	char buffer[BUFFER_SIZE];
	while (true)
	{
		if (!thread->running)
		{
			break;
		}
		std::unique_lock<std::mutex> lock(thread->mutex);
		if (this->getStatus() == SocketStatus::Online)
		{
			lock.unlock();
			ODSocket & socket = this->socket;
			int count = socket.Recv(buffer, BUFFER_SIZE, 0);
			std::unique_lock<std::mutex> lock2(thread->mutex);
			if (count <= 0)
			{
				std::ostringstream ostr;
				ostr << "socket error:" << socket.GetError() << "\n";
				SOCKETPRINT(ostr.str().c_str());

				socket.Clean();
				socket.Close();

				SOCKETPRINT("handleSend:socket closed~\n");


				this->setStatus(SocketStatus::Offline);
				continue;
			}
			std::vector<char> buffers(buffer, buffer + count);
			thread->recvBuffer->pushData(buffers);
			std::vector<ResponseMessage*> responseMessages;
			auto bufferVector = thread->recvBuffer->popData();
			while (bufferVector.size() > 0)
			{
				ResponseMessage* message = thread->writer->writeMessage(bufferVector);
				if (message != NULL)
				{
					responseMessages.push_back(message);
				}
				bufferVector = thread->recvBuffer->popData();
			}
			if (responseMessages.size() > 0)
			{
				for (auto it : responseMessages)
				{
					thread->queue.push_back(it);
				}
			}
		}
		else
		{
			thread->condition.wait(lock);
		}
	}
	delete thread;
}

NS_LIB_SOCKET_END
