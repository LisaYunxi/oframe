// ShmChannel.cpp: implementation of the CShmChannel class.
//

//////////////////////////////////////////////////////////////////////

#include "public.h"
#include "SocketInit.h"
#include "ShmChannel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShmChannel::CShmChannel(ConnectStruct  *pStruct) : CChannel(CT_STREAM, -1)
{
	memcpy(&m_stConnected,pStruct,sizeof(ConnectStruct));		
}

CShmChannel::~CShmChannel()
{
	//把共享内存还给server
	ConnectStruct  *pStruct = (ConnectStruct  *)(m_stConnected.pBaseAddr + sizeof(ConnectStruct)*m_stConnected.iPos);
	pStruct->iUsedFlag = 0;	
	if(m_stConnected.iType == 0)
	{
		printf("Server ~CShmChannel()\n");
	}
	else
	{
		printf(" Client ~CShmChannel()\n");
	}
}

int CShmChannel::ReadImp(int number, char *buffer)
{
	struct ConnectStruct *pConnectStruct = (struct ConnectStruct*)(m_stConnected.pBaseAddr + sizeof(ConnectStruct)*m_stConnected.iPos);
	//server端读取前半部分
	if(m_stConnected.iType == 0)
	{
		void  *pReadBaseAddr = m_stConnected.pBaseAddr + m_stConnected.iPos * m_stConnected.iBuffSize*2;  
	int size  = min(number, pConnectStruct->iUpWriteCount - pConnectStruct->iUpReadCount);        
    /* first get the data from pConnectStruct->iUpReadCount until the end of the buffer */
    int len = min(size, m_stConnected.iBuffSize - (pConnectStruct->iUpReadCount & (m_stConnected.iBuffSize - 1)));
    memcpy(buffer, pReadBaseAddr + (pConnectStruct->iUpReadCount & (m_stConnected.iBuffSize - 1)), len);
    /* then get the rest (if any) from the beginning of the buffer */
    memcpy(buffer + len, pReadBaseAddr, size - len);
    pConnectStruct->iUpReadCount += size;
    return size;	
	}
	//client端读取后半部分
	else
	{
		void  *pReadBaseAddr = m_stConnected.pBaseAddr + m_stConnected.iPos * m_stConnected.iBuffSize*2 +  m_stConnected.iBuffSize;
		int size  = min(number, pConnectStruct->iDownWriteCount - pConnectStruct->iDownReadCount);        
    /* first get the data from pConnectStruct->iUpReadCount until the end of the buffer */
    int len = min(size, m_stConnected.iBuffSize - (pConnectStruct->iDownReadCount & (m_stConnected.iBuffSize - 1)));
    memcpy(buffer, pReadBaseAddr + (pConnectStruct->iDownReadCount & (m_stConnected.iBuffSize - 1)), len);
    /* then get the rest (if any) from the beginning of the buffer */
    memcpy(buffer + len, pReadBaseAddr, size - len);
    pConnectStruct->iDownReadCount += size;
    return size;	
	}
}

int CShmChannel::WriteImp(int number, char *buffer)
{
	struct ConnectStruct *pConnectStruct = (struct ConnectStruct*)(m_stConnected.pBaseAddr + sizeof(ConnectStruct)*m_stConnected.iPos);
	//client端写前半部分
	if(m_stConnected.iType != 0)
	{
		void  *pWriteBaseAddr = m_stConnected.pBaseAddr + m_stConnected.iPos * m_stConnected.iBuffSize*2;
		int size = min(number, m_stConnected.iBuffSize - (pConnectStruct->iUpWriteCount - pConnectStruct->iUpReadCount));
    /* first put the data starting from fifo->in to buffer end */
    int len  = min(size, m_stConnected.iBuffSize - (pConnectStruct->iUpWriteCount & (m_stConnected.iBuffSize - 1)));
    memcpy(pWriteBaseAddr + (pConnectStruct->iUpWriteCount & (m_stConnected.iBuffSize - 1)), buffer, len);
    /* then put the rest (if any) at the beginning of the buffer */
    memcpy(pWriteBaseAddr, buffer + len, size - len);
    pConnectStruct->iUpWriteCount += size;
    return size;	
	}
	//server端写后半部分
	else
	{
		void  *pWriteBaseAddr = m_stConnected.pBaseAddr + m_stConnected.iPos * m_stConnected.iBuffSize*2 +  m_stConnected.iBuffSize;		
		int size = min(number, m_stConnected.iBuffSize - (pConnectStruct->iDownWriteCount - pConnectStruct->iDownReadCount));
    /* first put the data starting from pConnectStruct->iDownWriteCount to buffer end */
    int len  = min(size, m_stConnected.iBuffSize - (pConnectStruct->iDownWriteCount & (m_stConnected.iBuffSize - 1)));
    memcpy(pWriteBaseAddr + (pConnectStruct->iDownWriteCount & (m_stConnected.iBuffSize - 1)), buffer, len);
    /* then put the rest (if any) at the beginning of the buffer */
    memcpy(pWriteBaseAddr, buffer + len, size - len);
    pConnectStruct->iDownWriteCount += size;
    return size;		
	}	
}

bool CShmChannel::AvailableImp(void)
{
	return true;
}

bool CShmChannel::DisconnectImp(void)
{
	closesocket(GetId());
	return false;
}

bool CShmChannel::CheckConnectionImp(void)
{
	return true;
}

char *CShmChannel::GetRemoteName(void)
{
	struct in_addr addr;
	memcpy(&addr, &m_dwInAddr, sizeof(DWORD));
	return inet_ntoa(addr);
}


/** 获取本地的地址，例如IP、Mac等
* @return   本地地址
* lidp 20140716
*/
char *CShmChannel::GetLocalName(void)
{
    struct in_addr addr;
    memcpy(&addr, &m_dwLocalAddr, sizeof(DWORD));
    return inet_ntoa(addr);
}


