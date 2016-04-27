#include "MsgSeqNoManager.h"

#include <climits>

NS_LIB_SOCKET_START

MsgSeqNoManager* MsgSeqNoManager::_pInstance = nullptr;

MsgSeqNoManager* MsgSeqNoManager::getInstance()
{
	if (_pInstance == nullptr)
	{
		_pInstance = new MsgSeqNoManager();
	}
	return _pInstance;
}

int MsgSeqNoManager::getSequence()
{
	auto result = this->_sequence;
	_updateSequence();
	return result;
}

void MsgSeqNoManager::_updateSequence()
{
	long long next = this->_sequence * 2LL + 1;
	next = next >= INT_MAX ? 0 : next;
	this->_sequence = (int)next;
}

void MsgSeqNoManager::resetSequence()
{
	this->_sequence = kSequenceFrom;
}

MsgSeqNoManager::MsgSeqNoManager()
{
	this->_sequence = kSequenceFrom;
};

MsgSeqNoManager::~MsgSeqNoManager()
{
};

NS_LIB_SOCKET_END