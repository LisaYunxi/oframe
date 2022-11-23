// ShmClient.h: interface for the CShmClient class .
//
// 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ShmCLIENT_H__FE5CED65_6EC3_11D7_BB64_444553540000__INCLUDED_)
#define AFX_ShmCLIENT_H__FE5CED65_6EC3_11D7_BB64_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ServiceName.h"
#include "ShmCommon.h"

/**CShmClient是一个基于Shm的网络客户端
*/


class  CShmClient
{
public:
	/**构造函数，完成初始化
	*/
	CShmClient();
	
	/**析构函数
	*/
	virtual ~CShmClient();

	/**基于Shm的连接实现函数
	*/
	void *ShmConnect(CServiceName *pName,int number);
	
	void p();
	void v();
protected:
	//CChannel *CreateChannel(ConnectStruct  *pStruct);
	int shmsize;
private:

	int m_iShmid;
	//共享内存地址
	void *m_pShm;
	int m_iSemid;
	ConnectStruct m_stConnected;

#ifdef WIN32
    HANDLE hMapFile;
#endif
};


#endif // !defined(AFX_ShmCLIENT_H__FE5CED65_6EC3_11D7_BB64_444553540000__INCLUDED_)
