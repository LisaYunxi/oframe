#include "public.h"
#include "PipeEventHandler.h"
#include "SelectReactor.h"
#include "monitorIndex.h"

//产生通知的超时时间，毫秒
#define NOTIFY_TIMEOUT  1

#define FlUX_GENE   40       //流控因子
#define FlUX_CTL_INTER   15  //流控时间粒度 毫秒，表示检查流控的时间间隔。

//extern void prob(int no,int maxp);

CPipeEventHandler::CPipeEventHandler(CReactor *pReactor):CEventHandler(pReactor)
{
	//INIT_CRITICAL_VAR(m_OpCritical);
	m_pDestReactor=pReactor;
	
	m_nPreClock=m_pDestReactor->GetClock();
	m_nPerCnt=0;
	m_SendFlag=false;
	m_nWriteCnt=0;
	m_nReadCnt=0;
	
	//创建通知管道
	m_nReadPipe=0;
	m_nWritePipe=0;
	CreatePipe(m_nReadPipe,m_nWritePipe);
	
	//注册
	m_pDestReactor->RegisterIO(this);
	
	// 2013-2-5 longqy 初始化缓存区
	memset(m_buffer,0,sizeof(m_buffer));
}

CPipeEventHandler::~CPipeEventHandler()
{
	close(m_nWritePipe);
	close(m_nReadPipe);
}

void CPipeEventHandler::GetIds(int *pReadId, int *pWriteId)
{
	*pReadId = m_nReadPipe;
	*pWriteId = 0;
}

/*	从管道中读取数据。实际上只读一个字节。
//	longqy 20130910
*/
int CPipeEventHandler::HandleInput()
{
#ifdef WIN32
	int ret=::recv(m_nReadPipe,m_buffer,1,0);
#else
	int ret=::read(m_nReadPipe,m_buffer,1);
#endif

	if (ret!=1)
	{
		REPORT_EVENT(LOG_ERROR, "Pipe", "read notify error from pipe ret=%d last error=%d",ret,GET_LAST_SOCK_ERROR());
		perror("READ::");
		return 0;
	}
	m_nReadCnt++;
	
	return 0;
}

//	检查流控的时间间隔是 FlUX_CTL_INTER 毫秒。如果在时间间隔内，触发次数过多，则关闭发送开关。
//	每次流控检查，触发次数的计数都重新清零。
//	1、只有在发送开关打开的时候才发送通知 2、只有上次发送的通知已经被读取，才发送通知
//	longqy 20130910

void CPipeEventHandler::Notify()
{	
	m_pDestReactor->SetSpeciaFlag(true);
	
	//流控策略1:应对高速时情况，根据触发次数确定是否打开发送开关
	//每个流控周期的计数
	m_nPerCnt++;
	int inter=m_pDestReactor->GetClock()-m_nPreClock;
	if (inter > FlUX_CTL_INTER)
	{		
		m_nPreClock=m_pDestReactor->GetClock();	
		//如果每毫秒超过10笔，则不发送通知
		if (m_nPerCnt > inter * FlUX_GENE)
		{
			//关闭通知发送
			m_SendFlag=false;		
			m_nPerCnt=0;
			return;
		}
		else
		{
			//打开通知发送
			m_SendFlag=true;		
			m_nPerCnt=0;
		}
	}
	//如果不超过流控阀值，维持原来状态	
	if (!m_SendFlag)
	{
		return;
	}
	
	//流控策略2:应对中低速情况，如果有尚未处理的通知信号，则不再发送信号
	if (m_nWriteCnt>m_nReadCnt)
		return;
	
#ifdef WIN32
	int ret=::send(m_nWritePipe,m_buffer,1,0);
#else
	int ret=::write(m_nWritePipe,m_buffer,1);
#endif
	if (ret!=1)
	{
		REPORT_EVENT(LOG_ERROR, "Pipe", "write notify error to pipe ret=%d last error=%d",ret,GET_LAST_SOCK_ERROR());
		perror("write::");
		return;
	}
	m_nWriteCnt++;
	return ;
}

int CPipeEventHandler::CreateTmpServer(int &port)
{
#ifdef WIN32
	int m_nID=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_nID<0)
	{
		EMERGENCY_EXIT("Can not create Server socket for Pipe");
	}
	int on=0;
	setsockopt(m_nID, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	
	struct sockaddr_in me;
	memset(&me,0,sizeof(me));
	me.sin_family = AF_INET;

	int thisPrivatePort = 10000 + (getpid()%50);
	bool bindok = false;

	//不断尝试绑定端口号
	while(!bindok)
	{
		printf("Create pipe: use 127.0.0.1:%d \n",thisPrivatePort);		
		me.sin_port = htons(thisPrivatePort);
		if (::bind(m_nID, (struct sockaddr *)&me, sizeof(me)) < 0)
		{
			printf("Error, can not bind port: %d \n",thisPrivatePort);
			thisPrivatePort++;
		}
		else
		{
			bindok = true;
		}
	}
	
	port = thisPrivatePort;

	for (;;)
	{
		unsigned long on_windows=1;
		if (ioctlsocket(m_nID, FIONBIO, &on_windows) < 0)
		{
			if (GET_LAST_SOCK_ERROR() == EINTR)
				continue;
			EMERGENCY_EXIT("Can not set FIONBIO for socket");
			closesocket(m_nID);
			return -1;
		}
		break;
	}
	if (listen(m_nID,5)<0)
	{
		EMERGENCY_EXIT("Server can not listen");
	}
	return m_nID;
#else
	return 0;
#endif
}

