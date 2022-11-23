
#include "public.h"
#include "MultiNetworkFactory.h"
#include "MultiClient.h"
#include "MultiServer.h"

const char *Multi_NETWORK_NAME = "multi";

CMultiNetworkFactory CMultiNetworkFactory::m_instance;



CMultiNetworkFactory::CMultiNetworkFactory()
{
	RegisterFactory();
}

CMultiNetworkFactory::~CMultiNetworkFactory()
{

}

CServerBase *CMultiNetworkFactory::CreateServer(CServiceName *pName)
{
	if (strcmp(pName->GetChannel(), Multi_NETWORK_NAME) == 0)
	{
		return new CMultiServer(pName);
	}
	return CNetworkFactory::CreateServer(pName);
}
	
CClientBase *CMultiNetworkFactory::CreateClient(CServiceName *pName)
{
	if (strcmp(pName->GetChannel(), Multi_NETWORK_NAME) == 0)
	{
		return new CMultiClient();
	}
	return CNetworkFactory::CreateClient(pName);
}