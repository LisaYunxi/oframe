#include "public.h"
#include "CompServer.h"
#include "CompInitState.h"
#include "CompSingleState.h"
#include "CompPrimaryState.h"
#include "CompPrepareState.h"
#include "CompSecondaryState.h"
#include "CachedFlow.h"

const int UM_sequence_RESET = SM_USER + 1;

CHotlineSessionFactory::CHotlineSessionFactory(CReactor *pReactor, CCompServer *pCompServer,unsigned int dwDefaultAggrID)
:CAggrSessionFactory(pReactor, 2, dwDefaultAggrID)
{
	m_pCompServer = pCompServer;
}

CSessionAggregation *CHotlineSessionFactory::CreateSessionAggregation(unsigned int dwAggregationID)
{
	CHotlineSession *pSession = new CHotlineSession(m_pReactor, dwAggregationID, m_pCompServer);
	m_pReactor->RegisterIO(pSession);
	return pSession;
}

void CHotlineSessionFactory::OnSessionAggrLogin(CSessionAggregation *pSessionAggr)
{
	CAggrSessionFactory::OnSessionAggrLogin(pSessionAggr);
	m_pCompServer->OnHotlineConnected((CHotlineSession *)pSessionAggr);
}

void CHotlineSessionFactory::OnSessionAggrDisconnected(CSessionAggregation *pSessionAggr, int nReason)
{
	m_pCompServer->OnHotlineChannelLost((CHotlineSession *)pSessionAggr, nReason);
	CAggrSessionFactory::OnSessionAggrDisconnected(pSessionAggr, nReason);
}

void CHotlineSessionFactory::OnSessionConnected(CSession *pSession)
{
	CSessionFactory::OnSessionConnected(pSession);
}


CCompServer::CCompServer(CReactor *pReactor, CCompFlowFactory *pFlowFactory,unsigned int dwTSeriesSubject, unsigned int dwSubscriberID):CAggrSessionFactory(pReactor, 2, dwSubscriberID)
{
	m_dwSubscriberID = dwSubscriberID;
	m_pArbSession = NULL;
	m_pHotlineSession = NULL;
	m_pFlowFactory = pFlowFactory;
	m_dwTSeriesSubject = dwTSeriesSubject;
	m_pCurrState = NULL;

	ChangeState(CS_INIT);

	m_pHotlineSessionFactory = new CHotlineSessionFactory(m_pReactor, this, dwSubscriberID);
	m_pHotlineSessionFactory->EnableConnecter(false);
	m_pHotlineSessionFactory->EnableListener(false);
	m_pHotlineSessionFactory->Start();

	m_dwArbHeartbeatTimeout = 0;
	m_dwHotlineHeartbeatTimeout = 0;

	m_pStatusCtrlEng = NULL;
}

CCompServer::~CCompServer()
{
	delete m_pHotlineSessionFactory;
	delete m_pCurrState;
}

void CCompServer::RegisterHotlineListen(const char *pszLocation)
{
	m_pHotlineSessionFactory->RegisterListener(pszLocation);
}

void CCompServer::RegisterHotlineLocation(const char *pszLocation)
{
	m_pHotlineSessionFactory->RegisterConnecter(pszLocation);
}

CSessionAggregation *CCompServer::CreateSessionAggregation(unsigned int dwAggregationID)
{
	CGuardSession *pSession = new CGuardSession(m_pReactor, this, dwAggregationID);
	if (m_dwArbHeartbeatTimeout != 0)
	{
		pSession->SetHeartbeatTimeout(m_dwArbHeartbeatTimeout);
	}
	return pSession;
}

void CCompServer::OnSessionAggrLogin(CSessionAggregation *pSessionAggr)
{
	CAggrSessionFactory::OnSessionAggrLogin(pSessionAggr);
	OnArbConnected((CGuardSession *)pSessionAggr);
}

void CCompServer::OnSessionAggrDisconnected(CSessionAggregation *pSessionAggr, int nReason)
{
	OnGuardChannelLost((CGuardSession *)pSessionAggr, nReason);
	CAggrSessionFactory::OnSessionAggrDisconnected(pSessionAggr, nReason);
}

void CCompServer::HandleHotlinePackage(CHotlinePackage *pPackage, CHotlineSession *pSession)
{
	m_pCurrState->OnHotlinePackageReceived(pPackage);
}

