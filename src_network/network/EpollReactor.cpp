#include "public.h"
#include "EpollReactor.h"

#ifdef LINUX
const int SR_DEFAULT_EPOLL_TIMEOUT = 10000; //Î¢Ãë

CEpollReactor::CEpollReactor()
{
	m_fdEpoll = epoll_create(EPOLL_MAX_EVENTS);
}

CEpollReactor::~CEpollReactor()
{
	close(m_fdEpoll);
}

void CEpollReactor::RegisterIO(CEventHandler *pEventHandler)
{
	int nReadID, nWriteID;
	pEventHandler->GetIds(&nReadID, &nWriteID);
	if (nWriteID != 0 && nReadID ==0)
	{
		nReadID = nWriteID;
	}
	if (nReadID != 0)
	{
		m_mapEventHandlerId[pEventHandler] = nReadID;
		struct epoll_event ev;
		ev.data.ptr = pEventHandler;
		if(epoll_ctl(m_fdEpoll, EPOLL_CTL_ADD, nReadID, &ev) != 0)
		{
			perror("epoll_ctl EPOLL_CTL_ADD");
		}
	}
}

void CEpollReactor::RemoveIO(CEventHandler *pEventHandler)
{
	CEventHandlerIdMap::iterator it = m_mapEventHandlerId.find(pEventHandler);
	if (it != m_mapEventHandlerId.end())
	{
		struct epoll_event ev;
		epoll_ctl(m_fdEpoll, EPOLL_CTL_DEL, (*it).second, &ev);
		m_mapEventHandlerId.erase(it);
	}
}

void CEpollReactor::DispatchIOs()
{
	unsigned int dwSelectTimeOut = SR_DEFAULT_EPOLL_TIMEOUT;
	if (HandleOtherTask())
	{
		dwSelectTimeOut = 0;
	}

	struct epoll_event ev;
	CEventHandlerIdMap::iterator itor = m_mapEventHandlerId.begin();
	for(; itor!=m_mapEventHandlerId.end(); itor++)
	{
		CEventHandler *pEventHandler = (CEventHandler *)(*itor).first;
		if(pEventHandler == NULL){
			continue;
		}
		ev.data.ptr = pEventHandler;
		ev.events = 0;
		int nReadID, nWriteID;
		pEventHandler->GetIds(&nReadID, &nWriteID);	
		if (nReadID > 0)
		{
			ev.events |= EPOLLIN;
		}
		if (nWriteID > 0)
		{
			ev.events |= EPOLLOUT;
		}
		epoll_ctl(m_fdEpoll, EPOLL_CTL_MOD, (*itor).second, &ev);
	}
	
	struct epoll_event events[EPOLL_MAX_EVENTS];

	int nfds = epoll_wait(m_fdEpoll, events, EPOLL_MAX_EVENTS, dwSelectTimeOut/1000);

	for (int i=0; i<nfds; i++)
	{
		struct epoll_event &evref = events[i];
		CEventHandler *pEventHandler = (CEventHandler *)evref.data.ptr;
		if ((evref.events|EPOLLIN)!=0 && m_mapEventHandlerId.find(pEventHandler)!=m_mapEventHandlerId.end())
		{
			pEventHandler->HandleInput();
		}
		if ((evref.events|EPOLLOUT)!=0 && m_mapEventHandlerId.find(pEventHandler)!=m_mapEventHandlerId.end())
		{
			pEventHandler->HandleOutput();
		}
	}	
}

#endif
