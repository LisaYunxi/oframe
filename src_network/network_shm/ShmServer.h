// ShmServer.h: interface for the CShmServer class .
//
//  
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ShmSERVER_H__FE5CED67_6EC3_11D7_BB64_444553540000__INCLUDED_)
#define AFX_ShmSERVER_H__FE5CED67_6EC3_11D7_BB64_444553540000__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Server.h"
#include "ShmChannel.h"

/**CShmServer是一个基于Shm的网络服务器端
*/
class  CShmServer : public CServerBase
{
public:
	/**构造函数，在指定的服务名上侦听网络的连接
	* @param  pName	指定的服务名称
	*/
	CShmServer(CServiceName *pName);
	
	/**析构函数，释放占用的资源
	*/
	virtual ~CShmServer();
	
	/** 基于Shm的获取select文件号方法
	* @return select时的文件号
	*/
	virtual int GetId(void);
	
	void p();
	void v();
protected:
	/**基于Shm的等待客户端连接方法
	* @param  wait	表示需要等待到连接才返回，0表示不等待
	* @return NULL   新Channel未建成功
	* @return 非NULL 新的Channel
	*/
	virtual CChannel *Accept(int wait);

	virtual CChannel *CreateChannel(ConnectStruct  *pStruct);
private:
	int m_nID;	/**< 存储本服务器端的文件号 */
	int m_iShmid;
	void *m_pBaseShm;
	int m_iSemid;
	int m_iCount;
};

#endif // !defined(AFX_ShmSERVER_H__FE5CED67_6EC3_11D7_BB64_444553540000__INCLUDED_)
