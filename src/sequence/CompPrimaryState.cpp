#include "public.h"
#include "CompPrimaryState.h"
#include "ArbPackage.h"
#include "HotlinePackage.h"

CCompPrimaryState::CCompPrimaryState(CCompServer *pCommServer) :CCompState(pCommServer)
{
}

CCompPrimaryState::~CCompPrimaryState()
{
}

TCompStateID CCompPrimaryState::GetStateID()
{
	return CS_PRIMARY;
}

void CCompPrimaryState::OnEntry(TCompStateID nLastStateID)
{
	//	m_pCommServer->SetTimer(CCompServer::COMP_TIMER_TMSYNC_ID, 
	//		CCompServer::COMP_TIMER_TMSYNC_INTERVAL);
}

void CCompPrimaryState::OnHotlinePackageReceived(CHotlinePackage *pHotlinePackage)
{
	if (pHotlinePackage->GetTid() != HTID_CONFIRM_RSP)
	{
		REPORT_EVENT(LOG_CRITICAL, "ErrorPackage", "receive from sequence: error tid [0x%08x]", pHotlinePackage->GetTid());
		return;
	}
	m_pCommServer->ConfirmQueued(pHotlinePackage->GetSequenceNo() - 1);

	//设置以产生特殊事件标志
	CSelectReactor *pSReactor = (CSelectReactor *)(m_pCommServer->GetReactor());
	pSReactor->SetSpeciaFlag(true);
}

void CCompPrimaryState::OnHotlineChannelLost(CHotlineSession *pSession, int ErrorCode)
{
	if (m_pCommServer->m_pArbSession == NULL)
	{
		m_pCommServer->PostResetEvent("PrimaryState: guard and hotline channel lost");
	}
	m_pCommServer->NotifyArb(ARB_EVENT_HOTLINE_FAIL);
	m_pCommServer->SetTimer(CCompServer::COMP_TIMER_HOTLINE_ID, CCompServer::COMP_TIMER_HOTLINE_INTERVAL);
}

void CCompPrimaryState::OnGuardPackageReceived(CGuardPackage *pArbPackage)
{
	if (pArbPackage->GetTid() != HTID_STATE_COMMAND)
	{
		REPORT_EVENT(LOG_CRITICAL, "ErrorPackage", "receive from guard: error tid [0x%08x]", pArbPackage->GetTid());
		return;
	}
	switch (pArbPackage->GetStateID())
	{
	case CS_SINGLE:
		m_pCommServer->ChangeState(CS_SINGLE);
		break;
	case CS_DOWN:
		m_pCommServer->PostResetEvent("PrimaryState: receive force exit command");
		break;
	case CS_PRIMARY:
		break;
	default:
		REPORT_EVENT(LOG_CRITICAL, "InternalError", "receive from guard: irregular state id[primary]->[%d]", pArbPackage->GetStateID());
		break;
	}
}

void CCompPrimaryState::OnGuardChannelLost(CGuardSession *pSession, int ErrorCode)
{
	if (m_pCommServer->m_pHotlineSession == NULL)
	{
		m_pCommServer->PostResetEvent("PrimaryState: guard and hotline channel lost");
	}
}
