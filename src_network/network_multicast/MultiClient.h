// MultiClient.h: interface for the CMultiClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MultiCLIENT_H__CFF7736E_8042_42C5_847C_B3A779399B12__INCLUDED_)
#define AFX_MultiCLIENT_H__CFF7736E_8042_42C5_847C_B3A779399B12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Client.h"

class CMultiClient : public CClientBase 
{
public:
	CMultiClient();
	virtual ~CMultiClient();

	/**����Multi������ʵ�ֺ���
	* @param pName ��������������Ϣ
	* @return ��NULL �ɹ����ѽ�����Channel
	* @return NULL ʧ��
	*/
	virtual CChannel *Connect(CServiceName *pName);
	char *GetMatchIP(char *pszMask,char *OutIP);
private:
	char *m_pNetworkIP;/**< ������ε�ַ */
	char *m_pMultiCastIP;	/**< ��Ŷಥ��ַ*/
	char m_sHostIP[50];
};

#endif // !defined(AFX_MultiCLIENT_H__CFF7736E_8042_42C5_847C_B3A779399B12__INCLUDED_)
