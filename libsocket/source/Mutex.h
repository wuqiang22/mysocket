#ifndef __2014_12_9_MUTEX_H__
#define __2014_12_9_MUTEX_H__

#include <mutex>
#include <condition_variable>

/*
*	��������
*/
class Condition
{
public:
	Condition();
	virtual ~Condition();

	void notify_one();
	void notify_all();

	void wait();
private:
	std::condition_variable  m_condition;
	std::mutex				 m_mutex;
	int						 m_NotifyCount;  //Ϊ�˷�ֹαװ�Ļ���
	int						 m_WaitCount;
};

/*
*	��������ʹ�� C++11 �ṩ�Ķ�ʱ������ʵ��
*	����ʹ�õ��Ƕ�ʱ��
*/
class  Mutex
{
public:
	Mutex();
	virtual ~Mutex();
	void lock();
	void unlock();
protected:
	std::mutex	m_lock;
};

class Guard
{
public:
	Guard(Mutex* pMutex)
	{
		m_pGuardMuext = pMutex;
		if (nullptr != m_pGuardMuext)
		{
			m_pGuardMuext->lock();
		}
	}
	~Guard()
	{
		if (nullptr != m_pGuardMuext)
		{
			m_pGuardMuext->unlock();
		}
	}
private:
	Mutex	*m_pGuardMuext;
};
#endif//__2014_12_9_MUTEX_H__