// ShmServer.cpp: implementation of the CShmServer class.
//
// 
//////////////////////////////////////////////////////////////////////

#include "public.h"
#include "SocketInit.h"
#include "ShmServer.h"
#include "ShmChannel.h"
#include <sys/sem.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShmServer::CShmServer(CServiceName *pName) : CServerBase(pName)
{
	m_nID = -1;
	key_t shmKeyid = atoi(pName->GetHost());
	m_iCount = pName->GetPort();	
	//创建共享内存
	m_iShmid = shmget((key_t)shmKeyid, m_iCount*2048*1024UL, 0666|IPC_CREAT);
	if(m_iShmid == -1)
	{
		fprintf(stderr, "shmget failed\n");
		RAISE_RUNTIME_ERROR("shmget failed");
	}	
	//将共享内存连接到当前进程的地址空间
	m_pBaseShm = shmat(m_iShmid, (void*)0, 0);
	if(m_pBaseShm == (void*)-1)
	{
		fprintf(stderr, "shmat failed\n");
		RAISE_RUNTIME_ERROR("shmat failed");
	}
	else
	{
		memset(m_pBaseShm,0,m_iCount*1024*1024*2UL);
	}
	printf("Memory attached at %X\n", (long)m_pBaseShm);	
	
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
	
}

CShmServer::~CShmServer()
{
	//把共享内存从当前进程中分离
	if(shmdt(m_pBaseShm) == -1)
	{
		fprintf(stderr, "shmdt failed\n");
	}
	/*IPC必须显示删除。否则会一直留存在系统中*/
	if(semctl(m_iSemid,0,IPC_RMID,0)==-1)
	{
		printf("semctl delete error\n");
	}
	if(shmctl(m_iShmid,IPC_RMID,NULL)==-1)
	{
		printf("shmctl delete error\n");
	}
}

CChannel *CShmServer::Accept(int wait)
{
	bool bAccept = false;
	struct ConnectStruct * pConnectAddr = (struct ConnectStruct*)m_pBaseShm;
	struct ConnectStruct * pConnectAddr2 = NULL;
	int i = 0;
	for(i=0;i<5;i++)
	{
		p();
		if(pConnectAddr->iUsedFlag != 1)
		{
			v();
			break;				
		}
		else
		{
			pConnectAddr->iUsedFlag = 1;
			for(int j=1;j<m_iCount;j++)
			{
				pConnectAddr2 = (struct ConnectStruct*)(m_pBaseShm + sizeof(ConnectStruct)*j);
				if(pConnectAddr2->iUsedFlag != 1);
				{
					//初始化分配的区域
					pConnectAddr2->iUsedFlag = 1;
					pConnectAddr2->iPos = j;
					pConnectAddr2->iBuffSize = 1024*1024;
					pConnectAddr2->pBaseAddr = m_pBaseShm;
					pConnectAddr2->iType = 0; 
					pConnectAddr2->iUpWriteCount = 0;
					pConnectAddr2->iUpReadCount = 0;	
					pConnectAddr2->iDownWriteCount = 0;
					pConnectAddr2->iDownReadCount = 0;
		
					
					//设置连接交互区
					pConnectAddr->iUsedFlag = 2;
					pConnectAddr->iPos = j;
					
		printf("Received shm connect ok\n");
					bAccept = true;
					break;
				}				
			}
			v();	
			break;	
		}			
	}
	if(bAccept)
	{
			return CreateChannel(pConnectAddr2);
	}
	else
	{
		return NULL;
	}
}

int CShmServer::GetId(void)
{
	return m_nID;
}

CChannel *CShmServer::CreateChannel(ConnectStruct  *pStruct)
{
	return new CShmChannel(pStruct);
}
	/*信号量的P操作*/
void CShmServer::p()
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
void CShmServer::v()
{
	struct sembuf sem_v;
	sem_v.sem_num=0;
	sem_v.sem_op=1;
	if(semop(m_iSemid,&sem_v,1)==-1)
	{
		printf("v operation is fail\n");
	}
}
