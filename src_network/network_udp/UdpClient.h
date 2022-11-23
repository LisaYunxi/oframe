// UdpClient.h: interface for the CUdpClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UDPCLIENT_H__CFF7736E_8042_42C5_847C_B3A779399B12__INCLUDED_)
#define AFX_UDPCLIENT_H__CFF7736E_8042_42C5_847C_B3A779399B12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Client.h"
#include "Client.h"

class CUdpClient : public CClientBase 
{
public:
	CUdpClient();
	virtual ~CUdpClient();

	/**基于UDP的连接实现函数
	* @param pName 服务器的连结信息
	* @return 非NULL 成功，已建立的Channel
	* @return NULL 失败
	*/
	virtual CChannel *Connect(CServiceName *pName);

};

#endif // !defined(AFX_UDPCLIENT_H__CFF7736E_8042_42C5_847C_B3A779399B12__INCLUDED_)
