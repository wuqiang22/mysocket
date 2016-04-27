#ifndef __2014_12_19_MEMORY_POOL_H__
#define __2014_12_19_MEMORY_POOL_H__

#include <stdint.h>
#include "Mutex.h"

enum MemSize : uint64_t
{
	SIZE1K = 1024,
	SIZE1M = 1024 * SIZE1K,
	SIZE1G = 1024 * SIZE1M,
};

class Allocator;

class MemoryPool
{
public:
	enum{
		APR_ALLOCATOR_MAX_FREE_UNLIMITED = 0,	//���������������ڴ�û������
		DEFAULT_ALIGN = 1,						//���� DEFAULT_ALIGN �ֽڶ���
		DEFAULT_LIST_SIZE = 20,					//ȱʡ�� free list ��size
	};
	MemoryPool(uint32_t nMinIndex, uint64_t nMaxSize = APR_ALLOCATOR_MAX_FREE_UNLIMITED, uint32_t list_size = DEFAULT_LIST_SIZE);
	virtual ~MemoryPool();
	virtual void* Alloc(uint64_t nAllocaSize);
	virtual bool Free(void* pMem);
private:
	Allocator	*m_pAllocator;
	Mutex		m_mutex;
};

#endif//__2014_12_19_MEMORY_POOL_H__