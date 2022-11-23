// ShmNetworkFactory.cpp: implementation of the CShmNetworkFactory class.
//
// 
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "ShmNetworkFactory.h"
#include "ShmClient.h"
#include "ShmServer.h"

const char *Shm_NETWORK_NAME = "shm";

CShmNetworkFactory CShmNetworkFactory::m_instance;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShmNetworkFactory::CShmNetworkFactory()
{
	RegisterFactory();
}

CShmNetworkFactory::~CShmNetworkFactory()
{

}

CServerBase *CShmNetworkFactory::CreateServer(CServiceName *pName)
{
	if (strcmp(pName->GetChannel(), Shm_NETWORK_NAME) == 0)
	{
		return new CShmServer(pName);
	}
	return CNetworkFactory::CreateServer(pName);
}
	
CClientBase *CShmNetworkFactory::CreateClient(CServiceName *pName)
{
	if (strcmp(pName->GetChannel(), Shm_NETWORK_NAME) == 0)
	{
		return new CShmClient();
	}
	return CNetworkFactory::CreateClient(pName);
}


