
#include "public.h"
#include "BlockEventDispatcher.h"
#include "BlockTimerHeap.h"

const int SM_BLOCK_DISPATCHER_STOP	= 0x0001;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlockEventDispatcher::CBlockEventDispatcher(SPEED_MODE sm)
	: m_queueEvent(sm != SM_HIGHT)
	, m_speedMode(sm)
{
	SyncTime();
	m_nStopCode = 0;
	m_bShouldRun = false;
	m_pTimerQueue = new CBlockTimerHeap(m_nCurrClock);
	m_nUpdateTime = m_nCurrTime;
}

CBlockEventDispatcher::~CBlockEventDispatcher()
{

}
void CBlockEventDispatcher::RegisterTimer(CBlockEventHandler *pEventHandler, int nIDEvent, int nElapse)
{
	m_pTimerQueue->RemoveTimer(pEventHandler, nIDEvent);
	m_pTimerQueue->RegisterTimer(pEventHandler, nIDEvent, nElapse);
}

void CBlockEventDispatcher::RemoveTimer(CBlockEventHandler *pEventHandler, int nIDEvent)
{
	m_pTimerQueue->RemoveTimer(pEventHandler, nIDEvent);
}

void CBlockEventDispatcher::CheckTimer()
{
	m_pTimerQueue->Expire(m_nCurrClock);
}
void CBlockEventDispatcher::Run()
{
	m_bShouldRun = true;
	while(m_bShouldRun)
	{	
		SyncTime();
		CheckTimer();
		DispatchEvents();
	}
}


bool CBlockEventDispatcher::PostEvent(CBlockEventHandler *pEventHandler, int message, DWORD dwParam, void *pParam, release_func pReleaseFunc)
{
	CBlockEvent ev;
	ev.pEventHandler = pEventHandler;
	ev.nEventID = message;
	ev.dwParam = dwParam;
	ev.pParam = pParam;
	ev.pReleaseFunc = pReleaseFunc;
	m_queueEvent.push_back(ev);

	return true;
}


int CBlockEventDispatcher::SendEvent(CBlockEventHandler *pEventHandler, int message, DWORD dwParam, void *pParam)
{
	m_lockActive.Lock();
	if(IsCurrentThread()|| !m_bShouldRun)
	{
		int nRetVal;
		//同一线程发出的消息，只要直接调用消息处理函数即可
		if(pEventHandler != NULL)
		{
			nRetVal = pEventHandler->HandleEvent(message, dwParam, pParam, NULL);
		}
		else
		{
			nRetVal = HandleEvent(message, dwParam, pParam, NULL);
		}
		m_lockActive.UnLock();
		return nRetVal;
	}
	CBlockEvent ev;
	ev.pEventHandler = pEventHandler;
	ev.nEventID = message;
	ev.dwParam = dwParam;
	ev.pParam = pParam;
	ev.pReleaseFunc = NULL;

	CSyncBlockEvent<CBlockEvent> syncEvent;
	syncEvent.data = ev;
	syncEvent.pNext = NULL;
	syncEvent.sem.Lock();					//将信号量减为0
	m_queueEvent.sync_add(&syncEvent);
	m_lockActive.UnLock();

	syncEvent.sem.Lock();					//等待另一线程将消息处理完后释放信号量
	return syncEvent.ret;
}

void CBlockEventDispatcher::DispatchEvents()
{
	//CBlockEvent event;
	//int millsecond = 0;
	//if (m_nCurrTime - m_nUpdateTime > 5 * 60) // 五分钟没收到消息则阻塞
	//	millsecond = -1;

	//while(m_queueEvent.wait_and_pop(event, millsecond))
	//{		
	//	if (SM_BLOCK_DISPATCHER_STOP == event.nEventID)
	//	{
	//		m_bShouldRun = false;
	//		break;
	//	}
	//	else
	//	{
	//		int nRetval;

	//		if(event.pEventHandler != NULL)
	//		{
	//			nRetval = event.pEventHandler->HandleEvent(event.nEventID, event.dwParam, event.pParam);
	//		}
	//		else
	//		{
	//			nRetval = HandleEvent(event.nEventID, event.dwParam, event.pParam);
	//		}

	//		m_nUpdateTime = m_nCurrTime;
	//	}

	//	if (-1 == millsecond)  // 设置为不阻塞
	//		millsecond = 0;
	//}

	CBlockEvent event;
	CSyncBlockEvent<CBlockEvent> *pSyncEvent;

	int millsecond = -1;
	if (m_pTimerQueue->Count() > 0) // 有计时器 则不能阻塞
		millsecond = 0;

	while(m_queueEvent.wait_and_pop(event, millsecond))
	{		
		if (SM_BLOCK_DISPATCHER_STOP == event.nEventID)
		{
			m_bShouldRun = false;
			break;
		}
		else
		{
			int nRetval;

			if(event.pEventHandler != NULL)
			{
				nRetval = event.pEventHandler->HandleEvent(event.nEventID, event.dwParam, event.pParam, event.pReleaseFunc);
			}
			else
			{
				nRetval = HandleEvent(event.nEventID, event.dwParam, event.pParam, event.pReleaseFunc);
			}

			m_nUpdateTime = m_nCurrTime;

			if (event.pSync != NULL)
			{
				pSyncEvent = (CSyncBlockEvent<CBlockEvent> *)event.pSync;
				pSyncEvent->ret = nRetval;
				pSyncEvent->sem.UnLock();
			}
		}
	}
}

void CBlockEventDispatcher::Stop(DWORD nStopCode)
{
	m_nStopCode = nStopCode;

	PostEvent(NULL, SM_BLOCK_DISPATCHER_STOP, 0, NULL, NULL);
}

int CBlockEventDispatcher::HandleEvent(int nEventID, DWORD dwParam, void *pParam, release_func pReleaseFunc)
{
	if (nEventID == SM_BLOCK_DISPATCHER_STOP && IsCurrentThread()) 
	{
		m_lockActive.Lock();
		m_bShouldRun = false;
		m_lockActive.UnLock();
	}
	return 0;
}
