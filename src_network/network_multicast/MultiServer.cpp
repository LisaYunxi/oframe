// MultiServer.cpp: implementation of the CMultiServer class.
//
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "SocketInit.h"
#include "MultiServer.h"
#include "MultiChannel.h"

#ifndef WIN32
#include <stdio.h> 
#include <sys/types.h> 
#include <sys/param.h> 
#include <sys/ioctl.h> 
#include <sys/socket.h> 
#include <net/if.h> 
#include <netinet/in.h> 
#include <net/if_arp.h> 
#ifdef SOLARIS 
#include <sys/sockio.h> 
#endif 
#define MAXINTERFACES   16 
#endif


CMultiServer::CMultiServer(CServiceName *pName)
:CServerBase(pName), m_ServiceName(pName->GetLocation())
{
	m_bConnected = false;
	
	m_nID = socket(AF_INET, SOCK_DGRAM, 0); //建立Multi套接字

	/* set reuse and non block for this socket */
	int on=1;
	setsockopt(m_nID, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	//设置广播属性, add by wang.xiping 20071008
	//setsockopt(m_nID, SOL_SOCKET, SO_BROADCAST, (char *)&on, sizeof(on));
#ifdef HP_UX
	setsockopt(m_nID, SOL_SOCKET, SO_REUSEPORT, (char *)&on, sizeof(on));
#endif
	
	//设置TTL个数
	char routenum = 10;
	if(setsockopt(m_nID,IPPROTO_IP,IP_MULTICAST_TTL,&routenum,sizeof(routenum)) < 0)
	{
		EMERGENCY_EXIT("set TTL failed");
	}
	struct sockaddr_in servaddr; //IPv4套接口地址定义
	memset(&servaddr, 0, sizeof(servaddr)); //地址结构清零
	servaddr.sin_family = AF_INET; //IPv4协议
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//内核指定地址
	servaddr.sin_port = htons(m_ServiceName.GetPort()); //端口
	//分配协议地址,绑定端口
	if (::bind(m_nID, (sockaddr *) &servaddr, sizeof(servaddr)) != 0)
	{
		EMERGENCY_EXIT("bind fail");
	}
	
	//加入多播主
	struct ip_mreq mcaddr;
	//从IP&SerMultiCastIP&CliMultiCastIP中拆分出各个值
	char *pLocation=new char[strlen(pName->GetHost())+1];
	strcpy(pLocation, pName->GetHost());
		printf("pLocation %s\n",pLocation);
	fflush(stdout);
	char *m_pNetworkIP = pLocation;
	char *p=strchr(pLocation,'@');
	*p='\0';	
	m_pMultiCastIP = ++p;//加入的多播组
	//获取该网段对应的本机ip地址
	memset(m_sHostIP,0,sizeof(m_sHostIP));
	if(NULL == GetMatchIP(m_pNetworkIP,m_sHostIP))
	{
		EMERGENCY_EXIT("get hostip failed");
	}
	//绑定到多播组
#ifdef WIN32
	memset(&mcaddr, 0, sizeof(mcaddr));
	mcaddr.imr_interface.S_un.S_addr = inet_addr(m_sHostIP);
	mcaddr.imr_multiaddr.S_un.S_addr = inet_addr(m_pMultiCastIP);
#else
	if(inet_pton(AF_INET,m_sHostIP,&mcaddr.imr_interface) <= 0)
	{
		EMERGENCY_EXIT("set ip to imr_interface failed");
	}
	if(inet_pton(AF_INET,m_pMultiCastIP,&mcaddr.imr_multiaddr) <= 0)
	{
		EMERGENCY_EXIT("rong multicast IP address");
	}
#endif
	//指定多播从哪个网卡发送
	struct in_addr iaddr;
	iaddr.s_addr = inet_addr(m_sHostIP);
	if(setsockopt(m_nID,IPPROTO_IP,IP_MULTICAST_IF,(char*)&iaddr,sizeof(iaddr)) < 0)
	{
		EMERGENCY_EXIT("setsockopt error,join IP_MULTICAST_IF failed"); 
	}
	
	printf("bind to multiaddr:%s\n",inet_ntoa(mcaddr.imr_multiaddr));
	fflush(stdout);
	if(setsockopt(m_nID,IPPROTO_IP,IP_ADD_MEMBERSHIP,(char*)&mcaddr,sizeof(struct ip_mreq)) < 0)
	{
		EMERGENCY_EXIT("setsockopt error,join multicast failed"); 
	}
}

CMultiServer::~CMultiServer()
{

}

CChannel *CMultiServer::Accept(int wait)
{
	if (m_bConnected)
	{
		return NULL;
	}
	m_bConnected = true;
	CMultiChannel *pMultiChannel=new CMultiChannel(m_nID, m_pMultiCastIP, m_ServiceName.GetPort()+1);
	pMultiChannel->SetMultiCastIP(m_pMultiCastIP,m_sHostIP);
	return pMultiChannel;
}

int CMultiServer::GetId(void)
{
	if (!m_bConnected)
		return -1;
	else
		return 0;
}

#ifdef WIN32
char *CMultiServer::GetMatchIP(char *pszMask,char *OutIP)
{
	struct hostent *hp = NULL;
	char szName[255] = {0};
	char *pLocalIP = NULL;
	if(gethostname(szName,sizeof(szName)) == -1)
	{
		EMERGENCY_EXIT("gethostname error");
	}
	hp = gethostbyname(szName);
	if(NULL == hp)
	{
		EMERGENCY_EXIT("gethostbyname error");
	}
	char **pptr = hp->h_addr_list;
	for(;*pptr != NULL;pptr++)
	{
		pLocalIP = inet_ntoa(*(struct in_addr *)*pptr);
		if(0 == strncmp(pLocalIP,pszMask,strlen(pszMask)))
		{			
			strcpy(OutIP,pLocalIP);
			return pLocalIP;			
		}
	}
	return NULL;
}
#else
char *CMultiServer::GetMatchIP(char *pszMask,char *OutIP)
{
	char *pLocalIP = NULL;	
	register int fd, intrface; 
	struct ifreq buf[MAXINTERFACES]; 
	struct ifconf ifc; 
	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0) 
	{ 
		ifc.ifc_len = sizeof buf; 
		ifc.ifc_buf = (caddr_t) buf; 
		if (!ioctl (fd, SIOCGIFCONF, (char *) &ifc)) 
		{ 
		 //获取接口信息
		 intrface = ifc.ifc_len / sizeof (struct ifreq); 
		 printf("interface num is intrface=%d\n\n\n",intrface); 
		 //根据接口信息循环获取设备IP
		 while (intrface-- > 0) 
		 { 
				//获取设备名称
				printf ("net device %s\n", buf[intrface].ifr_name); 	
				//获取当前网卡的IP地址 
				if (!(ioctl (fd, SIOCGIFADDR,(char *) &buf[intrface]))) 
				{				 		
				 		pLocalIP = inet_ntoa(((struct sockaddr_in*)&(buf[intrface].ifr_addr))->sin_addr);
				 		printf("IP address is: %s\n",pLocalIP); 
						if(0 == strncmp(pLocalIP,pszMask,strlen(pszMask)))
						{			
							strcpy(OutIP,pLocalIP);
							close (fd); 
							return pLocalIP;			
						}
				}	
			} //while
		 } //if (!ioctl (fd, SIOCGIFCONF, (char *) &ifc)) 	
	} 
  close (fd); 
  return NULL;
}
#endif