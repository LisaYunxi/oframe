#ifndef AFX_TCPSERVER_H__
#define AFX_TCPSERVER_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Server.h"

//CTcpServer是一个基于TCP的网络服务器端

class  CTcpServer : public CServerBase
{
public:
	CTcpServer(CServiceName *pName);
	virtual ~CTcpServer();

	//基于TCP的获取select文件号方法
	//@return select时的文件号
	virtual int GetId(void);

protected:
	//基于TCP的等待客户端连接方法
	//@param  wait	表示需要等待到连接才返回，0表示不等待
	//@return NULL   新Channel未建成功
	//@return 非NULL 新的Channel
	virtual CChannel *Accept(int wait);

	virtual CChannel *CreateChannel(int id);

private:
	int m_nID;	//存储本服务器端的文件号
};

#endif