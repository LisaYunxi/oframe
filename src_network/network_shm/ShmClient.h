// ShmClient.h: interface for the CShmClient class .
//
// 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ShmCLIENT_H__FE5CED65_6EC3_11D7_BB64_444553540000__INCLUDED_)
#define AFX_ShmCLIENT_H__FE5CED65_6EC3_11D7_BB64_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Client.h"
#include "ShmChannel.h"
#include <sys/sem.h>
/**CShmClient是一个基于Shm的网络客户端
*/
class  CShmClient : public CClientBase
{
public:
	/**构造函数，完成初始化
	*/
	CShmClient();
	
	/**析构函数
	*/
	virtual ~CShmClient();

	/**基于Shm的连接实现函数
	* @param pName 服务器的连结信息
	* @return 非NULL 成功，已建立的Channel
	* @return NULL 失败
	*/
	virtual CChannel *Connect(CServiceName *pName);
	void p();
	void v();
protected:
	CChannel *CreateChannel(ConnectStruct  *pStruct);
private:
	int m_iShmid;
	void *m_pShm;
	int m_iSemid;
	ConnectStruct m_stConnected;
};


#endif // !defined(AFX_ShmCLIENT_H__FE5CED65_6EC3_11D7_BB64_444553540000__INCLUDED_)
