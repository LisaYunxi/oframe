#include "public.h"
#include "SocketInit.h"
#include "TcpClient.h"
#include "TcpChannel.h"

CTcpClient::CTcpClient()
{
}

CTcpClient::~CTcpClient()
{
}

CChannel *CTcpClient::Connect(CServiceName *pName)
{
	struct sockaddr_in dest;
	char *hostname;
	fd_set writefds;
	struct timeval timeout;

	/* create socket myself */
	int id = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (id < 0)
		EMERGENCY_EXIT("Can not create socket for tcp CClient: %s",pName->GetName());

	/* set reuse and non block for this socket */
	int on = 1;
	setsockopt(id, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));

	for (;;)
	{
#ifdef UNIX
		on = 1;
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
			EMERGENCY_EXIT("Can not set FIONBIO for socket");
			closesocket(id);
			return NULL;
		}
		break;
	}

	/* create dest socket address */
	dest.sin_family = AF_INET;
	if (pName->GetPort() == 0)
		EMERGENCY_EXIT("Invalid port 0");
	dest.sin_port = htons(pName->GetPort());

	if (pName->GetHost() != NULL)
		hostname = pName->GetHost();
	else
		hostname = "127.0.0.1";  
	//	hostent *pHostent=gethostbyname(hostname);
	//	if (pHostent==NULL)
	//		EMERGENCY_EXIT("Can not find host");
	//	memcpy(&(dest.sin_addr.s_addr),pHostent->h_addr,pHostent->h_length);

	dest.sin_addr.s_addr = inet_addr(hostname);  //gyd

	/* try to connect it */
	int ret = ::connect(id, (struct sockaddr  *)&dest, sizeof(dest));

	/* has connected, then return it */
	if (ret == 0)
	{
		SetErrorMsg(NULL);
		return CreateChannel(id);
	}

	/* use select to wait for it */
	FD_ZERO(&writefds);
	FD_SET(id, &writefds);
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	ret = select(id + 1, NULL, &writefds, NULL, &timeout);

	/* timeout? then return with fail */
	if (ret == 0)
	{
		SetErrorMsg("Can not connect to CServer due to timeout");
		closesocket(id);
		return NULL;
	}

	/* recheck whether connected */
	struct sockaddr name;
	if ((ret > 0) && FD_ISSET(id, &writefds))
	{
		SOCKADDRLEN nameLen = sizeof(name);
		if (getpeername(id, &name, &nameLen) == 0)
		{
			SetErrorMsg(NULL);
			return CreateChannel(id);
		}
	}
	closesocket(id);
	SetErrorMsg("Can not connect to CServer");

	return NULL;
}

CChannel *CTcpClient::CreateChannel(int id)
{
	return new CTcpChannel(id);
}
