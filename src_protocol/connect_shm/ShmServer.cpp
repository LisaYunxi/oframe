// ShmServer.cpp: implementation of the CShmServer class.
//
// 
//////////////////////////////////////////////////////////////////////

#include "public.h"
#include "SocketInit.h"
#include "ShmServer.h"

/************************************************************************/
/*  CShmServer	共享内存创建                                            */
/*  in		const char *key											    */
/*																		*/
/*  out																    */
/************************************************************************/
CShmServer::CShmServer(CServiceName *pName,int number,int shmsize) : m_size(shmsize)
{

	m_iCount = pName->GetPort();

	key_t shmKeyid = atoi(pName->GetHost())+m_iCount*number;

	m_size=m_size+sizeof(ConnectStruct);
	
	//create share memory
#ifdef UNIX
	//share memory for linux
	m_iShmid = shmget((key_t)shmKeyid, m_size, 0666|IPC_CREAT);
			
	if(m_iShmid == -1)
	{
		fprintf(stderr, "shmget failed\n");
		RAISE_RUNTIME_ERROR("shmget failed");
		return;
	}	

#else
	//share memory for windows
	char szName[100];

	memset(szName,0x00,sizeof(szName));

	sprintf(szName,"%s%d",pName->GetHost(),number);

    hMapFile = CreateFileMapping(
                 INVALID_HANDLE_VALUE, // use paging file
                 NULL, // default security
                 PAGE_READWRITE, // read/write access
                 0, // maximum object size (high-order DWORD)
                 m_size, // maximum object size (low-order DWORD)
                 szName); // name of mapping object
			
	if(hMapFile == NULL)
	{
		fprintf(stderr, "CreateFileMapping failed\n");
		RAISE_RUNTIME_ERROR("CreateFileMapping failed");
		return;
	}

#endif

	//share memory attach
#ifdef UNIX
	//attach to share memory
	m_pBaseShm = shmat(m_iShmid, (void*)0, 0);

#else
	//attach to share memory
	m_pBaseShm = (void *) MapViewOfFile(hMapFile, // handle to map object
                    FILE_MAP_ALL_ACCESS, // read/write permission
                    0,
                    0,
                    m_size);

#endif
	//if share memory attach failed
	if(m_pBaseShm == (void*)-1)
	{
		fprintf(stderr, "shmat failed\n");
		RAISE_RUNTIME_ERROR("shmat failed");
		return;
	}
	else
	{
		//success to attach share memory
		memset(m_pBaseShm,0x00,m_size);
	}

	printf("Memory attached at %X\n", (long)m_pBaseShm);	
	
// 	/*创建共享内存和信号量的IPC*/
// 	m_iSemid=semget(shmKeyid+1,1,0666|IPC_CREAT);
// 	if(m_iSemid==-1)
// 	{
// 			fprintf(stderr, "semget failed\n");
// 			RAISE_RUNTIME_ERROR("semget failed");
// 	}
// 	/*设置信号量的初始值，就是资源个数*/
// 	union semun{
// 		int val;
// 		struct semid_ds *buf;
// 		ushort *array;
// 	}sem_u;
// 	
// 	sem_u.val=1;
// 	semctl(m_iSemid,0,SETVAL,sem_u);
	
}


CShmServer::~CShmServer()
{
	//dittach
#ifdef UNIX
	if(shmdt(m_pBaseShm) == -1)
	{
		fprintf(stderr, "shmdt failed\n");
	}

	/*IPC remove*/
	if(semctl(m_iSemid,0,IPC_RMID,0)==-1)
	{
		printf("semctl delete error\n");
	}
	if(shmctl(m_iShmid,IPC_RMID,NULL)==-1)
	{
		printf("shmctl delete error\n");
	}
	printf("game over\n");
#else
	
   UnmapViewOfFile(m_pBaseShm);

   CloseHandle(hMapFile);
#endif
}

void *CShmServer::ShmSet()
{

	struct ConnectStruct * pConnectAddr = (struct ConnectStruct*)m_pBaseShm;
	pConnectAddr->iUsedFlag = 1;
	pConnectAddr->iPos = 0;
	pConnectAddr->iBuffSize = m_size;
	pConnectAddr->pBaseAddr = m_pBaseShm;
	pConnectAddr->iType = 0; 
	pConnectAddr->iUpWriteCount = 0;
	pConnectAddr->iUpReadCount = 0;	
	pConnectAddr->iDownWriteCount = 0;
	pConnectAddr->iDownReadCount = 0;
	return m_pBaseShm;

}



	/*信号量的P操作*/
void CShmServer::p()
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
/*信号量的V操作*/
void CShmServer::v()
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
