#include "public.h"
#include "PubFlow.h"
#include "FileFlow.h"
#include "FlowReader.h"
#include "monitorIndex.h"

CPubFlow::CPubFlow(CFlow *pFlow)
{
	m_pUnderFlow = pFlow;
	m_iAckSeriesId = -1;
	m_iAckResultId = 0;
	m_iLastNoAckSeriesId = -1;
	m_iLastNoAckResultId = -1;
}

CPubFlow::~CPubFlow()
{
}

UF_INT8 CPubFlow::GetCount()
{
	return m_iAckResultId;
}

int CPubFlow::Get(UF_INT8 id, void *pObject, int length)
{
	return m_pUnderFlow->Get(id, pObject, length);
}

void CPubFlow::ConfirmQueued(UF_INT8 nKey)
{
	//排队机调用过来
	m_iAckSeriesId = nKey;
	if (m_iLastNoAckSeriesId >= 0)
	{
		//撮合线程调用过来	
		if (m_iLastNoAckSeriesId <= m_iAckSeriesId)
		{
			m_iAckResultId = m_iLastNoAckResultId;
			//产生通知
			if (m_pEventHandler)
				m_pEventHandler->Notify();
			m_iLastNoAckResultId = -1;
			m_iLastNoAckSeriesId = -1;
		}
	}
}

bool CPubFlow::SetSIDAndRID(UF_INT8 iSeriesId, UF_INT8 iResultId)
{
	//把这个地方的变量定义拿到外面，防止while循环过多导致栈溢出
#ifndef	WIN32	
	struct timeval delay;
#endif

	while (m_iLastNoAckSeriesId >= 0)
	{
#ifdef	WIN32	
		Sleep(1);
#else	
		//struct timeval delay;
		delay.tv_sec = 0;
		delay.tv_usec = 10; //10 us
		int err = 0;
		do
		{
			err = select(0, NULL, NULL, NULL, &delay);
		} while (err < 0 && errno == EINTR);
#endif
	}
	//撮合线程调用过来	
	if (iSeriesId <= m_iAckSeriesId)
	{
		m_iAckResultId = iResultId;
		//产生通知
		if (m_pEventHandler)
			m_pEventHandler->Notify();
		return true;
	}
	else
	{
		m_iLastNoAckSeriesId = iSeriesId;
		m_iLastNoAckResultId = iResultId;
		return false;
	}
}

int CPubFlow::GetFlowType()
{
	return FLOW_TYPE_PUBFLOW;
}
