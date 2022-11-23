// EventQueue.h: interface for the CEventQueue class .

#ifndef AFX_EVENTQUEUE_H__
#define AFX_EVENTQUEUE_H__

#if _MSC_VER > 1000
#pragma once
#endif 


#include "Semaphore.h"
#include "Mutex.h"
#include "EventHandler.h"

struct CSyncEvent
{
	CEvent event;
	CSemaphore sem;
	int nRetval;
	CSyncEvent *pNext;
};

class  CEventQueue  
{
public:
	CEventQueue(int nQueueSize);
	virtual ~CEventQueue();

	bool AddPostEvent(CEventHandler *pEventHandler, int nEventID, unsigned int dwParam, void *pParam);
	bool AddSyncEvent(CSyncEvent *pEvent);
	bool PeekEvent(CEvent &event);

	void ClearEvent(CEventHandler *pEventHandler);
	void RemoveAll();
	bool Empty();
protected:
	CEvent *m_pPostEvent;
	int m_nEventSize;
	int m_nReadPos;
	int m_nWritePos;

	CSyncEvent *m_pFirstSyncEvent;
	CSyncEvent *m_pLastSyncEvent;

	CMutex m_lock;
};

#endif
