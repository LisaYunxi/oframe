#include "public.h"
#include "TcpNetworkFactory.h"
#include "TcpClient.h"
#include "TcpServer.h"

const char *TCP_NETWORK_NAME = "tcp";

CTcpNetworkFactory CTcpNetworkFactory::m_instance;



CTcpNetworkFactory::CTcpNetworkFactory()
{
	RegisterFactory();
}

CTcpNetworkFactory::~CTcpNetworkFactory()
{

}

CServerBase *CTcpNetworkFactory::CreateServer(CServiceName *pName)
{
	if (strcmp(pName->GetChannel(), TCP_NETWORK_NAME) == 0)
	{
		return new CTcpServer(pName);
	}
	return CNetworkFactory::CreateServer(pName);
}
	
CClientBase *CTcpNetworkFactory::CreateClient(CServiceName *pName)
{
	if (strcmp(pName->GetChannel(), TCP_NETWORK_NAME) == 0)
	{
		return new CTcpClient();
	}
	return CNetworkFactory::CreateClient(pName);
}


