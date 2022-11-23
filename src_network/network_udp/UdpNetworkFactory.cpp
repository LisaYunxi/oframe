// UdpNetworkFactory.cpp: implementation of the CUdpNetworkFactory class.
//
// 2006.10.11	xuzh	将变量UDP_NETWORK_NAME移入CPP中，
//						防止多次包含引起的重复定义 
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "UdpNetworkFactory.h"
#include "UdpClient.h"
#include "UdpServer.h"

const char *UDP_NETWORK_NAME = "udp";

CUdpNetworkFactory CUdpNetworkFactory::m_instance;



CUdpNetworkFactory::CUdpNetworkFactory()
{
	RegisterFactory();
}

CUdpNetworkFactory::~CUdpNetworkFactory()
{

}

CServerBase *CUdpNetworkFactory::CreateServer(CServiceName *pName)
{
	if (strcmp(pName->GetChannel(), UDP_NETWORK_NAME) == 0)
	{
		return new CUdpServer(pName);
	}
	return CNetworkFactory::CreateServer(pName);
}
	
CClientBase *CUdpNetworkFactory::CreateClient(CServiceName *pName)
{
	if (strcmp(pName->GetChannel(), UDP_NETWORK_NAME) == 0)
	{
		return new CUdpClient();
	}
	return CNetworkFactory::CreateClient(pName);
}


