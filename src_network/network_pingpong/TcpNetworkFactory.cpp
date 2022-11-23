// TcpNetworkFactory.cpp: implementation of the CTcpNetworkFactory class.
//
// 2006.10.11	hwp	将变量TCP_NETWORK_NAME移入CPP中，
//						防止多次包含引起的重复定义 
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "TcpNetworkFactory.h"
#include "TcpClient.h"
#include "TcpServer.h"

const char *TCP_NETWORK_NAME = "tcp";

CTcpNetworkFactory CTcpNetworkFactory::m_instance;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

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


