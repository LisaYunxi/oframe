#include "public.h"
#include "XtpServerStub.h"
#include "Config.h"
#include "monitorIndex.h"
#include "OutFogServiceApi.h"
#include "LinkManager.h"

CXtpServerStub::CXtpServerStub(CReactor *pReactor, CFlow *pSubFlow, int nThreads) :CSessionFactory(pReactor, 500)
{
	m_pReactor = pReactor;
	m_nSubjectNo = 0;
	m_pSubFlow = pSubFlow;
	m_pServerCallBack = NULL;
	//for (int i = 0; i < nThreads; i++)
	//{
	//	m_Reactors.push_back(new CSelectReactor());
	//	m_Reactors[i]->Create();
	//}
	m_nSessionCount = 0;
}

CXtpServerStub::~CXtpServerStub()
{

}

void CXtpServerStub::Init(const char *tmpAddress)
{
	this->RegisterListener(tmpAddress);
	m_pReactor->Create();
	Start();
}

void CXtpServerStub::EnableListener(bool bEnable)
{
	CSessionFactory::EnableListener(bEnable);
}

void CXtpServerStub::SetSubjectNo(unsigned int nSubjectNo)
{
	m_nSubjectNo = nSubjectNo;
}

CSession *CXtpServerStub::CreateSession(CChannel *pChannel, unsigned int bIsListener)
{
	//由于这里是全部流水全部返回，导致流水的数据量太大，所以需要一个连接使用一个线程
	//CTcpXtpSession *pSession = new CTcpXtpSession(m_Reactors[m_nSessionCount], pChannel, this, m_nSubjectNo);
	//m_nSessionCount++;
	//if (m_nSessionCount > m_Reactors.size() - 1)
	//	m_nSessionCount = 0;
	CTcpXtpSession *pSession = new CTcpXtpSession(m_pReactor, pChannel, this, m_nSubjectNo);
	return pSession;
}

void CXtpServerStub::OnSessionConnected(CSession *pSession)
{
	if (m_pServerCallBack != NULL)
		m_pServerCallBack->OnSessionConnected(pSession->GetSessionID());
	//先调用基类的方法
	CSessionFactory::OnSessionConnected(pSession);
}

void CXtpServerStub::OnSessionDisconnected(CSession *pSession, int nReason)
{
	if (m_pServerCallBack != NULL)
		m_pServerCallBack->OnSessionDisconnected(pSession->GetSessionID(), nReason);
	//最后调用基类的方法
	CSessionFactory::OnSessionDisconnected(pSession, nReason);
}

int CXtpServerStub::HandlePackage(CXTPPackage *pPackage, CTcpXtpSession *pSession)
{
	switch (pPackage->GetXTPHeader()->Type)
	{
	case XTP_Type_LOGIN:
	{
		OnReqClientLogin(pPackage, pSession);
		break;
	}
	default://默认情况下，放入流文件即可，有后续engine处理
	{
		if (m_pServerCallBack != NULL)
			m_pServerCallBack->HandlePackage(pPackage);

		if (m_pSubFlow != NULL)
		{
			pPackage->MakePackage();
			m_pSubFlow->Append(pPackage->Address(), pPackage->Length());
		}
	}
	break;
	}
	return 0;
}

