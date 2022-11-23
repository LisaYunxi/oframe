#include "public.h"
#include "CompPrepareState.h"
#include "ArbPackage.h"
#include "HotlinePackage.h"

CCompPrepareState::CCompPrepareState(CCompServer *pCommServer):CCompState(pCommServer)
{
}

CCompPrepareState::~CCompPrepareState()
{
}

TCompStateID CCompPrepareState::GetStateID()
{
	return CS_PREPARE;
}
	
void CCompPrepareState::OnEntry(TCompStateID nLastStateID)
{
}

void CCompPrepareState::OnHotlinePackageReceived(CHotlinePackage *pHotlinePackage)
{
	switch(pHotlinePackage->GetTid())
	{
	case HTID_CONFIRM_REQ:
		{
			m_pCommServer->ConfirmQueued(pHotlinePackage);
			break;
		}
	case HTID_LOGIN_ERROR:
		{
			//没有登录成功，应该是流水号码有误，需要重新启动
			EMERGENCY_EXIT("Can't login prease check snap and flow!");
			break;
		}
	default:
		{

		}
	}
}

void CCompPrepareState::OnHotlineChannelLost(CHotlineSession *pSession, int ErrorCode)
{
	m_pCommServer->PostResetEvent("PrepareState: hotline channel lost");
}

void CCompPrepareState::OnGuardPackageReceived(CGuardPackage *pArbPackage)
{
	if (pArbPackage->GetTid() != HTID_STATE_COMMAND) 
	{
		REPORT_EVENT(LOG_CRITICAL, "ErrorPackage", "receive from guard: error tid [0x%08x]", pArbPackage->GetTid());
		return;
	}
	switch (pArbPackage->GetStateID())
	{
	case CS_SECONDARY:
		m_pCommServer->EnableHotlineConn(true);
		m_pCommServer->EnableHotlineListen(false);
//		m_pCommServer->PublishTradeSeries();
//		m_pCommServer->StartTerminal();
		m_pCommServer->ChangeState(CS_SECONDARY);
		break;
	case CS_DOWN:
		m_pCommServer->PostResetEvent("PrepareState: receive force exit command");
		break;
	case CS_PREPARE:
		break;
	default:
		REPORT_EVENT(LOG_CRITICAL, "InternalError", "receive from guard: irregular state id[prepare]->[%d]", pArbPackage->GetStateID());
		return;
	}
}

void CCompPrepareState::OnGuardChannelLost(CGuardSession *pSession, int ErrorCode)
{
	m_pCommServer->PostResetEvent("PrepareState: guard channel lost");
}
