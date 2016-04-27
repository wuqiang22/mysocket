/*******************************************************************
**
** author : xzben 2014/12/09
** �洢�ͻ��˽��յ������ݰ�
*******************************************************************/

#ifndef __2014_12_09_PACKET_H__
#define __2014_12_09_PACKET_H__

#include <queue>
#include "Mutex.h"

#define USE_MEMORY_POOL 0
#define PACKET_MAX_SIZE 40960

class PacketBuffer
{
public:
	PacketBuffer();
	~PacketBuffer();

	void*	getBuffer();
	void*   getFreeBuffer();
	int		getFreeSize();
	int		getDataSize();
	void	FillData(int size, void* data = nullptr);
	void	ReadData(int size, void* buffer = nullptr);
#if USE_MEMORY_POOL > 0
	void*	operator new(size_t size);
	void*	operator new[](size_t size);
	void	operator delete(void *pobj);
	void	operator delete[](void *pobj);
#endif

private:
	int		m_dataSize;					//��ǰ��Ч���ݴ�С
	char	m_data[PACKET_MAX_SIZE];	//����buffer
	int		m_read;						//���ݶ�ȡindex
	int		m_write;					//����д��index
};

#endif//__2014_12_09_PACKET_H__