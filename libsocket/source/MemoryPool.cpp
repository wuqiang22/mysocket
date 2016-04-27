#include "MemoryPool.h"
#include <cassert>
#include <cstdlib>

typedef unsigned int uint32;
typedef int			 int32;

//basic memory node structure
struct apr_memnode_t{
	uint32			magic;			//���ڱ������ڴ�����ڴ�������			    
	apr_memnode_t	*next;			//ָ����һ���ڴ�ռ�ڵ�
	apr_memnode_t	**ref;			//ָ��ǰ�ڴ�ռ�ڵ�
	uint32			index;			//��ǰ�ڴ�ռ�ڵ���ܹ��ڴ��С
	uint32			free_index;		//��ǰ�ڴ�ռ��еĿ��ÿռ�
	char*			first_avail;	//ָ��ǰ���ÿռ����ʼ��ַ����	������
	char*			endp;			//ָ��ǰ���õ�ַ�Ľ�����ַ��
};

/*
*	function:	������ӽ�nSize �� nBoundary ������������������ð�ָ���ֽڶ����Ĵ�С
*	parameter:	__nSize Ϊ������ _nBoundary������Ϊ 2 �ı���
*	example:	Align(7�� 4) = 8��Align(21, 16) = 32
*/
#define Align(__nSize, __nBoundary) (( (__nSize) + (__nBoundary)-1) & ~((__nBoundary) - 1))

class Allocator
{
public:

public:
	/*
	*	nMinIndex		���ڼ�����С�ڴ���С ��С�ڴ浥Ԫ��Ĵ�С = Align(1<<nMinIndex, DEFAULT_ALIGN)
	*	nMaxSize		������list�����ҿ����ظ����õ��ڴ��С
	*/
	Allocator(uint32_t nMinIndex, uint64_t nMaxSize = MemoryPool::APR_ALLOCATOR_MAX_FREE_UNLIMITED, uint32_t list_size = MemoryPool::DEFAULT_LIST_SIZE);
	virtual ~Allocator();
	inline const int GetMemNodeSize()
	{
		const int nMemNodeSize = Align(sizeof(apr_memnode_t), MemoryPool::DEFAULT_ALIGN);
		return nMemNodeSize;
	}
	/*
	*	��õ�ǰ������������ڴ��ı��ֵ
	*/
	inline uint32 GetMagic()
	{
		return m_uiMagic;
	}
	/*
	*	��� nAllocSize �ռ��С�Ľڵ�
	*/
	apr_memnode_t*  Alloc(size_t nAllocSize);
	/*
	*	�ͷ�node�ڵ�Ŀռ䣬ע��������ͷŲ�һ����ֱ�Ӹ�ϵͳ����
	*	��������ʱ���ڷ������У����´�Ҫ�õ��ڴ�ʹ��
	*/
	void Free(apr_memnode_t *node);
private:
	/*
	*	����һ���ϴ���������
	*/
	static inline uint32	CreateMagic()
	{
		double start = 1, end = RAND_MAX;
		double uiMagic = (start + (end - start)*rand() / (RAND_MAX + 1.0));
		uiMagic *= uiMagic;
		return (uint32)uiMagic;
	}

	/*
	*	function:	���÷����ӵ�����ڴ����ռ����ƣ������ù�ϵ����
	*				�����������ж���ڴ�ʱ�Ὣ�ڴ淵�ظ�ϵͳ����
	*	paramter:	allocator : Ҫ���õķ����ӣ� nSize�� Ҫ���õĴ�С
	*
	*/
	void inline SetMaxSize(size_t nSize)
	{
		uint32 uiMaxIndex = Align(nSize, BOUNDARY_SIZE) >> BOUNDARY_INDEX;

		m_uiMaxIndex = uiMaxIndex;
	}
	/*
	*	���������й��صĿռ�ȫ����ϵͳ����
	*/
	void Destroy();
private:
	uint32_t			m_uiMagic;				//���ڼ�¼�η�����������ڴ��ı��ֵ
	uint32_t			m_uiCurMaxBlockIndex;	//�������е�ǰ���õ�����ĵĴ�Сindex
	uint32_t			m_uiMaxIndex;			//���������Դ洢�����ռ��С	m_uiMaxIndex * BOUNDARY_SIZE
	uint32_t			m_uiCurAllocIndex;		//��ǰ�Ѿ�����Ŀ����ڷ������еĿռ��С  m_uiCurAllocIndex * BOUNDARY_SIZE
	Mutex				m_mutex;				//���̷߳�����
	apr_memnode_t		**m_pfree;	//��������ǰ���صĿ����ڴ��
	const uint32_t		BOUNDARY_INDEX;			//��С���ڴ���index
	const uint32_t		BOUNDARY_SIZE;			//�ڴ���е��ڴ��ĵ�Ԫ��С
	const uint32_t		MIN_ALLOC;				//�ڴ������С������ڴ��С = 2*BOUNDAY_SIZE
	const uint32_t		LIST_SIZE;				//�ڴ���йҿ������ڴ��list�Ĵ�С,���С�����ڴ���пɹҿ������ֲ�ͬ��С���ڴ�顣

};

