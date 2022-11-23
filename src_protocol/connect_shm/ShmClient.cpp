// ShmClient.cpp: implementation of the CShmClient class.
//
// 
//////////////////////////////////////////////////////////////////////

#include "public.h"
#include "SocketInit.h"
#include "ShmClient.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CShmClient::CShmClient()
{

}

CShmClient::~CShmClient()
{

#ifdef WIN32
    UnmapViewOfFile(m_pShm);

    CloseHandle(hMapFile);
#endif
  
}

/************************************************************************/
/*  ShmConnect	�����ڴ�����	                                        */
/*  in		CServiceName *pName										    */
/*																		*/
/*  out		�ڴ��ַ												    */
/************************************************************************/
void *CShmClient::ShmConnect(CServiceName *pName,int number)
{

	int iCount = pName->GetPort();

	key_t shmKeyid = atoi(pName->GetHost())+number*iCount;
	
	shmsize=shmsize+sizeof(ConnectStruct);

	//link to server share memory
#ifdef UNIX
	//share memory for linux
	m_iShmid = shmget((key_t)shmKeyid, shmsize, IPC_EXCL);

	if(m_iShmid == -1)
	{
		fprintf(stderr, "shmget failed\n");
		RAISE_RUNTIME_ERROR("shmget failed");
		return NULL;
	}

#else
	//share memory for windows
	char szName[100];
	memset(szName,0x00,sizeof(szName));
	//mapping object create
	sprintf(szName,"%s%d",pName->GetHost(),number);

	hMapFile = OpenFileMapping(
                   FILE_MAP_ALL_ACCESS, // read/write access
                   FALSE, // do not inherit the name
                   szName); // name of mapping object

	if(hMapFile == NULL)
	{
		
		printf( "OpenFileMapping failed\n");
		RAISE_RUNTIME_ERROR("OpenFileMapping failed");
		return NULL;
	}
	
#endif

	//�������ڴ����ӵ���ǰ���̵ĵ�ַ�ռ�
#ifdef UNIX
	m_pShm = shmat(m_iShmid, (void*)0, 0);

#else

	m_pShm = (void *) MapViewOfFile(hMapFile, // handle to map object
                FILE_MAP_ALL_ACCESS, // read/write permission
                0,
                0,
                shmsize);
#endif

	if(m_pShm == (void*)-1)
	{
		printf( "shmat failed\n");
		//RAISE_RUNTIME_ERROR("shmat failed");
		return NULL;
	}
	printf("Memory attached at %X\n", (long)m_pShm);	
	
// 	/*���������ڴ���ź�����IPC*/
// 	m_iSemid=semget(shmKeyid+1,1,0666|IPC_CREAT);
// 	if(m_iSemid==-1)
// 	{
// 			fprintf(stderr, "semget failed\n");
// 			RAISE_RUNTIME_ERROR("semget failed");
// 	}
// 	/*�����ź����ĳ�ʼֵ��������Դ����*/
// 	union semun{
// 		int val;
// 		struct semid_ds *buf;
// 		ushort *array;
// 	}sem_u;
// 	
//	sem_u.val=1;
//  semctl(m_iSemid,0,SETVAL,sem_u);
	
	//"1"��ʾ���� ����2����ʾӦ��
	//�ȴ�50΢��
	struct ConnectStruct * pConnectAddr = (struct ConnectStruct*)m_pShm;

	//���server����Ĺ����ڴ��λ			
	m_stConnected.iUsedFlag = 1;
	m_stConnected.iPos = pConnectAddr->iPos;
	m_stConnected.iBuffSize = pConnectAddr->iBuffSize;
	m_stConnected.pBaseAddr = m_pShm;
	m_stConnected.iType = 1;//client��

	return m_pShm;

}

	/*�ź�����P����*/
void CShmClient::p()
{
#ifdef UNIX
	struct sembuf sem_p;
	sem_p.sem_num=0;
	sem_p.sem_op=-1;
	if(semop(m_iSemid,&sem_p,1)==-1)
	{
			printf("p operation is fail\n");	
	}
#endif
}
/*�ź�����V����*/
void CShmClient::v()
{
#ifdef UNIX
	struct sembuf sem_v;
	sem_v.sem_num=0;
	sem_v.sem_op=1;
	if(semop(m_iSemid,&sem_v,1)==-1)
	{
			printf("v operation is fail\n");
	}
#endif
}
