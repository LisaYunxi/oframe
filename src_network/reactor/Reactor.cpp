// Reactor.cpp: implementation of the CReactor class.
//
// 20090510 xuzh  NT-0074������HandleOtherTask���������ദ����ִ�еĹ���
//////////////////////////////////////////////////////////////////////

#include "public.h"
#include "Reactor.h"

CReactor::CReactor()
{
	m_bIOListHasNull = false;
	m_bSpeciaFlag=false;
}

CReactor::~CReactor()
{
}

void CReactor::RegisterIO(CEventHandler *pEventHandler)
{
	m_IOList.push_back(pEventHandler);
}

void CReactor::RemoveIO(CEventHandler *pEventHandler)
{
// ֱ��ɾ���ᵼ��ö����ʧЧ
// m_IOList.remove(pEventHandler);

	for (CEventHandlerList::iterator i = m_IOList.begin(); i != m_IOList.end(); i++)
	{
		if ((*i) == pEventHandler) 
		{
			(*i) = NULL;
			m_bIOListHasNull = true;
		}
	}
}

bool CReactor::HandleOtherTask()
{
	if (!m_queueEvent.Empty())
		return true;
	
	if (m_pTimerQueue->CheckExpire(m_nCurrClock))
		return true;
	return false;
}


void CReactor::SetSpeciaFlag(bool bFlag)
{
	m_bSpeciaFlag=bFlag;
}
bool CReactor::GetSpeciaFlag(void)
{
	return m_bSpeciaFlag;
}