void CCompServer::HandleGuardPackage(CGuardPackage *pPackage, CGuardSession *pSession)
{
	m_pCurrState->OnGuardPackageReceived(pPackage);
}

void CCompServer::OnHotlineChannelLost(CHotlineSession *pSession, int ErrorCode)
{
	REPORT_EVENT(LOG_INFO, "Connect", "OnHotlineChannelLost,ErrorCode=[0x%08x]", ErrorCode);
	m_pCurrState->OnHotlineChannelLost(pSession, ErrorCode);
	m_pHotlineSession = NULL;
}

void CCompServer::OnGuardChannelLost(CGuardSession *pSession, int ErrorCode)
{
	REPORT_EVENT(LOG_INFO, "Connect", "OnGuardChannelLost,ErrorCode=[0x%08x]", ErrorCode);
	m_pCurrState->OnGuardChannelLost(pSession, ErrorCode);
	m_pArbSession = NULL;
}

void CCompServer::ConfirmQueued(CHotlinePackage *pPackage)
{
	CXTPPackage XTPPackage;
	XTPPackage.BufAddRef(pPackage);
	if (XTPPackage.ValidPackage() <= 0) 
	{
		REPORT_EVENT(LOG_CRITICAL, "ErrorPackage", "Invalid hotline XTP [%d]",pPackage->GetSequenceNo());
		return;
	}
	
	//xuzh 20200123 由于在备状态时设置为不可写，防止反演写流，这里需要强制写
	UF_INT8 nKey =m_pFlowFactory->GetPreSeries()->Append(pPackage->Address(), pPackage->Length());

	XTPPackage.BufRelease();

	//REPORT_EVENT(LOG_CRITICAL,"ConfirmQueued","Confirm From Secondary PrimaryKey=[%d] SecondaryKey=[%d]\n", nKey, pPackage->GetSequenceNo());
	ConfirmQueued(nKey);
	m_pHotlineSession->SendConfirmRsp(pPackage->GetSequenceNo());
}

void CCompServer::ConfirmQueued(UF_INT8 nKey)
{
	//NET_STATUS_LOG3("Confirm Queued Key=[%d] UnderCount=[%d] UpperCount=[%d]\n", nKey,
	//	m_pFlowFactory->GetPreSeries()->GetUnderCount(),
	//	m_pFlowFactory->GetPreSeries()->GetCount());
	if (nKey < m_pFlowFactory->GetPreSeries()->GetUnderCount()) 
	{
		//已经确认过了，直接返回
		return;
	}
	if (m_pFlowFactory->GetPreSeries()->SyncUnderFlow(nKey) < 0) 
	{
		REPORT_EVENT(LOG_CRITICAL, "InternalError", "Confirm queued failed [%d]", nKey);
//		EMERGENCY_EXIT("Confirm queued error\n");
	}
	else
	{
		m_pStatusCtrlEng->ConfirmQueued(nKey);
	}
}

void CCompServer::ConfirmAllQueued()
{
	for (UF_INT8 nKey = m_pFlowFactory->GetPreSeries()->GetUnderCount();nKey<m_pFlowFactory->GetPreSeries()->GetCount(); nKey++)
	{
		ConfirmQueued(nKey);
	}
}

void CCompServer::ChangeState(TCompStateID newState)
{
	TCompStateID nLastStateID = (m_pCurrState==NULL)?CS_INIT:m_pCurrState->GetStateID();
	REPORT_EVENT(LOG_CRITICAL, "CompState", "change state [%d]->[%d]", nLastStateID, newState);
	printf("CompState change state [%d]->[%d]\n", nLastStateID, newState);
	
	CCompState *pState = NULL;

	switch (newState)
	{
	case CS_INIT:
		pState = new CCompInitState(this);
		break;
	case CS_SINGLE:
		if (NULL != m_pStatusCtrlEng)
		{
			m_pStatusCtrlEng->EnterSingle();
		}
		pState = new CCompSingleState(this);		
		break;
	case CS_PRIMARY:
		if (NULL != m_pStatusCtrlEng)
		{
			m_pStatusCtrlEng->EnterMaster();
		}
		pState = new CCompPrimaryState(this);		
		break;
	case CS_PREPARE:
		pState = new CCompPrepareState(this);
		break;
	case CS_SECONDARY:
		if (NULL != m_pStatusCtrlEng)
		{
			m_pStatusCtrlEng->EnterSlaver();
		}
		pState = new CCompSecondaryState(this);
		break;
	default:
		EMERGENCY_EXIT("Irregular new state");
	}
	delete m_pCurrState;
	m_pCurrState = pState;
	ReportState();
	m_pCurrState->OnEntry(nLastStateID);
}