/////////////////////////////////////////////////////////////////////////////////////////
//class Allocator public
Allocator::Allocator(uint32_t nMinIndex, uint64_t nMaxSize /* = APR_ALLOCATOR_MAX_FREE_UNLIMITED */, uint32_t list_size /*= DEFAULT_LIST_SIZE*/)
	:BOUNDARY_INDEX(nMinIndex),
	BOUNDARY_SIZE(Align(1 << BOUNDARY_INDEX, MemoryPool::DEFAULT_ALIGN)),
	MIN_ALLOC(2 * BOUNDARY_SIZE),
	LIST_SIZE(list_size)
{
	m_uiMagic = CreateMagic();
	m_uiCurMaxBlockIndex = 0; //��ʼ״̬�£�m_pfree[] Ϊ�գ�����û�������ÿ� 
	m_uiMaxIndex = MemoryPool::APR_ALLOCATOR_MAX_FREE_UNLIMITED;//��ʼ״̬Ϊ�ɴ洢�ռ�����
	m_uiCurAllocIndex = 0;//��ǰ�Ѿ�����Ŀ����ڷ������еĿռ��С����ֵ������ m_uiMaxIndex��Χ��
	m_pfree = new apr_memnode_t*[LIST_SIZE];

	memset(m_pfree, 0, sizeof(apr_memnode_t*)*LIST_SIZE);

	if (nMaxSize != MemoryPool::APR_ALLOCATOR_MAX_FREE_UNLIMITED)
		SetMaxSize(nMaxSize);
}

