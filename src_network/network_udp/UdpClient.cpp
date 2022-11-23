// UdpClient.cpp: implementation of the CUdpClient class.
//
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "UdpClient.h"
#include "SocketInit.h"
#include "UdpChannel.h"



CUdpClient::CUdpClient()
{

}

CUdpClient::~CUdpClient()
{

}

CChannel *CUdpClient::Connect(CServiceName *pName)
{
	struct sockaddr_in servaddr; //IPv4套接口地址定义
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0); //建立UDP套接字
	/* set reuse and non block for this socket */
	int on=1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));

	setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (char *)&on, sizeof(on));
#ifdef HP_UX
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (char *)&on, sizeof(on));
#endif

	memset(&servaddr, 0, sizeof(servaddr)); //地址结构清零
	servaddr.sin_family = AF_INET; //IPv4协议
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//内核指定地址
	servaddr.sin_port = htons(pName->GetPort()+1); //端口

	//分配协议地址,绑定端口
	if (::bind(sockfd, (sockaddr *) &servaddr, sizeof(servaddr)) != 0)
	{
		EMERGENCY_EXIT("bind fail\n");
	}

	CChannel *pChannel = new CUdpChannel(sockfd, pName->GetHost(), pName->GetPort());

	return pChannel;
}