int CPipeEventHandler::CreateTmpClient(int port, int &p1)
{
#ifdef WIN32
	struct sockaddr_in dest;	
	int ClntId=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ClntId<0)
	{
		EMERGENCY_EXIT("Can not create Client socket for Pipe");
	}
	
	int on=1;
	setsockopt(ClntId, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	for (;;)
	{	
		unsigned long on_windows=1;
		if (ioctlsocket(ClntId, FIONBIO, &on_windows) < 0)
		{
			if (GET_LAST_SOCK_ERROR() == EINTR)
				continue;
			EMERGENCY_EXIT("Can not set FIONBIO\n");
		}
		else
			break;
	}

	dest.sin_family = AF_INET;
	dest.sin_port = htons(port);
	dest.sin_addr.s_addr=inet_addr("127.0.0.1"); 
	int ret=::connect(ClntId,(struct sockaddr  *)&dest,sizeof(dest));
	if (ret==0)
	{
		//已经创建管道一端
		p1=ClntId;
	}
	return ClntId;
#else
	return 0;
#endif
}

void CPipeEventHandler::CreatePipe(int &p0, int &p1)
{
#ifndef WIN32
	//创建通知管道
	int pipefd[2];
	if (pipe(pipefd)<0)
	{
		EMERGENCY_EXIT("Can not create pipe");
	}
	p0=pipefd[0];
	p1=pipefd[1];
	m_nReadPipe=pipefd[0];
	m_nWritePipe=pipefd[1];
	
	//修改管道属性
	int on=1;
	for(;;)
	{
		if (fcntl(m_nReadPipe, F_SETFL,  O_NONBLOCK) < 0)
		//if (ioctlsocket(m_nReadPipe, FIONBIO, (char *)&on) < 0)
		{
			//if (GET_LAST_SOCK_ERROR() == -1)
			//if (GET_LAST_SOCK_ERROR() == EINTR)
			//	continue;
			EMERGENCY_EXIT("Can not set FIONBIO\n");
		}
		else
			break;
	}
	on=1;
	for(;;)
	{
		if (fcntl(m_nWritePipe, F_SETFL,  O_NONBLOCK) < 0)
		//if (ioctlsocket(m_nWritePipe, FIONBIO, (char *)&on) < 0)
		{
			//if (GET_LAST_SOCK_ERROR() == -1)
			//if (GET_LAST_SOCK_ERROR() == EINTR)
			//	continue;
			EMERGENCY_EXIT("Can not set FIONBIO\n");
		}
		else
			break;
	}
	p0=pipefd[0];
	p1=pipefd[1];
	return;
#else
	//初始化
	p0=0;
	p1=0;
	
	int s_port = 0;
	int SInt = CreateTmpServer(s_port);
	int CInt = CreateTmpClient(s_port,p1);
	
	//适当等待
	Sleep(100);

	//服务端开始尝试接收,获取服务器的socket
	int newId;
	struct sockaddr_in it;
	SOCKADDRLEN nameLen=sizeof(it);
	newId = ::accept(SInt, (struct sockaddr *)&it, &nameLen);
	if (newId < 0)
	{
		EMERGENCY_EXIT("create pipe_server failed!!");
	}
	else
		p0=newId;

	//如果客户端一开始就连接上了，就不需要获取了
	if (p1 > 0)
	{
		return;
	}


	//获取客户端的socket,检查是否创建完成
	fd_set writefds;
	FD_ZERO(&writefds);
	FD_SET(CInt,&writefds);
	struct timeval timeout;
	timeout.tv_sec=5;
	timeout.tv_usec=0;
	int ret=select(CInt+1,NULL,&writefds,NULL,&timeout);
	if (ret==0)
	{
		EMERGENCY_EXIT("create pipe_client failed!!");
	}

	struct sockaddr name;
	if ((ret>0)&&FD_ISSET(CInt,&writefds))
	{
		SOCKADDRLEN nameLen=sizeof(name);
		if (getpeername(CInt,&name,&nameLen)==0)
		{
			p1=CInt;
			return;
		}
	}

	closesocket(CInt);
	EMERGENCY_EXIT("create Pipe_client failed!!");
#endif

}