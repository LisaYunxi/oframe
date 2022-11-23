// CompInitState.cpp: implementation of the CCompInitState class.
//

//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "CompInitState.h"
#include "ArbPackage.h"


CCompInitState::CCompInitState(CCompServer *pCommServer)
:CCompState(pCommServer)
{
}

CCompInitState::~CCompInitState()
{
}

TCompStateID CCompInitState::GetStateID()
{
	return CS_INIT;
}

void CCompInitState::OnEntry(TCompStateID nLastStateID)
{
	//建立Session工厂，准备与仲裁机连接。
}

void CCompInitState::OnGuardPackageReceived(CGuardPackage *pArbPackage)
{
	if (pArbPackage->GetTid() != HTID_STATE_COMMAND) 
	{
		REPORT_EVENT(LOG_CRITICAL, "ErrorPackage","receive from guard: error tid [0x%08x]", pArbPackage->GetTid());
		return;
	}
	if (pArbPackage->GetStateID() == CS_SINGLE) 
	{
		m_pCommServer->EnableHotlineConn(false);
		m_pCommServer->EnableHotlineListen(true);
		m_pCommServer->ChangeState(CS_SINGLE);
	}
	else if (pArbPackage->GetStateID() == CS_PREPARE) 
	{
		m_pCommServer->EnableHotlineConn(true);
		m_pCommServer->EnableHotlineListen(false);
		m_pCommServer->ChangeState(CS_PREPARE);
	}
	else if(pArbPackage->GetStateID() != CS_INIT)
	{
		REPORT_EVENT(LOG_CRITICAL, "InternalError","receive from guard: irregular state id[init]->[%d]", pArbPackage->GetStateID());
	}
}
