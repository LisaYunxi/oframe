// Reactor.cpp: implementation of the CReactor class.
//
// 20090510 xuzh  NT-0074：新增HandleOtherTask用于派生类处理反复执行的工作
//////////////////////////////////////////////////////////////////////

#include "public.h"
#include "BlockReactor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlockReactor::CBlockReactor(SPEED_MODE sm)
	: CBlockEventDispatcher(sm)
{
	m_bIOListHasNull = false;
	m_bSpeciaFlag=false;
}

CBlockReactor::~CBlockReactor()
{
}

void CBlockReactor::RegisterIO(CBlockEventHandler *pEventHandler)
{
	m_IOList.push_back(pEventHandler);
}

void CBlockReactor::RemoveIO(CBlockEventHandler *pEventHandler)
{
// 直接删除会导致枚举器失效
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

bool CBlockReactor::HandleOtherTask()
{
	if (!m_queueEvent.empty())
	{
		return true;
	}
	
	//add by zbz
	
	if (m_pTimerQueue->CheckExpire(m_nCurrClock))
	{
		return true;
	}
	return false;
}


void CBlockReactor::SetSpeciaFlag(bool bFlag)
{
	m_bSpeciaFlag=bFlag;
}
bool CBlockReactor::GetSpeciaFlag(void)
{
	return m_bSpeciaFlag;
}

