#ifndef LIBSOCKET_MSGSEQNO_H
#define LIBSOCKET_MSGSEQNO_H

#include "SocketUtility.h"
#include <vector>

NS_LIB_SOCKET_START

class MsgSeqNoManager
{
public:
	static MsgSeqNoManager* getInstance();

	//重置Sequence,约定断线重连时重置
	void resetSequence();


	//每取一次都将更新Sequence
	int getSequence();

private:
	static MsgSeqNoManager* _pInstance;
	static const int kSequenceFrom = 0;
	int _sequence;
private:
	MsgSeqNoManager();
	~MsgSeqNoManager();
	void _updateSequence();
};

NS_LIB_SOCKET_END

#endif