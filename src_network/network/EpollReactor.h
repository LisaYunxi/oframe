#ifndef AFX_EPOLLREACTOR_H__
#define AFX_EPOLLREACTOR_H__

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef LINUX

#include "Reactor.h"
#include <sys/epoll.h>

const int EPOLL_MAX_EVENTS = 4096;

class CEpollReactor : public CReactor  
{
public:
	CEpollReactor();
	virtual ~CEpollReactor();
	virtual void RegisterIO(CEventHandler *pEventHandler);
	virtual void RemoveIO(CEventHandler *pEventHandler);
private:
	virtual void DispatchIOs();
private:
	int m_fdEpoll;
	typedef map<CEventHandler *, int> CEventHandlerIdMap;
	CEventHandlerIdMap m_mapEventHandlerId;
};

#endif

#endif
