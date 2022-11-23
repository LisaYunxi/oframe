#include "public.h"
#include "NetworkFactory.h"
#include "monitorIndex.h"

CNetworkFactory *CNetworkFactory::m_pInstance = NULL;

CNetworkFactory g_NetworkFactory;

CNetworkFactory::CNetworkFactory()
{
	m_pNext = NULL;
}

CNetworkFactory::~CNetworkFactory()
{
}

CNetworkFactory *CNetworkFactory::GetInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = &g_NetworkFactory;
	}
	return m_pInstance;
}

void CNetworkFactory::RegisterFactory()
{
	m_pNext = GetInstance();
	m_pInstance = this;
}

CServerBase *CNetworkFactory::CreateServer(CServiceName *pName)
{
	if (m_pNext != NULL)
	{
		return m_pNext->CreateServer(pName);
	}
	EMERGENCY_EXIT("Unknown channel in CServiceName");
	return NULL;
}

CChannel *CNetworkFactory::CreateChannel(CServiceName *pName)
{
	CClientBase *pClient = CreateClient(pName);
	if (pClient == NULL)
	{
		return NULL;
	}
	CChannel *pChannel = pClient->Connect(pName);
	delete pClient;
	return pChannel;
}

CClientBase *CNetworkFactory::CreateClient(CServiceName *pName)
{
	PRINT_TO_STD("CNetworkFactory", "CreateClient:%s", pName->GetLocation());
	if (m_pNext != NULL)
	{
		return m_pNext->CreateClient(pName);
	}
	EMERGENCY_EXIT("Unknown channel in CServiceName");
	return NULL;
}
