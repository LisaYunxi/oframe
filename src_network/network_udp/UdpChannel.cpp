// UdpChannel.cpp: implementation of the CUdpChannel class.
//
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "UdpChannel.h"
#include "SocketInit.h"
#include "monitorIndex.h"



CUdpChannel::CUdpChannel(int id, char *pTargetAddress, unsigned short wTargetPort)
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

	m_pTargetAddr = new sockaddr_in;
	memset(m_pTargetAddr, 0, sizeof(sockaddr_in));
	m_pTargetAddr->sin_family = AF_INET; //IPv4
	m_pTargetAddr->sin_port = htons(wTargetPort); 
	m_pTargetAddr->sin_addr.s_addr = inet_addr(pTargetAddress); 
}

CUdpChannel::~CUdpChannel()
{

}

int CUdpChannel::ReadImp(int number, char *buffer)
{
	sockaddr_in fromAddr;
	SOCKADDRLEN nFromLen = sizeof(sockaddr_in);
	int nCount = recvfrom(GetId(), buffer, number, 0, (sockaddr *)&fromAddr, &nFromLen);

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

int CUdpChannel::WriteImp(int number, char *buffer)
{
	int nCount = ::sendto(GetId(), buffer, number, 0,
		(sockaddr *)m_pTargetAddr, sizeof(sockaddr_in));
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
			REPORT_EVENT(LOG_EMERGENCY , "UdpChannel","GET_LAST_SOCK_ERROR()=%d\n",nErrno,__LINE__,__FILE__);
			return -1;
		}
	}
	return nCount;
}

bool CUdpChannel::AvailableImp(void)
{
	return true;
}

bool CUdpChannel::DisconnectImp(void)
{
	closesocket(GetId());
	return false;
}

bool CUdpChannel::CheckConnectionImp(void)
{
	return true;
}
