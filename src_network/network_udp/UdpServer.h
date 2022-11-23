// UdpServer.h: interface for the CUdpServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UDPSERVER_H__AA737DFB_1BC5_4B35_9EEB_BCC53F1617FC__INCLUDED_)
#define AFX_UDPSERVER_H__AA737DFB_1BC5_4B35_9EEB_BCC53F1617FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Channel.h"
#include "ServiceName.h"
#include "Server.h"

class CUdpServer : public CServerBase 
{
public:
	CUdpServer(CServiceName *pName);
	virtual ~CUdpServer();

	virtual CChannel *AcceptPublic(int wait);
protected:
	/*接收客户端连接
	* @param  wait	表示需要等待到连接才返回，0表示不等待
	* @return NULL   新Channel未建成功
	* @return 非NULL 新的Channel
	*/
	virtual CChannel *Accept(int wait);

	virtual int GetId(void);
private:
	int m_nID;	/**< 套接字 */
	CServiceName m_ServiceName;
	bool m_bConnected;
};

#endif // !defined(AFX_UDPSERVER_H__AA737DFB_1BC5_4B35_9EEB_BCC53F1617FC__INCLUDED_)