void CCompServer::OnTimer(int nIDEvent)
{
	if (nIDEvent == COMP_TIMER_HOTLINE_ID) 
	{
		NotifyArb(ARB_EVENT_HOTLINE_FAIL);
	}
	CSessionFactory::OnTimer(nIDEvent);
}

int CCompServer::HandleEvent(int nEventID, unsigned int dwParam, void *pParam)
{
	if (nEventID == UM_sequence_RESET)
	{
		Reset();
		return 0;
	}
	return CSessionFactory::HandleEvent(nEventID, dwParam, pParam);
}

void CCompServer::ReportState()
{
	if (m_pArbSession != NULL) 
	{
		((CGuardSession *)m_pArbSession)->SendStateReport(m_pCurrState->GetStateID());
	}
}

int CCompServer::GetCurrStateID()
{
	return m_pCurrState->GetStateID();
}

void CCompServer::HotlineAttachFlow(UF_INT8 nStartId)
{
	m_pHotlineSession->AttachFlow(m_pFlowFactory->GetPreSeries(), nStartId);
}

void CCompServer::NotifyArb(unsigned int nEvent)
{
	if (m_pArbSession != NULL)
	{
		((CGuardSession *)m_pArbSession)->SendEventNotify(nEvent);
	}
}

void CCompServer::EnableHotlineListen(bool bEnable)
{
	m_pHotlineSessionFactory->EnableListener(bEnable);
}

void CCompServer::EnableHotlineConn(bool bEnable)
{
	m_pHotlineSessionFactory->EnableConnecter(bEnable);
}

void CCompServer::OnHotlineConnected(CHotlineSession *pSession)
{
	REPORT_EVENT(LOG_INFO, "Connect", "Hotline Connected");
	m_pHotlineSession = pSession;
	if (m_dwHotlineHeartbeatTimeout != 0)
	{
		m_pHotlineSession->SetHeartbeatTimeout(m_dwHotlineHeartbeatTimeout);
	}
	if (m_pCurrState->GetStateID() == CS_PREPARE) 
	{
		m_pHotlineSession->SendLoginReq(m_pFlowFactory->GetPreSeries()->GetCount());
	}
	KillTimer(COMP_TIMER_HOTLINE_ID);
	NotifyArb(ARB_EVENT_HOTLINE_OK);
}

void CCompServer::OnArbConnected(CGuardSession *pSession)
{
	REPORT_EVENT(LOG_INFO, "Connect", "Guard Connected");
	m_pArbSession = pSession;
	ReportState();
}

void CCompServer::SetArbHeartbeatTimeout(unsigned int dwArbHeartbeatTimeout)
{
	m_dwArbHeartbeatTimeout = dwArbHeartbeatTimeout;
}

void CCompServer::SetHotlineHeartbeatTimeout(unsigned int dwHotlineHeartbeatTimeout)
{
	m_dwHotlineHeartbeatTimeout = dwHotlineHeartbeatTimeout;
}

void CCompServer::PostResetEvent(const char *pszResetReason)
{
	REPORT_EVENT(LOG_CRITICAL, "CompReset", pszResetReason);
	PostEvent(UM_sequence_RESET, 0, NULL);
}

void CCompServer::Reset()
{
	exit(-1);

/*	回到初始状态
	KillTimer(COMP_TIMER_TMSYNC_ID);
	UnPublishTradeSeries();
	UnSubscribeAll();
	m_pFlowFactory->Rollback();
	m_pHotlineSessionFactory->EnableConnecter(false);
	m_pHotlineSessionFactory->EnableListener(false);
	m_pHotlineSessionFactory->DisconnectAll(0);
	DisconnectAll(0);

	ChangeState(CS_INIT);
*/
}

CReactor *CCompServer::GetReactor()
{
	return m_pReactor;
}
void CCompServer::SetStatusCtrlEng(CStatusCtrlInterfaces *pStatusCtrlEng)
{
	m_pStatusCtrlEng = pStatusCtrlEng;
}
