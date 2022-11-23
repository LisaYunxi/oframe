// PublishStub.cpp: implementation of the CPublishStub class.
//
// 20070905 xuzh 增加了CancelPublish方法
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "PublishStub.h"
#include "NetworkFactory.h"

const int FIB_IMPL_HEADER_RESERVED = 200;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPublishStub::CPublishStub(CReactor *pReactor, DWORD nNodeID, char *pszAddress)
{
	m_nNodeID = nNodeID;
	strcpy(m_szAddress, pszAddress);
	
	CServiceName serviceName(pszAddress);
	CServerBase *pServer = CNetworkFactory::GetInstance()->CreateServer(&serviceName);
	
	CChannel *pChannel = pServer->Accept(0);		//只对UDP有效
	m_pSession = new CXTPPubSession(pReactor, pChannel, this);
	
	pReactor->RegisterIO(m_pSession);
	delete pServer;
}

CPublishStub::~CPublishStub()
{
	delete m_pSession;
}

const char *CPublishStub::GetAddress()
{
	return m_szAddress;
}

void CPublishStub::RegisterFlow(CReadOnlyFlow *pFlow, DWORD nSubjectNo)
{
	if (GetFlow(nSubjectNo) != NULL) 
	{
		return ;
	}
	m_mapFlow.Insert(nSubjectNo, pFlow);
}

void CPublishStub::CancelPublish(DWORD nSubjectNo)
{
	m_pSession->UnPublish(nSubjectNo);
}

CReadOnlyFlow *CPublishStub::GetFlow(DWORD nSubjectNo)
{
	CReadOnlyFlow **pFind = m_mapFlow.Find(nSubjectNo);
	if (pFind == NULL) 
	{
		return NULL;
	}
	return *pFind;
}

CXTPPackage *CPublishStub::CreateMessage()
{
	return m_pSession->CreatePackage();
}

void CPublishStub::Release()
{
	delete this;
}

CFibPublishApi *CFibPublishApi::CreatePublishApi(CReactor *pReactor, DWORD nNodeID, char *pszAddress)
{
	return new CPublishStub(pReactor, nNodeID, pszAddress);
}
