// SubscribeStub.cpp: implementation of the CSubscribeStub class.
//
// 20070905 xuzh 增加了UnSubscribe方法
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "SubscribeStub.h"
#include "NetworkFactory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSubscribeStub::CSubscribeStub(CReactor *pReactor, DWORD nNodeID, char *pszAddress)
{
	strcpy(m_szAddress, pszAddress);
	CServiceName serviceName(pszAddress);
	CChannel *pChannel = CNetworkFactory::GetInstance()->CreateChannel(&serviceName);		//只对UDP有效
	m_pSession = new CXTPPubSession(pReactor, pChannel, NULL);
	pReactor->RegisterIO(m_pSession);

}

CSubscribeStub::~CSubscribeStub()
{
	delete m_pSession;
}

const char *CSubscribeStub::GetAddress()
{
	return m_szAddress;
}

void CSubscribeStub::Subscribe(CSubscriber *pSubscriber, DWORD dwSubscriberID)
{
	m_pSession->RegisterSubscriber(pSubscriber, dwSubscriberID);
}

void CSubscribeStub::UnSubscribe(CSubscriber *pSubscriber)
{
	m_pSession->UnRegisterSubscriber(pSubscriber);
}

void CSubscribeStub::SetSubCommPhaseNo(WORD nCommPhaseNo)
{
	m_pSession->SetSubCommPhaseNo(nCommPhaseNo);
}

void CSubscribeStub::Release()
{
	delete this;
}

CFibSubscribeApi *CFibSubscribeApi::CreateSubscribeApi(CReactor *pReactor, DWORD nNodeID, char *pszAddress)
{
	return new CSubscribeStub(pReactor, nNodeID, pszAddress);
}
	
