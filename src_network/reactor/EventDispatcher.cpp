#include "public.h"
#include "EventDispatcher.h"
#include "TimerHeap.h"

const int SM_DISPATCHER_STOP = 0x0001;

CEventDispatcher::CEventDispatcher() :m_queueEvent(2048)
{
	SyncTime();
	m_nStopCode = 0;
	//修改release卡住的问题
	//开始这里需要设立成true，不然后面就没有开启的地方，就会导致任何消息都接受不到。
	//m_bShouldRun = false;
	m_bShouldRun = true;
	m_pTimerQueue = new CTimerHeap(m_nCurrClock);
}

CEventDispatcher::~CEventDispatcher()
{
}

void CEventDispatcher::RegisterTimer(CEventHandler *pEventHandler, int nIDEvent, int nElapse)
{
	m_pTimerQueue->RemoveTimer(pEventHandler, nIDEvent);
	m_pTimerQueue->RegisterTimer(pEventHandler, nIDEvent, nElapse);
}

void CEventDispatcher::RemoveTimer(CEventHandler *pEventHandler, int nIDEvent)
{
	m_pTimerQueue->RemoveTimer(pEventHandler, nIDEvent);
}

void CEventDispatcher::CheckTimer()
{
	m_pTimerQueue->Expire(m_nCurrClock);
}

void CEventDispatcher::Run()
{
	//线程 release的时候，m_bShouldRun=false,让线程退出.
	//但是while(m_bShouldRun)这里之前又会变成true，等于release的m_bShouldRun=false 永远不会起作用.
	//realse被run这里阻塞了,线程卡死在release
	while (m_bShouldRun)
	{
		DispatchIOs();
		SyncTime();
		CheckTimer();
		DispatchEvents();
	}
}

bool CEventDispatcher::PostEvent(CEventHandler *pEventHandler, int message, unsigned int dwParam, void *pParam)
{
	return m_queueEvent.AddPostEvent(pEventHandler, message, dwParam, pParam);
}

int CEventDispatcher::SendEvent(CEventHandler *pEventHandler, int nEventID, unsigned int dwParam, void *pParam)
{
	m_lockActive.Lock();
	if (IsCurrentThread() || !m_bShouldRun)
	{
		int nRetVal;
		//同一线程发出的消息，只要直接调用消息处理函数即可
		if (pEventHandler != NULL)
		{
			nRetVal = pEventHandler->HandleEvent(nEventID, dwParam, pParam);
		}
		else
		{
			nRetVal = HandleEvent(nEventID, dwParam, pParam);
		}
		m_lockActive.UnLock();
		return nRetVal;
	}
	CSyncEvent syncEvent;
	syncEvent.event.pEventHandler = pEventHandler;
	syncEvent.event.nEventID = nEventID;
	syncEvent.event.dwParam = dwParam;
	syncEvent.event.pParam = pParam;
	syncEvent.pNext = NULL;
	syncEvent.sem.Lock();					//将信号量减为0
	m_queueEvent.AddSyncEvent(&syncEvent);
	m_lockActive.UnLock();

	syncEvent.sem.Lock();					//等待另一线程将消息处理完后释放信号量
	return syncEvent.nRetval;
}

void CEventDispatcher::DispatchEvents()
{
	CEvent event;
	CSyncEvent *pSyncEvent;
	while (m_queueEvent.PeekEvent(event))
	{
		int nRetval;

		if (event.pEventHandler != NULL)
		{
			nRetval = event.pEventHandler->HandleEvent(event.nEventID, event.dwParam, event.pParam);
		}
		else
		{
			nRetval = HandleEvent(event.nEventID, event.dwParam, event.pParam);
		}

		if (event.pAdd != NULL)		//同步消息
		{
			pSyncEvent = (CSyncEvent *)event.pAdd;
			pSyncEvent->nRetval = nRetval;
			pSyncEvent->sem.UnLock();
		}
	}
}

void CEventDispatcher::Stop(unsigned int nStopCode)
{
	m_nStopCode = nStopCode;
	//if (m_bShouldRun)
	//{
	SendEvent(NULL, SM_DISPATCHER_STOP, 0, NULL);
	//}
}

int CEventDispatcher::HandleEvent(int nEventID, unsigned int dwParam, void *pParam)
{
	if (nEventID == SM_DISPATCHER_STOP && IsCurrentThread())
	{
		m_lockActive.Lock();
		m_bShouldRun = false;
		m_lockActive.UnLock();
	}
	return 0;
}

#ifdef _EVENT_DISPATHER_TEST

class CMyThread : public CEventDispatcher
{
public:
	CMyThread(CEventDispatcher *pTargetThread, int nThreadID)
	{
		m_pTargetThread = pTargetThread;
		m_nCount = 0;
		m_nThreadID = nThreadID;
	}
protected:
	virtual void DispatchIOs()
	{
		SleepMs(30);
		m_nCount++;
		if (m_pTargetThread != NULL)
		{
			if (m_nCount > 100)
			{
				m_pTargetThread->Stop();
				Stop();
			}
			else
			{
				m_pTargetThread->SendEvent(NULL, 0x1001, 0, NULL);
			}
		}
	}
	virtual void ExitInstance()
	{
		printf("ExitInstance %d\n", m_nThreadID);
	}
private:
	CEventDispatcher *m_pTargetThread;
	int m_nCount;
	int m_nThreadID;
};

int main()
{
	CMutex m;
	m.Lock();
	m.Lock();
	printf("double lock\n");
	m.UnLock();
	m.UnLock();

	const int THREAD_SIZE = 10;
	CMyThread target(NULL, 0);
	target.Create();
	CMyThread *threads[THREAD_SIZE];
	int i = 0;
	for (i = 0; i < THREAD_SIZE; i++)
	{
		threads[i] = new CMyThread(&target, i + 1);
		threads[i]->Create();
	}

	for (i = 0; i < THREAD_SIZE; i++)
	{
		threads[i]->Join();
		delete threads[i];
	}

	return 0;
}

#endif