Allocator::~Allocator()
{
	Destroy();
}
apr_memnode_t* Allocator::Alloc(size_t nAllocSize)
{
	apr_memnode_t *node, **ref;
	uint32 uiCurMaxBlockIndex;
	size_t nSize, i, index;

	const int nMemNodeSize = GetMemNodeSize();

	nSize = Align(nAllocSize + nMemNodeSize, BOUNDARY_SIZE);
	if (nSize < nAllocSize) //��������nAllocSize�����¼����nSize��nAllocSizeС
	{
		return NULL;
	}
	if (nSize < MIN_ALLOC)
		nSize = MIN_ALLOC;

	//������С��size = MIN_ALLOC = 2*BOUNDARY_SIZE ����index ��С����Ϊ 1
	index = (nSize >> BOUNDARY_INDEX) - 1;
	if (index > UINT32_MAX) //����Ŀռ�����򲻷���
	{
		return NULL;
	}

	Guard guard(&m_mutex);
	//��ǰ���ڴ�С���õ��ڴ��
	if (index <= m_uiCurMaxBlockIndex)
	{
		uiCurMaxBlockIndex = m_uiCurMaxBlockIndex;
		ref = &m_pfree[index];
		i = index;
		while (NULL == *ref && i < uiCurMaxBlockIndex)
			ref++, i++;

		if (NULL != (node = *ref))
		{
			//����ҵ��Ŀ����ڴ���ǵ�ǰ�����������Ŀ飬�������һ������
			//����·������е�ǰ�Ŀ�������
			if (NULL == (*ref = node->next) && i >= uiCurMaxBlockIndex)
			{
				do{
					ref--;
					uiCurMaxBlockIndex--;
				} while (NULL == *ref && uiCurMaxBlockIndex > 0);
				m_uiCurMaxBlockIndex = uiCurMaxBlockIndex;
			}

			m_uiCurAllocIndex -= (node->index + 1);
			if (m_uiCurAllocIndex < 0) m_uiCurAllocIndex = 0;

			node->next = NULL;
			node->first_avail = (char*)node + nMemNodeSize;
			return node;
		}
	}
	else if (m_pfree[0])//����п��õĴ��ڴ���ڿ��õĴ��ڴ����Ѱ��
	{
		ref = &m_pfree[0];
		while (NULL != (node = *ref) && index > node->index)
			ref = &node->next;

		if (node)
		{
			*ref = node->next;
			m_uiCurAllocIndex -= (node->index + 1);
			if (m_uiCurAllocIndex < 0) m_uiCurAllocIndex = 0;

			node->next = NULL;
			node->first_avail = (char*)node + nMemNodeSize;
			return node;
		}
	}

	//����������ڴ�ʧ��
	if (NULL == (node = (apr_memnode_t*)malloc(nSize)))
	{
		return NULL;
	}

	node->magic = m_uiMagic;
	node->next = NULL;
	node->index = index;
	node->first_avail = (char*)node + nMemNodeSize;
	node->endp = (char*)node + nSize;

	return node;
}
void Allocator::Free(apr_memnode_t *node)
{
	apr_memnode_t *next, *freelist = NULL;
	uint32 index, uiCurMaxBlockIndex;
	uint32 uiMaxIndex, uiCurAllocIndex;

	m_mutex.lock();

	uiCurMaxBlockIndex = m_uiCurMaxBlockIndex;
	uiMaxIndex = m_uiMaxIndex;
	uiCurAllocIndex = m_uiCurAllocIndex;
	do{
		next = node->next;
		index = node->index;

		if (MemoryPool::APR_ALLOCATOR_MAX_FREE_UNLIMITED != uiMaxIndex
			&& uiCurAllocIndex >= uiMaxIndex) //�����ǰ list �ҿ����ڴ���size ������ ����������ͷŴ˽ڵ�
		{
			node->next = freelist;
			freelist = node;
		}
		else if (index < LIST_SIZE)
		{
			if (NULL == (node->next = m_pfree[index])
				&& index > uiCurMaxBlockIndex)
			{
				uiCurMaxBlockIndex = index;
			}
			m_pfree[index] = node;

			uiCurAllocIndex += (index + 1);
		}
		else
		{
			node->next = m_pfree[0];
			m_pfree[0] = node;

			uiCurAllocIndex += (index + 1);
		}
	} while (NULL != (node = next));
	m_uiCurMaxBlockIndex = uiCurMaxBlockIndex;
	m_uiCurAllocIndex = uiCurAllocIndex;
	m_mutex.unlock();

	while (NULL != freelist)
	{
		node = freelist;
		freelist = node->next;
		free(node);
	}
}

void Allocator::Destroy()
{
	uint32 index;
	apr_memnode_t *node, **ref;

	for (index = 0; index < LIST_SIZE; index++)
	{
		ref = &m_pfree[index];
		while ((node = *ref) != NULL){
			*ref = node->next;
			free(node);
		}
	}
	delete[] m_pfree;
}
/////////////////////////////////////////////////////////////////////////////////////////
//class MemoryPool public
MemoryPool::MemoryPool(uint32_t nMinIndex, uint64_t nMaxSize /*= MemoryPool::APR_ALLOCATOR_MAX_FREE_UNLIMITED*/, uint32_t list_size /*= MemoryPool::DEFAULT_LIST_SIZE*/)
{
	m_pAllocator = new Allocator(nMinIndex, nMaxSize, list_size);
}

void* MemoryPool::Alloc(uint64_t nAllocaSize)
{
	apr_memnode_t* node = m_pAllocator->Alloc(nAllocaSize);
	if (node == NULL)
	{
		return NULL;
	}
	return node->first_avail;
}
bool MemoryPool::Free(void* pMem)
{
	if (NULL == pMem)
	{
		return false;
	}
	apr_memnode_t* node = (apr_memnode_t*)((char*)pMem - m_pAllocator->GetMemNodeSize());
	if (node->magic != m_pAllocator->GetMagic()) //����˶��ڴ治�Ǵ��ڴ�صķ����������
	{
		assert(false);
		return false;
	}
	m_pAllocator->Free(node);
	return true;
}
MemoryPool::~MemoryPool()
{
	if (m_pAllocator)
		delete m_pAllocator;
}