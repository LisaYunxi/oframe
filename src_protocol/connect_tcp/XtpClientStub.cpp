#include "XtpClientStub.h"
#include "CachedFlow.h"
#include "DialogFlow.h"
#include "Config.h"
#include "monitorIndex.h"

//同时只有一个会话存在
CXtpClientStub::CXtpClientStub(CReactor *pReactor, CFlow *pSerailFlow, int nSubjectNo, UF_INT8 nSequenceNo) :CSessionFactory(pReactor, 1)
{
	m_XtpPackage.ConstructAllocate(PACKAGE_MAX_SIZE, PACKAGE_RESERVE_SIZE);
	m_pClientCallBack = NULL;
	m_bConnected = false;
	//流水不能重复使用，注意FileFlow的话，不能读取
	m_pSerialFlow = pSerailFlow;
	//ToDo
	//m_pSerialFlow->Truncate(0);
	m_nSubjectNo = nSubjectNo;
	m_nSequenceNo = nSequenceNo;
	m_nSubCount = -1;
	m_pDialogReqFlow = NULL;

	m_pPipeEventHandler = new CPipeEventHandler(pReactor);
	CreateDialogFlow();
}
CXtpClientStub::~CXtpClientStub()
{
	delete m_pPipeEventHandler;
}
void CXtpClientStub::RegisterReportServer(const char *pszServerAddress)
{
	RegisterConnecter(pszServerAddress);
}

void CXtpClientStub::Init()
{
	m_pReactor->Create();
	Start();
}

//将主动断开服务器连接，如果要恢复连接需要执行StartConnect()
void CXtpClientStub::StopConnect()
{
	PostEvent(USER_CHANNEL_DISCONNECTED_ALL, 0, NULL);
}

//开始连接服务器，执行init()后，该方法自动被执行
void CXtpClientStub::StartConnect()
{
	EnableConnecter(true);
}

void CXtpClientStub::RegClientCallback(CXtpClientCallback *pClientCallback)
{
	m_pClientCallBack = pClientCallback;
}

CSession *CXtpClientStub::CreateSession(CChannel *pChannel, unsigned int dwMark)
{
	CTcpXtpSession *pSession = new CTcpXtpSession(m_pReactor, pChannel, this);
	pSession->PublishFlow(m_pDialogReqFlow, 0);
	return pSession;
}

void CXtpClientStub::OnSessionConnected(CSession *pSession)
{
	m_bConnected = true;
	InterLogin((CTcpXtpSession *)pSession);
	CSessionFactory::OnSessionConnected(pSession);
}

void CXtpClientStub::InterLogin(CTcpXtpSession *pSession)
{
	m_XtpPackage.PrepareRequest(1, '1');
	m_XtpPackage.GetXTPHeader()->Type = XTP_Type_LOGIN;
	m_XtpPackage.GetXTPHeader()->SubjectNo = m_nSubjectNo;

	m_XtpPackage.GetXTPHeader()->SequenceNo = m_nSequenceNo;

	REPORT_EVENT(LOG_CRITICAL, "CXtpClientStub", "Req SequenceNo[%0.f]", m_XtpPackage.GetXTPHeader()->SequenceNo);
	REPORT_EVENT(LOG_CRITICAL, "CXtpClientStub", "Local m_nSubCount=%0.f", m_nSubCount);
	m_nSubCount = -1;

	pSession->SendReportPackage(&m_XtpPackage);
}

void CXtpClientStub::RemoveDialogFlow()
{
	if (NULL != m_pDialogReqFlow)
	{
		delete m_pDialogReqFlow;
	}
	m_pDialogReqFlow = NULL;
}

void CXtpClientStub::CreateDialogFlow()
{
	RemoveDialogFlow();
	m_pDialogReqFlow = new CFtdcUserDialogFlow(false, 10000, 10000);
	((CFtdcUserDialogFlow *)m_pDialogReqFlow)->AttachObserver(m_pPipeEventHandler);
}

int CXtpClientStub::RequestToDialogFlow(CXTPPackage *pXTPPackage)
{
	if (m_pDialogReqFlow == NULL)
	{
		return -1;
	}

	//	pXTPPackage->MakePackage();
	UF_INT8 rtn = m_pDialogReqFlow->Append(pXTPPackage->Address(), pXTPPackage->Length());
	if (rtn < 0)
	{
		return -1;
	}
	return 0;
}

void CXtpClientStub::OnSessionDisconnected(CSession *pSession, int nReason)
{
	//设置为不可发送
	m_bConnected = false;
	CSessionFactory::OnSessionDisconnected(pSession, nReason);
	RemoveDialogFlow();
}

int CXtpClientStub::SendReportPackage(CXTPPackage *pXTPPackage)
{
	if (!m_bConnected)
	{
		return -1;
	}
	return RequestToDialogFlow(pXTPPackage);
}

int CXtpClientStub::HandlePackage(CXTPPackage *pXTPPackage, CTcpXtpSession *pSession)
{
	//如果非连续报文，重新连接
	if (m_nSubCount == -1)
	{
		if (pXTPPackage->GetXTPHeader()->SequenceNo != -1)
			REPORT_EVENT(LOG_CRITICAL, "CXtpClientStub", "Receive First SequenceNo[%0.f],GlobalNo[%0.f]!", pXTPPackage->GetXTPHeader()->SequenceNo, pXTPPackage->GetXTPHeader()->GlobalNo);
	}
	else
	{
		if (pXTPPackage->GetXTPHeader()->SequenceNo != m_nSubCount + 1)
		{
			REPORT_EVENT(LOG_CRITICAL, "CXtpClientStub", "SequenceNo[%0.f] != %0.f+1,ReConnect!", pXTPPackage->GetXTPHeader()->SequenceNo, m_nSubCount);
			pSession->Disconnect(0);
			return -1;
		}
	}

	m_nSubCount = pXTPPackage->GetXTPHeader()->SequenceNo;

	if (m_pClientCallBack != NULL)
	{
		m_pClientCallBack->HandlePackage(pXTPPackage);
	}
	pXTPPackage->MakePackage();
	m_pSerialFlow->Append(pXTPPackage->Address(), pXTPPackage->Length());

	return 0;
}

int CXtpClientStub::HandleEvent(int nEventID, unsigned int dwParam, void *pParam)
{
	if (nEventID == USER_CHANNEL_DISCONNECTED_ALL)
	{
		CSessionFactory::DisconnectAll(100);
		CSessionFactory::EnableConnecter(false);
	}
	else
		CSessionFactory::HandleEvent(nEventID, dwParam, pParam);
	return 0;
}

CXtpClient* CXtpClient::CreateClientApi(CReactor *pReactor, CFlow *pSerailFlow, int nSubjectNo, UF_INT8 nSequenceNo)
{
	CReactor * pThisReactor = pReactor;
	if (pThisReactor == NULL)
		pThisReactor = new CSelectReactor();

	return new CXtpClientStub(pThisReactor, pSerailFlow, nSubjectNo, nSequenceNo);
}