#include "public.h"
#include "PipeEventHandler.h"
#include "SelectReactor.h"
#include "monitorIndex.h"

//����֪ͨ�ĳ�ʱʱ�䣬����
#define NOTIFY_TIMEOUT  1

#define FlUX_GENE   40       //��������
#define FlUX_CTL_INTER   15  //����ʱ������ ���룬��ʾ������ص�ʱ������

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
	
	//����֪ͨ�ܵ�
	m_nReadPipe=0;
	m_nWritePipe=0;
	CreatePipe(m_nReadPipe,m_nWritePipe);
	
	//ע��
	m_pDestReactor->RegisterIO(this);
	
	// 2013-2-5 longqy ��ʼ��������
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

/*	�ӹܵ��ж�ȡ���ݡ�ʵ����ֻ��һ���ֽڡ�
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

//	������ص�ʱ������ FlUX_CTL_INTER ���롣�����ʱ�����ڣ������������࣬��رշ��Ϳ��ء�
//	ÿ�����ؼ�飬���������ļ������������㡣
//	1��ֻ���ڷ��Ϳ��ش򿪵�ʱ��ŷ���֪ͨ 2��ֻ���ϴη��͵�֪ͨ�Ѿ�����ȡ���ŷ���֪ͨ
//	longqy 20130910

void CPipeEventHandler::Notify()
{	
	m_pDestReactor->SetSpeciaFlag(true);
	
	//���ز���1:Ӧ�Ը���ʱ��������ݴ�������ȷ���Ƿ�򿪷��Ϳ���
	//ÿ���������ڵļ���
	m_nPerCnt++;
	int inter=m_pDestReactor->GetClock()-m_nPreClock;
	if (inter > FlUX_CTL_INTER)
	{		
		m_nPreClock=m_pDestReactor->GetClock();	
		//���ÿ���볬��10�ʣ��򲻷���֪ͨ
		if (m_nPerCnt > inter * FlUX_GENE)
		{
			//�ر�֪ͨ����
			m_SendFlag=false;		
			m_nPerCnt=0;
			return;
		}
		else
		{
			//��֪ͨ����
			m_SendFlag=true;		
			m_nPerCnt=0;
		}
	}
	//������������ط�ֵ��ά��ԭ��״̬	
	if (!m_SendFlag)
	{
		return;
	}
	
	//���ز���2:Ӧ���е���������������δ�����֪ͨ�źţ����ٷ����ź�
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

	//���ϳ��԰󶨶˿ں�
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
		//�Ѿ������ܵ�һ��
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
	//����֪ͨ�ܵ�
	int pipefd[2];
	if (pipe(pipefd)<0)
	{
		EMERGENCY_EXIT("Can not create pipe");
	}
	p0=pipefd[0];
	p1=pipefd[1];
	m_nReadPipe=pipefd[0];
	m_nWritePipe=pipefd[1];
	
	//�޸Ĺܵ�����
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
	//��ʼ��
	p0=0;
	p1=0;
	
	int s_port = 0;
	int SInt = CreateTmpServer(s_port);
	int CInt = CreateTmpClient(s_port,p1);
	
	//�ʵ��ȴ�
	Sleep(100);

	//����˿�ʼ���Խ���,��ȡ��������socket
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

	//����ͻ���һ��ʼ���������ˣ��Ͳ���Ҫ��ȡ��
	if (p1 > 0)
	{
		return;
	}


	//��ȡ�ͻ��˵�socket,����Ƿ񴴽����
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