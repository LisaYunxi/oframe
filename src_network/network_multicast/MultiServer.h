// MultiServer.h: interface for the CMultiServer class.
//
#ifndef _MultiSERVER_H__
#define _MultiSERVER_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Channel.h"
#include "ServiceName.h"
#include "Server.h"

class CMultiServer : public CServerBase 
{
public:
	CMultiServer(CServiceName *pName);
	virtual ~CMultiServer();
	
protected:
	/*���տͻ�������
	* @param  wait	��ʾ��Ҫ�ȴ������Ӳŷ��أ�0��ʾ���ȴ�
	* @return NULL   ��Channelδ���ɹ�
	* @return ��NULL �µ�Channel
	*/
	virtual CChannel *Accept(int wait);

	virtual int GetId(void);
	
	char *GetMatchIP(char *pszMask,char *OutIP);
	
private:
	int m_nID;	/**< �׽��� */
	CServiceName m_ServiceName;
	bool m_bConnected;
	char *m_pNetworkIP;/**< ������ε�ַ */
	char *m_pMultiCastIP;	/**< ��Ŷಥ��ַ*/
	char m_sHostIP[50];
	
};

#endif
