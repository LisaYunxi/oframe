#include "public.h"
#include "CompSingleState.h"
#include "HotlinePackage.h"
#include "ArbPackage.h"

CCompSingleState::CCompSingleState(CCompServer *pCompServer):CCompState(pCompServer)
{
}

CCompSingleState::~CCompSingleState()
{
}

TCompStateID CCompSingleState::GetStateID()
{
	return CS_SINGLE;
}

void CCompSingleState::OnEntry(TCompStateID nLastStateID)
{
	m_bPreapreFlag = false;
	m_pCommServer->KillTimer(CCompServer::COMP_TIMER_HOTLINE_ID);
	m_pCommServer->ConfirmAllQueued();
}

void CCompSingleState::OnHotlinePackageReceived(CHotlinePackage *pHotlinePackage)
{
	switch(pHotlinePackage->GetTid()) 
	{
	case HTID_LOGIN_REQ:
		m_pCommServer->HotlineAttachFlow(pHotlinePackage->GetSequenceNo());
		break;
	case HTID_CONFIRM_RSP:
		/*
		NET_IO_LOG2("GetSequenceNo=[%0.f], flowcount=[%0.f]\n",pHotlinePackage->GetSequenceNo(),m_pCommServer->m_pFlowFactory->GetPreSeries()->GetCount());
		*/
		if (pHotlinePackage->GetSequenceNo()+1 >= m_pCommServer->m_pFlowFactory->GetPreSeries()->GetCount() && !m_bPreapreFlag) 
		{
			m_pCommServer->NotifyArb(ARB_EVENT_PREPARE_OK);
			m_bPreapreFlag = true;
		}
		break;
	default:
		REPORT_EVENT(LOG_CRITICAL, "ErrorPackage", "receive error hotline package [0x%08x]", pHotlinePackage->GetTid());
		break;
	}
}

void CCompSingleState::OnHotlineChannelLost(CHotlineSession *pSession, int ErrorCode)
{
	REPORT_EVENT(LOG_INFO, "Connect", "CCompSingleState::OnHotlineChannelLost,ErrorCode=[0x%08x]", ErrorCode);
	m_bPreapreFlag = false;
	m_pCommServer->NotifyArb(ARB_EVENT_HOTLINE_FAIL);
}

void CCompSingleState::OnGuardPackageReceived(CGuardPackage *pArbPackage)
{
	if (pArbPackage->GetTid() != HTID_STATE_COMMAND) 
	{
		REPORT_EVENT(LOG_CRITICAL, "ErrorPackage","receive from guard: error tid [0x%08x]", pArbPackage->GetTid());
		return;
	}
	switch (pArbPackage->GetStateID())
	{
	case CS_PRIMARY:
		m_pCommServer->ChangeState(CS_PRIMARY);
		break;
	case CS_DOWN:
		m_pCommServer->PostResetEvent("SingleState: receive force exit command");
		break;
	case CS_SINGLE:
		break;
	default:
		REPORT_EVENT(LOG_CRITICAL,"InternalError","receive from guard: irregular state id[single]->[%d]",pArbPackage->GetStateID());
		return;
	}
}

void CCompSingleState::OnGuardChannelLost(CGuardSession *pSession, int ErrorCode)
{
	m_pCommServer->PostResetEvent("SingleState: guard channel lost");
}
