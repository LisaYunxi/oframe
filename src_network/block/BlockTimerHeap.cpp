// TimerHeap.cpp: implementation of the CTimerHeap class.
//
// 2006.12.15 xuzh 修改毫秒溢出时定时器失效的问题
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "BlockTimerHeap.h"

const DWORD MAX_BLOCK_TIMER_HEAP_TIMEOUT = 3600*24*1000;	//定时器最大不能起过一天
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlockTimerHeap::CBlockTimerHeap(DWORD nClock)
{
	m_nClock = 0;
	m_nClockAlter = nClock;
}

CBlockTimerHeap::~CBlockTimerHeap()
{

}

void CBlockTimerHeap::RegisterTimer(CBlockEventHandler *pEventHandler, int nIDEvent, int nElapse)
{
	CBlockTimerHeapNode node;
	node.nElapse = nElapse;
	node.nIDEvent = nIDEvent;
	node.pTimer = pEventHandler;
	node.nExpire = m_nClock + nElapse;

	push(node);
}

void CBlockTimerHeap::RemoveTimer(CBlockEventHandler *pEventHandler, int nIDEvent)
{
	for (int i=0; i<size(); i++)
	{
		CBlockTimerHeapNode &node = c[i];
		if (node.pTimer == pEventHandler && 
			(nIDEvent == 0 || node.nIDEvent == nIDEvent))
		{
			node.pTimer = NULL;
		}
	}
}

bool CBlockTimerHeap::CheckExpire(DWORD nClock)
{
	if (size() > 0)
	{
		CBlockTimerHeapNode node = top();
		if (node.nExpire <= m_nClock)
		{
			return true;
		}
		else
			return false;
	}
	else
		return false;
}


void CBlockTimerHeap::Expire(DWORD nClock)
{
	SyncTime(nClock);

	while (size() > 0)
	{
		CBlockTimerHeapNode node = top();
		if (node.nExpire > m_nClock)
		{
			//最近的定时器尚未到期，不需要做更多的检查了
			break;
		}
		
		pop();

		if (node.pTimer != NULL)
		{
			node.nExpire = m_nClock + node.nElapse;

			push(node);

			node.pTimer->OnTimer(node.nIDEvent);
		}		
	}
}

void CBlockTimerHeap::SyncTime(DWORD nClock)
{
	m_nClock = nClock - m_nClockAlter;
	if (m_nClock > MAX_BLOCK_TIMER_HEAP_TIMEOUT)
	{
		vector<CBlockTimerHeapNode> tmpNodes;
		while (size() > 0)
		{
			tmpNodes.push_back(top());
			pop();
		}
		for (int i=0; i<tmpNodes.size(); i++)
		{
			if (tmpNodes[i].nExpire <= m_nClock)
			{
				tmpNodes[i].nExpire = 0;
			}
			else
			{
				tmpNodes[i].nExpire -= m_nClock;
			}
			push(tmpNodes[i]);
		}
		m_nClockAlter += m_nClock;
		m_nClock = 0;
	}

}

size_t CBlockTimerHeap::Count() const
{
	return size();
}