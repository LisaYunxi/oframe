#ifndef AFX_TCPCLIENT_H__
#define AFX_TCPCLIENT_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Client.h"
//CTcpClient��һ������TCP������ͻ���

class  CTcpClient : public CClientBase
{
public:
	CTcpClient();
	virtual ~CTcpClient();

	//����TCP������ʵ�ֺ���
	//@param pName ��������������Ϣ
	//@return ��NULL �ɹ����ѽ�����Channel
	//@return NULL ʧ��
	virtual CChannel *Connect(CServiceName *pName);
	
protected:
	virtual CChannel *CreateChannel(int id);
};


#endif
