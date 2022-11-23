#include "public.h"
#include "MultiChannel.h"
#include "SocketInit.h"
#include "monitorIndex.h"

CMultiChannel::CMultiChannel(int id, char *pTargetAddress, unsigned short wTargetPort)
 : CChannel(CT_DATAGRAM, id)
{
	for (;;)
	{
#ifdef UNIX
		int on=1;
		if (ioctlsocket(id, FIONBIO, (char *)&on) < 0)
#endif

#ifdef WIN32
		unsigned long on_windows=1;
		if (ioctlsocket(id, FIONBIO, &on_windows) < 0)
#endif


#ifdef VOS
		int off=0;
		if (ioctlsocket(id, FIONBIO, (char *)&off) <0)
#endif
		{
			if (GET_LAST_SOCK_ERROR() == EINTR)
				continue;
			EMERGENCY_EXIT("Can not set FIONBIO\n");
		}
		else
			break;
	}
	
  int rcvbufsize = 1*1024*1024;
	SOCKADDRLEN paramlen = sizeof(rcvbufsize);
	int ret = setsockopt(id, SOL_SOCKET, SO_RCVBUF, (const char *)&rcvbufsize, paramlen);
	if(ret != 0)
	{
		EMERGENCY_EXIT("Can not setsockopt revbuf\n");
	}
	int sndbufsize = 1*1024*1024;
	SOCKADDRLEN sndparamlen = sizeof(sndbufsize);
	int sndret = setsockopt(id, SOL_SOCKET, SO_SNDBUF, (const char *)&sndbufsize, sndparamlen);
	if(sndret != 0)
	{
		EMERGENCY_EXIT("Can not setsockopt sndbuf\n");
	}

	m_pTargetAddr = new sockaddr_in;
	memset(m_pTargetAddr, 0, sizeof(sockaddr_in));
	m_pTargetAddr->sin_family = AF_INET; //IPv4
	m_pTargetAddr->sin_port = htons(wTargetPort); 
	m_pTargetAddr->sin_addr.s_addr = inet_addr(pTargetAddress); 
	
	//memset(m_MultiCastIP,0,sizeof(m_MultiCastIP));
	m_MultiCastIP = new char[100];
	m_sHostIP = new char[100];	
	
	m_sMultiCastName = new char[100];
	m_nPort = wTargetPort;
}

CMultiChannel::~CMultiChannel()
{
	delete m_MultiCastIP;
	delete m_sHostIP;
	delete m_sMultiCastName;
}

int CMultiChannel::ReadImp(int number, char *buffer)
{
	sockaddr_in fromAddr;
	SOCKADDRLEN nFromLen = sizeof(sockaddr_in);
	int nCount = recvfrom(GetId(), buffer, number, 0, (sockaddr *)&fromAddr, &nFromLen);
//printf("CMultiChannel ReadImp count=%d\n",nCount);

	if (nCount == 0)
		return -1;

	if (nCount == -1)
	{
		int nErrno = GET_LAST_SOCK_ERROR();
		if (nErrno == EWOULDBLOCK || nErrno == 0)		//20060224 IA64 add 0
			return 0;
		else
			return -1;
	}
	return nCount;
}

int CMultiChannel::WriteImp(int number, char *buffer)
{
	int nCount = ::sendto(GetId(), buffer, number, 0,
		(sockaddr *)m_pTargetAddr, sizeof(sockaddr_in));
//printf("CMultiChannel WriteImp count=%d\n",nCount);
	if (nCount == 0)
	{
#if !defined(HP_UX) && !defined(AIX)
		return -1;
#endif			
	}

	if (nCount == -1)
	{
		int nErrno = GET_LAST_SOCK_ERROR();

#ifdef AIX
		if (nErrno == EWOULDBLOCK || nErrno == 2 || nErrno==0)
#elif defined(HP_UX)
		if (nErrno == EWOULDBLOCK || nErrno==16 || nErrno==0 || nErrno==2 || nErrno==251)
#else
	    if (nErrno == EWOULDBLOCK)
#endif			
			return 0;
		else
		{
			//NET_EXCEPTION_LOG1("GET_LAST_SOCK_ERROR()=%d\n", nErrno);
			REPORT_EVENT(LOG_EMERGENCY , "MultiChannel","GET_LAST_SOCK_ERROR()=%d\n",nErrno,__LINE__,__FILE__);
			return -1;
		}
	}
	return nCount;
}

bool CMultiChannel::AvailableImp(void)
{
	return true;
}

bool CMultiChannel::DisconnectImp(void)
{
	closesocket(GetId());
	return false;
}

bool CMultiChannel::CheckConnectionImp(void)
{
	return true;
}

void CMultiChannel::SetMultiCastIP(char *pMultiCastIP,char *pHostIP)
{
	strcpy(m_MultiCastIP,pMultiCastIP);
	strcpy(m_sHostIP,pHostIP);
	sprintf(m_sMultiCastName,"%s:%d",pMultiCastIP,m_nPort);
}

bool CMultiChannel::LeaveMultiGroup()
{
		//绑定到多播组
	struct ip_mreq mcaddr;
#ifdef WIN32
	memset(&mcaddr, 0, sizeof(mcaddr));
	mcaddr.imr_interface.S_un.S_addr = inet_addr(m_sHostIP);
	mcaddr.imr_multiaddr.S_un.S_addr = inet_addr(m_MultiCastIP);
#else
	if(inet_pton(AF_INET,m_sHostIP,&mcaddr.imr_interface) <= 0)
	{
		EMERGENCY_EXIT("set ip to imr_interface failed");
	}
	if(inet_pton(AF_INET,m_MultiCastIP,&mcaddr.imr_multiaddr) <= 0)
	{
		EMERGENCY_EXIT("rong multicast IP address");
	}
#endif
	/*
	struct in_addr iaddr;
	iaddr.s_addr = inet_addr(m_sHostIP);
	if(setsockopt(m_nID,IPPROTO_IP,IP_MULTICAST_IF,(char*)&iaddr,sizeof(iaddr)) < 0)
	{
		EMERGENCY_EXIT("setsockopt error,join IP_MULTICAST_IF failed"); 
	}
	
	printf("bind to multiaddr:%s\n",inet_ntoa(mcaddr.imr_multiaddr));
	fflush(stdout);
	*/
	if(setsockopt(m_nID,IPPROTO_IP,IP_DROP_MEMBERSHIP,(char*)&mcaddr,sizeof(struct ip_mreq)) < 0)
	{
		EMERGENCY_EXIT("setsockopt error,join multicast failed"); 
	}
	return true;
}
