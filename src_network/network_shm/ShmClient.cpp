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
	//���������ڴ�
	m_iShmid = shmget((key_t)shmKeyid, iCount*1024*1024*2UL, 0666|IPC_CREAT);
	if(m_iShmid == -1)
	{
		fprintf(stderr, "shmget failed\n");
		RAISE_RUNTIME_ERROR("shmget failed");
	}
	//�������ڴ����ӵ���ǰ���̵ĵ�ַ�ռ�
	m_pShm = shmat(m_iShmid, (void*)0, 0);
	if(m_pShm == (void*)-1)
	{
		fprintf(stderr, "shmat failed\n");
		RAISE_RUNTIME_ERROR("shmat failed");
	}
	printf("Memory attached at %X\n", (long)m_pShm);	
	
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
	
	//"1"��ʾ���� ����2����ʾӦ��
	//�ȴ�50΢��
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
			//���server����Ĺ����ڴ��λ			
			m_stConnected.iUsedFlag = 1;
			m_stConnected.iPos = pConnectAddr->iPos;
			m_stConnected.iBuffSize = 1024*1024;
			m_stConnected.pBaseAddr = m_pShm;
			m_stConnected.iType = 1;//client��

	printf("Connect to Server successfully ipos = %d\n", m_stConnected.iPos);	
			//�ָ����ӱ�ʶ���Ա������û��ɷ�������
			pConnectAddr->iUsedFlag = 0;
			v();		
			return CreateChannel(&m_stConnected);
		}			
	}
	SetErrorMsg("Can not connect to CServer");
	/*�������ڴ��뵱ǰ���̶Ͽ�*/
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
	/*�ź�����P����*/
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
/*�ź�����V����*/
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
