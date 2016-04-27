#ifndef LIBSOCKET_MSGSEQNO_H
#define LIBSOCKET_MSGSEQNO_H

#include "SocketUtility.h"
#include <vector>

NS_LIB_SOCKET_START

class MsgSeqNoManager
{
public:
	static MsgSeqNoManager* getInstance();

	//����Sequence,Լ����������ʱ����
	void resetSequence();


	//ÿȡһ�ζ�������Sequence
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