#ifndef AFX_TCPSERVER_H__
#define AFX_TCPSERVER_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Server.h"

//CTcpServer��һ������TCP�������������

class  CTcpServer : public CServerBase
{
public:
	CTcpServer(CServiceName *pName);
	virtual ~CTcpServer();

	//����TCP�Ļ�ȡselect�ļ��ŷ���
	//@return selectʱ���ļ���
	virtual int GetId(void);

protected:
	//����TCP�ĵȴ��ͻ������ӷ���
	//@param  wait	��ʾ��Ҫ�ȴ������Ӳŷ��أ�0��ʾ���ȴ�
	//@return NULL   ��Channelδ���ɹ�
	//@return ��NULL �µ�Channel
	virtual CChannel *Accept(int wait);

	virtual CChannel *CreateChannel(int id);

private:
	int m_nID;	//�洢���������˵��ļ���
};

#endif