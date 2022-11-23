// SelectReactor.cpp: implementation of the CSelectReactor class.
//
//////////////////////////////////////////////////////////////////////

#include "public.h"
#include "SelectReactor.h"

static CSocketInit _SOCKET_INIT;

//const int SR_DEFAULT_SELECT_TIMEOUT = 10000; //微秒
const int SR_DEFAULT_SELECT_TIMEOUT = 60; //微秒


CSelectReactor::CSelectReactor(bool bBindThreadsToCPU)
{
	m_bWait = true;
	m_bBindThreadsToCPU = bBindThreadsToCPU;

#ifdef LINUX
	//	if(m_bBindThreadsToCPU)
	//	{// 线程绑定CPU
	//		BindThd2CPU(pthread_self());
	//	}
#endif
	m_bNoShmChannelFlag = true;
}

CSelectReactor::~CSelectReactor()
{

}

void CSelectReactor::DispatchIOs()
{
	unsigned int dwSelectTimeOut = SR_DEFAULT_SELECT_TIMEOUT;
	if (HandleOtherTask())
		dwSelectTimeOut = 0;

	fd_set readfds, writefds;
	int MaxID = 0;
	PrepareIds(readfds, writefds, MaxID);
	int ret = 0;
	if (MaxID == 0)
	{
		//SleepMs(dwSelectTimeOut/1000);
		if (m_bNoShmChannelFlag)
			SleepMs(1);
	}
	else 
	{
		MaxID++;
		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		if (m_bNoShmChannelFlag)
		{
			if (m_bWait)
			{
				timeout.tv_sec = dwSelectTimeOut / 1000000;
				timeout.tv_usec = dwSelectTimeOut % 1000000;
			}
		}
		ret = select(MaxID, &readfds, &writefds, NULL, &timeout);
	}
	SyncTime();
	if (m_bNoShmChannelFlag)
	{
		if (ret > 0)
			DispatchIO(readfds, writefds, MaxID);
	}
	else
	{
		DispatchIO(readfds, writefds, MaxID);
	}

}

void CSelectReactor::PrepareIds(fd_set &readfds, fd_set  &writefds, int &MaxID)
{
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);

	if (m_bIOListHasNull)
	{
		m_IOList.remove(NULL);
		m_bIOListHasNull = false;
	}

	CEventHandlerList::iterator itor = m_IOList.begin();
	for (; itor != m_IOList.end(); itor++)
	{
		if ((*itor) == NULL)
			continue;

		int nReadID, nWriteID;

		nReadID = 0;
		nWriteID = 0;

		CEventHandler *pp = (CEventHandler *)(*itor);
		((CEventHandler *)(*itor))->GetIds(&nReadID, &nWriteID);

		if (nReadID > 0)
		{
			FD_SET(nReadID, &readfds);
			if (MaxID < nReadID)
				MaxID = nReadID;
		}
		if (nWriteID > 0)
		{
			FD_SET(nWriteID, &writefds);
			if (MaxID < nWriteID)
				MaxID = nWriteID;
		}
	}
}

void CSelectReactor::DispatchIO(fd_set &readfds, fd_set  &writefds, int &MaxID)
{
	CEventHandlerList::iterator itor = m_IOList.begin();
	for (; itor != m_IOList.end(); itor++) {
		if ((*itor) == NULL) {
			continue;	//事件处理对象可能已被删除
		}

		int nReadID, nWriteID;
		((CEventHandler *)(*itor))->GetIds(&nReadID, &nWriteID);

		if (nReadID < 0 || (nReadID > 0 && FD_ISSET(nReadID, &readfds)))
		{
			((CEventHandler *)(*itor))->HandleInput();
		}

		if ((*itor) == NULL) {
			continue;	//事件处理对象可能已被删除
		}

		if (nWriteID < 0 || (nWriteID > 0 && FD_ISSET(nWriteID, &writefds)))
		{
			((CEventHandler *)(*itor))->HandleOutput();
		}
	}
}
void CSelectReactor::SetNoShmChannelFlag(bool bFlag)
{
	m_bNoShmChannelFlag = bFlag;
}
