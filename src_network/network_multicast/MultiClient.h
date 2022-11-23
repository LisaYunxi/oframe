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

	/**基于Multi的连接实现函数
	* @param pName 服务器的连结信息
	* @return 非NULL 成功，已建立的Channel
	* @return NULL 失败
	*/
	virtual CChannel *Connect(CServiceName *pName);
	char *GetMatchIP(char *pszMask,char *OutIP);
private:
	char *m_pNetworkIP;/**< 存放网段地址 */
	char *m_pMultiCastIP;	/**< 存放多播地址*/
	char m_sHostIP[50];
};

#endif // !defined(AFX_MultiCLIENT_H__CFF7736E_8042_42C5_847C_B3A779399B12__INCLUDED_)
