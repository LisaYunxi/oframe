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

///�̰߳�ȫ�Ķ���
template<typename Data>
class ConcurrentQueue
{
public:
	ConcurrentQueue(bool bWait)
		: m_pFirstSyncEvent(NULL)
		, m_pLastSyncEvent(NULL)
		, m_sem(0) // Ĭ�� nonsignaled
		, m_size(0)
		, m_bWait(bWait)
	{
		INIT_CRITICAL_VAR(m_varCritical);
	}

	~ConcurrentQueue()
	{
		DELETE_CRITICAL_VAR(m_varCritical);
	}

	//ͷ�˴����µ�����
	void push_front(Data const& data)
	{
		ENTER_CRITICAL(m_varCritical);

		m_size++;
		the_queue.push_front(data);	
		notify();

		LEAVE_CRITICAL(m_varCritical);
	}

	//β�˴����µ�����
	void push_back(Data const& data)
	{
		ENTER_CRITICAL(m_varCritical);

		m_size++;
		the_queue.push_back(data);

		notify();

		LEAVE_CRITICAL(m_varCritical);
	}

	//�������Ƿ�Ϊ��
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

	//ȡ�� millsecond -1 ���� 0 ������
	bool wait_and_pop(Data& popped_value, int millsecond)
	{
		ENTER_CRITICAL(m_varCritical);

		if (0 == m_size)
		{
			LEAVE_CRITICAL(m_varCritical);

			if (!m_bWait) // ���ȴ�
				return false;

			// 0�� �������ȴ�
			if (0 == millsecond)
				return false;
			// -1�� �����ȴ�
			else if (-1 == millsecond)
			{
				m_sem.Lock();
				ENTER_CRITICAL(m_varCritical); // �յ���Ϣ ���»�ȡ��
			}
			// >0�� �������ȴ�
			else 
			{
				if (m_sem.Lock(millsecond)) // thread is released
					ENTER_CRITICAL(m_varCritical); // �յ���Ϣ ���»�ȡ��
				else // ��ʱ
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

		popped_value = the_queue.front();		//��ȡ�����еĵ�һ������
		popped_value.pSync = NULL;
		the_queue.pop_front();					//ɾ��������

		LEAVE_CRITICAL(m_varCritical);

		return true;
	}

private:
	void notify()
	{
		if (m_bWait && m_size == 1) // ��һ������
			m_sem.UnLock(); // signaled
	}


private:
	bool m_bWait; // �Ƿ���Ҫ�ȴ�
	std::deque<Data> the_queue;
	CRITICAL_VAR m_varCritical;

	int m_size;

	CSemaphore m_sem;

	CSyncBlockEvent<Data> *m_pFirstSyncEvent;
	CSyncBlockEvent<Data> *m_pLastSyncEvent;
};