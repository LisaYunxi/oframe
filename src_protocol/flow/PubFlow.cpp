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
	//�Ŷӻ����ù���
	m_iAckSeriesId = nKey;
	if (m_iLastNoAckSeriesId >= 0)
	{
		//����̵߳��ù���	
		if (m_iLastNoAckSeriesId <= m_iAckSeriesId)
		{
			m_iAckResultId = m_iLastNoAckResultId;
			//����֪ͨ
			if (m_pEventHandler)
				m_pEventHandler->Notify();
			m_iLastNoAckResultId = -1;
			m_iLastNoAckSeriesId = -1;
		}
	}
}

bool CPubFlow::SetSIDAndRID(UF_INT8 iSeriesId, UF_INT8 iResultId)
{
	//������ط��ı��������õ����棬��ֹwhileѭ�����ർ��ջ���
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
	//����̵߳��ù���	
	if (iSeriesId <= m_iAckSeriesId)
	{
		m_iAckResultId = iResultId;
		//����֪ͨ
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
