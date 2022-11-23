// ShmClient.cpp: implementation of the CShmClient class.
//
// 
//////////////////////////////////////////////////////////////////////

#include "public.h"
#include "SocketInit.h"
#include "ShmClient.h"
#include "ShmChannel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CShmClient::CShmClient()
{
}

CShmClient::~CShmClient()
{
}

CChannel *CShmClient::Connect(CServiceName *pName)
{
	key_t shmKeyid = atoi(pName->GetHost());
	int iCount = pName->GetPort();	
	//创建共享内存
	m_iShmid = shmget((key_t)shmKeyid, iCount*1024*1024*2UL, 0666|IPC_CREAT);
	if(m_iShmid == -1)
	{
		fprintf(stderr, "shmget failed\n");
		RAISE_RUNTIME_ERROR("shmget failed");
	}
	//将共享内存连接到当前进程的地址空间
	m_pShm = shmat(m_iShmid, (void*)0, 0);
	if(m_pShm == (void*)-1)
	{
		fprintf(stderr, "shmat failed\n");
		RAISE_RUNTIME_ERROR("shmat failed");
	}
	printf("Memory attached at %X\n", (long)m_pShm);	
	
	/*创建共享内存和信号量的IPC*/
	m_iSemid=semget(shmKeyid+1,1,0666|IPC_CREAT);
	if(m_iSemid==-1)
	{
			fprintf(stderr, "semget failed\n");
			RAISE_RUNTIME_ERROR("semget failed");
	}
	/*设置信号量的初始值，就是资源个数*/
	union semun{
		int val;
		struct semid_ds *buf;
		ushort *array;
	}sem_u;
	
	sem_u.val=1;
	semctl(m_iSemid,0,SETVAL,sem_u);
	
	//"1"表示请求 ，“2”表示应答
	//等待50微秒
	struct ConnectStruct * pConnectAddr = (struct ConnectStruct*)m_pShm;
	int i = 0;
	for(i=0;i<5;i++)
	{
		p();
		if(pConnectAddr->iUsedFlag != 0)
		{
			v();
			usleep(10);						
		}
		else
		{
			pConnectAddr->iUsedFlag = 1;
			v();	
			break;	
		}			
	}
	for(i=0;i<5;i++)
	{
		p();
		if(pConnectAddr->iUsedFlag != 2)
		{
			v();
			usleep(10);						
		}
		else
		{		
			//获得server分配的共享内存槽位			
			m_stConnected.iUsedFlag = 1;
			m_stConnected.iPos = pConnectAddr->iPos;
			m_stConnected.iBuffSize = 1024*1024;
			m_stConnected.pBaseAddr = m_pShm;
			m_stConnected.iType = 1;//client端

	printf("Connect to Server successfully ipos = %d\n", m_stConnected.iPos);	
			//恢复连接标识，以便其他用户可发起连接
			pConnectAddr->iUsedFlag = 0;
			v();		
			return CreateChannel(&m_stConnected);
		}			
	}
	SetErrorMsg("Can not connect to CServer");
	/*将共享内存与当前进程断开*/
	if(shmdt(m_pShm)==-1)
	{
		printf("shmdt is fail\n");
	}

	return NULL;
}

CChannel *CShmClient::CreateChannel(ConnectStruct  *pStruct)
{
	return new CShmChannel(pStruct);
}
	/*信号量的P操作*/
void CShmClient::p()
{
	struct sembuf sem_p;
	sem_p.sem_num=0;
	sem_p.sem_op=-1;
	if(semop(m_iSemid,&sem_p,1)==-1)
	{
			printf("p operation is fail\n");	
	}
	
}
/*信号量的V操作*/
void CShmClient::v()
{
	struct sembuf sem_v;
	sem_v.sem_num=0;
	sem_v.sem_op=1;
	if(semop(m_iSemid,&sem_v,1)==-1)
	{
			printf("v operation is fail\n");
	}
}
