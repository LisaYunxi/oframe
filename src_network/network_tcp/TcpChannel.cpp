#include "public.h"
#include "SocketInit.h"
#include "TcpChannel.h"

CTcpChannel::CTcpChannel(int id) : CChannel(CT_STREAM, id)
{
	for (;;)
	{
#ifdef UNIX
		int on = 1;
		if (ioctlsocket(id, FIONBIO, (char *)&on) < 0)
#endif

#ifdef WIN32
			unsigned long on_windows = 1;
		if (ioctlsocket(id, FIONBIO, &on_windows) < 0)
#endif


#ifdef VOS
			int off = 0;
		if (ioctlsocket(id, FIONBIO, (char *)&off) < 0)
#endif
		{
			if (GET_LAST_SOCK_ERROR() == EINTR)
				continue;
			EMERGENCY_EXIT("Can not set FIONBIO\n");
		}
		else
			break;
	}

	int on = 1;
	setsockopt(id, IPPROTO_TCP, TCP_NODELAY, (char*)&on, sizeof(on));

	m_dwInAddr = 0;

	struct sockaddr name;
	SOCKADDRLEN nameLen = sizeof(name);
	if (getpeername(GetId(), &name, &nameLen) == 0)
	{
		memcpy(&m_dwInAddr, &(((struct sockaddr_in *)&name)->sin_addr), sizeof(unsigned int));
	}

	if (getsockname(GetId(), &name, &nameLen) == 0)
	{
		memcpy(&m_dwLocalAddr, &(((struct sockaddr_in *)&name)->sin_addr), sizeof(unsigned int));
	}
}

CTcpChannel::~CTcpChannel()
{
}

int CTcpChannel::ReadImp(int number, char *buffer)
{
	int nCount = ::recv(GetId(), buffer, number, 0);
	if (nCount == 0)
		return -1;

	if (nCount == -1)
	{
		int nErrno = GET_LAST_SOCK_ERROR();
		if (nErrno == EWOULDBLOCK || nErrno == 0)		//IA64 add 0
			return 0;
		else
		{
			//EPORT_EVENT(LOG_EMERGENCY, "TcpChannel", "ReadImp[%d][%d] GET_LAST_SOCK_ERROR()=%d\n",m_dwLocalAddr,m_dwInAddr, nErrno, __LINE__, __FILE__);
			PRINT_TO_STD("TcpChannel:ReadImp[%s][%s] GET_LAST_SOCK_ERROR()=%d", GetLocalName(), GetRemoteName(), nErrno);
			return -1;
		}
	}
	return nCount;
}

int CTcpChannel::WriteImp(int number, char *buffer)
{
	int nCount = ::send(GetId(), buffer, number, 0);

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
		if (nErrno == EWOULDBLOCK || nErrno == 2 || nErrno == 0)
#elif defined(HP_UX)
		if (nErrno == EWOULDBLOCK || nErrno == 16 || nErrno == 0 || nErrno == 2 || nErrno == 251)
#else
		if (nErrno == EWOULDBLOCK)
#endif			
			return 0;
		else
		{
			//NET_EXCEPTION_LOG1("GET_LAST_SOCK_ERROR()=%d\n", nErrno);
//			REPORT_EVENT(LOG_EMERGENCY , "TcpChannel","WriteImp[%d][%d] GET_LAST_SOCK_ERROR()=%d\n", m_dwLocalAddr, m_dwInAddr,nErrno,__LINE__,__FILE__);
			PRINT_TO_STD("TcpChannel:WriteImp[%s][%s] GET_LAST_SOCK_ERROR()=%d", GetLocalName(), GetRemoteName(), nErrno);
			return -1;
		}
	}
	return nCount;
}

bool CTcpChannel::AvailableImp(void)
{
	return true;
}

bool CTcpChannel::DisconnectImp(void)
{
	closesocket(GetId());
	return false;
}

bool CTcpChannel::CheckConnectionImp(void)
{
	return true;
}

char *CTcpChannel::GetRemoteName(void)
{
	struct in_addr addr;
	memcpy(&addr, &m_dwInAddr, sizeof(unsigned int));
	return inet_ntoa(addr);
}


/** 获取本地的地址，例如IP、Mac等
* @return   本地地址
*/
char *CTcpChannel::GetLocalName(void)
{
	struct in_addr addr;
	memcpy(&addr, &m_dwLocalAddr, sizeof(unsigned int));
	return inet_ntoa(addr);
}



