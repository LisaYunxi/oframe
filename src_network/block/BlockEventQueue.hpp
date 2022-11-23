#pragma once

//#include <atomic>
#include <deque>
//#include <mutex>
//#include <condition_variable>
#include "Semaphore.h"

template<typename T>
struct CSyncBlockEvent
{
	T data;
	CSemaphore sem;
	int ret;

	CSyncBlockEvent *pNext;
};

///线程安全的队列
template<typename Data>
class ConcurrentQueue
{
public:
	ConcurrentQueue(bool bWait)
		: m_pFirstSyncEvent(NULL)
		, m_pLastSyncEvent(NULL)
		, m_sem(0) // 默认 nonsignaled
		, m_size(0)
		, m_bWait(bWait)
	{
		INIT_CRITICAL_VAR(m_varCritical);
	}

	~ConcurrentQueue()
	{
		DELETE_CRITICAL_VAR(m_varCritical);
	}

	//头端存入新的任务
	void push_front(Data const& data)
	{
		ENTER_CRITICAL(m_varCritical);

		m_size++;
		the_queue.push_front(data);	
		notify();

		LEAVE_CRITICAL(m_varCritical);
	}

	//尾端存入新的任务
	void push_back(Data const& data)
	{
		ENTER_CRITICAL(m_varCritical);

		m_size++;
		the_queue.push_back(data);

		notify();

		LEAVE_CRITICAL(m_varCritical);
	}

	//检查队列是否为空
	bool empty()
	{
		ENTER_CRITICAL(m_varCritical);
		bool ret = (0 == m_size);
		LEAVE_CRITICAL(m_varCritical);

		return ret;
	}

	void sync_add(CSyncBlockEvent<Data> *pEvent)
	{
		ENTER_CRITICAL(m_varCritical);

		m_size++;

		if (NULL == m_pFirstSyncEvent)
		{
			m_pFirstSyncEvent = m_pLastSyncEvent = pEvent;
		}
		else
		{
			m_pLastSyncEvent->pNext = pEvent;

			m_pLastSyncEvent = pEvent;
		}

		notify();

		LEAVE_CRITICAL(m_varCritical);
	}

	//取出 millsecond -1 阻塞 0 不阻塞
	bool wait_and_pop(Data& popped_value, int millsecond)
	{
		ENTER_CRITICAL(m_varCritical);

		if (0 == m_size)
		{
			LEAVE_CRITICAL(m_varCritical);

			if (!m_bWait) // 不等待
				return false;

			// 0： 不阻塞等待
			if (0 == millsecond)
				return false;
			// -1： 阻塞等待
			else if (-1 == millsecond)
			{
				m_sem.Lock();
				ENTER_CRITICAL(m_varCritical); // 收到信息 重新获取锁
			}
			// >0： 半阻塞等待
			else 
			{
				if (m_sem.Lock(millsecond)) // thread is released
					ENTER_CRITICAL(m_varCritical); // 收到信息 重新获取锁
				else // 超时
					return false;
			}
		}

		if (m_pFirstSyncEvent)
		{
			popped_value = m_pFirstSyncEvent->data;
			popped_value.pSync = m_pFirstSyncEvent;

			m_pFirstSyncEvent = m_pFirstSyncEvent->pNext;

			m_size--;

			LEAVE_CRITICAL(m_varCritical);

			return true;
		}

		if (the_queue.empty())
		{
			LEAVE_CRITICAL(m_varCritical);

			return false;
		}

		m_size--;

		popped_value = the_queue.front();		//获取队列中的第一个任务
		popped_value.pSync = NULL;
		the_queue.pop_front();					//删除该任务

		LEAVE_CRITICAL(m_varCritical);

		return true;
	}

private:
	void notify()
	{
		if (m_bWait && m_size == 1) // 第一个数据
			m_sem.UnLock(); // signaled
	}


private:
	bool m_bWait; // 是否需要等待
	std::deque<Data> the_queue;
	CRITICAL_VAR m_varCritical;

	int m_size;

	CSemaphore m_sem;

	CSyncBlockEvent<Data> *m_pFirstSyncEvent;
	CSyncBlockEvent<Data> *m_pLastSyncEvent;
};