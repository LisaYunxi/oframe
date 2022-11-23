// ServiceSkeleton.cpp: implementation of the CServiceSkeleton class.
//
// 20080727 xuzh NT-0048:扩展CSessionFactory::CreateSession参数，便于创建
//                 会话时区分不同的连接
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "ServiceSkeleton.h"

const int FIB_IMPL_HEADER_RESERVED = 200;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServiceSkeleton::CServiceSkeleton(CReactor *pReactor, DWORD nServerID, int nMaxSession)
:CSessionFactory(pReactor, nMaxSession)
{
	m_pReactor = pReactor;
	m_nServerID = nServerID;
	m_pService = NULL;
}

CServiceSkeleton::~CServiceSkeleton()
{

}

DWORD CServiceSkeleton::GetServerID()
{
	return m_nServerID;
}

void CServiceSkeleton::RegisterListener(char *pszAddress)
{
	CSessionFactory::RegisterListener(pszAddress);
	Start();
}

CXTPPackage *CServiceSkeleton::CreateMessage()
{
	CXTPPackage *pMessage = new CXTPPackage;
	pMessage->ConstructAllocate(XTP_PACKAGE_MAX_SIZE, FIB_IMPL_HEADER_RESERVED);
	return pMessage;
}

int CServiceSkeleton::Response(CXTPPackage *pMessage, DWORD nSessionID)
{
	CServiceSession *pSession = (CServiceSession *)GetSession(nSessionID);
	if (pSession == NULL)
	{
		return -1;
	}

	pSession->SendRequestPackage(pMessage);
	return 0;
}

void CServiceSkeleton::RegisterService(CService *pService)
{
	m_pService = pService;
}

void CServiceSkeleton::Release()
{
	delete this;
}

CSession *CServiceSkeleton::CreateSession(CChannel *pChannel, DWORD)
{
	CServiceSession *pSession = new CServiceSession(m_pReactor, pChannel);
	pSession->RegisterReqHandler(this);
	return pSession;
}

int CServiceSkeleton::HandleRequest(CXTPPackage *pXTPPackage, CXTPSession *pSessin)
{
	m_pService->OnRequest(pXTPPackage, pSessin->GetSessionID());
	
	return 0;
}

void CServiceSkeleton::OnSessionConnected(CSession *pSession)
{
	CSessionFactory::OnSessionConnected(pSession);
	m_pService->OnSessionConnected(pSession->GetSessionID());
}

void CServiceSkeleton::OnSessionDisconnected(CSession *pSession, int nReason)
{
	m_pService->OnSessionDisconnected(pSession->GetSessionID());
	CSessionFactory::OnSessionDisconnected(pSession, nReason);
}

CFibServerApi *CFibServerApi::CreateServerApi(CReactor *pReactor, DWORD nServerID, int nMaxSession)
{
	return new CServiceSkeleton(pReactor, nServerID, nMaxSession);
}