int CXtpServerStub::OnReqClientLogin(CXTPPackage *pPackage, CTcpXtpSession *pSession)
{
	unsigned int SubjectNo = pPackage->GetXTPHeader()->SubjectNo;
	UF_INT8 SequenceNo = pPackage->GetXTPHeader()->SequenceNo;
	REPORT_EVENT(LOG_CRITICAL, "CXtpServer", "Recv Login: SubjectNo[%d],SequenceNo[%0.f]", SubjectNo, SequenceNo);

	CReadFlow *pPubFlow = GetFlow(SubjectNo);

	if (pPubFlow == NULL)
	{
		REPORT_EVENT(LOG_EMERGENCY, "CXtpServer", "Cannot Found Flow of SubjectNo[%d]", SubjectNo);
		return 0;
	}

	//发送session信息
	if (SequenceNo == -1)
	{
		char SendBuffer[256];
		g_SessionMapRWLock.ReadLock();
		CHttpSessionMap::iterator iter = g_SessionMap.begin();
		while (iter != g_SessionMap.end())
		{
			CXTPPackage m_XtpPackage;
			m_XtpPackage.ConstructAllocate(PACKAGE_MAX_SIZE, PACKAGE_RESERVE_SIZE);
			m_XtpPackage.PreparePublish(0, OUTFOG_CHAIN_SINGLE);
			m_XtpPackage.GetXTPHeader()->Direction = OUTFOG_DIRECTION_TO_FRONT;
			m_XtpPackage.GetXTPHeader()->SequenceNo = -1;
			m_XtpPackage.GetXTPHeader()->Type = OUTFOG_TYPE_ADDAPIKEY;
			m_XtpPackage.GetXTPHeader()->SubjectNo = SubjectNo;
			strcpy(m_XtpPackage.GetXTPHeader()->Remark, iter->second.AccessKey.c_str());
			sprintf(SendBuffer, "%s,%s", iter->second.UserID.c_str(), iter->second.SecretKey.c_str());
			m_XtpPackage.GetXTPHeader()->SessionNo = iter->second.MaxSpeed;
			m_XtpPackage.GetXTPHeader()->BusinessNo = iter->second.ExpireTime;
			m_XtpPackage.AddStream(0, SendBuffer, 256);
			pSession->SendReportPackage(&m_XtpPackage);

			set<string>::iterator iterIP = iter->second.IPAddress.begin();
			while (iterIP != iter->second.IPAddress.end())
			{
				CXTPPackage m_XtpPackage;
				m_XtpPackage.ConstructAllocate(PACKAGE_MAX_SIZE, PACKAGE_RESERVE_SIZE);
				m_XtpPackage.PreparePublish(0, OUTFOG_CHAIN_SINGLE);
				m_XtpPackage.GetXTPHeader()->Type = OUTFOG_TYPE_ADDAPIKEYALLOWIP;
				m_XtpPackage.GetXTPHeader()->Direction = OUTFOG_DIRECTION_TO_FRONT;
				m_XtpPackage.GetXTPHeader()->SubjectNo = SubjectNo;
				m_XtpPackage.GetXTPHeader()->SequenceNo = -1;
				strcpy(m_XtpPackage.GetXTPHeader()->Remark, iter->second.AccessKey.c_str());
				strcpy(SendBuffer, (*iterIP).c_str());
				m_XtpPackage.AddStream(0, SendBuffer, 256);
				pSession->SendReportPackage(&m_XtpPackage);
				iterIP++;
			}
			iter++;
		}
		g_SessionMapRWLock.UnLock();
	}

	UF_INT8 nBeginNo = SequenceNo;
	//如果是默认模式，则需要判断当前server流中的数量与client中收到流的数量之间的关系
	if (SequenceNo < 0)
		nBeginNo = pPubFlow->GetCount() + SequenceNo;

	if (nBeginNo > pPubFlow->GetCount())
		nBeginNo = pPubFlow->GetCount();

	if (nBeginNo < pPubFlow->GetFirstID())
		nBeginNo = pPubFlow->GetFirstID();

	pSession->PublishFlow(pPubFlow, nBeginNo);

	REPORT_EVENT(LOG_CRITICAL, "CXtpServer", "Send TCP Login Rsponse: SubjectNo[%d],SequenceNo[%0.f]", SubjectNo, nBeginNo);
	return 0;
}

void CXtpServerStub::setXtpServerCallback(CXtpServerCallback *XtpServerCallback)
{
	m_pServerCallBack = XtpServerCallback;
}

void CXtpServerStub::RegisterFlow(CReadFlow *pFlow, unsigned int nSubjectNo)
{
	if (GetFlow(nSubjectNo) != NULL)
		return;
	m_mapFlow.Insert(nSubjectNo, pFlow);
}

CReadFlow* CXtpServerStub::GetFlow(unsigned int nSubjectNo)
{
	CReadFlow **pFind = m_mapFlow.Find(nSubjectNo);
	if (pFind == NULL)
		return NULL;
	return *pFind;
}

CXtpServer* CXtpServer::CreateServerApi(CReactor *pReactor, CFlow *pSubFlow, int nThreads)
{
	CReactor * pThisReactor = pReactor;
	if (pThisReactor == NULL)
		pThisReactor = new CSelectReactor();
	return new CXtpServerStub(pThisReactor, pSubFlow, nThreads);
}