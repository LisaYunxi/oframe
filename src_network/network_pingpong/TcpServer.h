// TcpServer.h: interface for the CTcpServer class .
//
// 2006.10.11	hwp	为支持SSL，增加方法CreateChannel 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCPSERVER_H__FE5CED67_6EC3_11D7_BB64_444553540000__INCLUDED_)
#define AFX_TCPSERVER_H__FE5CED67_6EC3_11D7_BB64_444553540000__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Server.h"

/**CTcpServer是一个基于TCP的网络服务器端
*/
class  CTcpServer : public CServerBase
{
public:
	/**构造函数，在指定的服务名上侦听网络的连接
	* @param  pName	指定的服务名称
	*/
	CTcpServer(CServiceName *pName);
	
	/**析构函数，释放占用的资源
	*/
	virtual ~CTcpServer();
	
	/** 基于TCP的获取select文件号方法
	* @return select时的文件号
	*/
	virtual int GetId(void);
protected:
	/**基于TCP的等待客户端连接方法
	* @param  wait	表示需要等待到连接才返回，0表示不等待
	* @return NULL   新Channel未建成功
	* @return 非NULL 新的Channel
	*/
	virtual CChannel *Accept(int wait);

	virtual CChannel *CreateChannel(int id);
private:
	int m_nID;	/**< 存储本服务器端的文件号 */
};

#endif // !defined(AFX_TCPSERVER_H__FE5CED67_6EC3_11D7_BB64_444553540000__INCLUDED_)
