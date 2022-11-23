#include "public.h"
#include "TimerHeap.h"

const unsigned int MAX_TIMER_HEAP_TIMEOUT = 3600 * 24 * 1000;	//定时器最大不能起过一天


CTimerHeap::CTimerHeap(time_t nClock)
{
	m_nClock = 0;
	m_nClockAlter = nClock;
}

CTimerHeap::~CTimerHeap()
{

}

void CTimerHeap::RegisterTimer(CEventHandler *pEventHandler, int nIDEvent, int nElapse)
{
	CTimerHeapNode node;
	node.nElapse = nElapse;
	node.nIDEvent = nIDEvent;
	node.pTimer = pEventHandler;
	node.nExpire = m_nClock + nElapse;

	push(node);
}

void CTimerHeap::RemoveTimer(CEventHandler *pEventHandler, int nIDEvent)
{
	for (int i = 0; i < size(); i++)
	{
		CTimerHeapNode &node = c[i];
		if (node.pTimer == pEventHandler &&
			(nIDEvent == 0 || node.nIDEvent == nIDEvent))
		{
			node.pTimer = NULL;
		}
	}
}

bool CTimerHeap::CheckExpire(unsigned int nClock)
{
	if (size() > 0)
	{
		CTimerHeapNode node = top();
		if (node.nExpire <= m_nClock)
			return true;
		else
			return false;
	}
	else
		return false;
}


void CTimerHeap::Expire(unsigned int nClock)
{
	SyncTime(nClock);

	while (size() > 0)
	{
		CTimerHeapNode node = top();
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

void CTimerHeap::SyncTime(time_t nClock)
{
	//如果后续时间比之前时间差距太大（5分钟，如果随便向前调整5分钟，会造成5分钟没有定时器工作），则忽略。
	if (nClock - m_nClockAlter - m_nClock > 5 * 60 * 1000)
	{
		int nGap =(int)(nClock - m_nClockAlter);
		PRINT_TO_STD("CurrClock[%d] is bigger than LastClock[%d] for 5*60*1000!", nGap, m_nClock);
		return;
	}

	m_nClock = nClock - m_nClockAlter;
	if (m_nClock > MAX_TIMER_HEAP_TIMEOUT)
	{
		vector<CTimerHeapNode> tmpNodes;
		while (size() > 0)
		{
			tmpNodes.push_back(top());
			pop();
		}
		for (int i = 0; i < tmpNodes.size(); i++)
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

