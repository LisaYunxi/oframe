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
	//���������ڴ�
	m_iShmid = shmget((key_t)shmKeyid, m_iCount*2048*1024UL, 0666|IPC_CREAT);
	if(m_iShmid == -1)
	{
		fprintf(stderr, "shmget failed\n");
		RAISE_RUNTIME_ERROR("shmget failed");
	}	
	//�������ڴ����ӵ���ǰ���̵ĵ�ַ�ռ�
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
	
	/*���������ڴ���ź�����IPC*/
	m_iSemid=semget(shmKeyid+1,1,0666|IPC_CREAT);
	if(m_iSemid==-1)
	{
			fprintf(stderr, "semget failed\n");
			RAISE_RUNTIME_ERROR("semget failed");
	}
	/*�����ź����ĳ�ʼֵ��������Դ����*/
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
	//�ѹ����ڴ�ӵ�ǰ�����з���
	if(shmdt(m_pBaseShm) == -1)
	{
		fprintf(stderr, "shmdt failed\n");
	}
	/*IPC������ʾɾ���������һֱ������ϵͳ��*/
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
					//��ʼ�����������
					pConnectAddr2->iUsedFlag = 1;
					pConnectAddr2->iPos = j;
					pConnectAddr2->iBuffSize = 1024*1024;
					pConnectAddr2->pBaseAddr = m_pBaseShm;
					pConnectAddr2->iType = 0; 
					pConnectAddr2->iUpWriteCount = 0;
					pConnectAddr2->iUpReadCount = 0;	
					pConnectAddr2->iDownWriteCount = 0;
					pConnectAddr2->iDownReadCount = 0;
		
					
					//�������ӽ�����
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
	/*�ź�����P����*/
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
/*�ź�����V����*/
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
