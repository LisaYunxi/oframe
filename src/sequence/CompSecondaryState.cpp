#include "public.h"
#include "CompSecondaryState.h"
#include "ArbPackage.h"
#include "HotlinePackage.h"

CCompSecondaryState::CCompSecondaryState(CCompServer *pCompServer)
:CCompState(pCompServer)
{
}

CCompSecondaryState::~CCompSecondaryState()
{
}

TCompStateID CCompSecondaryState::GetStateID()
{
	return CS_SECONDARY;
}
	
void CCompSecondaryState::OnEntry(TCompStateID nLastStateID)
{
}

void CCompSecondaryState::OnHotlinePackageReceived(CHotlinePackage *pHotlinePackage)
{
	if (pHotlinePackage->GetTid() == HTID_CONFIRM_REQ)
		m_pCommServer->ConfirmQueued(pHotlinePackage);
}

void CCompSecondaryState::OnHotlineChannelLost(CHotlineSession *pSession, int ErrorCode)
{
	if (m_pCommServer->m_pArbSession == NULL) 
	{
		m_pCommServer->PostResetEvent("CCompSecondaryState: guard and hotline channel lost");
	}
	m_pCommServer->NotifyArb(ARB_EVENT_HOTLINE_FAIL);
	m_pCommServer->SetTimer(CCompServer::COMP_TIMER_HOTLINE_ID,	CCompServer::COMP_TIMER_HOTLINE_INTERVAL);
}

void CCompSecondaryState::OnGuardPackageReceived(CGuardPackage *pArbPackage)
{
	if (pArbPackage->GetTid() != HTID_STATE_COMMAND) 
	{
		REPORT_EVENT(LOG_CRITICAL, "ErrorPackage", "receive from guard: error tid [0x%08x]", pArbPackage->GetTid());
		return;
	}
	switch (pArbPackage->GetStateID())
	{
	case CS_SINGLE:
		m_pCommServer->EnableHotlineListen(true);
		m_pCommServer->EnableHotlineConn(false);
		m_pCommServer->ChangeState(CS_SINGLE);
		break;
	case CS_DOWN:
		m_pCommServer->PostResetEvent("SecondaryState: receive force exit command");
		break;
	case CS_SECONDARY:
		break;
	default:
		REPORT_EVENT(LOG_CRITICAL, "InternalError","receive from guard: irregular state id[secondary]->[%d]", pArbPackage->GetStateID());
		break;
	}
}

void CCompSecondaryState::OnGuardChannelLost(CGuardSession *pSession, int ErrorCode)
{
	if (m_pCommServer->m_pHotlineSession == NULL) 
	{
		m_pCommServer->PostResetEvent("SecondaryState: guard and hotline channel lost");
	}
}
