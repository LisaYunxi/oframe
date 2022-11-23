#ifndef AFX_TCPCLIENT_H__
#define AFX_TCPCLIENT_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Client.h"
//CTcpClient是一个基于TCP的网络客户端

class  CTcpClient : public CClientBase
{
public:
	CTcpClient();
	virtual ~CTcpClient();

	//基于TCP的连接实现函数
	//@param pName 服务器的连结信息
	//@return 非NULL 成功，已建立的Channel
	//@return NULL 失败
	virtual CChannel *Connect(CServiceName *pName);
	
protected:
	virtual CChannel *CreateChannel(int id);
};


#endif
